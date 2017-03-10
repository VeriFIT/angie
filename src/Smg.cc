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
/** @file Smg.cc */

#include "Definitions.hh"
#include "Exceptions.hh"
#include "Type.hh"
#include "Values.hh"
//#include <gsl/gsl_algorithm>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <string>
#include <sstream>

#include <iostream>

#include <tuple>

//#include <gsl/gsl>
#include <range/v3/all.hpp>

typedef int FrontendId;
using ValueIdTypePair = std::tuple<ValueId, Type>;

namespace Smg {

namespace Impl {

class Object;
typedef Object IObject;

typedef size_t ObjectSize;
typedef int ObjectType; // not used

class ObjectIdToken;
using ObjectId = Id<ObjectIdToken>;

struct EdgeBase {

public:

  ValueId sourceOffset;
  ValueId value;
  Type    valueType; // potential place for memory optimisation, union with objecttype for PtEdge

  virtual void Print(std::ostream& os) const = 0;
  std::string ToString() const
  {
    std::ostringstream ss;
    Print(ss);
    return ss.str();
  }

  constexpr
    bool operator<(const ValueId& offset) const { return sourceOffset < offset; }

protected:

  constexpr /*ctr*/ EdgeBase(ValueId sourceOffset, ValueId value, Type type) :
    sourceOffset{sourceOffset},
    value{value},
    valueType{type}
  {
  }

  // TODO: private copy and move constructors and operator

};

std::ostream& operator<<(std::ostream& os, const EdgeBase& edge)
{
  edge.Print(os);
  return os;
}

struct HvEdge : EdgeBase {

public:
  /*ctr*/ HvEdge(ValueId sourceOffset, ValueId value, Type type) :
    EdgeBase(sourceOffset, value, type)
  {
  }

  void Modify(ValueId value, Type type)
  {
    this->value = value;
    this->valueType = type;
  }

  virtual void Print(std::ostream& os) const override
  {
    os << "srcOffset: " << sourceOffset << ", value: " << value;
  }

  // TODO: using for the priate copy and move constructors and operator

};

class PtEdge : public HvEdge {

public:

  ObjectId targetObjectId; // optimization, could be replaced by map with value(address)<->object in Smg
  ValueId  targetOffset;

  /*ctr*/ PtEdge(ValueId sourceOffset, ValueId value, Type type, ObjectId targetObjectId, ValueId targetOffset) :
    targetObjectId{targetObjectId},
    targetOffset{targetOffset},
    HvEdge(sourceOffset, value, type)
  {
  }

  void Modify(ValueId value, Type type, ObjectId targetObjectId, ValueId targetOffset)
  {
    this->value = value;
    this->valueType = type;
    this->targetObjectId = targetObjectId;
    this->targetOffset = targetOffset;
  }

  // Copy source edge in place of this and restore Offset
  void Modify(const PtEdge& baseEdge)
  {
    auto tmpSourceOffset = sourceOffset;
    *this = baseEdge;
    sourceOffset = tmpSourceOffset;
  }

  // From existing PtEdge, different source, same target
  PtEdge(ValueId offset, const PtEdge& baseEdge) :
    PtEdge{baseEdge}
  {
    sourceOffset = offset;
  }

  // From existing PtEdge, same source, same target object, different target field
  PtEdge(const PtEdge& baseEdge, ValueId value, Type type, ValueId targetOffset) :
    targetObjectId{baseEdge.targetObjectId},
    targetOffset{targetOffset},
    HvEdge(baseEdge.sourceOffset, value, type)
  {
  }

  virtual void Print(std::ostream& os) const override
  {
    HvEdge::Print(os);
    os << " target: " << targetObjectId;
  }

};


template<typename T>
struct ModificationObserver;

template<>
struct ModificationObserver<HvEdge>
{
  void operator()(const HvEdge&) {};
};

template<>
struct ModificationObserver<PtEdge> {
  void operator()(const PtEdge& edge) { (void)edge.targetObjectId; };
};

// Interface for Smg::Object
// contains virtual methods' declarations

// Base implementation of Object
class Object {

public:

