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
/** @file IdImpl.hpp */

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

template<typename Token, typename UnderlyingT = uint64_t, UnderlyingT FIRST_ID = 0>
class Id {
private:
  // Fields
  static Id nextIdToGive;

public:
  using type = UnderlyingT;
  UnderlyingT id;
  // Constructors
  constexpr           Id()               : id{ 0 }  { }
  constexpr explicit  Id(UnderlyingT id) : id{ id } { }
                     ~Id() = default;

  // Conversion methods
  explicit operator UnderlyingT() const { return id; }
  UnderlyingT Unwrap() const { return id; }

  // Creator methods
  static Id GetNextId() { return nextIdToGive++; }

  // Methods
  Id operator++()    { id++; return *this; } // prefix
  Id operator++(int) { auto copy = *this; id++; return copy; } // postfix

  // Implementation of all comparison operators (only the operator< is needed for most STD functionality) 
  //REVIEW: to use or not use the canonical way to implement the rest of operators
  //   canonical way is to use the other already defined operators

  constexpr bool operator==(const Id& other) const { return this->id == other.id; } // canonically implemented
  constexpr bool operator!=(const Id& other) const { return this->id != other.id; }
  constexpr bool operator< (const Id& other) const { return this->id <  other.id; }  // canonically implemented
  constexpr bool operator> (const Id& other) const { return this->id >  other.id; }
  constexpr bool operator<=(const Id& other) const { return this->id <= other.id; }
  constexpr bool operator>=(const Id& other) const { return this->id >= other.id; }

  // Friend printer
  friend std::ostream& operator<<(std::ostream& os, const Id& idValue) { return os << idValue.id; }
  friend std::string   to_string(                   const Id& idValue) { return std::to_string(idValue.id); }
};

template<typename Token, typename UnderlyingT, UnderlyingT FIRST_ID>
Id<Token, UnderlyingT, FIRST_ID> Id<Token, UnderlyingT, FIRST_ID>::nextIdToGive(FIRST_ID);