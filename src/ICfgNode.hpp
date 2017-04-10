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
/** @file ICfgNode.hpp */

#pragma once

#include "Definitions.hpp"
#include "General.hpp"
#include "IOperation.hpp"

class IState; //forward delcaration -- include colision ICfgNode vs IState
class StatesManager;

class CfgNode;
class LlvmCfgNode;


//class INavigation {
//public:
//  virtual bool HasTwoNext();
//  virtual INavigation& GetNext();
//  virtual INavigation& GetNextTrue();
//  virtual INavigation& GetNextFalse();
//  virtual const ref_vector<INavigation> GetPrevs(); //?
//
//  virtual void GetDebugInfo();
//  virtual OperationArgs GetArguments();
//};


class ICfgNode : public IOperation {
  friend CfgNode;
  friend LlvmCfgNode;
public:
  virtual bool HasTwoNext() { return false; }
  virtual ICfgNode& GetNext() const = 0;
  virtual ICfgNode& GetNextTrue() const = 0;
  virtual ICfgNode& GetNextFalse() const = 0;
  virtual const ref_vector<ICfgNode>& GetPrevs() const = 0;

  virtual StatesManager& GetStatesManager() = 0;
  virtual void PrintInstruction() const = 0;
  virtual void PrintLocation() const = 0;
  virtual void GetDebugInfo() const = 0; //TODO@review: maybe find a btter name for this method?
  virtual OperationArgs GetArguments() const = 0;

  virtual void Execute(IState& s, const OperationArgs& args) override = 0;
  void Execute(IState& s)
  {
    return Execute(s, GetArguments());
  }

  
  ICfgNode& operator=(ICfgNode&) = delete;
  ICfgNode& operator=(ICfgNode&&) = delete;

  /*dst*/ virtual ~ICfgNode() noexcept = default;

protected:
  ICfgNode* next;
  ICfgNode* nextFalse = nullptr;
  ref_vector<ICfgNode> prevs;

  virtual bool IsStartNode() { return false; }
  virtual bool IsTerminalNode() { return false; }

  /*ctr*/ ICfgNode() : next{nullptr}, prevs{} {}
  /*ctr*/ ICfgNode(ICfgNode* next) : next{next}, prevs{} {}
  /*ctr*/ ICfgNode(ref_vector<ICfgNode> prevs) : next{nullptr}, prevs{prevs} {}
  /*ctr*/ ICfgNode(ICfgNode* next, ref_vector<ICfgNode> prevs) : next{next}, prevs{prevs} {}
};


class StartCfgNode : public ICfgNode {
  friend CfgNode;
  friend LlvmCfgNode;
public:
  virtual ICfgNode& GetNext() const override { return *next; }
  virtual ICfgNode& GetNextTrue() const override { throw NotSupportedException{}; }
  virtual ICfgNode& GetNextFalse() const override { throw NotSupportedException{}; }
  virtual const ref_vector<ICfgNode>& GetPrevs() const override { throw NotSupportedException{}; }

  virtual StatesManager& GetStatesManager() override { throw NotSupportedException{}; }
  virtual void PrintInstruction() const override { throw NotSupportedException{}; }
  virtual void PrintLocation() const override { throw NotSupportedException{}; }
  virtual void GetDebugInfo() const override { throw NotSupportedException{}; }
  virtual OperationArgs GetArguments() const override { throw NotSupportedException{}; }

  virtual bool IsStartNode() override { return true; }

  virtual void Execute(IState& s, const OperationArgs& args) override  { throw NotSupportedException{}; }

private:
  /*ctr*/ StartCfgNode() {}
};

class TerminalCfgNode : public ICfgNode {
  friend CfgNode;
  friend LlvmCfgNode;
public:
  virtual ICfgNode& GetNext() const override { throw NotSupportedException{}; }
  virtual ICfgNode& GetNextTrue() const override { throw NotSupportedException{}; }
  virtual ICfgNode& GetNextFalse() const override { throw NotSupportedException{}; }
  virtual const ref_vector<ICfgNode>& GetPrevs() const override { return prevs; }

  virtual StatesManager& GetStatesManager() override { throw NotSupportedException{}; }
  //! It might be worth implementing theese as no-ops -> autonomous end of analysis
  virtual void PrintInstruction() const override  { return; }
  //! It might be worth implementing theese as no-ops -> autonomous end of analysis
  virtual void PrintLocation() const override { return; }
  //! It might be worth implementing theese as no-ops -> autonomous end of analysis
  virtual void GetDebugInfo() const override { return; }
  //! It might be worth implementing theese as no-ops -> autonomous end of analysis
  virtual OperationArgs GetArguments() const override { return OperationArgs{}; }

  virtual bool IsTerminalNode() override { return true; }

  //! It might be worth implementing theese as no-ops -> autonomous end of analysis
  virtual void Execute(IState& s, const OperationArgs& args) override { return; }

private:
  /*ctr*/ TerminalCfgNode() {}
};
