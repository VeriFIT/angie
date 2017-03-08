/*******************************************************************************

Copyright (C) 2017 Michal Kotoun

This file is a part of Angie project.

Angie is free software: you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option)
any later version.

Angie is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with Angie.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/
/** @file ForwardNullAnalysis.hh */

#pragma once

#include "Definitions.hh"
#include "General.hh"
#include "IOperation.hh"
#include "ICfgNode.hh"
#include "IState.hh"
#include "StateStorage.hh"
#include "DummyState.hh"

#include <cassert>

#include "FrontedValueMapper.hh"
//#include <range/v3/all.hpp>

using namespace ::std;

class StackFrame {
public:
  int currentOffset{0};
  void AllocateMem(size_t size) { currentOffset += size; }
};

// ------------
// ForwardNullAnalysis
class ForwardNullAnalysisState : public IState {
private:
  
  Mapper&     globalMapping;
public:
  FuncMapper& funcMapping;
private:
  Mapper      localMapping;

public:

  /*ctr*/ ForwardNullAnalysisState(
      ICfgNode& lastCfgNode, 
      ICfgNode& nextCfgNode,
      IValueContainer& vc,
      Mapper& globalMapping,
      FuncMapper& funcMapping
      ) :
    IState(lastCfgNode, nextCfgNode),
    vc(&vc),
    globalMapping(globalMapping),
    funcMapping(funcMapping),
    localMapping(vc)
  {
    //stack.push_back(StackFrame{});
    stackCurrentAddr = ValueId{0};//GetVC().CreateVal(Type::CreateCharPointerType());
    //stackBaseAddr = GetVC().CreateVal(Type::CreateCharPointerType());
    //heapBaseAddr = GetVC().CreateVal(Type::CreateCharPointerType());
  }

  // copy ctr ??? or what
  /*ctr*/ ForwardNullAnalysisState(
      const ForwardNullAnalysisState& state, 
      ICfgNode& lastCfgNode, ICfgNode& 
      nextCfgNode
      ) :
    IState(lastCfgNode, nextCfgNode),
    vc(state.vc),
    globalMapping(state.globalMapping),
    funcMapping(state.funcMapping),
    localMapping(state.localMapping),
    //stack(state.stack),
    //stackMemory(state.stackMemory),
    //heapMemory(state.heapMemory),
    stackCurrentAddr(state.stackCurrentAddr),
    hasValue(state.hasValue)
  {
  }
  
  //TODO: same algo as in method bellow
  //TODO: rename
  
  virtual ValueId GetAnyVar(FrontendIdTypePair var) override
  {
    try { return globalMapping.GetValueId(var.id); }
    catch (exception e) { return localMapping.GetValueId(var.id); }
  }
  //// DO NOT USE - use GetAnyOrCreateLocalVar
  //[[deprecated]] virtual ValueId GetOrCreateGlobalVar(FrontendIdTypePair var) override
  //{
  //  return globalMapping.CreateOrGetValueId(var);
  //}
  //// DO NOT USE - use GetAnyOrCreateLocalVar
  //[[deprecated]] virtual ValueId GetOrCreateLocalVar(FrontendIdTypePair var) override
  //{
  //  return localMapping.CreateOrGetValueId(var);
  //}

  //[[deprecated]] virtual ValueId GetAnyOrCreateLocalVar(FrontendIdTypePair var) override
  //{
  //  try { return globalMapping.GetValueId(var.id); }
  //  catch(exception e) { return localMapping.CreateOrGetValueId(var); }
  //}
  virtual void LinkGlobalVar(FrontendIdTypePair var, ValueId value) override
  {
    globalMapping.LinkToValueId(var.id, value);
  }
  virtual void LinkLocalVar(FrontendIdTypePair var, ValueId value) override
  {
    localMapping.LinkToValueId(var.id, value);
  }

  IValueContainer* vc;
  IValueContainer& GetVC() { return *vc; }

  vector<string> stackMemory;
  //vector<string> heapMemory;
  //vector<StackFrame> stack;

  //ValueId stackBaseAddr;
  //ValueId heapBaseAddr;
  ValueId stackCurrentAddr;
  //ValueId heapCurrentAddr;

  std::map<ValueId, ValueId> hasValue;

