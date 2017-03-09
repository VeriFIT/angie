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
/** @file FrontedValueMapper.hh */

#pragma once


#include "Definitions.hh"
#include "General.hh"
#include "IOperation.hh"
#include "ICfgNode.hh"
#include "IState.hh"
#include "StateStorage.hh"
#include "DummyState.hh"

#include <map>
#include <cassert>

//#include <range/v3/all.hpp>

typedef int FunctionInfo;

class FunctionHandle {
public:

  ICfgNode& cfg;
  OperationArgs params;
  FunctionInfo info;

  /*ctr*/ FunctionHandle(ICfgNode& cfg, OperationArgs&& params, FunctionInfo info) : cfg{cfg}, params{params}, info{info} {}
  /*dt*/ ~FunctionHandle()
  {
    delete &cfg;
  }

};

class FuncMapper {
private:

  std::map<ValueId, std::unique_ptr<FunctionHandle>> funcMap;

public:

  void RegisterFunction(ValueId id, uptr<FunctionHandle>&& func)
  {
    funcMap.emplace(std::move(id), std::move(func));
  }

  FunctionHandle& GetFunction(ValueId id)
  {
    return *funcMap.at(id); 
  }
};

class Mapper {

private:
  std::map<FrontendValueId, ValueId> innerMap;

public:

  ValueId GetValueId(FrontendValueId id) const
  {
    return innerMap.at(id);
  }
  
  auto GetFrontendIds(ValueId id) const
  {
    return nullptr;//innerMap | ranges::view::remove_if([=](STL_ITEM_T(innerMap) pair){ return pair.second == id; });
  }

  void LinkToValueId(FrontendValueId id, ValueId value)
  {
    innerMap.insert({id, value});
  }

  ValueId CreateOrGetValueId(FrontendIdTypePair arg, IValueContainer& vc)
  {
    FrontendValueId id = arg.id;

    // src: http://stackoverflow.com/a/101980

    decltype(innerMap)::iterator lb = innerMap.lower_bound(id);

    if (lb != innerMap.end() && !(innerMap.key_comp()(id, lb->first)))
    {
      // key already exists
      // update lb->second if you care to
      return lb->second;
    }
    else
    {
      // the key does not exist in the innerMap
      // add it to the innerMap
      auto newValue = vc.CreateVal(arg.type);
      innerMap.insert(lb, decltype(innerMap)::value_type{id, newValue}); // Use lb as a hint to insert,
                                                                         // so it can avoid extra lookup
      return newValue;
    }
  }
  
  ValueId CreateOrGetValueId(FrontendValueId id) 
  {
    // src: http://stackoverflow.com/a/101980

    decltype(innerMap)::iterator lb = innerMap.lower_bound(id);

    if (lb != innerMap.end() && !(innerMap.key_comp()(id, lb->first)))
    {
      // key already exists
      // update lb->second if you care to
      return lb->second;
    }
    else
    {
      // the key does not exist in the innerMap
      // add it to the innerMap
      auto newValue = ValueId::GetNextId();
      innerMap.insert(lb, decltype(innerMap)::value_type{id, newValue}); // Use lb as a hint to insert,
                                                                             // so it can avoid extra lookup
      return newValue;
    }
  }

};



class MapperDebug {
private:

  std::map<FrontendValueId, ValueId> map;

public:

  void LinkToValueId(FrontendValueId id, ValueId value)
  {
    map.insert({id, value});
  }

  ValueId CreateOrGetValueId(FrontendValueId id) 
  {
    // src: http://stackoverflow.com/a/101980
    typedef std::map<FrontendValueId, ValueId> MapType;   // Your innerMap type may vary, just change the typedef

    MapType::iterator lb = map.lower_bound(id);

    if (lb != map.end() && !(map.key_comp()(id, lb->first)))
    {
      // key already exists
      // update lb->second if you care to
      return lb->second;
    }
    else
    {
      // the key does not exist in the innerMap
      // add it to the innerMap
      ValueId newValue{};
      map.insert(lb, MapType::value_type(id, newValue));    // Use lb as a hint to insert,
                                                            // so it can avoid another lookup
      return newValue;
    }
  }

};