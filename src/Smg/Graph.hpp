#pragma once

#include "Smg_fwd.hpp"
#include "Edge.hpp"
#include "Object.hpp"

#include "../Definitions.hpp"
#include "../Exceptions.hpp"
#include "../Type.hpp"
#include "../Values.hpp"

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

class InvalidDereferenceException_smg : public std::logic_error {
public:
  InvalidDereferenceException_smg() : std::logic_error("") {}
  InvalidDereferenceException_smg(const char* s) : std::logic_error(s) {}
};

namespace Smg {
namespace Impl {

class Graph {
private:

public:
  Region handles;
  std::map<ObjectId, uptr<Object>> objects;

public:

  Graph(IValueContainer& vc)
  {
    ObjectId nullId{0};
    ObjectId unknId{-1};
    auto& nullObj    = objects.emplace(nullId, Region::Create(nullId)).first.operator*().second.operator*();
    auto& unknownObj = objects.emplace(unknId, Region::Create(unknId)).first.operator*().second.operator*();
    nullObj.size    = {};
    unknownObj.size = {};
    handles.CreatePtEdge(PtEdge{{}, {}, PTR_TYPE, ObjectId{0}, {}});
  }
  Graph(const Graph& g) :
    handles(g.handles)
  {
    ////objects = ranges::transform(
    ////  g.objects, 
    ////  [](const decltype(objects)::value_type& kvp) { 
    ////    return decltype(objects)::value_type{kvp.first, std::make_unique<Region>(*kvp.second)}; 
    ////    }
    ////  );
    for (const auto& kvp : g.objects)
    {
      objects.emplace(kvp.first, kvp.second->Clone());
    }
  }


  // Temporary function, should be replaced by CmpPointers or similiar 
  bool ExistsPtEdge(ValueId ptr)
  {
    // The given pointer must be bound to an existing object
    auto objectHandle = handles.FindPtEdgeByValue(ptr);
    return objectHandle != nullptr;
  }

  // Returns PtEdge [object, offset, type] corresponding to given pointer value
  // The given pointer must be bound to an existing object, otherwise it is an undefined behaviour!
  const PtEdge& FindPtEdge(ValueId ptr)
  {
    // The given pointer must be bound to an existing object
    auto objectHandle = handles.FindPtEdgeByValue(ptr);
    assert(objectHandle != nullptr); //TODO: maybe an exception?
    return *objectHandle;
  }
  
  // Returns PtEdge [object, offset, type] corresponding to given pointer value and type
  // The given pointer must be bound to an existing object, otherwise it is an undefined behaviour!
  const PtEdge& FindPtEdge(ValueId ptr, Type type)
  {
    // The given pointer must be bound to an existing object
    auto objectHandle = handles.FindPtEdgeByValueType(ptr, type);
    assert(objectHandle != nullptr); //TODO: maybe an exception?
    return *objectHandle;
  }

  // Returns new pointer to different field [baseOffset + offset, type] of the same object
  auto CreateDerivedPointer(ValueId basePtr, ValueId offset, Type type, IValueContainer& vc)
  {
    auto& baseEdge = FindPtEdge(basePtr);
    auto derivedOffset = vc.Add(baseEdge.targetOffset, offset       , PTR_TYPE, ArithFlags::Default);
    auto derivedValue  = vc.Add(basePtr              , derivedOffset, PTR_TYPE, ArithFlags::Default);
    //TODO@michkot duplicates edges
    auto& derEdge = handles.CreatePtEdge(PtEdge{baseEdge, derivedValue, type, derivedOffset}); 
    //std::vector<int>().em
    return std::make_pair<decltype(derivedValue), ref_wr<std::decay<decltype(derEdge)>::type>>(std::move(derivedValue), derEdge);
  }

  enum class MemorySpace : int8_t {
    Static,
    ThreadLocal,
    Stack,
    Heap
  };

  // Returns a pointer to newly allocated object
  ValueId AllocateRegion(Type type, IValueContainer& vc, MemorySpace ms = MemorySpace::Heap)
  {
    // assign a new ObjectId and new ValueId representing the resultant pointer
    //TODO: different acquisition of ValueId based on MemorySpace

    auto ptrToTypeT = Type::CreatePointerTo(type);

    ObjectId oid = ObjectId::GetNextId();
    ValueId  ptr = vc.CreateVal(ptrToTypeT);//ValueId ::GetNextId();

    // move from new uptr<Region>
    // create new object and place it into map
    auto& obj = objects.emplace(oid, std::make_unique<Region>()).first.operator*().second.operator*();

    // initialize the object
    obj.id = oid;
    obj.size = vc.CreateConstIntVal(type.GetSizeOf());

    handles.CreatePtEdge(PtEdge{ValueId{0}, ptr, ptrToTypeT, oid, vc.GetZero(PTR_TYPE)});

    //TODO: should allocation be allowed to fail, to model malloc, etc. returning null?

    //? if the type is pointer [can happen with llvm.alloca, not with malloc ofc.], create implicit ptedge?
    // problem is: do i want a guarantee, that when Pointer to Pointer value leaves SMGs, 
    //   the target pointer edge already exists 
    //   and i do not need to relly on read on reinterpretation to create one out of nothing?
    // this applies also to ReadValue, when reading Pointer to Pointer value [currently creates new ValId]
    //   and to cast instruction, when Pointer values can "randomly" appear for integers, etc...
    //! it is probably easier just to catch them in "read" "write" "getderivedptr" -> in findptedge
    //!   and to create new edges leading to object -1 ?

    return ptr;
  }