  void Store(ValueId what, ValueId where)
  {
    hasValue[where] = what;
  }
  ValueId Load(ValueId where)
  {
    return hasValue.at(where);
  }


};

class AnalysisErrorException : public std::logic_error {
public:
  /*ctr*/ AnalysisErrorException()
    : logic_error("A fatal error was discovered by the analysis. Abstract execution can not continue in this path.")
  {
  }
  /*ctr*/ AnalysisErrorException(const char* c)
    : logic_error(c)
  {
  }

};

class InvalidDereferenceException : public AnalysisErrorException {
public:
  /*ctr*/ InvalidDereferenceException()
    : AnalysisErrorException("Program tried to dereference unallocated or uninitialized memory.")
  {
  }
};

class PossibleNullDereferenceException : public AnalysisErrorException {
public:
  /*ctr*/ PossibleNullDereferenceException()
    : AnalysisErrorException("Possible null dereference exception occured - getElementPtr.")
  {
  }
};



/// <summary>
/// Base class for implementing Operation. 
/// </summary>
////template<bool isBranching>
class BaseOperation : public IOperation {
public:

  virtual uptr<IState> CreateSuccessor     (IState& initialState) = 0;
          uptr<IState> CreateSuccessorTrue (IState& initialState) { return CreateSuccessor(initialState); }
  virtual uptr<IState> CreateSuccessorFalse(IState& initialState) { throw NotImplementedException(); }

  virtual void         ExecuteOnNewState  (IState& newState, const OperationArgs& args) = 0;

  // Override this again to switch to branching implementation
  virtual void Execute(IState& originalState, const OperationArgs& args) override
  {
    ExecuteImplNonbranching(originalState, args);
  }

private:

  // non-branching variant
  ////template <bool T = isBranching, typename std::enable_if_t<!T>* = nullptr>
  void ExecuteImplNonbranching(IState& originalState, const OperationArgs& args)
  {
    assert(!originalState.nextCfgNode.HasTwoNext()); //TODO: comment

    uptr<IState> successor;
    successor = CreateSuccessor(originalState);    
    try
    {
      //TODO: moved into the try/catch/finally block, is it correct?
      originalState.SetExplored();
      ExecuteOnNewState(*successor, args);

      // Handled in TerminalCfgNode::Execute()
      /*if (originalState.nextCfgNode.IsTerminalNode())
        successor->SetExplored();*/

      originalState.nextCfgNode.GetStatesManager().InsertAndEnqueue(move(successor));
    }
    catch (AnalysisErrorException e)
    {
      originalState.nextCfgNode.PrintLocation();
      printf("%s\n",e.what());
    }
    
    return;
  }

  // branching variant
  ////template <bool T = isBranching, typename std::enable_if_t<T>* = nullptr>
  void ExecuteImplBranching(IState& originalState, const OperationArgs& args)
  {
    assert(originalState.nextCfgNode.HasTwoNext()); //TODO: comment

    uptr<IState> successor;
    successor = CreateSuccessorTrue(originalState);
    ExecuteOnNewState(*successor, args);
    originalState.nextCfgNode.GetStatesManager().InsertAndEnqueue(move(successor));

    successor = CreateSuccessorFalse(originalState);
    ExecuteOnNewState(*successor, args);
    originalState.nextCfgNode.GetStatesManager().InsertAndEnqueue(move(successor));

    originalState.SetExplored();
    return;
  }

};

#define PTR_TYPE Type::CreateCharPointerType()

////template<bool isBranching>
class BaseForwardNullAnalysisOperation : public BaseOperation/*<isBranching>*/ {
public:
  
