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

using ObjectId = Impl::ObjectId;
//using Graph    = Impl::Graph;

//class ValueWrapper {
//  ValueId id;
//  Type type;
//  IValueContainer& vc;
//
//  ValueWrapper operator+(const ValueWrapper& rhs) { vc.Add(id, rhs.id, type,  }
//}

class ISmgVisitor;
class Object;

class Graph {
  Impl::Graph& graph;
public:
  //auto GetInEdges();
  void Accept(ISmgVisitor&);
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

//ImplEdgeEnumerable GetEdgesTo(Graph) const;
//ImplEdgeEnumerable GetPtEdgesTo(Graph) const;

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
  void Accept(ISmgVisitor);
  Edge(Impl::Graph& graph, Impl::EdgeBase& edge) : graph{graph}, edge{edge} {}
};
class HvEdge : public Edge {
public:
  ValueId GetValue() { return edge.value; }
  Type    GetType()  { return edge.valueType; }
};
class PtEdge : public HvEdge {
protected:
  Impl::PtEdge& GetEdge() { return static_cast<Impl::PtEdge&>(edge); }
public:
  Object   GetTargetObject()   { return Object{graph, *graph.objects[GetEdge().targetObjectId]}; }
  ObjectId GetTargetObjectId() { return                              GetEdge().targetObjectId; }
  ValueId  GetTargetOffset()   { return                              GetEdge().targetOffset; }
};

//template <class ContainerT>
//auto make_range(ContainerT& vect)
//{
//  return ranges::range<decltype(vect)::iterator>(vect.begin(), vect.end());
//}

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
