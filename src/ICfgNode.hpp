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
#include "IdImpl.hpp"

class IState; //forward declaration -- include collision ICfgNode vs IState
class StatesManager;

class CfgNode;
class LlvmCfgNode;

class ICfgNode : public IOperation {
  friend CfgNode;
  friend LlvmCfgNode;
public:
  virtual bool IsBranching() { return false; }
  virtual ICfgNode& GetNext() const = 0;
  virtual ICfgNode& GetNextTrue() const = 0;
  virtual ICfgNode& GetNextFalse() const = 0;
  virtual const ref_vector<ICfgNode>& GetPrevs() const = 0;

  virtual StatesManager& GetStatesManager() = 0;
  virtual void PrintInstruction() const = 0;
  virtual void PrintLocation() const = 0;
  virtual void GetDebugInfo() const = 0; //TODO@review: maybe find a better name for this method?
  virtual bool HasBreakpoint() const = 0;
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
  Id<class CfgNodeIdToken> id = decltype(id)::GetNextId();
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
