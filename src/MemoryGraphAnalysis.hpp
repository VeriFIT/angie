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
/** @file MemoryGraphAnalysis.hpp */


#pragma once

#include "Definitions.hpp"
#include "General.hpp"
#include "IOperation.hpp"
#include "ICfgNode.hpp"
#include "IState.hpp"
#include "StateStorage.hpp"

#include "FrontedValueMapper.hpp"
#include "Operation.hpp"

#include "Smg.hpp"

class MemoryGraphAnalysisState : public State {
public:

  /*ctr*/ MemoryGraphAnalysisState(
    ICfgNode& node,
    IValueContainer& vc,
    Mapper& globalMapping,
    FuncMapper& funcMapping
  ) :
    State(
      //lastCfgNode, 
      node, 
      vc, 
      globalMapping,
      funcMapping),
    graph{vc}
  {
  }

  // copy ctr ??? or what
  /*ctr*/ MemoryGraphAnalysisState(
    const MemoryGraphAnalysisState& state, 
    ICfgNode& node
  ) :
    State(
      state, 
      node),
    graph(state.graph),
    stack(state.stack)
  {
  }

  virtual uptr<IState> CreateSuccessor(ICfgNode& nextStep) const override
  {
    return std::make_unique<MemoryGraphAnalysisState>(*this, nextStep);
  }

private:
  Smg::Impl::Graph graph;
  

  //------------------------------------
public:

  void Store(ValueId where, ValueId what, Type ofType)
  {
    graph.WriteValue(where, what, ofType, GetVc());
  }

  ValueId Load(ValueId ptr, Type ptrType, Type tarType)
  {
    try
    {
      return graph.ReadValue(ptr, ptrType, tarType, GetVc());
    }
    catch(std::out_of_range e)
    {
      //! We assume an invalid read if the address was not written to previously
      throw InvalidDereferenceException();
    }
    catch(InvalidDereferenceException_smg e)
    {
      //! We assume an invalid read if the address was not written to previously
      throw InvalidDereferenceException();
    }
  }

  ValueId Alloca(Type type, ValueId count)
  {
    auto& vc = GetVc();
    if (vc.GetAbstractionStatus(count) != AbstractionStatus::Constant)
    {
      throw NotSupportedException("Alloca with abstract count is not supported");
    }
    if (vc.GetConstantIntInnerVal(count) != 1)
    {
      throw NotSupportedException("Alloca with count != 1 is not supported");
    }
    return graph.AllocateRegion(type, GetVc());
  }

  ValueId Malloc(ValueId size)
  {
    auto& vc = GetVc();
    if (vc.GetAbstractionStatus(size) != AbstractionStatus::Constant)
    {
      throw NotSupportedException("Malloc with abstract size is not supported");
    }
    auto byteArrayT = Type::CreateArrayOf(INT8_TYPE, vc.GetConstantIntInnerVal(size));
    return graph.AllocateRegion(byteArrayT, GetVc());
  }

  ValueId Calloc(ValueId size)
  {
    auto mem = Malloc(size);
    Memclear(mem, size);
    return mem;
  }

  ValueId Realloc()
  {
    throw NotSupportedException("Realloc is not supported");
  }

  void Memset(ValueId target, ValueId value, ValueId size)
  {
    auto& vc = GetVc();
    if (vc.IsZero(value))
    {
      return Memclear(target, size);
    }
    if (vc.GetAbstractionStatus(size) != AbstractionStatus::Constant)
    {
      throw NotSupportedException("Memset of arbitraty value with abstract size is not supported");
    }

    // size is constant    
    throw NotSupportedException("Memset of arbitraty value is not supported");
  }

