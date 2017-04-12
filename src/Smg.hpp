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
/** @file Smg.hpp */

#pragma once

#include "Definitions.hpp"

#include "Smg/Graph.hpp"
#include "Smg/Wrappers.hpp"

#include "ValueContainerV1.hpp"

#include "SmgPrinter.hpp"
#include "SmgCrawler.hpp"

void playground()
{  
  ValueContainer vc{};

  Smg::Impl::Graph ig{vc};
  Smg::Graph        g{ig};

  {
    using namespace ::Smg::Impl;
    auto& objs = ig.objects;
    auto& hndl = ig.handles;

    Type int8  = Type::CreateIntegerType(8 );
    Type int16 = Type::CreateIntegerType(16);
    Type int32 = Type::CreateIntegerType(32);
    Type int64 = Type::CreateIntegerType(64);
    Type int32Ptr = Type::CreatePointerTo(int32);
    Type voidT = Type::CreateVoidType();

    Smg::ObjectId o1{1};
    Smg::ObjectId o2{2};
    Smg::ObjectId o3{3};
    Smg::ObjectId o4{4};
    Smg::ObjectId o5{5};
    auto oids 
      = {o1,o2,o3,o4,o5};

    ValueId  v0{0};
    ValueId  v1{1};
    ValueId  v2{2};
    ValueId  v3{3};
    ValueId  v4{4};
    ValueId  v5{5};
    ValueId  v6{6};
    ValueId  v7{7};
    ValueId  v8{8};
    ValueId  v10{10};
    ValueId  v16{16};
    std::vector<ValueId> vals
      = {v1,v2,v3,v4,v5,v6,v7,v8};

    ValueId  vPtr1{100 +1};
    ValueId  vPtr2{100 +2};
    ValueId  vPtr3{100 +3};
    ValueId  vPtr4{100 +4};
    ValueId  vPtr5{100 +5};
    ValueId  vPtr6{100 +6};
    ValueId  vPtr7{100 +7};
    ValueId  vPtr8{100 +8};
    std::vector<ValueId> vPtrs 
      = {vPtr1,vPtr2,vPtr3,vPtr4,vPtr5,vPtr6,vPtr7,vPtr8};

#define AL_O std::make_unique<Object>()
    
    int i = 0;
    for (auto oid : oids)
    {
      Object& obj =
        objs
        .emplace(oid, std::make_unique<Object>())
        .first.operator*().second.operator*();

      obj.id = oid;
      hndl.CreatePtEdge(PtEdge{v0, vPtrs[i], voidT, oid, v0});
      i++;
    }

    objs[o1]->size = v16;
    objs[o1]->CreateHvEdge(HvEdge{v0, v1, int16});
    objs[o1]->CreateHvEdge(HvEdge{v2, v2, int16});
    objs[o1]->CreateHvEdge(HvEdge{v4, v3, int16});

    objs[o2]->size = v4;
    objs[o2]->CreateHvEdge(v0, v5, int32);

    objs[o1]->CreatePtEdge(PtEdge{v8, vPtr1, int32Ptr, o2, v0});

  }

  SmgPrinter printer{};
  SmgCrawler crawler{printer};

  g.Accept(crawler);
  auto plot_string = printer.GetDot();

  return;
}
