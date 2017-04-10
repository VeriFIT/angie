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
/** @file DummyState.hpp */

//#pragma once
//
//#include <map>
//#include "Definitions.hpp"
//#include "General.hpp"
//#include "IOperation.hpp"
//#include "Values.hpp"
//#include "IState.hpp"
//
//typedef int DummyValueId;
//
//class DummyState : public IState {
//private:
//  std::map<FrontendValueId, ValueId> globalVarMapping;
//  std::map<FrontendValueId, ValueId> varMapping;
//
//public:
//  virtual ~DummyState() override {}
//
//  virtual void AddGlobalVar(OperArg var) override
//  {
//    globalVarMapping.insert_or_assign(var.id, ValueId{});
//  }
//
//  virtual void LinkLocalVar(OperArg var, ValueId value) override {}
//  virtual ValueId AddOrGetLocalVar(OperArg var) override
//  {
//    varMapping.insert_or_assign(var.id, ValueId{});
//    return ValueId{};
//  }
//
//  virtual void DelLocalVar(OperArg var) override
//  {
//    throw NotImplementedException{"DelLocalVar"};
//  }
//  virtual void PushFrame(FunctionCallInfo info) override
//  {
//    throw NotImplementedException{"PushFrame"};
//  }
//  virtual void PopFrame(OperArg retVar) override
//  {
//    throw NotImplementedException{"PopFrame"};
//  }
//
//  /*ctr*/ DummyState(ICfgNode& lastNode, ICfgNode& nextNode) :
//    IState(lastNode, nextNode),
//    globalVarMapping{},
//    varMapping{}
//  { }
//
//};
