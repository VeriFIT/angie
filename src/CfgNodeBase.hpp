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
/** @file CfgNodeBase.hpp */

#pragma once

#include "Definitions.hpp"
#include "General.hpp"

#include "StateStorage.hpp"
#include "ICfgNode.hpp"

class CfgNode : public ICfgNode {
private:
  OperationArgs args;
  IOperation& op;
  StatesManager states;

public:
  virtual bool HasTwoNext() override { return nextFalse != nullptr; }
  virtual ICfgNode& GetNext() const override { return *next; }
  virtual ICfgNode& GetNextTrue() const override
  {
    if (nextFalse == nullptr)
      throw std::runtime_error("not branch");
    return *next;
  }
  virtual ICfgNode& GetNextFalse() const override
  {
    if (nextFalse == nullptr)
      throw std::runtime_error("not branch");
    return *nextFalse;
  }

  virtual const ref_vector<ICfgNode>& GetPrevs() const override { return prevs; }

  virtual void Execute(IState& s, const OperationArgs& args) override
  {
    return op.Execute(s, args);
  }
  virtual StatesManager& GetStatesManager() override { return states; }
  virtual OperationArgs GetArguments() const override { return args; }

protected:
  /*ctr*/ CfgNode(IOperation& op, OperationArgs args, ICfgNode& prev, ICfgNode& next) :
    op{op},
    args{args},
    ICfgNode(&next, ref_vector<ICfgNode>{1, prev})
  {
  }
  //
  //  // ---------------------- for dev needs only ---------------------------- //
  //public:
  //  virtual void GetDebugInfo() const override { throw NotImplementedException{}; }
  //
  //  static CfgNode& CreateNode(IOperation& op)
  //  {
  //    CfgNode* newNode = new CfgNode{op, *new StartCfgNode{}, *new TerminalCfgNode{}};
  //    ((ICfgNode&)newNode->prevs[0]).next = newNode;
  //    newNode->next->prevs.push_back(*newNode);
  //    return *newNode;
  //  }
  //
  //  //beware - adding a node after terminal node here(after inproper cast) would not raise exception
  //  CfgNode& InsertNewAfter(IOperation& op)
  //  {
  //    CfgNode* newNode = new CfgNode{op, *this, *this->next};
  //    this->next = newNode;
  //    return *newNode;
  //  }
  //  // ---------------------- for dev needs only ---------------------------- //
};

//TODO: Place for code comments of this ICfgNode block
// add / check virtual destructors need
// add mass deleter for CFG? (memory leaks)
