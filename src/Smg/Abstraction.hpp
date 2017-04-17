#pragma once

#include "Wrappers.hpp"
#include <vector>

namespace Smg {
namespace Abstraction {

//class Abstraction {


//TODO@michkot: possible optimization, return the linked-to object too
std::pair<bool, DlsOffsets> IsCandidateObject(Object c, IValueContainer& vc)
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

  auto candidateId = c.GetId();
  auto size = c.GetSize();
  auto sizeFilter = [size](PtEdge edge) { return edge.GetTargetObject().GetSize() == size; };

  std::vector<PtEdge> ptEdges = c.GetPtOutEdges() | ranges::to_vector;

  // optional sort?
  // ptEdges |= ranges::sort([vc&](PtEdge a, PtEdge b) { return (bool)vc.IsCmp(a.GetSourceOffset(), b.GetSourceOffset(), PTR_TYPE, CmpFlags::Default) });

  auto end = ptEdges.end();
  for (auto it = ptEdges.begin(); it != end; ++it)
  {
    // 1) cnadidate NF edge
    PtEdge nfEdge = *it;
      
    // 1b) load the target object
    auto obj = nfEdge.GetTargetObject();

    //TODO@michkot: make a template for these == --> vc.IsCmp TODOs
    //TODO@michkot: this is only subset of possible equality, should be vc.IsCmp(.....)
    // 1c) test that the sizes match
    if (obj.GetSize() == size)
    {
      // 2) deduce NFO and HFO
      auto nfo = nfEdge.GetSourceOffset();
      auto hfo = nfEdge.GetTargetOffset();

      // find an edge pointing to the original object wih HF offset
      // if found, succeed, else return to 1)
      for (PtEdge pfEdge : obj.GetPtOutEdges())
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

          DlsOffsets offsets = {hfo, nfo, pfo};
          return {true, offsets};
        }          
      }  
      // PF edge not found
    }
    // failed candidate        
  }

  return {false, {}};
}

void JoinTwoObjects()
{
}
void JoinTwoRegion()
{
}

void LookForCandidates(Impl::Graph& graph, IValueContainer& vc)
{
  for (auto& pair : graph.objects)
  {
    auto obj = Object(*pair.second, graph);
    auto resTup = IsCandidateObject(obj, vc);
    if (resTup.first)
    {
      //auto linkedToObject 
    }
  }
}

//};

}
}