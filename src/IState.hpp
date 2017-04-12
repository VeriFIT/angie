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
/** @file IState.hpp */

#pragma once

#include "Definitions.hpp"
#include "General.hpp"

#include "Values.hpp"
#include "FrontedValueMapper.hpp"


class ICfgNode; //forward declaration -- include collision ICfgNode vs IState
//#include "ICfgNode.hpp"


// Posibly replaced by "IsXXX" methods in State
/// <summary>
/// Describes the status of the state
/// </summary>
enum class StateStatus {
  New           = 0, // The state was created, but no successors were created
  Explored      = 1, // Successors were created [and may have been planned for execution]
  AbstractedOut = 2, // This state was replaced by a more abstract one
};

class IState {
public:
  virtual ~IState() {}

  virtual uptr<IState> CreateSuccessor(ICfgNode& nextStep) const = 0;

  // informace o posledni provedene op. na ktere je stav zalozen?

  virtual StateStatus GetStatus() const = 0;

  virtual ICfgNode& GetNode() const = 0; // mandatory, this program node should be executed on this to futrher advance
  
  // optional, all states that participated on creating this state (JOIN)
  // might be changed to Get/Set property
  virtual ref_span<IState> GetPredecessors() = 0;
  // optional, all states that are based on this state
  // might be changed to Get/Set property
  virtual ref_span<IState> GetSuccessors() = 0;

  // meaning: All paths(states) leading from this state were prepared for processing or already processed
  virtual void SetExplored() = 0;
  //TODO@michkot: find a better name for this
  // meaning: Successors of this state are unprocessed
  // Usefull when states placed in worklist might be affected by meta-operation
  virtual bool IsNew() const = 0;

  virtual IValueContainer& GetVc() = 0;
  virtual FuncMapper& GetFuncMapping() const = 0;

  virtual ValueId GetValue (FrontendIdTypePair var) const = 0;
  virtual void AssignValue (FrontendIdTypePair var, ValueId value) = 0;
  // pro dead value analysis / memory leaks:
  // momentálně pro toto nevidím use-case, neboť nemám jak zjistit že je proměná (resp její SSA následník)
  // již out of scope.
  virtual void DelLocalVar(FrontendIdTypePair var) { throw NotImplementedException(); }

  //předávané argumenty, návratový typ, návratová lokace/instrukce // FunctionCallInfo
  virtual void PushFrame(int info) { throw NotImplementedException(); }
  //pozn lokace návratu musí být uložena ve stavu, na adekvátní urovni, přilepena na stack frame
  virtual void PopFrame(FrontendIdTypePair retVar) { throw NotImplementedException(); }

  virtual ICfgNode& GetStackRetNode() const         { throw NotImplementedException(); }
  virtual void      SetStackRetNode(ICfgNode& node) { throw NotImplementedException(); }
};

class StateBase : public IState {
private:

  StateStatus condition = StateStatus::New;

  Mapper&     globalMapping;
  FuncMapper& funcMapping;
  IValueContainer& vc; 
  //TODO: VC should be copied when stated is cloned?! -> how about other parts of program, like Smg,... each has own reference
  // ... we will probaly end up with thread local "current VC reference" and the owner will be state

  //ICfgNode& lastCfgNode;
  ICfgNode& nextCfgNode; 

protected:

  Mapper      localMapping;

  // Basic constructor
  /*ctr*/ StateBase(
    //ICfgNode& lastCfgNode, 
    ICfgNode& nextCfgNode,
    IValueContainer& vc,
    Mapper& globalMapping,
    FuncMapper& funcMapping
  ) :
    //lastCfgNode(lastCfgNode), 
    nextCfgNode(nextCfgNode),
    vc(vc),
    globalMapping(globalMapping),
    funcMapping(funcMapping)
  {
  }

  // Clone constructor
  /*ctr*/ StateBase(
    const StateBase& state, 
    //ICfgNode& lastCfgNode, 
    ICfgNode& nextCfgNode
  ) :
    //lastCfgNode(lastCfgNode), 
    nextCfgNode(nextCfgNode),
    vc(state.vc),
    globalMapping(state.globalMapping),
    funcMapping(state.funcMapping),
    localMapping(state.localMapping)
  {
  }

public:

  virtual StateStatus GetStatus() const override { return condition; }
  virtual ICfgNode& GetNode() const override { return nextCfgNode; }
  virtual ref_span<IState> GetPredecessors() override { throw NotImplementedException(); }
  virtual ref_span<IState> GetSuccessors() override { throw NotImplementedException(); }
  virtual void SetExplored() override { condition = StateStatus::Explored; }
  virtual bool IsNew() const override { return condition == StateStatus::New; }

  //virtual ref_span<IState> GetPredecessors() override
  //{
  //  return ref_span<IState>(previousStates);
  //}

  //------------------------------------


  virtual IValueContainer& GetVc() override final { return vc; }

  virtual FuncMapper& GetFuncMapping() const override { return funcMapping; }

  //TODO: rename / refactor..

  //TODO: GetAnyValue, GetValue ?
  virtual ValueId GetValue(FrontendIdTypePair var) const override
  {
    try { return globalMapping.GetValueId(var.id); }
    catch (std::exception e) { return localMapping.GetValueId(var.id); }
  }
  virtual void AssignValue(FrontendIdTypePair var, ValueId value) override
  {
    try { localMapping.LinkToValueId(var.id, value); }
    catch (std::exception e) { return localMapping.LinkToValueId(var.id, value); }
    
  }

  //------------------------------------


};