  void Memclear(ValueId target, ValueId size)
  {
    auto& vc = GetVc();
    if (vc.GetAbstractionStatus(size) != AbstractionStatus::Constant)
    {
      throw NotSupportedException("Memclear with abstract size is not supported");
    }
    auto byteArrayT = Type::CreateArrayOf(INT8_TYPE, vc.GetConstantIntInnerVal(size));
    auto& edge = graph.CreateDerivedPointer(target, vc.GetZero(PTR_TYPE), Type::CreatePointerTo(byteArrayT), GetVc()).second;
    graph.WriteValue(edge, vc.GetZero(INT8_TYPE), byteArrayT, GetVc());
  }

  ValueId CreateDerivedPointer(ValueId basePtr, ValueId offset, Type type)
  {
    return graph.CreateDerivedPointer(basePtr, offset, type, GetVc()).first;
  }

  std::vector<std::tuple<Mapper,FrontendValueId,ICfgNode&>> stack;
  ICfgNode* stackRetNode;

  virtual ICfgNode& GetStackRetNode() const override
  {
    return std::get<2>(stack.back());
  }
  virtual void      SetStackRetNode(ICfgNode& node) override 
  {
    stackRetNode = &node;
  }

  void StackPush(FrontendValueId retFrontendId)
  {
    //TODO: push mappins in graph
    stack.emplace_back(std::move(this->localMapping), retFrontendId, *stackRetNode);
    this->localMapping = decltype(this->localMapping){};
  }

  void StackPop(ValueId retValId)
  {
    //TODO: pop mappins in graph
    this->localMapping = std::get<0>(stack.back());
    this->localMapping.LinkToValueId(std::get<1>(stack.back()), retValId);
    stack.pop_back();
  }
  void StackPop()
  {
    //TODO: pop mappins in graph
    this->localMapping = std::get<0>(stack.back());
    stack.pop_back();
  }

};

class MemGraphOpLoad : public BasicOperation<MemoryGraphAnalysisState> {
  virtual void ExecuteOnNewState(MemoryGraphAnalysisState& newState, const OperationArgs& args) override final
  {

    // this operation should somehow load a value from memory to register
    // as registers are "direct values",
    // it in fact means just to bind an existing value to another FrontendValueId
    // which value is to be loaded is but entirely up to the specific analysis

    auto ptr = args.GetOperand(0);
    auto reg = args.GetTarget();

    auto ptrId = newState.GetValue(ptr);
    ValueId value = newState.Load(ptrId, ptr.type, reg.type);

    newState.AssignValue(reg, value);
  }
};

class MemGraphOpStore : public BasicOperation<MemoryGraphAnalysisState> {
  virtual void ExecuteOnNewState(MemoryGraphAnalysisState& newState, const OperationArgs& args) override final
  {

    // this operation should somehow Store a value in register to certain address in memory
    // the way for the operation to handle such a "write" is completely analysis specific
    auto value = args.GetOperand(0);

    auto valueId  = newState.GetValue(value);
    auto target = newState.GetValue(args.GetOperand(1));

    newState.Store(target, valueId, value.type);
  }
};

class MemGraphOpAlloca : public BasicOperation<MemoryGraphAnalysisState> {
  virtual void ExecuteOnNewState(MemoryGraphAnalysisState& newState, const OperationArgs& args) override final
  {
    auto count = newState.GetValue(args.GetOperand(0));
    auto type  = args.GetTarget().type;

    ValueId count64       = newState.GetVc().ExtendInt(count, args.GetOperand(0).type, PTR_TYPE, ArithFlags::Default);
    ValueId retVal        = newState.Alloca(type.GetPointerElementType(), count64);

    newState.AssignValue(args.GetTarget(), retVal);
  }
};

class MemGraphOpMalloc : public BasicOperation<MemoryGraphAnalysisState> {
  virtual void ExecuteOnNewState(MemoryGraphAnalysisState& newState, const OperationArgs& args) override final
  {
    auto size = newState.GetValue(args.GetOperand(0));

    ValueId size64        = newState.GetVc().ExtendInt(size, args.GetOperand(0).type, PTR_TYPE, ArithFlags::Default);
    ValueId retVal        = newState.Malloc(size64);

    newState.AssignValue(args.GetTarget(), retVal);
  }
};