  ObjectId id;
  ObjectSize size;
  std::vector<HvEdge> hvEdges;
  std::vector<PtEdge> ptEdges;
  int32_t refCounter{0};

  void IncRefCounter() { ++refCounter; }
  void DecRefCounter() { --refCounter; }

private:

  template<typename T, typename... Args>
  static inline auto& CreateEdge(std::vector<T>& container, Args&&... args)
  {
    container.emplace_back(std::forward<Args>(args)...);
    return container.back();
  }

  template<typename T, typename... Args>
  static inline auto& CreateOrModifyEdge(std::vector<T>& container, ValueId&& offset, Args&&... args)
  {
    {
      T* edge;
      if (edge = FindEdgeByOffset(container, offset))
      {
        //TODO: edge modification -> should have a reference counter, when the original value is no longer accessible - FOR OBJECTS, DEFINITELY
        ModificationObserver<T>{}(*edge);
        edge->Modify(std::forward<Args>(args)...);
        return *edge;
      }
    }
    // In case we are using container different than vector, also provide an insertion hint
    return CreateEdge(container, std::forward<ValueId>(offset), std::forward<Args>(args)...);
  }

  template<typename RangeT, typename ValueT = typename RangeT::value_type>
  static auto FindEdgeByOffset(RangeT& range, ValueId offset)
  {
    using namespace ::ranges;
    auto res = ::ranges::find(range, offset, &EdgeBase::sourceOffset);
    if (res != ::ranges::end(range))
    {
      return &*res;
    }
    else
    {
      nullptr;
    }
  }
  template<typename RangeT, typename ValueT = typename RangeT::value_type>
  static auto FindEdgeByValue(RangeT& range, ValueId value)
  {
    using namespace ::ranges;
    auto res = ::ranges::find(range, value, &EdgeBase::value);
    if (res != ::ranges::end(range))
    {
      return &*res;
    }
    else
    {
      nullptr;
    }
  }
  /*
  template<typename T>
  static auto FindEdge(T&& range, ValueId value, Type type)
  {
  using namespace ::ranges;
  auto res = find_if(range, [const=](auto& edge) { return edge.value == value && edge.valueType == type});
  if (res != end(range))
  {
  return &*res;
  }
  else
  {
  nullptr;
  }
  }
  */

public:

  virtual ~Object() = default;

  ObjectId GetId() const { return id; }
  ObjectSize GetSize() const { return size; }

  //Relies on GetPtOutEdges  //GetSucessors
  const auto  GetOutEdges() const   { return ::ranges::view::concat(hvEdges, ptEdges); }
  const auto& GetHvOutEdges() const { return hvEdges; }
  const auto& GetPtOutEdges() const { return ptEdges; }

        HvEdge* FindHvEdgeByValue (ValueId offset)       { return FindEdgeByValue(hvEdges, offset); }
  const HvEdge* FindHvEdgeByValue (ValueId offset) const { return FindEdgeByValue(hvEdges, offset); }

        PtEdge* FindPtEdgeByValue (ValueId offset)       { return FindEdgeByValue(ptEdges, offset); }
  const PtEdge* FindPtEdgeByValue (ValueId offset) const { return FindEdgeByValue(ptEdges, offset); }

        HvEdge* FindHvEdgeByOffset(ValueId offset)       { return FindEdgeByOffset(hvEdges, offset); }
  const HvEdge* FindHvEdgeByOffset(ValueId offset) const { return FindEdgeByOffset(hvEdges, offset); }

        PtEdge* FindPtEdgeByOffset(ValueId offset)       { return FindEdgeByOffset(ptEdges, offset); }
  const PtEdge* FindPtEdgeByOffset(ValueId offset) const { return FindEdgeByOffset(ptEdges, offset); }

  template<typename... Args>
  HvEdge& CreateHvEdge(Args&&... args) { return CreateEdge(hvEdges, std::forward<Args>(args)...); }
  template<typename... Args>
  PtEdge& CreatePtEdge(Args&&... args) { return CreateEdge(ptEdges, std::forward<Args>(args)...); }

