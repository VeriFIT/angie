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

#if 0 //! defined(NDEBUG)

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

NonHeapFreeException::NonHeapFreeException(const char * c) : 
  AnalysisErrorException(std::string(NonHeapFreeException_msg) + " " + c)
{}
