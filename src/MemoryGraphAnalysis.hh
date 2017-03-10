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

#include <cassert>

#include "FrontedValueMapper.hh"
#include "Operation.hh"

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
      nextCfgNode)
  {
  }

  //------------------------------------


  void Store(ValueId what, ValueId where)
  {
  }

  ValueId Load(ValueId where)
  {
    return ValueId{};
  }

  void Alloca()
  {
  }

  void Malloc()
  {
  }

  void Calloc()
  {
  }

  void Realloc()
  {
  }

};

class MemGraphOpNoop : public BasicOperation<MemoryGraphAnalysisState, OperationArgs> {
  virtual void ExecuteOnNewState(MemoryGraphAnalysisState& newState, const OperationArgs& args) override
  {
  }
};

class MemGraphOpLoad : public BasicOperation<MemoryGraphAnalysisState> {
  virtual void ExecuteOnNewState(MemoryGraphAnalysisState& newState, const OperationArgs& args) override
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

class MemGraphOpStore : public BasicOperation<MemoryGraphAnalysisState> {
  virtual void ExecuteOnNewState(MemoryGraphAnalysisState& newState, const OperationArgs& args) override
  {

    // this operation should somehow Store a value in register to certain address in memory
    // the way for the operation to handle such a "write" is completely analysis specific

    auto value  = newState.GetAnyVar(args.GetOperand(0));
    auto target = newState.GetAnyVar(args.GetOperand(1));

    newState.Store(value, target);
  }
};

// ================== ^^^^^ COMMON CODE ^^^^^ ===============

class MemGraphOpAlloca : public BasicOperation<MemoryGraphAnalysisState> {
  virtual void ExecuteOnNewState(MemoryGraphAnalysisState& newState, const OperationArgs& args) override
  {
    auto count = newState.GetAnyVar(args.GetOperand(0));
    auto type  = args.GetTarget().type;

    ValueId elementSize64 = newState.GetVC().CreateConstIntVal(type.GetPointerElementType().GetSizeOf(), PTR_TYPE);
    ValueId count64       = newState.GetVC().ExtendInt(count, args.GetOperand(0).type, PTR_TYPE, ArithFlags::Default);
    ValueId size64        = newState.GetVC().Mul(elementSize64, count64, PTR_TYPE, ArithFlags::Default);
    ValueId retVal        = newState.GetVC().CreateVal(PTR_TYPE);
    newState.LinkLocalVar(args.GetTarget(), retVal);
  }
};

class MemGraphOpFactory : public IOperationFactory {
private:

  IOperation* nsop     = new OperationNotSupportedOperation();
  IOperation* noop     = new MemGraphOpNoop();
  IOperation* load     = new MemGraphOpLoad();
  IOperation* store    = new MemGraphOpStore();
  IOperation* memset   = new MemGraphOpAlloca();
  IOperation* binop    = new MemGraphOpNoop();
  IOperation* gep      = new MemGraphOpNoop();
  IOperation* allocaop = new MemGraphOpNoop();
  IOperation* callop   = new MemGraphOpNoop();
  IOperation* trunc    = new MemGraphOpNoop();
  IOperation* extend   = new MemGraphOpNoop();
  IOperation* cast     = new MemGraphOpNoop();
  IOperation* cmp      = new MemGraphOpNoop();
  IOperation* br       = new MemGraphOpNoop();
  IOperation* ret      = new MemGraphOpNoop();

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
