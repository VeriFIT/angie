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
/** @file Exceptions.cpp */

#include "Exceptions.hpp"

#if ! defined(NDEBUG)

#else

NotImplementedException::NotImplementedException(const char* name) :
  logic_error(
    std::string{"Method "}
    .append(name)
    .append("is not implemented.")
    )
  {}

#endif

NotSupportedException::NotSupportedException(const char* name, const char* type) :
  logic_error(
    std::string{"Method "}
      .append(name)
      .append("is not supported in class of type")
      .append(type)
      .append(".")
    )
  {}

NotSupportedException::NotSupportedException(const char* msg) :
  logic_error(msg)
{}

// Here i used originaly operator+ for concat but I ran into this problem:
// C:/Apps/msys64/mingw64/lib/libLLVMAsmParser.a(LLParser.cpp.obj):(.text$_ZStplIcSt11char_traitsIcESaIcEENSt7__cxx1112basic_stringIT_T0_T1_EEOS8_PKS5_[_ZStplIcSt11char_traitsIcESaIcEENSt7__cxx1112basic_stringIT_T0_T1_EEOS8_PKS5_]+0x0): multiple definition of `std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > std::operator+<char, std::char_traits<char>, std::allocator<char> >(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&&, char const*)'
// CMakeFiles/bordel1.dir/objects.a(Exceptions.cpp.obj):(.text[_ZStplIcSt11char_traitsIcESaIcEENSt7__cxx1112basic_stringIT_T0_T1_EEOS8_PKS5_]+0x0): first defined here
