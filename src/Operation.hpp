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

#include "Definitions.hpp"
#include "General.hpp"
#include "IOperation.hpp"
#include "IState.hpp"
#include "StateStorage.hpp"
#include "FrontedValueMapper.hpp"

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
    : AnalysisErrorException("Possible null dereference exception occurred - getElementPtr.")
  {
  }
};

//TODO: maybe remove use of IState& lastSt from templated helpers, we can access that via successor
//TODO: maybe replace dynamic_cast with static_cast ?
//TODO: comment about executing last instruction -> it is OK now, TerminalNode implements no-op Execute()

template<typename StateT, typename OperArgsT, typename branching = std::false_type>
class OperationHelpers;

template<typename StateT, typename OperArgsT>
class OperationHelpers<StateT, OperArgsT, std::false_type> {
public:
  void SafelyExecuteAndEnque(IState& lastSt, uptr<IState>&& successor, const OperationArgs& args)
  {
    try
    {
      lastSt.SetExplored();
      ExecuteOnNewState(static_cast<StateT&>(*successor), static_cast<const OperArgsT&>(args));
      lastSt.GetNextStep().GetStatesManager().InsertAndEnqueue(std::move(successor));
    }
    catch (AnalysisErrorException e)
    {
      lastSt.GetNextStep().PrintLocation();
      printf("%s\n", e.what());
    }
  }
  virtual void ExecuteOnNewState(StateT& newState, const OperArgsT& args) = 0;
};