  template<typename... Args>
  HvEdge& CreateOrModifyHvEdge(ValueId offset, Args&&... args)
  { return CreateOrModifyEdge(hvEdges, std::forward<ValueId>(offset), std::forward<Args>(args)...); }
  template<typename... Args>
  PtEdge& CreateOrModifyPtEdge(ValueId offset, Args&&... args)
  { return CreateOrModifyEdge(ptEdges, std::forward<ValueId>(offset), std::forward<Args>(args)...); }

};

class Region : public Object {

};

class Graph {
private:

  Object handles;
  std::map<ObjectId, uptr<Object>> objects;

public:

  Graph() = default;
  Graph(const Graph& g) :
    handles(g.handles),
    vc(g.vc)
  {
    //objects = ranges::transform(
    //  g.objects, 
    //  [](const decltype(objects)::value_type& kvp) { 
    //    return decltype(objects)::value_type{kvp.first, std::make_unique<Object>(*kvp.second)}; 
    //    }
    //  );
    for (const auto& kvp : g.objects)
    {
      objects.emplace(kvp.first, std::make_unique<Object>(*kvp.second));
    }
  }


  IValueContainer* vc;
  //gsl::not_null<IValueContainer*> vc;
  IValueContainer& GetVc() { return *vc; }

  // Returns PtEdge [object, offset, type] corresponding to given pointer value
  // The given pointer must be bound to an existing object, otherwise it is an undefined behaviour!
  const PtEdge& FindPtEdge(ValueId ptr)
  {
    // The given pointer must be bound to an existing object
    auto objectHandle = handles.FindPtEdgeByValue(ptr);
    assert(objectHandle != nullptr); //TODO: maybe an exception?
    return *objectHandle;
  }

  // Returns new pointer to different field [baseOffset + offset, type] of the same object
  auto CreateDerivedPointer(ValueId basePtr, ValueId offset, Type type)
  {
    auto& baseEdge = FindPtEdge(basePtr);
    auto derivedOffset = GetVc().Add(baseEdge.targetOffset, offset       , PTR_TYPE, ArithFlags::Default);
    auto derivedValue  = GetVc().Add(basePtr              , derivedOffset, PTR_TYPE, ArithFlags::Default);
    auto& derEdge = handles.CreatePtEdge(PtEdge{baseEdge, derivedValue, type, derivedOffset});
    //std::vector<int>().em
    return std::make_pair(derivedValue, derEdge);
  }

  enum class MemorySpace : int8_t {
    Static,
    ThreadLocal,
    Stack,
    Heap
  };

  // Returns a pointer to newly allocated object
  ValueId AllocateObject(Type type, MemorySpace ms = MemorySpace::Heap)
  {
    // assign a new ObjectId and new ValueId representing the resultant pointer
    //TODO: different aquisition of ValueId based on MemorySpace

    ObjectId oid = ObjectId::GetNextId();
    ValueId  ptr = ValueId ::GetNextId();

    // move from new uptr<Object>
    // create new object and place it into map
    auto& obj = objects.emplace(oid, std::make_unique<Object>()).first.operator*().second.operator*();

    // initialize the object
    obj.id = oid;
    obj.size = type.GetSizeOf();

    return ptr;
  }

  //template<typename UniqueOrderedMap, typename ModifiedValueType>
  //ValueId CreateOrModifyManual(
  //  UniqueOrderedMap& map,
  //  UniqueOrderedMap::key_type&& key,
  //  UniqueOrderedMap::mapped_type&& newValue
  //  ModifiedValueType&& value,
  //  ModifiedValueType* accessor = &UniqueOrderedMap::key_type
  //  )
  //{
  //  // src: http://stackoverflow.com/a/101980

  //  decltype(map)::iterator lb = map.lower_bound(arg.id);

  //  if (lb != map.end() && !(map.key_comp()(arg.id, lb->first)))
  //  {
  //    // key already exists
  //    // update lb->second if you care to
  //    lb->second::*accessor = std::forward<ModifiedValueType&&>(value);
  //    return lb->second;
  //  }
  //  else
  //  {
  //    // the key does not exist in the innerMap
  //    // add it to the innerMap, use hint
  //    map.insert(lb, arg.id, std::forward<mapped_type&&>(newValue));
  //    return newValue;
  //  }
  //}

