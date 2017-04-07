#pragma once

#include "Graph.hh"

class ISmgVisitor;

namespace Smg {

//template <typename ViewT, typename ObjT>
//class ObjectSet : public SmgSet, ranges::v3::view_facade<ObjectSet<ViewT, ObjT>> {
//private:
//  friend ranges::v3::range_access;
//
//  ViewT view_adaptor;
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
//    return {view_adaptor.begin()};
//  }
//  cursor end_cursor() const
//  {
//    return {view_adaptor.end()};
//  }
//  public:
//  ObjectSet() = default;
//  ObjectSet(ViewT&& rng, Graph& context)
//    : ObjectSet::view_adaptor{std::forward<ViewT>(rng)}
//    , SmgSet(context)
//  {}
//};

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
  auto GetGraph() -> decltype(graph) { return graph; }
};

template <class ViewT>
class ObjectSet : public SmgSet, public ranges::view_adaptor<ObjectSet<ViewT>, ViewT> {
private:
  friend ranges::range_access;
  class adaptor : public ranges::adaptor_base
  {
  public:
    adaptor() = default;
    auto read(ranges::range_iterator_t<ViewT> it) const -> decltype(*it) { return *it; }
  };
  adaptor begin_adaptor() const { return {}; }
  adaptor end_adaptor() const { return {}; } 
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

class Object {
protected:
  Impl::Graph&  graph;
  Impl::Object& object;
public:
  auto GetPtOutEdges() { return objset(object.GetPtOutEdges(), graph); }
  auto GetHvOutEdges() { return objset(object.GetHvOutEdges(), graph); }
  auto GetOutEdges()   { throw NotImplementedException(); }
  auto GetPtInEdges()  { throw NotImplementedException(); }
  ValueId GetSize()    { return object.GetSize(); }
  ObjectId GetId()     { return object.id; }

  void Accept(ISmgVisitor& visitor);
  Object(Impl::Graph& graph, Impl::Object& object) : graph{graph}, object{object} {}
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
};
class Sls : public Object {
public:
  uint16_t GetLevel() { throw NotImplementedException(); }
  uint16_t GetRank()  { throw NotImplementedException(); }
};

class Edge {
protected:
  Impl::Graph&    graph;
  Impl::EdgeBase& edge;
public:
  ValueId  GetSourceOffset() { return edge.sourceOffset; }
  Edge(Impl::Graph& graph, Impl::EdgeBase& edge) : graph{graph}, edge{edge} {}
};
class HvEdge : public Edge {
public:
  ValueId GetValue() { return edge.value; }
  Type    GetType()  { return edge.valueType; }
  void Accept(ISmgVisitor& visitor);
};
class PtEdge : public HvEdge {
protected:
  Impl::PtEdge& GetEdge() { return static_cast<Impl::PtEdge&>(edge); }
public:
  Object   GetTargetObject()   { return Object{graph, *graph.objects[GetEdge().targetObjectId]}; }
  ObjectId GetTargetObjectId() { return                              GetEdge().targetObjectId; }
  ValueId  GetTargetOffset()   { return                              GetEdge().targetOffset; }
  void Accept(ISmgVisitor& visitor);
};

//template <class ContainerT>
//auto make_range(ContainerT& vect)
//{
//  return ranges::range<decltype(vect)::iterator>(vect.begin(), vect.end());
//}

} // namespace Smg
