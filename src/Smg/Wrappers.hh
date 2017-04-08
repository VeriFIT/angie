#pragma once

#include "Graph.hh"

class ISmgVisitor;

namespace Smg {

//class ValueWrapper {
//  ValueId id;
//  Type type;
//  IValueContainer& vc;
//
//  ValueWrapper operator+(const ValueWrapper& rhs) { vc.Add(id, rhs.id, type,  }
//}

using ObjectId = Impl::ObjectId;
class Object;

/*
Wrappers:

ImplObject { vector<hrany>; nejake status }
objekt.DejMiVsehcnyHranyCoNaMeVedou() ???
smg.DejMiHrany.....(objekt)

Object -> GetOutEdges();
Region : Object

HvEdge : BaseEdge
PtEdge : BaseEdge

BaseEdge
*/

class Graph {
  Impl::Graph& graph;
public:
  //auto GetInEdges();
  void Accept(ISmgVisitor& visitor);
  Graph(Impl::Graph& graph) : graph{graph} {}
};

class SmgSet{
  Impl::Graph& graph;
public:
  SmgSet(decltype(graph) graph) : graph{graph} {}
  auto GetGraph() const -> decltype(graph) { return graph; }
};

template <class ViewT, class OutT>
class ObjectSeo : public SmgSet, public ranges::view_adaptor<ObjectSeo<ViewT, OutT>, ViewT> {
private:
  friend ranges::range_access;
  class adaptor : public ranges::adaptor_base
  {
    Impl::Graph* const graph;
  public:
    adaptor() = default;
    adaptor(Impl::Graph& g) : graph{&g} {};
    OutT current(ranges::range_iterator_t<ViewT> it) const {
      return {*it, *graph};
    }
  };
  adaptor begin_adaptor() const { return {GetGraph()}; }
  adaptor end_adaptor()   const { return {GetGraph()}; } 
public:
  ObjectSeo() = default;
  ObjectSeo(ViewT&& rng, Impl::Graph& context)
    : ObjectSeo::view_adaptor{std::forward<ViewT>(rng)}
    , SmgSet(context)
  {}
};

template <class ViewT>
class ObjectSet : public SmgSet, public ranges::view_adaptor<ObjectSet<ViewT>, ViewT> {
private:
  friend ranges::range_access;
public:
  typename std::remove_reference<ViewT>::type::iterator begin()
  {
    return ranges::view_adaptor<ObjectSet<ViewT>, ViewT>::base().begin();
  }
  typename std::remove_reference<ViewT>::type::iterator end()
  {
    return ranges::view_adaptor<ObjectSet<ViewT>, ViewT>::base().end();
  }
  ObjectSet() = default;
  ObjectSet(ViewT&& rng, Impl::Graph& context)
    : ObjectSet::view_adaptor{std::forward<ViewT>(rng)}
    , SmgSet(context)
  {}
};

//
//template <class ViewT>
//class ObjectSet : public SmgSet, ranges::v3::view_facade<ObjectSet<ViewT>> {
//private:
//  friend ranges::v3::range_access;
//
//  ViewT& container;
//
//  struct cursor
//  {
//  private:
//    ranges::range_iterator_t<ViewT> iter;
//  public:
//    cursor() = default;
//    cursor(ranges::range_iterator_t<ViewT> it)
//      : iter(it)
//    {}
//    int const & read() const
//    {
//      return *iter;
//    }
//    bool equal(cursor const &that) const
//    {
//      return iter == that.iter;
//    }
//    void next()
//    {
//      ++iter;
//    }
//    void prev()
//    {
//      --iter;
//    }
//    std::ptrdiff_t distance_to(cursor const &that) const
//    {
//      return that.iter - iter;
//    }
//    void advance(std::ptrdiff_t n)
//    {
//      iter += n;
//    }
//  };
//  cursor begin_cursor() const
//  {
//    return {container.begin()};
//  }
//  cursor end_cursor() const
//  {
//    return {container.end()};
//  }
//  public:
//  ObjectSet() = default;
//  ObjectSet(ViewT& rng, Graph& context)
//    : container{rng}
//    , SmgSet(context)
//  {}
//};

template <class ViewT>
ObjectSet<ViewT> objset(ViewT && rng, Impl::Graph& context)
{
  return {std::forward<ViewT>(rng), context};
}

template <class OutT, class ViewT>
ObjectSeo<ViewT, OutT> objseo(ViewT && rng, Impl::Graph& context)
{
  return {std::forward<ViewT>(rng), context};
}

class Edge;
class HvEdge;
class PtEdge;

class Object {
protected:
  Impl::Object& object;
  Impl::Graph&  graph;
public:
  auto GetPtOutEdges() { return objseo<Smg::PtEdge>(object.GetPtOutEdges(), graph); }
  auto GetHvOutEdges() { return objseo<Smg::HvEdge>(object.GetHvOutEdges(), graph); }
  auto GetOutEdges()   { return objseo<Smg::Edge>  (object.GetPtOutEdges(), graph); }
  auto GetPtInEdges()  { throw NotImplementedException(); }
  ValueId GetSize()    { return object.GetSize(); }
  ObjectId GetId()     { return object.id; }

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
  bool IsValid()     { throw NotImplementedException(); }
  bool IsFreed()     { throw NotImplementedException(); }
  bool IsNullified() { throw NotImplementedException(); }
  Region(Impl::Region& object, Impl::Graph& graph) : Object{object, graph} {}
};
class Sls : public Object {
public:
  uint16_t GetLevel() { throw NotImplementedException(); }
  uint16_t GetRank()  { throw NotImplementedException(); }
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
  ValueId GetValue() { return edge.value; }
  Type    GetType()  { return edge.valueType; }
  HvEdge(Impl::HvEdge& edge, Impl::Graph& graph) : Edge{edge, graph} {}
};
class PtEdge : public HvEdge {
protected:
  Impl::PtEdge& GetEdge() { return static_cast<Impl::PtEdge&>(edge); }
public:
  Object   GetTargetObject()   { return Object{*graph.objects[GetEdge().targetObjectId], graph}; }
  ObjectId GetTargetObjectId() { return                       GetEdge().targetObjectId;          }
  ValueId  GetTargetOffset()   { return                       GetEdge().targetOffset;            }
  PtEdge(Impl::PtEdge& edge, Impl::Graph& graph) : HvEdge{edge, graph} {}
};

// move to file Smg_Wrappers_delayed.hh

// namespace Smg
namespace Impl {

inline void HvEdge::Accept(ISmgVisitor& visitor, Impl::Graph& ctx) { visitor.Visit(Smg::HvEdge{*this, ctx}); }
inline void PtEdge::Accept(ISmgVisitor& visitor, Impl::Graph& ctx) { visitor.Visit(Smg::PtEdge{*this, ctx}); }
inline void Object::Accept(ISmgVisitor& visitor, Impl::Graph& ctx) { visitor.Visit(Smg::Object{*this, ctx}); }
inline void Region::Accept(ISmgVisitor& visitor, Impl::Graph& ctx) { visitor.Visit(Smg::Region{*this, ctx}); }

} // namespace Smg::Impl

} // namespace Smg
