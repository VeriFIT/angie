#include <gtest/gtest.h>
#include "../src/Definitions.hpp"

#include "../src/Smg/Graph.hpp"
#include "../src/Smg/Wrappers.hpp"

#include "../src/ValueContainerV1.hpp"

#include "../src/SmgPrinter.hpp"
#include "../src/SmgCrawler.hpp"

#include "../src/OsUtils.hpp"
#include "../src/SmgUtils.hpp"


#pragma comment(lib, "gtest.lib")

    
Smg::ObjectId o0{0};
    Smg::ObjectId o1{12};
    Smg::ObjectId o2{25};
    Smg::ObjectId o3{21};
    Smg::ObjectId o4{51};
    Smg::ObjectId o5{35};
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



TEST(SMGTest, Graph1)
{
  Type int8 = Type::CreateIntegerType(8);
  Type int16 = Type::CreateIntegerType(16);
  Type int32 = Type::CreateIntegerType(32);
  Type int64 = Type::CreateIntegerType(64);
  Type int32Ptr = Type::CreatePointerTo(int32);
  Type voidT = Type::CreateVoidType();

  ValueContainer vc{};

  Smg::Impl::Graph ig{vc};
  Smg::Graph        g{ig};
  
  {
    using namespace ::Smg;
    auto& objs = ig.objects;
    auto& hndl = ig.handles;
    
    

    int i = 0;

    EXPECT_EQ(objs.size(), 3);

    for (auto oid : oids)
    {
      Impl::Object& obj =
        objs
        .emplace(oid, std::make_unique<Impl::Region>())
        .first.operator*().second.operator*();

      EXPECT_NE(objs.find(oid), objs.end());


      obj.id = oid;
      Impl::PtEdge e = Impl::PtEdge{vals[i], vPtrs[i], PTR_TYPE, oid, vals[i]};
      hndl.CreatePtEdge(e);

     

      auto edge = hndl.FindPtEdgeByOffset(vals[i]);
      EXPECT_NE(edge, nullptr);
      EXPECT_EQ(edge->valueType, PTR_TYPE);
      EXPECT_EQ(edge->value, vPtrs[i]);
      EXPECT_EQ(edge->sourceOffset, vals[i]);
      EXPECT_EQ(edge->targetOffset, vals[i]);
      EXPECT_EQ(edge->targetObjectId, oid);
      EXPECT_EQ(hndl.FindPtEdgeByValue(vPtrs[i]), hndl.FindPtEdgeByOffset(vals[i]));
      //EXPECT_EQ(hndl.FindPtEdgeByValueType(vPtrs[i], voidT), hndl.FindPtEdgeByValue(vPtrs[i]));
      EXPECT_EQ(hndl.FindPtEdgeByOffset(ValueId{99}), nullptr);
      EXPECT_EQ(hndl.FindPtEdgeByValue(ValueId(55)), nullptr);
      //EXPECT_EQ(hndl.FindPtEdgeByValueType(vPtrs[i], int32), nullptr);
      
      EXPECT_EQ(hndl.FindHvEdgeByOffset(vals[i]), nullptr);
      EXPECT_EQ(hndl.FindHvEdgeByValue(vPtrs[i]), nullptr);

      EXPECT_EQ(hndl.GetHvOutEdges().size(), 0);
      EXPECT_EQ(hndl.GetPtOutEdges().size(), i+2);
      EXPECT_EQ(hndl.ptEdges.size(), i+2);

      i++;
    }

    objs[o1]->size = v16;
    EXPECT_EQ(objs[o1]->GetSize(), v16);

    objs[o1]->CreateHvEdge(Impl::HvEdge{v0, v1, int16});
    objs[o1]->CreateHvEdge(Impl::HvEdge{v2, v2, int16});
    objs[o1]->CreateHvEdge(Impl::HvEdge{v4, v3, int16});

    objs[o2]->size = v4;
    objs[o2]->CreateHvEdge(v0, v5, int32);

    objs[o1]->CreatePtEdge(Impl::PtEdge{v8, vPtr1, PTR_TYPE, o2, v0});

  }

}

TEST(SMGTest, Graph2)
{
  using namespace Smg;

  Type int8 = Type::CreateIntegerType(8);
  Type int16 = Type::CreateIntegerType(16);
  Type int32 = Type::CreateIntegerType(32);
  Type int64 = Type::CreateIntegerType(64);
  Type int32Ptr = Type::CreatePointerTo(int32);
  Type voidT = Type::CreateVoidType();


  ValueContainer vc{};
  Smg::Impl::Graph g{vc};

  auto& objs = g.objects;
  auto& hndl = g.handles;

  g.handles.CreatePtEdge(Impl::PtEdge{v0, vPtr1, PTR_TYPE, o2, v0});
  Impl::Object& obj =  objs.emplace(o2, std::make_unique<Impl::Region>()).first.operator*().second.operator*();
  obj.CreateHvEdge(Impl::HvEdge{v8, v16, int64});
  obj.CreatePtEdge(Impl::PtEdge{v0, vPtr6, PTR_TYPE, o5, v0});
  obj =  objs.emplace(o5, std::make_unique<Impl::Region>()).first.operator*().second.operator*();
  obj.CreatePtEdge(Impl::PtEdge{v4, vPtr2, PTR_TYPE, o4, v4});
  obj =  objs.emplace(o4, std::make_unique<Impl::Region>()).first.operator*().second.operator*();
  obj.CreatePtEdge(Impl::PtEdge{v0, vPtr3, PTR_TYPE, o0, v0});

  SmgPrinter printer{};
  SmgCrawler crawler{printer};

  crawler.CrawlSmg(Smg::Object{g.handles,g});
  std::string dot = printer.GetDot();
  OsUtils::WriteToFile(dot, "dot_test.dot");
  PrintDot(g, false, "dot_test2.svg"); // not need to use this, but nice to know of it...

}





int main(int argc, char **argv)
{
  Type::InitTypeSystem();

  testing::InitGoogleTest(&argc, argv);
  int res = RUN_ALL_TESTS();
  
  
  
  std::cin.get();
  std::cin.get();
  return res;
}


#if defined(_MSC_VER)

#include "../src/SmgCrawler.cpp"
#include "../src/SmgPrinter.cpp"

#endif
