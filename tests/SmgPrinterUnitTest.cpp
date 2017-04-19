#include <gtest/gtest.h>
#include "../src/Definitions.hpp"

#include "../src/Smg/Graph.hpp"
#include "../src/Smg/Wrappers.hpp"

#include "../src/ValueContainerV1.hpp"

#include "../src/SmgPrinter.hpp"
#include "../src/SmgCrawler.hpp"



#pragma comment(lib, "gtest.lib")


TEST(SMGTest, Graph1)
{
  ValueContainer vc{};

  Smg::Impl::Graph ig{vc};
  Smg::Graph        g{ig};
  
  {
    using namespace ::Smg::Impl;
    auto& objs = ig.objects;
    auto& hndl = ig.handles;

    Type int8 = Type::CreateIntegerType(8);
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

    ValueId  vPtr1{100 + 1};
    ValueId  vPtr2{100 + 2};
    ValueId  vPtr3{100 + 3};
    ValueId  vPtr4{100 + 4};
    ValueId  vPtr5{100 + 5};
    ValueId  vPtr6{100 + 6};
    ValueId  vPtr7{100 + 7};
    ValueId  vPtr8{100 + 8};
    std::vector<ValueId> vPtrs
      = {vPtr1,vPtr2,vPtr3,vPtr4,vPtr5,vPtr6,vPtr7,vPtr8};

    int i = 0;
    for (auto oid : oids)
    {
      Object& obj =
        objs
        .emplace(oid, std::make_unique<Region>())
        .first.operator*().second.operator*();

      EXPECT_NE(objs.find(oid), objs.end());


      obj.id = oid;
      PtEdge e = PtEdge{v0, vPtrs[i], voidT, oid, v0};
      hndl.CreatePtEdge(e);

      auto edge = hndl.FindPtEdgeByOffset(v0);
      EXPECT_NE(edge, nullptr);
      EXPECT_EQ(edge->valueType, voidT);
      EXPECT_EQ(edge->value, vPtrs[i]);
      EXPECT_EQ(edge->sourceOffset, v0);
      EXPECT_EQ(edge->targetOffset, v0);
      EXPECT_EQ(edge->targetObjectId, oid);
      EXPECT_EQ(hndl.FindPtEdgeByValue(vPtrs[i]), hndl.FindPtEdgeByOffset(v0));
      //EXPECT_EQ(hndl.FindPtEdgeByValueType(vPtrs[i], voidT), hndl.FindPtEdgeByValue(vPtrs[i]));
      EXPECT_EQ(hndl.FindPtEdgeByOffset(ValueId{99}), nullptr);
      EXPECT_EQ(hndl.FindPtEdgeByValue(ValueId(55)), nullptr);
      //EXPECT_EQ(hndl.FindPtEdgeByValueType(vPtrs[i], int32), nullptr);
      
      EXPECT_EQ(hndl.FindHvEdgeByOffset(v0), nullptr);
      EXPECT_EQ(hndl.FindHvEdgeByValue(vPtrs[i]), nullptr);

      EXPECT_EQ(hndl.GetHvOutEdges().size(), 0);
      EXPECT_EQ(hndl.GetPtOutEdges().size(), i+1);
      EXPECT_EQ(hndl.ptEdges.size(), i + 1);
      EXPECT_EQ(hndl.GetSize(), i+1);

      i++;
    }

    objs[o1]->size = v16;
    EXPECT_EQ(objs[o1]->GetSize(), v16);

    objs[o1]->CreateHvEdge(HvEdge{v0, v1, int16});
    objs[o1]->CreateHvEdge(HvEdge{v2, v2, int16});
    objs[o1]->CreateHvEdge(HvEdge{v4, v3, int16});

    objs[o2]->size = v4;
    objs[o2]->CreateHvEdge(v0, v5, int32);

    objs[o1]->CreatePtEdge(PtEdge{v8, vPtr1, int32Ptr, o2, v0});

  }

 



}

TEST(asdf, asfdasdf) { EXPECT_EQ(true, true); }


int main(int argc, char **argv)
{
  Type::InitTypeSystem();

  testing::InitGoogleTest(&argc, argv);
  int res = RUN_ALL_TESTS();
  
  
  
  std::cin.get();
  std::cin.get();
  return res;
}
