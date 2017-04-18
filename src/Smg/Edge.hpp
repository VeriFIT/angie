#pragma once

#include "Smg_fwd.hpp"
#include "ObjectId.hpp"

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

class EdgeBase {

public:

  virtual void Accept(ISmgVisitor& visitor, Impl::Graph& ctx) = 0;

  ValueId sourceOffset;
  ValueId value;
  Type    valueType; // potential place for memory optimisation, union with object type for PtEdge

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

inline std::ostream& operator<<(std::ostream& os, const EdgeBase& edge)
{
  edge.Print(os);
  return os;
}

class HvEdge : public EdgeBase {

public:

  virtual void Accept(ISmgVisitor& visitor, Impl::Graph& ctx) override;

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

  // TODO: using for the private copy and move constructors and operator

};

class PtEdge : public HvEdge {

public:

  virtual void Accept(ISmgVisitor& visitor, Impl::Graph& ctx) override;

  ObjectId targetObjectId; // optimization, could be replaced by map with value(address)<->object in Smg
  ValueId  targetOffset;

  static const PtEdge GetNullPtr()
  {
    return PtEdge{ValueId{0}, ValueId{0}, PTR_TYPE, ObjectId{0}, ValueId{0}};
  }

  /*ctr*/ PtEdge(ValueId sourceOffset, ValueId value, Type type, ObjectId targetObjectId, ValueId targetOffset) :
    HvEdge(sourceOffset, value, type),
    targetObjectId{targetObjectId},
    targetOffset{targetOffset}
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

} // namespace Smg::Impl
} // namespace Smg
