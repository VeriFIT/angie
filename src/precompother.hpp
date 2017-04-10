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
/** @file precompother.hpp */

#pragma once

#if defined(_MSC_VER)

#include <vector>
#include <map>
#include <set>

#include <algorithm>
#include <functional>

#include <iostream>
#include <string>
#include <sstream>

#pragma warning(push, 3)
#include <range/v3/all.hpp>
#pragma warning(pop)

#pragma warning(push, 3)
#include <gsl/gsl>
#pragma warning(pop)

#include <boost/utility/string_view.hpp>
#include <boost/logic/tribool.hpp>

#endif
