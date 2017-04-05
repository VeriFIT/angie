#pragma once

#include "../Values.hh"

using ValueIdTypePair = std::tuple<ValueId, Type>;

namespace Smg {
namespace Impl {

class Object;
typedef Object IObject;

class EdgeBase;

using ObjectSize = ValueId;
using ObjectType = int; // not used

} // namespace Smg::Impl
} // namespace Smg
