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
/** @file Definitions.hpp */

#pragma once

#include "Exceptions.hpp"

#include <memory>
#include <functional>
#include <vector>
#include <queue>
#include <list>
#include <set>

#include <gsl/gsl>

template<class T>
using uptr = ::std::unique_ptr<T>;
template<class T>
using ref_wr = ::std::reference_wrapper<T>;
template<class T>
using ref_vector = ::std::vector<::std::reference_wrapper<T>>;
template<class T>
using ref_queue = ::std::queue<::std::reference_wrapper<T>>;
template<class T>
using ref_deque = ::std::deque<::std::reference_wrapper<T>>;
template<class T>
using ref_list = ::std::list<::std::reference_wrapper<T>>;
template<class T>
using ref_set = ::std::set<::std::reference_wrapper<T>>;
template<class T>
using ref_span = ::gsl::span<std::reference_wrapper<T>>;

#define STL_UNWRAP(stl_obj) decltype(stl_obj)::value_type::type
#define STL_ITEM_T(stl_obj) decltype(stl_obj)::value_type

template<typename T> 
T& deconst_cast(const T& const_value) { return const_cast<T&>(const_value); }
