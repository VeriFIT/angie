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
/** @file Exceptions.hpp */

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

class AnalysisErrorException : public std::logic_error {
public:
  /*ctr*/ AnalysisErrorException()
    : logic_error("A fatal error was discovered by the analysis. Abstract execution can not continue in this path.")
  {
  }
  /*ctr*/ AnalysisErrorException(const char* c)
    : logic_error(c)
  {
  }
  /*ctr*/ AnalysisErrorException(const std::string& c)
    : logic_error(c)
  {
  }
};

class NulldDereferenceException : public AnalysisErrorException {
public:
  /*ctr*/ NulldDereferenceException()
    : AnalysisErrorException("Program tried to dereference a null pointer.")
  {
  }
};

class InvalidDereferenceException : public AnalysisErrorException {
public:
  /*ctr*/ InvalidDereferenceException()
    : AnalysisErrorException("Program tried to dereference an invalid location.")
  {
  }
};

class InvalidFreeException : public AnalysisErrorException {
public:
  /*ctr*/ InvalidFreeException()
    : AnalysisErrorException("Program tried to free an invalid location.")
  {
  }
};

namespace {
static const char* NonHeapFreeException_msg = "Program tried to free an location that was not on heap.";
}
class NonHeapFreeException : public AnalysisErrorException {
public:
  /*ctr*/ NonHeapFreeException()
    : NonHeapFreeException(NonHeapFreeException_msg)
  {
  }
  // concats the detail with default text
  /*ctr*/ NonHeapFreeException(const char* c);
};

class DoubleFreeException : public AnalysisErrorException {
public:
  /*ctr*/ DoubleFreeException()
    : AnalysisErrorException("Program tried to free an already freed memory.")
  {
  }
};

class PossibleNullDereferenceException : public AnalysisErrorException {
public:
  /*ctr*/ PossibleNullDereferenceException()
    : AnalysisErrorException("Possible null dereference exception occurred - getElementPtr.")
  {
  }
};

