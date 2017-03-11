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
/** @file Exceptions.hh */

#pragma once

#include <cassert>
#include <stdexcept>


#if 0 // ! defined(NDEBUG)

#define NotImplementedException(arg) (assert(false),std::logic_error("Not implemented"))

#else

class NotImplementedException : public std::logic_error {
public:
  NotImplementedException() : logic_error("Not implemented") {}
  NotImplementedException(const char* name);
};

#endif

class NotSupportedException : public std::logic_error {
public:
  NotSupportedException() : logic_error("Not supported") {}
  NotSupportedException(const char* nameOfMethod, const char* realTypeOfObject);
  NotSupportedException(const char* whatIsNotSupported);
};