  // Type is potentially not needed, because the targetPtr must be a pointer to correctly typed edge
  // We use it here to skip the ptrEdge.valueType.GetPointerElementType()
  void WriteValue(ValueId ptr, ValueId value, Type type)
  {
    auto& ptrEdge = FindPtEdge(ptr);

    // The pointer target type and the type of value beeing written must not differ
    assert(ptrEdge.valueType.GetPointerElementType() == type);

    // The object has to be valid for an operation
    // The flag should be stored inside the object, because the edge is unique to [object, offset, type]
    auto  objectId = ptrEdge.targetObjectId;
    auto& object   = *objects.at(objectId);
    auto  offset   = ptrEdge.targetOffset;

    if (!ptrEdge.valueType.IsPointer()) // it is just a plain HV edge
    {
      object.CreateOrModifyHvEdge(offset, value, type);

      //// Repeat the same logic - does the edge exists? -> modify or create
      //HvEdge* edge;
      //if (edge = object.FindHvEdge(offset))
      //{ // Then modify it
      //  //TODO: edge modification -> should have a reference counter, when the original value is no longer accessible - FOR VALUES? PROBABLY NO
      //  edge->value = value;
      //}
      //else
      //{ // Create new edge
      //  object.CreateHvEdge(HvEdge{offset, value, type});
      //}

    }
    else /* type is pointer && and is known pointer; debug if second fails */
    {
      if (PtEdge* assignedPtr = handles.FindPtEdgeByOffset(value))
      {
        object.CreateOrModifyPtEdge(offset, *assignedPtr);
      }
      else
      {
        // Throw!
        // Or fallback to HvEdge scenario
        // Or use undefined value / special meaning value
        // Find out whether it is undefined-unknown or abstracted-unknown
        auto status = GetVc().GetAbstractionStatus(value);
        std::cout << AbstractionStatusToString(status) << std::endl;
        throw std::runtime_error{"Writing unknown pointer value"};
      }
    }

  }


};


} // namespace Smg::Impl

// namespace Smg

using ObjectId = Impl::ObjectId;
//using Graph    = Impl::Graph;

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


//class ValueWrapper {
//  ValueId id;
//  Type type;
//  IValueContainer& vc;
//
//  ValueWrapper operator+(const ValueWrapper& rhs) { vc.Add(id, rhs.id, type,  }
//}

class ISmgVisitor;
class Object;

class Edge {
private:
  Impl::EdgeBase& edge;
  Impl::Graph&    graph;
public:
  ValueId  GetSourceOffset();
  void Accept(ISmgVisitor);
};
class HvEdge : public Edge {
public:
  ValueId  GetValue();
  Type     GetType();
};
class PtEdge : public HvEdge {
public:
  //ObjectId GetTargetObjectId(); //GetTargetBasePtr();
  Object& GetTargetObject();
  ValueId GetTargetOffset();
};
class Object {
private:
  Impl::Object& object;
  Impl::Graph&  graph;
public:
  auto GetPtOutEdges();
  auto GetHvOutEdges();
  auto GetOutEdges();
  auto GetPtInEdges();
  ValueId GetSize();
  ObjectId GetId();
  void Accept(ISmgVisitor&);
};
////These functions require access to whole graph, to see which edges ends in this object
//ImplEdgeEnumerable GetEdgesAll() const;

//ImplEdgeEnumerable GetEdgesTo(Graph) const;
//ImplEdgeEnumerable GetPtEdgesTo(Graph) const;

//ImplObjectEnumerable GetPredecessorObjects() const;
class Region : public Object {
public:
  bool IsValid();
  bool IsFreed();
  bool IsNullified();
};
class Sls : public Object {
public:
  uint16_t GetLevel();
  uint16_t GetRank();
};
class Graph {
  Impl::Graph& graph;
private:
  //auto GetInEdges();
  void Accept(ISmgVisitor&);
};

} // namespace Smg


void playground()
{
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


template<>
Smg::Impl::ObjectId Smg::Impl::ObjectId::nextIdToGive{};