  virtual uptr<IState> CreateSuccessor(IState& s) override
  {
    auto successorPtr = make_unique<ForwardNullAnalysisState>(dynamic_cast<ForwardNullAnalysisState&>(s), s.nextCfgNode, s.nextCfgNode.GetNext());
    auto& successor = *successorPtr; 
    // FIXME: when compiler is in compliance with newer copy elision rules
    // http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#1579
    return move(successorPtr);
  }
  virtual uptr<IState> CreateSuccessorFalse(IState& s) override
  {
    auto successorPtr = make_unique<ForwardNullAnalysisState>(dynamic_cast<ForwardNullAnalysisState&>(s), s.nextCfgNode, s.nextCfgNode.GetNextFalse());
    auto& successor = *successorPtr; 
    
    // FIXME: when compiler is in compliance with newer copy elision rules
    // http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#1579
    return move(successorPtr);
  }
  virtual void ExecuteOnNewState(IState& newState, const OperationArgs& args) override
  {
    return ExecuteOnNewState(static_cast<ForwardNullAnalysisState&>(newState), args);
  }
  virtual void ExecuteOnNewState(ForwardNullAnalysisState& newState, const OperationArgs& args) = 0;
};


class FnaOperationBranch : public IOperation {
public:

  // Override this again to switch to branching implementation
  virtual void Execute(IState& originalState, const OperationArgs& args) override
  {
    assert(originalState.nextCfgNode.HasTwoNext()); //TODO: comment

    uptr<IState> successor;
    successor = CreateSuccessorTrue(originalState);
    ExecuteOnNewState(static_cast<ForwardNullAnalysisState&>(*successor), args, true);
    originalState.nextCfgNode.GetStatesManager().InsertAndEnqueue(move(successor));

    successor = CreateSuccessorFalse(originalState);
    ExecuteOnNewState(static_cast<ForwardNullAnalysisState&>(*successor), args, false);
    originalState.nextCfgNode.GetStatesManager().InsertAndEnqueue(move(successor));

    originalState.SetExplored();
    return;
  }

  uptr<IState> CreateSuccessorTrue(IState& s)
  {
    auto successorPtr = make_unique<ForwardNullAnalysisState>(dynamic_cast<ForwardNullAnalysisState&>(s), s.nextCfgNode, s.nextCfgNode.GetNextTrue());
    auto& successor = *successorPtr; 
    // FIXME: when compiler is in compliance with newer copy elision rules
    // http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#1579
    return move(successorPtr);
  }

  uptr<IState> CreateSuccessorFalse(IState& s)
  {
    auto successorPtr = make_unique<ForwardNullAnalysisState>(dynamic_cast<ForwardNullAnalysisState&>(s), s.nextCfgNode, s.nextCfgNode.GetNextFalse());
    auto& successor = *successorPtr; 
    // FIXME: when compiler is in compliance with newer copy elision rules
    // http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#1579
    return move(successorPtr);
  }

  void ExecuteOnNewState(ForwardNullAnalysisState& newState, const OperationArgs& args, bool br)
  {
    auto     lhs          = newState.GetAnyVar   (args.GetOperand(0));

    if (br)
      newState.GetVC().AssumeTrue(lhs);
    else
      newState.GetVC().AssumeFalse(lhs);
  }
};


class FnaOperationBinary : public BaseForwardNullAnalysisOperation {
  virtual void ExecuteOnNewState(ForwardNullAnalysisState& newState, const OperationArgs& args)
  {
    return ExecuteOnNewStateImpl(newState, static_cast<const BinaryOpArgs&>(args));
  }
  void ExecuteOnNewStateImpl(ForwardNullAnalysisState& newState, const BinaryOpArgs& args)
  {
    auto opts = args.GetOptions();

    auto lhs  = newState.GetAnyVar(args.GetOperand(0));
    auto rhs  = newState.GetAnyVar(args.GetOperand(1));
    auto type = args.GetOperand(0).type;
    
    ValueId retVal = newState.GetVC().BinOp(lhs, rhs, type, opts);
    newState.LinkLocalVar(args.GetTarget(), retVal);
  }

};

class FnaOperationGetElementPtr : public BaseForwardNullAnalysisOperation {
  virtual void ExecuteOnNewState(ForwardNullAnalysisState& newState, const OperationArgs& args) override
  {
    // consider packing and aligment!!!

    //auto numOfIndexes = args.size() - 2;

    //auto lvl0Size = args.GetOperand(0).type.GetPointerElementType().GetSizeOf();

    //if (args.GetOperand(0).type.IsStruct())
    //{
    //  auto lvl1Size = args.GetOperand(0).type.GetPointerElementType().GetStructElementOffset(/*and here index*/);
    //}
    
    auto     lhs    = newState.GetAnyVar   (args.GetOperand(0));
    uint64_t offset = static_cast<uint64_t>(args.GetOptions().idTypePair.id); //HACK relaying on ValueId == constant value stored by that id    

    //! We assume, that getelementptr instruction is always generated as forerunner of load/store op.
    if (newState.GetVC().IsZero(lhs))
    {
      throw PossibleNullDereferenceException();
    }
    
    ValueId offsetVal     = newState.GetVC().CreateConstIntVal(offset, PTR_TYPE);
    ValueId retVal        = newState.GetVC().Add(lhs, offsetVal, PTR_TYPE, ArithFlags::Default);
    newState.LinkLocalVar(args.GetTarget(), retVal);
  }
};

