#include "Smg_fwd.hpp"
#include "Edge.hpp"

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

#include "../ISmgVisitor.hpp"

namespace Smg {
namespace Impl {

template<typename T>
struct ModificationObserver;

template<>
struct ModificationObserver<HvEdge> {
  void operator()(const HvEdge&) {};
};

template<>
struct ModificationObserver<PtEdge> {
  void operator()(const PtEdge& edge) { (void)edge.targetObjectId; };
};

// Interface for Smg::Object
// contains virtual methods' declarations

// Base implementation of Object
// Base implementation of Object
class Object {

public:

  virtual void Accept(ISmgVisitor& visitor, Impl::Graph& ctx);

  ObjectId id;
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
    auto res = find(range, offset, &EdgeBase::sourceOffset);
    if (res != end(range))
    {
      return &*res;
    }
    else
    {
      return (decltype(&*res))nullptr;
    }
  }
  template<typename RangeT, typename ValueT = typename RangeT::value_type>
  static auto FindEdgeByValue(RangeT& range, ValueId value)
  {
    using namespace ::ranges;
    auto res = find(range, value, &EdgeBase::value);
    if (res != end(range))
    {
      return &*res;
    }
    else
    {
      return (decltype(&*res))nullptr;
    }
  }
  template<typename RangeT, typename ValueT = typename RangeT::value_type>
  static auto FindEdgeByValueType(RangeT& range, ValueId value, Type type)
  {
    using namespace ::ranges;
    auto res = find_if(range, [=](const ValueT& ed) { return ed.value == value && ed.valueType == type; });
    if (res != end(range))
    {
      return &*res;
    }
    else
    {
      return (decltype(&*res))nullptr;
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

  //Relies on GetPtOutEdges  //GetSucessors
        auto  GetOutEdges()         { return ::ranges::view::concat(hvEdges, ptEdges); }
  const auto  GetOutEdges() const   { return ::ranges::view::concat(hvEdges, ptEdges); }
        auto& GetHvOutEdges()       { return hvEdges; }
  const auto& GetHvOutEdges() const { return hvEdges; }
        auto& GetPtOutEdges()       { return ptEdges; }
  const auto& GetPtOutEdges() const { return ptEdges; }

        HvEdge* FindHvEdgeByValue     (ValueId value )       { return FindEdgeByValue (hvEdges, value ); }
  const HvEdge* FindHvEdgeByValue     (ValueId value ) const { return FindEdgeByValue (hvEdges, value ); }

        PtEdge* FindPtEdgeByValue     (ValueId value )       { return FindEdgeByValue (ptEdges, value ); }
  const PtEdge* FindPtEdgeByValue     (ValueId value ) const { return FindEdgeByValue (ptEdges, value ); }

        HvEdge* FindHvEdgeByOffset    (ValueId offset)       { return FindEdgeByOffset(hvEdges, offset); }
  const HvEdge* FindHvEdgeByOffset    (ValueId offset) const { return FindEdgeByOffset(hvEdges, offset); }

        PtEdge* FindPtEdgeByOffset    (ValueId offset)       { return FindEdgeByOffset(ptEdges, offset); }
  const PtEdge* FindPtEdgeByOffset    (ValueId offset) const { return FindEdgeByOffset(ptEdges, offset); }
  
        PtEdge* FindPtEdgeByValueType (ValueId value, Type type)       { return FindEdgeByValueType (ptEdges, value, type); }
  const PtEdge* FindPtEdgeByValueType (ValueId value, Type type) const { return FindEdgeByValueType (ptEdges, value, type); }

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

public:

  bool isValid = true;
  bool isNullified = false;
  bool isFreed = false;

  virtual void Accept(ISmgVisitor& visitor, Impl::Graph& ctx);

  ObjectSize size;

public:
  ObjectSize GetSize() const { return size; }

};

} // namespace Smg::Impl
} // namespace Smg
