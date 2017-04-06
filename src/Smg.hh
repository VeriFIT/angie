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
/** @file Smg.hh */

#pragma once

#include "Smg/Graph.hh"
#include "Smg/Wrappers.hh"

namespace Smg {

void xx()
{
  IValueContainer* vc = new ValueContainer();
  Impl::Graph ig{vc};
  Graph g{ig};
  Impl::Object o{};
  std::vector<decltype(o)> vect{};
  vect.push_back(o);

  //auto rnx = ranges::view::all(vect);
  auto rng = ranges::range<decltype(vect)::iterator>(vect.begin(), vect.end());
  auto v = objset(std::move(rng), ig);
  auto y = objset(vect, ig);
 
  //auto w = objset(o.GetHvOutEdges(), g);
  
  //RANGES_FOR(auto& var, v)
  //{
  //  std::cout << var.id;
  //}
  for(auto& varr : v)
  {
    std::cout << varr.id;
  }
  for(auto& varr : rng)
  {
      std::cout << varr.id;
  }
  for(Impl::Object& varr : v)
  {
    std::cout << varr.id;
  }
  return;
}

} // namespace Smg

void playground()
{
  Smg::xx();
  using namespace ::Smg::Impl;
  Object o;

  Type int8     = Type::CreateIntegerType(8);
  Type int32    = Type::CreateIntegerType(32);
  Type int64    = Type::CreateIntegerType(64);
  Type int32Ptr = Type::CreatePointerTo(int32);

  ObjectId o1{1};
  ObjectId o2{2};
  ObjectId o3{3};
  ObjectId o4{4};
  ObjectId o5{5};

  ValueId  v0{0};
  ValueId  v1{1};
  ValueId  v2{2};
  ValueId  v3{3};
  ValueId  v4{4};
  ValueId  v5{5};
  ValueId  v8{8};
  ValueId  v10{10};
  ValueId  v16{16};

  ValueId  vPtr1{~0ull - 1};
  ValueId  vPtr2{~0ull - 2};
  ValueId  vPtr3{~0ull - 3};

  o.hvEdges.push_back(HvEdge{v0, v2   , int32});
  o.ptEdges.push_back(PtEdge{v4, vPtr1, int32Ptr, o2, v0});
  //auto all = o.GetOutEdges();
  //for(auto& x : all)
  //{
  //  std::cout << x << std::endl;
  //}

  ////o.FindPtEdge(4);
  //o.FindHvEdge(ValueId{1});
  //const Object& o2 = o;
  //o2.FindHvEdge(ValueId{1});
  ////o.CreateHvEdge(11, 12, 13);

  return;
  //gsl::
}
