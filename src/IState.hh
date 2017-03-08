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
/** @file IState.hh */

#pragma once

#include "Definitions.hh"
#include "General.hh"

#include "Values.hh"

class ICfgNode; //forward delcaration -- include colision ICfgNode vs IState
//#include "ICfgNode.hh"

using namespace ::std;

enum class StateCondition {
  New = 0,
  Explored = 1,
  AbstractedOut = 2
};

//TODO: probably rename lastCfgNode - associatedCfgNode?, nextCfgNode, previousStates

class IState {
public:
  // informace o posledni provedene op. na ktere je stav zalozen?

  StateCondition condition = StateCondition::New;

  ICfgNode& lastCfgNode; // optional, this state is has been created in an execution of this program node
  ICfgNode& nextCfgNode; // mandatory, this program node should be executed on this to futrher advance

  // optional, all states that participated on creating this state (JOIN)
  // might be changed to Get/Set property
  ref_vector<IState> previousStates; 
  // optional, all states that are based on this state
  // might be changed to Get/Set property
  ref_vector<IState> followingStates; 

  virtual ~IState() {}

  // meaning: All paths(states) leading from this state were prepared for processing or already processed
  virtual void SetExplored() { condition = StateCondition::Explored; }
  // meaning: Succesors of this state are un-processed
  virtual bool IsNew() { return condition == StateCondition::New; }
  
  virtual ValueId GetAnyVar (FrontendIdTypePair var) = 0;
  virtual void LinkGlobalVar(FrontendIdTypePair var, ValueId value) = 0;
  virtual void LinkLocalVar (FrontendIdTypePair var, ValueId value) = 0;
  // pro dead value analysis / memory leaks:
  // momentálně pro toto nevidím use-case, neboť nemám jak zjistit že je proměná (resp její SSA následník)
  // již out of scope.
  virtual void DelLocalVar(FrontendIdTypePair var) { throw NotImplementedException(); }

  //předávané argumenty, návratový typ, návratová lokace/instrukce // FunctionCallInfo
  virtual void PushFrame(int info) { throw NotImplementedException(); }
  //pozn lokace návratu musí být uložena ve stavu, na adekvátní urovni, přilepena na stack frame
  virtual void PopFrame(FrontendIdTypePair retVar) { throw NotImplementedException(); }


protected:
  /*ctr*/ IState(ICfgNode& lastNode, ICfgNode& nextNode) :
    lastCfgNode{lastNode},
    nextCfgNode{nextNode}
  {
  }
};
