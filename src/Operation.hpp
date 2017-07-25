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
/** @file Operation.hpp */

#pragma once

#include "Exceptions.hpp"
#include "Definitions.hpp"
#include "General.hpp"
#include "IOperation.hpp"
#include "IState.hpp"
#include "StateStorage.hpp"
#include "FrontedValueMapper.hpp"
#include "debugbreak.h"

//TODO: maybe remove use of IState& state from templated helpers, we can access that via successor
//TODO: maybe replace dynamic_cast with static_cast ?
//TODO: comment about executing last instruction -> it is OK now, TerminalNode implements no-op Execute()

template<typename StateT, typename OperArgsT, typename branching = std::false_type>
class OperationHelpers;

template<typename StateT, typename OperArgsT>
class OperationHelpers<StateT, OperArgsT, std::false_type> {
public:
  void SafelyExecuteAndEnque(IState& state, uptr<IState>&& successor, const OperationArgs& args)
  {
    //TODO@michkot: do not use exceptions to handle analysis warnings and errors
    // Also applies for the branching variant bellow 
    try
    {
      state.SetExplored();
      if (state.GetNode().HasBreakpoint())
        debug_break();
      ExecuteOnNewState(static_cast<StateT&>(*successor), static_cast<const OperArgsT&>(args));
      state.GetNode().GetStatesManager().InsertAndEnqueue(std::move(successor));
    }
    catch (BranchException e)
    {
    }
    catch (AnalysisErrorException e)
    {
      state.GetNode().PrintLocation();
      std::cout << e.what() << std::endl;
    }
  }
  virtual void ExecuteOnNewState(StateT& newState, const OperArgsT& args) = 0;
};

template<typename StateT, typename OperArgsT>
class OperationHelpers<StateT, OperArgsT, std::true_type> {
public:
  void SafelyExecuteAndEnque(IState& state, uptr<IState>&& successor, const OperationArgs& args, bool br)
  {
    try
    {
      state.SetExplored();
      if (state.GetNode().HasBreakpoint())
        debug_break();
      ExecuteOnNewState(static_cast<StateT&>(*successor), static_cast<const OperArgsT&>(args), br);
      state.GetNode().GetStatesManager().InsertAndEnqueue(std::move(successor));
    }
    catch (BranchException e)
    {
    }
    catch (AnalysisErrorException e)
    {
      state.GetNode().PrintLocation();
      std::cout << e.what() << std::endl;
    }
  }
  virtual void ExecuteOnNewState(StateT& newState, const OperArgsT& args, bool br) = 0;
};

/// <summary>
/// Base class for implementing most of Operation. 
/// Branching, Call and Ret operation has to be implemented in different way;
/// </summary>
template<typename StateT, typename OperArgsT = OperationArgs>
class BasicOperation : public IOperation, OperationHelpers<StateT, OperArgsT> {
public:
  virtual void Execute(IState& state, const OperationArgs& args) override final
  {
    // check whether this non-branching operation is placed in non-branching node
    assert(!state.GetNode().IsBranching()); 

    uptr<IState> successor = state.CreateSuccessor(state.GetNode().GetNext());
    this->SafelyExecuteAndEnque(state, std::move(successor), args);
  }
};

template<typename StateT, typename OperArgsT = OperationArgs>
class OperationRet : public IOperation, OperationHelpers<StateT, OperArgsT> {
  void Execute(IState& state, const OperArgsT& args)
  {
    // check whether this non-branching operation is placed in non-branching node
    assert(!state.GetNode().IsBranching());

    uptr<IState> successor = state.CreateSuccessor(state.GetStackRetNode());
    this->SafelyExecuteAndEnque(state, std::move(successor), args);
  }
};

template<typename StateT>
class OperationCall : public IOperation, OperationHelpers<StateT, CallOpArgs> {
public:

  virtual void Execute(IState& state, const OperationArgs& args) override
  {
    Execute(state, static_cast<const CallOpArgs&>(args));
  }

private:

  void Execute(IState& state, const CallOpArgs& args)
  {
    // check whether this non-branching operation is placed in non-branching node
    assert(!state.GetNode().IsBranching());

    uptr<IState> successor;
    {
      auto& nextJump = state.GetFuncMapping().GetFunction(state.GetValue(args.GetOptions())).cfg;
      successor = state.CreateSuccessor(nextJump);

      // Prepares the instruction-after-call to be pushed on the stack
      (*successor).SetStackRetNode(state.GetNode().GetNext());
    }
    this->SafelyExecuteAndEnque(state, std::move(successor), args);
  }
};

template<typename StateT, typename OperArgsT = OperationArgs>
class OperationBranch : public IOperation, OperationHelpers<StateT, OperArgsT, std::true_type> {
public:

  // Override this again to switch to branching implementation
  virtual void Execute(IState& state, const OperationArgs& args) override final
  {
    // check whether this branching operation is placed in branching node
    assert(state.GetNode().IsBranching()); 

    uptr<IState> successor;
    successor = state.CreateSuccessor(state.GetNode().GetNextFalse());
    this->SafelyExecuteAndEnque(state, std::move(successor), args, false);

    successor = state.CreateSuccessor(state.GetNode().GetNextTrue());
    this->SafelyExecuteAndEnque(state, std::move(successor), args, true);

    state.SetExplored();
    return;
  }
};

// ================== \/ \/ \/ COMMON CODE \/ \/ \/ ===============

#define INT8_TYPE Type::CreateIntegerType(8)
#define PTR_TYPE Type::CreateCharPointerType()

class BasicOperationNoop : public BasicOperation<IState, OperationArgs> {
  virtual void ExecuteOnNewState(IState& newState, const OperationArgs& args) override final
  {
  }
};

class BasicOperationAbort : public BasicOperation<IState, OperationArgs> {
  virtual void ExecuteOnNewState(IState& newState, const OperationArgs& args) override final
  {
    throw AnalysisErrorException("abort or exit called");
  }
};

class BasicOperationCreateUnknown : public BasicOperation<IState, OperationArgs> {
  virtual void ExecuteOnNewState(IState& newState, const OperationArgs& args) override final
  {
    ValueId retVal = newState.GetVc().CreateVal(args.GetTarget().type);
    newState.AssignValue(args.GetTarget(), retVal);
  }
};

class BasicOperationBinOp : public BasicOperation<IState, BinaryOpArgs> {
  virtual void ExecuteOnNewState(IState& newState, const BinaryOpArgs& args) override final
  {
    auto opts        = args.GetOptions();

    auto type        = args.GetOperand(0).type;
    auto lhs         = newState.GetValue(args.GetOperand(0));
    auto rhs         = newState.GetValue(args.GetOperand(1));

    ValueId retVal = newState.GetVc().BinOp(lhs, rhs, type, opts);
    newState.AssignValue(args.GetTarget(), retVal);
  }
};

class BasicOperationTruncate : public BasicOperation<IState, TruncExtendOpArgs> {
  virtual void ExecuteOnNewState(IState& newState, const TruncExtendOpArgs& args) override final
  {
    ArithFlags flags = args.GetOptions();
    auto lhs         = newState.GetValue(args.GetOperand(0));
    auto tarType     = args.GetTarget().type;
    auto srcType     = args.GetOperand(0).type;

    ValueId retVal = newState.GetVc().TruncateInt(lhs, srcType, tarType);
    newState.AssignValue(args.GetTarget(), retVal);
  }
};

class BasicOperationExtend : public BasicOperation<IState, TruncExtendOpArgs> {
  virtual void ExecuteOnNewState(IState& newState, const TruncExtendOpArgs& args) override final
  {
    ArithFlags flags = args.GetOptions();
    auto lhs         = newState.GetValue(args.GetOperand(0));
    auto tarType     = args.GetTarget().type;
    auto srcType     = args.GetOperand(0).type;

    ValueId retVal = newState.GetVc().ExtendInt(lhs, srcType, tarType, flags);
    newState.AssignValue(args.GetTarget(), retVal);
  }
};

class BasicOperationCmp : public BasicOperation<IState, CmpOpArgs> {
  virtual void ExecuteOnNewState(IState& newState, const CmpOpArgs& args) override final
  {
    auto lhs         = newState.GetValue(args.GetOperand(0));
    auto rhs         = newState.GetValue(args.GetOperand(1));
    auto srcType     = args.GetOperand(0).type;
    auto flags       = args.GetOptions();

    ValueId retVal = newState.GetVc().Cmp(lhs, rhs, srcType, flags);
    newState.AssignValue(args.GetTarget(), retVal);
  }
};

class BasicOperationBranch : public OperationBranch<IState> {
public:
  virtual void ExecuteOnNewState(IState& newState, const OperationArgs& args, bool br) override
  {
    auto lhs = newState.GetValue(args.GetOperand(0));
    if (br)
    {
      // if this state can not happen
      if (newState.GetVc().IsFalse(lhs, Type::CreateIntegerType(1)))
        throw BranchException("Branch: can not go true");
      newState.GetVc().AssumeTrue(lhs);
    }
    else
    {
      // if this state can not happen
      if (newState.GetVc().IsTrue(lhs, Type::CreateIntegerType(1)))
        throw BranchException("Branch: can not go false");
      newState.GetVc().AssumeFalse(lhs);
    }
  }
};

// ================== ^^^^^ COMMON CODE ^^^^^ ===============
