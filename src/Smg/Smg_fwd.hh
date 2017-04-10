#pragma once

#include "../Values.hh"

using ValueIdTypePair = std::tuple<ValueId, Type>;

namespace Smg {
namespace Impl {

class EdgeBase;
class Object;
class Graph;

using ObjectSize = ValueId;
using ObjectType = int; // not used

} // namespace Smg::Impl
} // namespace Smg
