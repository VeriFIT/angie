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
/** @file IdImpl.hh */

#pragma once
#include <cstdint>
#include <iostream>

// How to use:

/* Into header file
class SometNameForPlaceholderToken;
using SomeId = Id<SometNameForPlaceholderToken>;
*/

/* Into source file
template<>
SomeId SomeId::nextIdToGive{};
*/

template<typename T>
class Id {
private:
  // Fields
  static Id nextIdToGive;

public:
  uint64_t id;
  // Constructors
  constexpr Id()            : id{ 0 }  { }
  explicit  Id(uint64_t id) : id{ id } { }
           ~Id() = default;

  // Conversion methods
  explicit operator uint64_t() const { return id; }

  // Creator methods
  static Id GetNextId() { return nextIdToGive++; }

  // Methods
  Id operator++()    { id++; return *this; } // prefix
  Id operator++(int) { auto copy = *this; id++; return copy; } // postfix

  // Implementation of all comparsion operators (only the operator< is needed for most STD functionality) 
  //REVIEW: to use or not use the canonical way to implement the rest of operators
  //   canoncial way is to use the other already defined operators

  constexpr bool operator==(const Id& other) const { return this->id == other.id; } // canonicaly implemented
  constexpr bool operator!=(const Id& other) const { return this->id != other.id; }
  constexpr bool operator< (const Id& other) const { return this->id < other.id; }  // canonicaly implemented
  constexpr bool operator> (const Id& other) const { return this->id > other.id; }
  constexpr bool operator<=(const Id& other) const { return this->id <= other.id; }
  constexpr bool operator>=(const Id& other) const { return this->id >= other.id; }

  // Friend printer
  friend std::ostream& operator<<(std::ostream& os, const Id<T>& idValue) { return os << idValue.id; }
};
