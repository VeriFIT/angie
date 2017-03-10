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
/** @file Operation.hh */

#pragma once

#include "Definitions.hh"
#include "General.hh"
#include "IOperation.hh"
#include "IState.hh"
#include "StateStorage.hh"
#include "FrontedValueMapper.hh"

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

//TODO: maybe remove use of IState& lastSt from templated helpers, we can acces that via succesor
//TODO: maybe replace dynamic_cast with static_cast ?
//TODO: comment about executing last instruction -> it is ok now, TerminalNode implements no-op Execute()

template<typename StateT, typename OperArgsT, typename branching = std::false_type>
class OperationHelpers;

template<typename StateT, typename OperArgsT>
class OperationHelpers<StateT, OperArgsT, std::false_type> {
public:
  void SafelyExecuteAndEnque(IState& lastSt, uptr<StateT>&& successor, const OperationArgs& args)
  {
    try
    {
      lastSt.SetExplored();
      ExecuteOnNewState(*successor, static_cast<const OperArgsT&>(args));
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
  void SafelyExecuteAndEnque(IState& lastSt, uptr<StateT>&& successor, const OperationArgs& args, bool br)
  {
    try
    {
      lastSt.SetExplored();
      ExecuteOnNewState(*successor, static_cast<const OperArgsT&>(args), br);
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

    uptr<StateT> successor = make_unique<StateT>(dynamic_cast<StateT&>(lastSt), lastSt.GetNextStep().GetNext());
    SafelyExecuteAndEnque(lastSt, std::move(successor), args);
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

    uptr<StateT> successor;
    {
      auto& nextJump = lastSt.GetFuncMapping().GetFunction(lastSt.GetAnyVar(args.GetOptions())).cfg;
      successor = make_unique<StateT>(dynamic_cast<StateT&>(lastSt), nextJump);
    }
    SafelyExecuteAndEnque(lastSt, std::move(successor), args);

    return;
  }
};

template<typename StateT, typename OperArgsT = OperationArgs>
class OperationBranch : public IOperation, OperationHelpers<StateT, OperArgsT, std::true_type> {
public:

  // Override this again to switch to branching implementation
  virtual void Execute(IState& lastSt, const OperationArgs& args) override final
  {
    assert(lastSt.GetNextStep().HasTwoNext()); //TODO: comment

    uptr<StateT> successor;
    successor = make_unique<StateT>(dynamic_cast<StateT&>(lastSt), lastSt.GetNextStep().GetNextTrue());
    SafelyExecuteAndEnque(lastSt, std::move(successor), args, true);

    successor = make_unique<StateT>(dynamic_cast<StateT&>(lastSt), lastSt.GetNextStep().GetNextFalse());
    SafelyExecuteAndEnque(lastSt, std::move(successor), args, false);

    lastSt.SetExplored();
    return;
  }
};
