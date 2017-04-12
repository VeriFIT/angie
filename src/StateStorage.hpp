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
/** @file StateStorage.hpp */

#pragma once

#include "Definitions.hpp"
#include "General.hpp"
#include "IState.hpp"

/// <summary>
/// Priorities to add new states into worklist with [DFS/BFS strategies, ...]
/// </summary>
enum class WorklistPriority {
  Standard = 0
};

class StatesManager {
  static std::vector<uptr<IState>> statePool;
  static ref_deque<IState> worklist;

  ref_vector<IState> localStates;
private:

public:
  void InsertAndEnqueue(
    uptr<IState>&& statePtr,
    WorklistPriority prioroty = WorklistPriority::Standard
  )
  {
    //TODO: Somehow, handle the priority (and add them!)

    if (false) // is state equal to to other states here == equality check
    {
      // throw it out completely
      return;
    }
    if (false) // we are able to join something?
    {
      // for all states that took part in the join:
      // set them to AbstractedOut

      // and add the result of join operation
    }

    // else -> just add it
    {
      IState& state = *statePtr;
      statePool.push_back(std::move(statePtr));
      localStates.push_back(state);

      // here we should utilize priorities
      // now DFS
      worklist.push_front(state);
    }
  }

  static ref_wr<IState> WorklistDequeue()
  {
    auto& ret = worklist.front();
    worklist.pop_front();
    return ret;
  }
  static bool IsWorklistEmpty()
  {
    return worklist.empty();
  }
};