class FnaOperationAlloca : public BaseForwardNullAnalysisOperation {
  virtual void ExecuteOnNewState(ForwardNullAnalysisState& newState, const OperationArgs& args) override
  {
    auto count = newState.GetAnyVar(args.GetOperand(0));
    auto type  = args.GetTarget().type;

    ValueId elementSize64 = newState.GetVC().CreateConstIntVal(type.GetPointerElementType().GetSizeOf(), PTR_TYPE);
    ValueId count64       = newState.GetVC().ExtendInt(count, args.GetOperand(0).type, PTR_TYPE, ArithFlags::Default);
    ValueId size64        = newState.GetVC().Mul(elementSize64, count64, PTR_TYPE, ArithFlags::Default);
    ValueId retVal        = newState.GetVC().Add(newState.stackCurrentAddr, size64, PTR_TYPE, ArithFlags::Default);

    newState.stackCurrentAddr = retVal;
    newState.LinkLocalVar(args.GetTarget(), retVal);
    newState.stackMemory.push_back("alokovano xy");
  }
};

class FnaOperationCall : public IOperation {
public:

  virtual void Execute(IState& originalState, const OperationArgs& args) override
  {
    Execute(originalState, static_cast<const CallOpArgs&>(args));
  }

private:

  void Execute(IState& originalState, const CallOpArgs& args)
  {
    assert(!originalState.nextCfgNode.HasTwoNext()); //TODO: comment

    uptr<IState> successor;
    {
      auto& typedS = dynamic_cast<ForwardNullAnalysisState&>(originalState);
      auto& nextJump = typedS.funcMapping.GetFunction(typedS.GetAnyVar(args.GetOptions())).cfg;
      successor = make_unique<ForwardNullAnalysisState>(typedS, typedS.nextCfgNode, nextJump);
    }
    try
    {
      //TODO: moved into the try/catch/finally block, is it correct?
      originalState.SetExplored();
      ExecuteOnNewState(dynamic_cast<ForwardNullAnalysisState&>(*successor), args);

      // Handled in TerminalCfgNode::Execute()
      /*if (originalState.nextCfgNode.IsTerminalNode())
      successor->SetExplored();*/

      originalState.nextCfgNode.GetStatesManager().InsertAndEnqueue(move(successor));
    }
    catch (AnalysisErrorException e)
    {
      originalState.nextCfgNode.PrintLocation();
      printf("%s\n",e.what());
    }

    return;
  }
  void ExecuteOnNewState(ForwardNullAnalysisState& newState, const CallOpArgs& args)
  {
    auto callTargetId = args.GetOptions().id;
    auto callTargetType = args.GetOptions().type;

    // ne need to do several thinks in here
    // A) prepare new stack frame 
    // B) bind argument values to formal parameters in called function
    // C) prepare "return info" which will bind the returned value to FrontendId of returned value in caller
    // D) plan execution of the first node in the function with state prepared according to previous points

    // for that, we need:
    // support for stack frames with "return info"
    // a system which binds FunctionPointers (of all kinds) to functions
    
    // for variable addressing of function, I propose the same idea I originaly had for stack:
    // that is, "base address", here base address of function, beeing in unknown (abstract) value
    // and other addresses somehow based on this address
    
    auto& func = newState.funcMapping.GetFunction(newState.GetAnyVar(args.GetOptions()));

    int i = 0;
    for (auto& param : func.params.GetArgs())
    {
      newState.LinkLocalVar(param.idTypePair, newState.GetAnyVar(args.GetOperand(i)));
      i++;
    }

    return;
    throw NotImplementedException();
  }
};

