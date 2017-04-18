#pragma once

#include "Wrappers.hpp"
#include <vector>

namespace Smg {
namespace Abstraction {

//class Abstraction {


//TODO@michkot: possible optimization, return the linked-to object too
std::tuple<bool, DlsOffsets, ObjectId> IsCandidateObject(Object c, IValueContainer& vc)
{
  /*
  What are we trying to do: deduce NFO, HFO and PFO and if successful, get the linked-to region

  0) deduce segment size from this region
  1) pick a NF edge (same size as this region) -> note this already requires loading the target object!
  2) deduce NFO and HFO from the NF edge (source and target offset)

  in case of DLS, variant look forth now!
  3) get the NF target object (operating on that from now on) <-- see the remark on 1)
  4) PF must point to the previous object with HFO offset
  5) must it contain an edge on NFO with adequte target size? presume not! [see SMG TR p.15]
  6) DONE

  ------------ other, unfinished, alghorithms

  in case of DLS, variant look for incoming!
  4) find an edge pointing to this object with HFO offset, from [NF target object] -> PFO is its srcOffset
  5) DONE

  in case of DLS, variant look back now! -> previous object might be list head with different size
  3) pick PF as
    a) the next with srcOffset higher then NFO
    b) any with higer srcOffset higher then NFO
    c) any other (will allow for NF a PF to swap)
  4) PF must: point to the same HFO, .... -> if it is list head, we can not merge with it, it is useless....

  in case of SLS - ???

  */

  auto o1 = c;
  auto candidateId = o1.GetId();
  auto size = o1.GetSize();
  auto sizeFilter = [size](PtEdge edge) { return edge.GetTargetObject().GetSize() == size; };

  auto ptEdges = o1.GetPtOutEdges();// | ranges::to_vector;

  // optional sort?
  //// ptEdges |= ranges::sort([vc&](PtEdge a, PtEdge b) { return (bool)vc.IsCmp(a.GetSourceOffset(), b.GetSourceOffset(), PTR_TYPE, CmpFlags::Default) });

  auto end = ptEdges.end();
  for (auto it = ptEdges.begin(); it != end; ++it)
  {
    // 1) candidate NF edge
    PtEdge nfEdge = *it;

    // 1b) load the target object
    auto o2 = nfEdge.GetTargetObject();

    //TODO@michkot: make a template for these == --> vc.IsCmp TODOs
    //TODO@michkot: this is only subset of possible equality, should be vc.IsCmp(.....)
    // 1c) test that the sizes match
    if (o2.GetSize() == size)
    {
      // 2) deduce NFO and HFO
      auto nfo = nfEdge.GetSourceOffset();
      auto hfo = nfEdge.GetTargetOffset();

      // find an edge pointing to the original object with HF offset
      // if found, succeed, else return to 1)
      for (PtEdge pfEdge : o2.GetPtOutEdges())
      {
        if (
          (pfEdge.GetTargetObjectId() == candidateId) &&
          //TODO@michkot: this is only subset of possible equality, should be vc.IsCmp(.....)
          (pfEdge.GetTargetOffset() == hfo)
          )
        {
          // found a PF edge!
          // get the PFO
          auto pfo = pfEdge.GetSourceOffset();

          // Here, we could check whether there exists fields:
          // at PFO on o1
          // at NFO at o2
          // and later keep the algo. from checking for them / using reinterpret cast
          //HACK: doing eager next/prev checking, cause we can not do reinterpret cast yet
          if ((o2.FindPtEdgeByOffset(nfo)) && o1.FindPtEdgeByOffset(pfo))
          {
            DlsOffsets offsets = {hfo, nfo, pfo};
            return{true, offsets, o2.GetId()};
          }
        }
      }
      // PF edge not found
    }
    // failed candidate        
  }

  return{false, {}, {}};
}

struct myless {
  /*constexpr*/ bool operator()(const ref_wr<Impl::Object>& _Left, const ref_wr<Impl::Object>& _Right) const
  {
    return (_Left.get().GetId() < _Right.get().GetId());
  }
};

//TODO make sub-graph dervied from Graph / GraphBase ... we need all objects &&&& edges! in the subgraph
class SubGraph {
  Smg::Object root;
  std::set<ref_wr<Impl::Object>, myless> objects;
  SubGraph(Impl::Object& root, Impl::Graph& graph) :
    root{root, graph}
  {
    Init();
  }
  void Init()
  {
    using namespace ::ranges::view;
    std::queue<ref_wr<Impl::Object>> worklist;
    worklist.push(root.GetObject());

    while (!worklist.empty())
    {
      for (Impl::PtEdge edge : worklist.front().get().GetPtOutEdges())
      {
        auto& objRef = PtEdge{edge, root.GetGraph()}.GetTargetObject().GetObject();
        auto insResult = objects.insert(objRef);
        if (insResult.second) // if newly inserted, queue for processing
          worklist.push(objRef);
      };
      worklist.pop();
    }
  }
};

void JoinTwoSubSmgs()
{
}

void JoinTwoRegions(ObjectPair pair, DlsOffsets offsets);
void JoinTwoObjects(ObjectPair pair, DlsOffsets offsets)
{
  JoinTwoRegions(pair, offsets);
}
void JoinTwoRegions(ObjectPair pair, DlsOffsets offsets)
{
  auto o1 = pair.GetFirst();
  auto o2 = pair.GetSecond();
  auto& graph = o1.GetGraph();
  
  auto nextEdge = o2.GetPtEdgeByOffset(offsets.nfo);
  auto prevEdge = o1.GetPtEdgeByOffset(offsets.pfo);

  auto dlsId = ObjectId::GetNextId();
  auto dlsPtr = Impl::Dls::Create(dlsId);
  auto& dls = *dlsPtr;
  dls.offsets = offsets;
  dls.minLength = 2;
  dls.CreatePtEdge(Impl::PtEdge{nextEdge.GetEdge()});
  dls.CreatePtEdge(Impl::PtEdge{prevEdge.GetEdge()});
  
  // put the DLS to the graph
  graph.objects.emplace(dlsId, std::move(dlsPtr));
  //! use DlsFront and DlsBack instead of just DLS -> we need the target specifiers

  // redirect edges to the DLS
  RANGES_FOR(ObjectPtEdgePair incEdge, o1.GetPtInEdges())
  {
    incEdge.GetEdge().GetEdge().targetObjectId = dlsId;
  }
  RANGES_FOR(ObjectPtEdgePair incEdge, o2.GetPtInEdges())
  {
    incEdge.GetEdge().GetEdge().targetObjectId = dlsId;
  }
  
  //HACK: should be cleaning up the whole SMG after everything is done
  // erase the o1 and o2 from graph
  auto o1Id = o1.GetId();
  auto o2Id = o2.GetId();
  graph.objects.erase(o1Id);
  graph.objects.erase(o2Id);

  // following is needed only for subSMG joining
  // null the edges to cut the SMGs
  ////nextEdge.GetEdge() = Impl::PtEdge::GetNullPtr();
  ////prevEdge.GetEdge() = Impl::PtEdge::GetNullPtr();
}

bool LookForCandidates(Impl::Graph& graph, IValueContainer& vc)
{
  for (auto& pair : graph.objects)
  {
    auto obj = Object(*pair.second, graph);
    auto resTup = IsCandidateObject(obj, vc);
    if (std::get<bool>(resTup))
    {
      auto linkedToObjectId = std::get<ObjectId>(resTup);
      auto linkedToObject = Object{*graph.objects[linkedToObjectId], graph};
      JoinTwoObjects({obj, linkedToObject}, std::get<DlsOffsets>(resTup));
      return true;
    }
  }
  return false;
}

//};

}
}