  ////template<typename UniqueOrderedMap, typename ModifiedValueType>
  ////ValueId CreateOrModifyManual(
  ////  UniqueOrderedMap& map,
  ////  UniqueOrderedMap::key_type&& key,
  ////  UniqueOrderedMap::mapped_type&& newValue
  ////  ModifiedValueType&& value,
  ////  ModifiedValueType* accessor = &UniqueOrderedMap::key_type
  ////  )
  ////{
  ////  // src: http://stackoverflow.com/a/101980
  ////
  ////  decltype(map)::iterator lb = map.lower_bound(arg.id);
  ////
  ////  if (lb != map.end() && !(map.key_comp()(arg.id, lb->first)))
  ////  {
  ////    // key already exists
  ////    // update lb->second if you care to
  ////    lb->second::*accessor = std::forward<ModifiedValueType&&>(value);
  ////    return lb->second;
  ////  }
  ////  else
  ////  {
  ////    // the key does not exist in the innerMap
  ////    // add it to the innerMap, use hint
  ////    map.insert(lb, arg.id, std::forward<mapped_type&&>(newValue));
  ////    return newValue;
  ////  }
  ////}

  ValueId ReadValue(ValueId ptr, Type ptrType, Type tarType, IValueContainer& vc)
  {
    auto& ptrEdge = FindPtEdge(ptr, ptrType);

    auto  objectId = ptrEdge.targetObjectId;
    auto& object   = *objects.at(objectId);
    auto  offset   = ptrEdge.targetOffset;

    if (!tarType.IsPointer()) // type is just a value, so it is just a plain HV edge
    {
      if (HvEdge* edge = object.FindHvEdgeByOffset(offset /*, tarType */)) //TODO: we should search by type too!
      { // HvEdge already exists        
        return (*edge).value;
      }
      else
      { // HvEdges does not yet exist -> we should attempt read reinterpretation, if there are any values
        //TODO: should newly allocated object be covered by unknown value of object's size,
        // or there just should not be any HvEdges at all?
        if (object.hvEdges.size() == 0)
        { // Unknown!
          throw InvalidDereferenceException_smg();
        }
        //TODO: read reinterpretation
        throw InvalidDereferenceException_smg(
          "HvEdges for such offset does not yet exists and read re-interpretation is not yet supported"
          );
      }
    }
    else /* type is pointer && and is known pointer; debug if second fails */
    {
      if (PtEdge* edge = object.FindPtEdgeByOffset(offset))
      {
        handles.CreatePtEdge(PtEdge{ValueId{0},*edge});
        return (*edge).value;
      }
      else
      {
        // Throw!
        // Or fallback to HvEdge scenario
        // Or use undefined value / special meaning value
        // Find out whether it is undefined-unknown or abstracted-unknown
        auto status = vc.GetAbstractionStatus(ptr);
        std::cout << AbstractionStatusToString(status) << std::endl;
        //HACK: need this for argv 
        ////throw std::runtime_error{"Writing unknown pointer value"}; 
        return ValueId{(uint64_t)-1};
      }
    }

  }

  // Type is potentially not needed, because the targetPtr must be a pointer to correctly typed edge
  // We use it here to skip the ptrEdge.valueType.GetPointerElementType()
  void WriteValue(ValueId ptr, ValueId value, Type type, IValueContainer& vc)
  {
    auto& ptrEdge = FindPtEdge(ptr, Type::CreatePointerTo(type));

    WriteValue(ptrEdge, value, type, vc);
  }
  void WriteValue(const PtEdge& ptrEdge, ValueId value, Type type, IValueContainer& vc)
  {
    // The pointer target type and the type of value being written must not differ
    assert(ptrEdge.valueType.GetPointerElementType() == type);

    // The object has to be valid for an operation
    // The flag should be stored inside the object, because the edge is unique to [object, offset, type]
    auto  objectId = ptrEdge.targetObjectId;
    auto& object   = *objects.at(objectId);
    auto  offset   = ptrEdge.targetOffset;

    if (!type.IsPointer()) // type is just a value, so it is just a plain HV edge
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
      if (PtEdge* assignedPtr = handles.FindPtEdgeByValue(value))
      {
        object.CreateOrModifyPtEdge(offset, *assignedPtr);
      }
      else
      {
        // Throw!
        // Or fallback to HvEdge scenario
        // Or use undefined value / special meaning value
        // Find out whether it is undefined-unknown or abstracted-unknown
        auto status = vc.GetAbstractionStatus(value);
        std::cout << AbstractionStatusToString(status) << std::endl;
        //HACK: need this for argv 
        ////throw std::runtime_error{"Writing unknown pointer value"}; 
      }
    }

  }

  auto FindPtInEdges(ObjectId id)
  {
    using namespace ranges::view;
    auto objRng = objects
      | transform([=](auto& idObjPair) { return &*idObjPair.second; });
    return concat(objRng, single(&handles))
      | for_each([=](Impl::Object* obj)
    {
      return ranges::yield_from(obj->ptEdges
        | filter([=](Impl::PtEdge& edge) { return edge.targetObjectId == id; })
        | transform([&](Impl::PtEdge& edge) { return std::pair<Object&, PtEdge&>(*obj, edge); }));
    }
    );
  }

  void Free(ValueId ptr)
  {
    auto& graph = *this;
    auto& ptrEdge = FindPtEdge(ptr);

    //TODO: materialize as region, edge gets updates to point to materialized object now

    // Set regions state to invalid
    Region& object = static_cast<Region&>(*graph.objects[ptrEdge.targetObjectId]);

    // Remove references from other objects, set them to invalid [freed] memory
    using ret_t = std::pair<Impl::Object&, Impl::PtEdge&>;
    RANGES_FOR(ret_t pair, graph.FindPtInEdges(object.GetId()))
    {
      pair.second.targetObjectId = ObjectId{-1};
    }

    // Delete the object
    graph.objects.erase(object.GetId());
  }

};

} // namespace Smg::Impl
} // namespace Smg