class MemGraphOpMemset : public BasicOperation<MemoryGraphAnalysisState> {
  virtual void ExecuteOnNewState(MemoryGraphAnalysisState& newState, const OperationArgs& args) override
  {
    // this operation should somehow Store a value in register to certain address in memory
    // the way for the operation to handle such a "write" is completely analysis specific

    auto target = newState.GetValue(args.GetOperand(0));
    auto value  = newState.GetValue(args.GetOperand(1));
    auto len    = newState.GetValue(args.GetOperand(2));

    newState.Memset(target, value, len);
  }
};

// ================== \/ \/ \/ COMMON CODE \/ \/ \/ ===============

// ================== ^^^^^ COMMON CODE ^^^^^ ===============

class MemGraphOpGetElementPtr : public BasicOperation<MemoryGraphAnalysisState> {
  virtual void ExecuteOnNewState(MemoryGraphAnalysisState& newState, const OperationArgs& args) override
  {
    // consider packing and aligment!!!
    auto& vc = newState.GetVc();
    ValueId offsetVal;

    auto source = args.GetOperand(0);
    auto sourceId = newState.GetValue(source);
    auto elementType = source.type.GetPointerElementType();

    // opt
    //! We assume, that getelementptr instruction is always generated as forerunner of load/store op.
    if (vc.IsZero(sourceId))
    {
      throw PossibleNullDereferenceException();
    }

    // possible opt
    //HACK relaying on ValueId == constant value stored by that id    
    ////offset = static_cast<uint64_t>(args.GetOptions().idTypePair.id); 
    
    auto numOfIndexes = args.GetOperandCount() - 1;
    // lvl0: array/pointer athimetic indexation
    auto fIdx0 = args.GetOperand(1);
    auto vIdx0 = newState.GetValue(fIdx0);

    // Do the math
    auto vL0Size = elementType.GetSizeOfV(vc);
    offsetVal = vc.Mul(vIdx0, vL0Size, PTR_TYPE, ArithFlags::Default);

    if (numOfIndexes > 1)
    {
      // lvl1 indexing should be only possible on structs right now
      assert(elementType.IsStruct());
      auto fIdx1 = args.GetOperand(2);
      auto vIdx1 = newState.GetValue(fIdx1);
      // structure field index must be a constant
      assert(vc.GetAbstractionStatus(vIdx1) == AbstractionStatus::Constant);
      auto fieldIndex = vc.GetConstantIntInnerVal(vIdx1);

      // Do the math
      auto vL1Offset = elementType.GetStructElementOffsetV(fieldIndex, vc);
      offsetVal = vc.Add(offsetVal, vL1Offset, PTR_TYPE, ArithFlags::Default);

      if (numOfIndexes > 2)
      {
        throw NotSupportedException("Only 2 levels (L0 and L1) of indexation via GEP are supported.");
      }
    }

    ValueId retVal = newState.CreateDerivedPointer(sourceId, offsetVal, args.GetTarget().type);

    newState.AssignValue(args.GetTarget(), retVal);
  }
};

class MemGraphOpCast : public BasicOperation<MemoryGraphAnalysisState, CastOpArgs> {
  virtual void ExecuteOnNewState(MemoryGraphAnalysisState& newState, const CastOpArgs& args) override  
  {
    auto lhs           = newState.GetValue(args.GetOperand(0));
    auto opts          = args.GetOptions();
    //ArithFlags flags = static_cast<ArithFlags>(static_cast<uint64_t>(args[1].id) & 0xffff);
    auto tarType     = args.GetTarget().type;
    auto srcType     = args.GetOperand(0).type;

    if (opts.opKind == CastOpKind::BitCast)
    {
      newState.AssignValue(args.GetTarget(), lhs);
      newState.CreateDerivedPointer(lhs, newState.GetVc().GetZero(PTR_TYPE), tarType);
    }
    else if(opts.opKind == CastOpKind::Extend)      
      newState.AssignValue(args.GetTarget(), lhs); //TODO: hack!
    else if (opts.opKind == CastOpKind::Truncate)
    {
      auto trunc = newState.GetVc().TruncateInt(lhs, srcType, tarType);
      newState.AssignValue(args.GetTarget(), trunc); //TODO: hack!
    }
    else
      throw NotImplementedException();
  }
};


