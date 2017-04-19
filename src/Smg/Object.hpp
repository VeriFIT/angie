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

namespace {

template<typename T, typename... Args>
static inline auto& CreateEdge(std::vector<T>& container, Args&&... args)
{
  container.emplace_back(std::forward<Args>(args)...);
  return container.back();
}

template<typename RangeT, typename ValueT = typename RangeT::value_type>
static inline auto FindEdgeByOffset(RangeT& range, ValueId offset)
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
static inline auto FindEdgeByValue(RangeT& range, ValueId value)
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
/*
template<typename T>
static inline auto FindEdge(T&& range, ValueId value, Type type)
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

} // anonymous namespace

class Object {

public:

  virtual void Accept(ISmgVisitor& visitor, Impl::Graph& ctx);

  ObjectId id;
  std::vector<HvEdge> hvEdges;
  std::vector<PtEdge> ptEdges;
  int32_t refCounter{0};

  ObjectSize size;
  size_t level{};
  MemorySpace memSpace{MemorySpace::Heap};

  void IncRefCounter() { ++refCounter; }
  void DecRefCounter() { --refCounter; }

private:


  template<typename T, typename... Args>
  static inline auto& CreateOrModifyEdge(std::vector<T>& container, ValueId&& offset, Args&&... args)
  {
    {
      if (T* edge = FindEdgeByOffset(container, offset))
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

public:

  virtual ~Object() = 0;
  ObjectId GetId() const { return id; }
  ObjectSize GetSize() const { return size; }
  size_t GetLevel() const { return level; }
  virtual uptr<Object> Clone() const = 0;

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

  //TODO@michkot: reconsider uses of Create vs CreateOrModify + whether to guarante uniqueness of the edges
  // motivation ->CreateDerivedPointer use currently creating duplicate edges
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

inline ::Smg::Impl::Object::~Object() {}

class Region : public Object {
public:

  bool isValid     = true;
  bool isNullified = false;
  bool isFreed     = false;

  static auto Create(ObjectId id) 
  {
    auto newObj = std::make_unique<Region>(); newObj->id = id; return newObj;
  }
  virtual uptr<Object> Clone() const override { return std::make_unique<std::decay<decltype(*this)>::type>(*this); }
  virtual void Accept(ISmgVisitor& visitor, Impl::Graph& ctx) override;
};

class Sls : public Object {
public:

  size_t minLength;

public:

  size_t GetMinLength() { return minLength; }

  static auto Create(ObjectId id) 
  {
    auto newObj = std::make_unique<Sls>(); newObj->id = id; return newObj;
  }
  virtual uptr<Object> Clone() const override { return std::make_unique<std::decay<decltype(*this)>::type>(*this); }
  virtual void Accept(ISmgVisitor& visitor, Impl::Graph& ctx) override;
};

class Dls : public Object {
public:

  size_t minLength;
  DlsOffsets offsets;

public:

  size_t GetMinLength() { return minLength; }
  DlsOffsets GetOffsets() { return offsets; }

  static auto Create(ObjectId id) 
  {
    auto newObj = std::make_unique<Dls>(); newObj->id = id; return newObj;
  }
  virtual uptr<Object> Clone() const override { return std::make_unique<std::decay<decltype(*this)>::type>(*this); }
  virtual void Accept(ISmgVisitor& visitor, Impl::Graph& ctx) override;
};

class DlsAdapter : Dls {};
class DlsFirstAdapter : DlsAdapter {};
class DlsLastAdapter : DlsAdapter {};

} // namespace Smg::Impl
} // namespace Smg