class FnaOperationRet : public IOperation {
  virtual void Execute(IState & originalState, const OperationArgs & args) override 
  {
    (void)args;
    return;
  }
};

class FnaOperationLoad : public BaseForwardNullAnalysisOperation {
  virtual void ExecuteOnNewState(ForwardNullAnalysisState& newState, const OperationArgs& args) override
  {

    // this operation should somehow load a value from memory to register
    // as registers are "direct values",
    // it in fact means just to bind an existing value to another FrontendValueId
    // which value is to be loaded is but entirely up to the specific analysis
    
    auto target = newState.GetAnyVar(args.GetOperand(0));
    ValueId value;
    try
    {
      value = newState.Load(target);
    }
    catch(out_of_range e)
    {
      //! We assume an invalid read if the address was not written to previously
      throw InvalidDereferenceException();
    }

    newState.LinkLocalVar(args.GetTarget(), value);
  }
};

class FnaOperationStore : public BaseForwardNullAnalysisOperation {
  virtual void ExecuteOnNewState(ForwardNullAnalysisState& newState, const OperationArgs& args) override
  {

    // this operation should somehow Store a value in register to certain address in memory
    // the way for the operation to handle such a "write" is completely analysis specific
    
    auto value  = newState.GetAnyVar(args.GetOperand(0));
    auto target = newState.GetAnyVar(args.GetOperand(1));

    newState.Store(value, target);
  }
};

class FnaOperationMemset : public BaseForwardNullAnalysisOperation {
  virtual void ExecuteOnNewState(ForwardNullAnalysisState& newState, const OperationArgs& args) override
  {

    // this operation should somehow Store a value in register to certain address in memory
    // the way for the operation to handle such a "write" is completely analysis specific
    
    auto target = newState.GetAnyVar(args.GetOperand(0));
    auto value  = newState.GetAnyVar(args.GetOperand(1));
    auto len    = newState.GetAnyVar(args.GetOperand(2));

    auto& vc = newState.GetVC();
    auto i = vc.CreateConstIntVal(0, PTR_TYPE);
    auto one = vc.CreateConstIntVal(1, PTR_TYPE);
    while (vc.IsCmp(len, i, PTR_TYPE, CmpFlags::UnsigGt))
    {
      newState.Store(value, target); 
      i      = vc.Add(i, one, PTR_TYPE, ArithFlags::Default);      
      target = vc.Add(target, one, PTR_TYPE, ArithFlags::Default);
    }
  }
};

class FnaOperationTrunc : public BaseForwardNullAnalysisOperation {
  virtual void ExecuteOnNewState(ForwardNullAnalysisState& newState, const OperationArgs& args)
  {
    return ExecuteOnNewStateImpl(newState, static_cast<const TruncExtendOpArgs&>(args));
  }
  void ExecuteOnNewStateImpl(ForwardNullAnalysisState& newState, const TruncExtendOpArgs& args)
  {
    //newstate
    ArithFlags flags = args.GetOptions();
    auto lhs         = newState.GetAnyVar(args.GetOperand(0));
    auto tarType     = args.GetTarget().type;
    auto srcType     = args.GetOperand(0).type;
    
    ValueId retVal = newState.GetVC().TruncateInt(lhs, srcType, tarType);
    newState.LinkLocalVar(args.GetTarget(), retVal);
  }
};

class FnaOperationExtend : public BaseForwardNullAnalysisOperation {
  virtual void ExecuteOnNewState(ForwardNullAnalysisState& newState, const OperationArgs& args)
  {
    return ExecuteOnNewStateImpl(newState, static_cast<const TruncExtendOpArgs&>(args));
  }
  void ExecuteOnNewStateImpl(ForwardNullAnalysisState& newState, const TruncExtendOpArgs& args)
  {
    //newstate
    ArithFlags flags = args.GetOptions();
    auto lhs         = newState.GetAnyVar(args.GetOperand(0));
    auto tarType     = args.GetTarget().type;
    auto srcType     = args.GetOperand(0).type;
    
    ValueId retVal = newState.GetVC().TruncateInt(lhs, srcType, tarType);
    newState.LinkLocalVar(args.GetTarget(), retVal);
  }
};