class FnaxOperationCall : public OperationCall<MemoryGraphAnalysisState> {
  virtual void ExecuteOnNewState(MemoryGraphAnalysisState& newState, const CallOpArgs& args) override
  {
    auto callTargetId = args.GetOptions().id;
    auto callTargetType = args.GetOptions().type;
    
    auto& func = newState.GetFuncMapping().GetFunction(newState.GetValue(args.GetOptions()));

    std::vector<ValueId> callArgs;
    int i = 0;
    for (auto& param : func.params.GetArgs())
    {      
      callArgs.push_back(newState.GetValue(args.GetOperand(i)));
      i++;
    } 
    newState.StackPush(args.GetTarget().id);
    i = 0;
    for (auto& param : func.params.GetArgs())
    {      
      newState.AssignValue(
        param.idTypePair, 
        callArgs[i]
        );
      i++;
    }
  }
};

class MemGraphOpRet : public OperationRet<MemoryGraphAnalysisState> {
  virtual void ExecuteOnNewState(MemoryGraphAnalysisState& newState, const OperationArgs& args) override
  {
    if (args.GetArgs().size() > 2)
    {
      auto retVal = newState.GetValue(args.GetOperand(0));
      newState.StackPop(retVal);
    }
    else
    {
      newState.StackPop();
    }
  }
};

class MemGraphOpFactory : public IOperationFactory {
private:

  IOperation* nsop     = new OperationNotSupportedOperation();
  IOperation* noop     = new BasicOperationNoop();
  IOperation* load     = new MemGraphOpLoad();
  IOperation* store    = new MemGraphOpStore();
  IOperation* binop    = new BasicOperationBinOp();
  IOperation* cmp      = new BasicOperationCmp();
  IOperation* trunc    = new BasicOperationTruncate();
  IOperation* extend   = new BasicOperationExtend();
  IOperation* malloc   = new MemGraphOpMalloc();
  IOperation* allocaop = new MemGraphOpAlloca();
  IOperation* memset   = new MemGraphOpMemset();

  IOperation* gep      = new MemGraphOpGetElementPtr();
  IOperation* cast     = new MemGraphOpCast();

  IOperation* callop   = new FnaxOperationCall();
  IOperation* br       = new BasicOperationBranch();
  IOperation* ret      = new MemGraphOpRet();

public:
  // Inherited via IOperationFactory
  virtual IOperation & Ret() override { return *ret; }
  virtual IOperation & Br()  override { return *br; }

  virtual IOperation & BinOp() override { return *binop; }

  virtual IOperation & Alloca() override { return *allocaop; }
  virtual IOperation & Load() override { return *load; }
  virtual IOperation & Store() override { return *store; }
  virtual IOperation & Call() override { return *callop; }
  virtual IOperation & Invoke() override { return *nsop; }
  virtual IOperation & Cast() override { return *cast; }

  virtual IOperation & GetElementPtr() override { return *gep; }

  virtual IOperation & Cmp() override { return *cmp; }

  virtual IOperation& Memset() override { return *memset; }
  virtual IOperation& Memcpy() override { return *nsop; }
  virtual IOperation& Memmove() override { return *nsop; }
  virtual IOperation& Malloc() override { return *malloc; }
  virtual IOperation& Free() override { return *nsop; }

  virtual IOperation & NotSupportedInstr() override { return *nsop; }
  virtual IOperation & Noop() override { return *noop; }
};
