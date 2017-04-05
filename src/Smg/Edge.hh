#pragma once

#include "Smg_fwd.hh"
#include "ObjectId.hh"

#include "../Definitions.hh"
#include "../Exceptions.hh"
#include "../Type.hh"
#include "../Values.hh"

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

namespace Smg {
namespace Impl {

class EdgeBase {

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

class HvEdge : public EdgeBase {

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

} // namespace Smg::Impl
} // namespace Smg
