#pragma once

#include "../Values.hpp"

using ValueIdTypePair = std::tuple<ValueId, Type>;

namespace Smg {
namespace Impl {

class EdgeBase;
class Region;
class Graph;

using ObjectSize = ValueId;
using ObjectType = int; // not used

} // namespace Smg::Impl
} // namespace Smg
