#pragma once

#include "Graph.hpp"
#include "ObjectId.hpp"
#include "Wrappers_fwd.hpp"

#include <boost/optional.hpp>

class ISmgVisitor;

namespace Smg {


template <class OutT, class ViewT>
auto /*ObjectSeo<ViewT, OutT>*/ objseo(ViewT && rng, Impl::Graph& graph)
{
  return ranges::view::transform(rng, [&](auto& elem){ return OutT{elem, graph}; });
  //return {std::forward<ViewT>(rng), context};
}

class PtEdge;

class Graph {
public:
  Impl::Graph& graph;
  void Accept(ISmgVisitor& visitor) { visitor.Visit(*this); }
  Graph(Impl::Graph& graph) : graph{graph} {}
};

class Object {
protected:
  Impl::Object& object;
  Impl::Graph&  graph;
public:
  Impl::Graph&  GetGraph()      { return graph; }
  Impl::Object& GetObject()     { return static_cast<Impl::Object&>(object); }
  auto          GetPtOutEdges() ;
  auto          GetHvOutEdges() ;
  auto          GetOutEdges()   ;
  auto          GetPtInEdges()  ;
  ObjectId      GetId()         { return object.id; }
  ValueId       GetSize()       { return GetObject().GetSize(); }
  size_t        GetLevel()      { return object.level; }

  // check for existence
  boost::optional<PtEdge> FindPtEdgeByOffset(ValueId offset);
  // only and assert check
  PtEdge GetPtEdgeByOffset(ValueId offset);

  void Accept(ISmgVisitor& visitor) { object.Accept(visitor, graph); }
  Object(Impl::Object& object, Impl::Graph& graph) : object{object}, graph{graph} {}
};
////These functions require access to whole graph, to see which edges ends in this object
//ImplEdgeEnumerable GetEdgesAll() const;
//
//ImplEdgeEnumerable GetEdgesTo(Graph) const;
//ImplEdgeEnumerable GetPtEdgesTo(Graph) const;
//
//ImplObjectEnumerable GetPredecessorObjects() const;
class Region : public Object {
public:
  Impl::Region& GetObject() { return static_cast<Impl::Region&>(object); }
  bool IsValid()            { return GetObject().isValid; }
  bool IsFreed()            { return GetObject().isFreed; }
  bool IsNullified()        { return GetObject().isNullified; }

  Region(Impl::Region& object, Impl::Graph& graph) : Object{object, graph} {}
};
class Sls : public Object {
public:
  Impl::Sls& GetObject() { return static_cast<Impl::Sls&>(object); }
  size_t GetRank()     { return GetObject().minLength; }

  Sls(Impl::Sls& object, Impl::Graph& graph) : Object{object, graph} {}
};
class Dls : public Object {
public:
  Impl::Dls& GetObject() { return static_cast<Impl::Dls&>(object); }
  size_t GetRank()     { return GetObject().minLength; }

