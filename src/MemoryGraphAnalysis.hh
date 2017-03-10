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
/** @file MemoryGraphAnalysis.hh */


#pragma once

#include "Definitions.hh"
#include "General.hh"
#include "IOperation.hh"
#include "ICfgNode.hh"
#include "IState.hh"
#include "StateStorage.hh"

#include "FrontedValueMapper.hh"
#include "Operation.hh"

#include "Smg.cc"

class MemoryGraphAnalysisState : public StateBase {
public:

  /*ctr*/ MemoryGraphAnalysisState(
    ICfgNode& nextCfgNode,
    IValueContainer& vc,
    Mapper& globalMapping,
    FuncMapper& funcMapping
  ) :
    StateBase(
      //lastCfgNode, 
      nextCfgNode, 
      vc, 
      globalMapping,
      funcMapping)
  {
  }

  // copy ctr ??? or what
  /*ctr*/ MemoryGraphAnalysisState(
    const MemoryGraphAnalysisState& state, 
    ICfgNode& nextCfgNode
  ) :
    StateBase(
      state, 
      nextCfgNode),
    graph(state.graph)
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
    graph.WriteValue(where, what, ofType);
  }

  ValueId Load(ValueId where)
  {
    try
    {
      // Something
      return ValueId{};
    }
    catch(out_of_range e)
    {
      //! We assume an invalid read if the address was not written to previously
      throw InvalidDereferenceException();
    }
  }

  ValueId Alloca(Type type)
  {
    return graph.AllocateObject(type);
  }

  ValueId Malloc()
  {
    return ValueId{};
  }

  ValueId Calloc()
  {
    return ValueId{};
  }

  ValueId Realloc()
  {
    return ValueId{};
  }

};

class MemGraphOpLoad : public BasicOperation<MemoryGraphAnalysisState> {
  virtual void ExecuteOnNewState(MemoryGraphAnalysisState& newState, const OperationArgs& args) override final
  {

    // this operation should somehow load a value from memory to register
    // as registers are "direct values",
    // it in fact means just to bind an existing value to another FrontendValueId
    // which value is to be loaded is but entirely up to the specific analysis

    auto target = newState.GetAnyVar(args.GetOperand(0));
    ValueId value = newState.Load(target);

    newState.LinkLocalVar(args.GetTarget(), value);
  }
};

class MemGraphOpStore : public BasicOperation<MemoryGraphAnalysisState> {
  virtual void ExecuteOnNewState(MemoryGraphAnalysisState& newState, const OperationArgs& args) override final
  {

    // this operation should somehow Store a value in register to certain address in memory
    // the way for the operation to handle such a "write" is completely analysis specific
    auto value = args.GetOperand(0);

    auto valueId  = newState.GetAnyVar(value);
    auto target = newState.GetAnyVar(args.GetOperand(1));

    newState.Store(target, valueId, value.type);
  }
};

// ================== \/ \/ \/ COMMON CODE \/ \/ \/ ===============

// ================== ^^^^^ COMMON CODE ^^^^^ ===============

class MemGraphOpAlloca : public BasicOperation<MemoryGraphAnalysisState> {
  virtual void ExecuteOnNewState(MemoryGraphAnalysisState& newState, const OperationArgs& args) override final
  {
    auto count = newState.GetAnyVar(args.GetOperand(0));
    auto type  = args.GetTarget().type;

    ValueId elementSize64 = newState.GetVc().CreateConstIntVal(type.GetPointerElementType().GetSizeOf(), PTR_TYPE);
    ValueId count64       = newState.GetVc().ExtendInt(count, args.GetOperand(0).type, PTR_TYPE, ArithFlags::Default);
    ValueId size64        = newState.GetVc().Mul(elementSize64, count64, PTR_TYPE, ArithFlags::Default);
    ValueId retVal        = newState.Alloca(type); //! Not enough!
    //TODO: count is unhandled... probably solve this by converting type to array<type, count>

    newState.LinkLocalVar(args.GetTarget(), retVal);
  }
};

class MemGraphOpFactory : public IOperationFactory {
private:

  IOperation* nsop     = new OperationNotSupportedOperation();
  IOperation* noop     = new BasicOperationNoop();
  IOperation* load     = new MemGraphOpLoad();
  IOperation* store    = new MemGraphOpStore();
  IOperation* allocaop = new MemGraphOpAlloca();
  IOperation* binop    = new BasicOperationBinOp();
  IOperation* trunc    = new BasicOperationTruncate();
  IOperation* extend   = new BasicOperationExtend();
  IOperation* cmp      = new BasicOperationCmp();

  IOperation* memset   = new BasicOperationNoop();
  IOperation* gep      = new BasicOperationNoop();
  IOperation* callop   = new BasicOperationNoop();
  IOperation* cast     = new BasicOperationNoop();
  IOperation* br       = new BasicOperationNoop();
  IOperation* ret      = new BasicOperationNoop();

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
  virtual IOperation& Malloc() override { return *nsop; }
  virtual IOperation& Free() override { return *nsop; }

  virtual IOperation & NotSupportedInstr() override { return *nsop; }
  virtual IOperation & Noop() override { return *noop; }
};
