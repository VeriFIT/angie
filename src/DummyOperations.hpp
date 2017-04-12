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
/** @file DummyOperations.hpp */

//#pragma once
//
//#include "Definitions.hpp"
//#include "General.hpp"
//#include "IOperation.hpp"
//#include "ICfgNode.hpp"
//#include "IState.hpp"
//#include "StateStorage.hpp"
//#include "DummyState.hpp"
//
//#include <cassert>
//
//using namespace ::std;
//
//class DummyOperation : public IOperation {
//public:
//  virtual void Execute(IState& s, const vector<OperArg>& args) override
//  {
//    if (s.node.IsBranching())
//    {
//      s.node.GetStatesManager().InsertAndEnqueue(
//        make_unique<DummyState>(s.node, s.node.GetNextTrue())
//      );
//      s.node.GetStatesManager().InsertAndEnqueue(
//        make_unique<DummyState>(s.node, s.node.GetNextFalse())
//      );
//    }
//    else
//    {
//      s.node.GetStatesManager().InsertAndEnqueue(
//        make_unique<DummyState>(s.node, s.node.GetNext())
//      );
//    }
//
//    s.SetExplored();
//    return;
//  }
//};
//
//class DummyOperationFactory : public IOperationFactory {
//public:
//
//  // Inherited via IOperationFactory
//  virtual IOperation & Ret() override { return *new DummyOperation{}; }
//  virtual IOperation & Br() override  { return *new DummyOperation{}; }
//  virtual IOperation & Add() override { return *new DummyOperation{}; }
//  virtual IOperation & Sub() override { return *new DummyOperation{}; }
//  virtual IOperation & Mul() override { return *new DummyOperation{}; }
//  virtual IOperation & Div() override { return *new DummyOperation{}; }
//  virtual IOperation & Rem() override { return *new DummyOperation{}; }
//  virtual IOperation & Shl() override { return *new DummyOperation{}; }
//  virtual IOperation & Shr() override { return *new DummyOperation{}; }
//  virtual IOperation & And() override { return *new DummyOperation{}; }
//  virtual IOperation & Or() override { return *new DummyOperation{}; }
//  virtual IOperation & Xor() override { return *new DummyOperation{}; }
//
//  virtual IOperation & Alloca() override { return *new DummyOperation{}; }
//  virtual IOperation & Load() override { return *new DummyOperation{}; }
//  virtual IOperation & Store() override { return *new DummyOperation{}; }
//
//  virtual IOperation & GetElementPtr() override { return *new DummyOperation{}; }
//
//  virtual IOperation & Cmp() override { return *new DummyOperation{}; }
//
//  virtual IOperation & NotSupportedInstr() override { return *new DummyOperation{}; }
//
//};
