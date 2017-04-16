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

#include "IdImpl.hpp"

using StateId = Id<class StateIdToken, int64_t, 0>;

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

  virtual StateId       GetId()         const = 0;
  virtual StateId::type GetGeneration() const = 0;

  virtual uptr<IState> CreateSuccessor(ICfgNode& nextStep) const = 0;

  // informace o posledni provedene op. na ktere je stav zalozen?

  virtual StateStatus GetStatus() const = 0;

  virtual ICfgNode& GetNode() const = 0; // mandatory, this program node should be executed on this to futrher advance
  
  // optional, all states that participated on creating this state (JOIN)
  // might be changed to Get/Set property
  virtual gsl::span<const std::reference_wrapper<const IState>> GetPredecessors() const = 0;
  // optional, all states that are based on this state
  // might be changed to Get/Set property
  virtual gsl::span<const std::reference_wrapper<const IState>> GetSuccessors() const = 0;

  // possibly useless now
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

class State : public IState {
private:

  StateId id = StateId::GetNextId();
  StateId::type generation;

  StateStatus condition = StateStatus::New;

  Mapper&          globalMapping;
  FuncMapper&      funcMapping;
  IValueContainer& vc; 
  //TODO: VC should be copied when stated is cloned?! -> how about other parts of program, like Smg,... each has own reference
  // ... we will probaly end up with thread local "current VC reference" and the owner will be state

  ICfgNode& node; 

  std::vector<std::reference_wrapper<const IState>> predecessors;
  std::vector<std::reference_wrapper<const IState>> successors;

protected:

  Mapper      localMapping;

  // Basic constructor
  /*ctr*/ State(
    //ICfgNode& lastCfgNode, 
    ICfgNode& node,
    IValueContainer& vc,
    Mapper& globalMapping,
    FuncMapper& funcMapping
  ) :
    //lastCfgNode(lastCfgNode), 
    node(node),
    vc(vc),
    globalMapping(globalMapping),
    funcMapping(funcMapping),
    generation(0)
  {
  }

  // Clone constructor
  /*ctr*/ State(
    const State& state, 
    //ICfgNode& lastCfgNode, 
    ICfgNode& node
  ) :
    //lastCfgNode(lastCfgNode), 
    node(node),
    vc(state.vc),
    globalMapping(state.globalMapping),
    funcMapping(state.funcMapping),
    localMapping(state.localMapping),
    generation(state.generation + 1),
    predecessors{ref_wr<const IState>(state)}
  {
  }

public:

  virtual StateId     GetId()       const override { return id; }
  virtual StateStatus GetStatus()   const override { return condition; }
  virtual ICfgNode&   GetNode()     const override { return node; }
  virtual void        SetExplored()       override {        condition = StateStatus::Explored; }
  virtual bool        IsNew()       const override { return condition == StateStatus::New; }


  virtual IValueContainer& GetVc() override final { return vc; }
  virtual gsl::span<const std::reference_wrapper<const IState>> GetPredecessors() const override { return {predecessors}; }
  virtual gsl::span<const std::reference_wrapper<const IState>> GetSuccessors()   const override { return {successors}; }

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

  //TODO@michkot: move to analysis context
  virtual FuncMapper& GetFuncMapping() const override { return funcMapping; }
  


};