class FnaOperationCast : public BaseForwardNullAnalysisOperation {
  virtual void ExecuteOnNewState(ForwardNullAnalysisState& newState, const OperationArgs& args)
  {
    return ExecuteOnNewStateImpl(newState, static_cast<const CastOpArgs&>(args));
  }
  void ExecuteOnNewStateImpl(ForwardNullAnalysisState& newState, const CastOpArgs& args)
  {
    auto lhs           = newState.GetAnyVar(args.GetOperand(0));
    auto opts          = args.GetOptions();
    //ArithFlags flags = static_cast<ArithFlags>(static_cast<uint64_t>(args[1].id) & 0xffff);
    //auto tarType     = args.GetTarget().type;
    //auto srcType     = args.GetOperand(0).type;
    
    if (opts.opKind == CastOpKind::BitCast)
      newState.LinkLocalVar(args.GetTarget(), lhs);
    else if(opts.opKind == CastOpKind::Extend)      
      newState.LinkLocalVar(args.GetTarget(), lhs); //TODO: hack!
    else
      throw NotImplementedException();
  }
};

class FnaOperationCmp : public BaseForwardNullAnalysisOperation {
  virtual void ExecuteOnNewState(ForwardNullAnalysisState& newState, const OperationArgs& args)
  {
    return ExecuteOnNewStateImpl(newState, static_cast<const CmpOpArgs&>(args));
  }
  void ExecuteOnNewStateImpl(ForwardNullAnalysisState& newState, const CmpOpArgs& args)
  {
    //newstate
    auto lhs         = newState.GetAnyVar(args.GetOperand(0));
    auto rhs         = newState.GetAnyVar(args.GetOperand(1));
    auto srcType     = args.GetOperand(0).type;
    auto flags       = args.GetOptions();
    
    ValueId retVal = newState.GetVC().Cmp(lhs, rhs, srcType, flags);
    newState.LinkLocalVar(args.GetTarget(), retVal);
  }
};

class FnaOperationNoop : public BaseForwardNullAnalysisOperation {
  virtual void ExecuteOnNewState(ForwardNullAnalysisState& newState, const OperationArgs& args)
  {
    (void)args;
    return;
  }
};


class FnaOperationFactory : public IOperationFactory {

  IOperation* noop = new FnaOperationNoop();
  IOperation* notSupported = new OperationNotSupportedOperation();
  IOperation* binop = new FnaOperationBinary();
  IOperation* gep = new FnaOperationGetElementPtr();
  IOperation* allocaop = new FnaOperationAlloca();
  IOperation* callop = new FnaOperationCall();
  IOperation* trunc = new FnaOperationTrunc();
  IOperation* extend = new FnaOperationExtend();
  IOperation* load = new FnaOperationLoad();
  IOperation* store = new FnaOperationStore();
  IOperation* cast = new FnaOperationCast();
  IOperation* memset = new FnaOperationMemset();
  IOperation* cmp = new FnaOperationCmp();
  IOperation* br = new FnaOperationBranch();
  IOperation* ret = new FnaOperationRet();
public:
  /*ctr*/ FnaOperationFactory()
  {
  }
  // Inherited via IOperationFactory
  virtual IOperation & Ret() override { return *ret; }
  virtual IOperation & Br()  override { return *br; }

  virtual IOperation & BinOp() override { return *binop; }

  virtual IOperation & Alloca() override { return *allocaop; }
  virtual IOperation & Load() override { return *load; }
  virtual IOperation & Store() override { return *store; }
  virtual IOperation & Call() override { return *callop; }
  virtual IOperation & Invoke() override { return *notSupported; }
  virtual IOperation & Cast() override { return *cast; }

  virtual IOperation & GetElementPtr() override { return *gep; }

  virtual IOperation & Cmp() override { return *cmp; }

  virtual IOperation& Memset() override { return *memset; }
  virtual IOperation& Memcpy() override { return *notSupported; }
  virtual IOperation& Memmove() override { return *notSupported; }
  virtual IOperation& Malloc() override { return *notSupported; }
  virtual IOperation& Free() override { return *notSupported; }

  virtual IOperation & NotSupportedInstr() override { return *notSupported; }
  virtual IOperation & Noop() override { return *noop; }
};
