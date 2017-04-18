#pragma once

#include "../IdImpl.hpp"

namespace Smg {
namespace Impl {

  class ObjectIdToken;
  using ObjectId = Id<ObjectIdToken, int64_t, 1>;
}
}