  Dls(Impl::Dls& object, Impl::Graph& graph) : Object{object, graph} {}
};

class Edge {
protected:
  Impl::EdgeBase& edge;
  Impl::Graph&    graph;
public:
  ValueId  GetSourceOffset() { return edge.sourceOffset; }
  void Accept(ISmgVisitor& visitor) { edge.Accept(visitor, graph); }
  Edge(Impl::EdgeBase& edge, Impl::Graph& graph) : edge{edge}, graph{graph} {}
};
class HvEdge : public Edge {
public:
  Impl::HvEdge& GetEdge()  { return static_cast<Impl::HvEdge&>(edge); }
  ValueId       GetValue() { return edge.value; }
  Type          GetType()  { return edge.valueType; }
  HvEdge(Impl::HvEdge& edge, Impl::Graph& graph) : Edge{edge, graph} {}
};
class PtEdge : public HvEdge {
public:
  Impl::PtEdge& GetEdge()      { return static_cast<Impl::PtEdge&>(edge); }
  Object   GetTargetObject()   { return Object{*graph.objects[GetEdge().targetObjectId], graph}; }
  ObjectId GetTargetObjectId() { return                       GetEdge().targetObjectId;          }
  ValueId  GetTargetOffset()   { return                       GetEdge().targetOffset;            }
  PtEdge(Impl::PtEdge& edge, Impl::Graph& graph) : HvEdge{edge, graph} {}
};

class ObjectPair {
private:
  Impl::Object& o1;
  Impl::Graph&  graph;
  Impl::Object& o2;
public:
  Object GetFirst()  { return {o1, graph}; }
  Object GetSecond() { return {o2, graph}; }
  ObjectPair(Object first, Object second) 
    : 
    o1{first.GetObject()}, 
    o2{second.GetObject()}, 
    graph{first.GetGraph()}
  {}
  ObjectPair(Impl::Object& first, Impl::Graph& graph, Impl::Object& second) 
    : 
    o1{first},
    o2{second}, 
    graph{graph}
  {}
};

class ObjectPtEdgePair {
private:
  Impl::Object& object;
  Impl::Graph&  graph;
  Impl::PtEdge& edge;
public:
  Object GetObject() { return {object, graph}; }
  PtEdge GetEdge()   { return {edge,   graph}; }
  ObjectPtEdgePair(Object first, PtEdge second) 
    : 
    object{first.GetObject()}, 
    edge{second.GetEdge()}, 
    graph{first.GetGraph()}
  {}
  ObjectPtEdgePair(Impl::Object& first, Impl::Graph& graph, Impl::PtEdge& second) 
    : 
    object{first},
    edge{second}, 
    graph{graph}
  {}
};


inline boost::optional<PtEdge> Object::FindPtEdgeByOffset(ValueId offset) 
{
  auto edgePtr = object.FindPtEdgeByOffset(offset);
  if (edgePtr != nullptr)
    return PtEdge{*edgePtr, graph};
  else
    return {};
}
inline PtEdge Object::GetPtEdgeByOffset(ValueId offset) 
{
  auto edgePtr = object.FindPtEdgeByOffset(offset);
  assert(edgePtr != nullptr && "Edge does not exist");
  return PtEdge{*edgePtr, graph};
}

inline auto Object::GetPtOutEdges() { return objseo<Smg::PtEdge>(object.GetPtOutEdges(), graph); }
inline auto Object::GetHvOutEdges() { return objseo<Smg::HvEdge>(object.GetHvOutEdges(), graph); }
inline auto Object::GetOutEdges()   { return objseo<Smg::Edge>  (object.GetOutEdges(), graph); }

inline auto Object::GetPtInEdges() 
{ 
  return /*ranges::view::empty<ObjectPtEdgePair>();*/graph.FindPtInEdges(object.GetId())
    | ranges::view::transform([&](std::pair<Impl::Object&, Impl::PtEdge&> stdPair) { 
    return ObjectPtEdgePair{stdPair.first, graph, stdPair.second};
    }
    );
}

// move to file Smg_Wrappers_delayed.hpp

// namespace Smg
namespace Impl {

inline void HvEdge::Accept(ISmgVisitor& visitor, Impl::Graph& ctx) { visitor.Visit(Smg::HvEdge{*this, ctx}); }
inline void PtEdge::Accept(ISmgVisitor& visitor, Impl::Graph& ctx) { visitor.Visit(Smg::PtEdge{*this, ctx}); }
inline void Object::Accept(ISmgVisitor& visitor, Impl::Graph& ctx) { visitor.Visit(Smg::Object{*this, ctx}); }
inline void Region::Accept(ISmgVisitor& visitor, Impl::Graph& ctx) { visitor.Visit(Smg::Region{*this, ctx}); }
inline void Dls   ::Accept(ISmgVisitor& visitor, Impl::Graph& ctx) { visitor.Visit(Smg::Dls   {*this, ctx}); }

} // namespace Smg::Impl

} // namespace Smg