template<typename StateT, typename OperArgsT>
class OperationHelpers<StateT, OperArgsT, std::true_type> {
public:
  void SafelyExecuteAndEnque(IState& lastSt, uptr<IState>&& successor, const OperationArgs& args, bool br)
  {
    try
    {
      lastSt.SetExplored();
      ExecuteOnNewState(static_cast<StateT&>(*successor), static_cast<const OperArgsT&>(args), br);
      lastSt.GetNextStep().GetStatesManager().InsertAndEnqueue(std::move(successor));
    }
    catch (AnalysisErrorException e)
    {
      lastSt.GetNextStep().PrintLocation();
      printf("%s\n", e.what());
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
  virtual void Execute(IState& lastSt, const OperationArgs& args) override final
  {
    assert(!lastSt.GetNextStep().HasTwoNext()); //TODO: comment

    uptr<IState> successor = lastSt.CreateSuccessor(lastSt.GetNextStep().GetNext());
    this->SafelyExecuteAndEnque(lastSt, std::move(successor), args);
  }
};

template<typename StateT, typename OperArgsT = OperationArgs>
class OperationRet : public IOperation, OperationHelpers<StateT, OperArgsT> {
  void Execute(IState& lastSt, const OperArgsT& args)
  {
    assert(!lastSt.GetNextStep().HasTwoNext()); //TODO: comment

    uptr<IState> successor = lastSt.CreateSuccessor(lastSt.GetStackRetNode());
    this->SafelyExecuteAndEnque(lastSt, std::move(successor), args);
  }
};

template<typename StateT>
class OperationCall : public IOperation, OperationHelpers<StateT, CallOpArgs> {
public:

  virtual void Execute(IState& lastSt, const OperationArgs& args) override
  {
    Execute(lastSt, static_cast<const CallOpArgs&>(args));
  }

private:

  void Execute(IState& lastSt, const CallOpArgs& args)
  {
    assert(!lastSt.GetNextStep().HasTwoNext()); //TODO: comment

    uptr<IState> successor;
    {
      auto& nextJump = lastSt.GetFuncMapping().GetFunction(lastSt.GetAnyVar(args.GetOptions())).cfg;
      successor = lastSt.CreateSuccessor(nextJump);

      // Prepares the instruction-after-call to be pushed on the stack
      (*successor).SetStackRetNode(lastSt.GetNextStep().GetNext());
    }
    this->SafelyExecuteAndEnque(lastSt, std::move(successor), args);
  }
};

template<typename StateT, typename OperArgsT = OperationArgs>
class OperationBranch : public IOperation, OperationHelpers<StateT, OperArgsT, std::true_type> {
public:

  // Override this again to switch to branching implementation
  virtual void Execute(IState& lastSt, const OperationArgs& args) override final
  {
    assert(lastSt.GetNextStep().HasTwoNext()); //TODO: comment

    uptr<IState> successor;
    successor = lastSt.CreateSuccessor(lastSt.GetNextStep().GetNextTrue());
    this->SafelyExecuteAndEnque(lastSt, std::move(successor), args, true);

    successor = lastSt.CreateSuccessor(lastSt.GetNextStep().GetNextFalse());
    this->SafelyExecuteAndEnque(lastSt, std::move(successor), args, false);

    lastSt.SetExplored();
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

class BasicOperationBinOp : public BasicOperation<IState, BinaryOpArgs> {
  virtual void ExecuteOnNewState(IState& newState, const BinaryOpArgs& args) override final
  {
    auto opts        = args.GetOptions();

    auto type        = args.GetOperand(0).type;
    auto lhs         = newState.GetAnyVar(args.GetOperand(0));
    auto rhs         = newState.GetAnyVar(args.GetOperand(1));

    ValueId retVal = newState.GetVc().BinOp(lhs, rhs, type, opts);
    newState.LinkLocalVar(args.GetTarget(), retVal);
  }
};

class BasicOperationTruncate : public BasicOperation<IState, TruncExtendOpArgs> {
  virtual void ExecuteOnNewState(IState& newState, const TruncExtendOpArgs& args) override final
  {
    ArithFlags flags = args.GetOptions();
    auto lhs         = newState.GetAnyVar(args.GetOperand(0));
    auto tarType     = args.GetTarget().type;
    auto srcType     = args.GetOperand(0).type;

    ValueId retVal = newState.GetVc().TruncateInt(lhs, srcType, tarType);
    newState.LinkLocalVar(args.GetTarget(), retVal);
  }
};

class BasicOperationExtend : public BasicOperation<IState, TruncExtendOpArgs> {
  virtual void ExecuteOnNewState(IState& newState, const TruncExtendOpArgs& args) override final
  {
    ArithFlags flags = args.GetOptions();
    auto lhs         = newState.GetAnyVar(args.GetOperand(0));
    auto tarType     = args.GetTarget().type;
    auto srcType     = args.GetOperand(0).type;

    ValueId retVal = newState.GetVc().ExtendInt(lhs, srcType, tarType, flags);
    newState.LinkLocalVar(args.GetTarget(), retVal);
  }
};

class BasicOperationCmp : public BasicOperation<IState, CmpOpArgs> {
  virtual void ExecuteOnNewState(IState& newState, const CmpOpArgs& args) override final
  {
    auto lhs         = newState.GetAnyVar(args.GetOperand(0));
    auto rhs         = newState.GetAnyVar(args.GetOperand(1));
    auto srcType     = args.GetOperand(0).type;
    auto flags       = args.GetOptions();

    ValueId retVal = newState.GetVc().Cmp(lhs, rhs, srcType, flags);
    newState.LinkLocalVar(args.GetTarget(), retVal);
  }
};

class BasicOperationBranch : public OperationBranch<IState> {
public:
  virtual void ExecuteOnNewState(IState& newState, const OperationArgs& args, bool br) override
  {
    auto lhs = newState.GetAnyVar(args.GetOperand(0));
    if (br)
    {
      // if this state can not happen
      if (newState.GetVc().IsFalse(lhs, Type::CreateIntegerType(1)))
        throw AnalysisErrorException("Branch: can not go true");
      newState.GetVc().AssumeTrue(lhs);
    }
    else
    {
      // if this state can not happen
      if (newState.GetVc().IsTrue(lhs, Type::CreateIntegerType(1)))
        throw AnalysisErrorException("Branch: can not go false");
      newState.GetVc().AssumeFalse(lhs);
    }
  }
};

// ================== ^^^^^ COMMON CODE ^^^^^ ===============
