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
/** @file Values.hh */

#pragma once

#include "Definitions.hh"
#include "General.hh"
#include "Type.hh"
#include "IdImpl.hh"

#include "enum_flags.h"

#include <boost/logic/tribool.hpp>


class ValueIdToken;
/// <summary>
/// ValueId class is an type-safe encapsulation of integer-based IDs for values
/// </summary>
using ValueId = Id<ValueIdToken>;

ENUM_FLAGS(AbstractionStatus)
enum class AbstractionStatus {
  Undefined = 0x0000, // Value is purely abstract and no information were given to the VC
  Unknown = 0x0001, // Value might have had some asociated information but now purely unknown
};

constexpr const char* AbstractionStatusToString(const AbstractionStatus s)
{
  return
    s == AbstractionStatus::Undefined ?
      "Undefined" :
      "Unknown";
}

//TODO@michkot: Document a way one should implement the following interface/abstract class

/// <summary>
/// Class holding a set of values / their constraint based representations.
/// Internaly everything is a string of bits.
/// All integer values are reperesented in two's complement code
///
/// Arithemtic and shift operations which tends to have separated signed and unsigned instructions
/// are abstracted into single operation.
/// Also note, wrapping/overflow of certain operations is undefined behaviour in some languages.
/// Use the <paramref name="flags"/> parameter to specify wanted behaviour in these cases.
/// </summary>
class IValueContainer {
public:

  // General note for implementor of this abstract class / interface:
  // Methods need implementing for compiling:
  // IsCmp, IsInternalRepEq, IsZero, IsUnknown, 
  // Assume, Cmp, BinOp, BitNot, 
  // ExtendInt, TruncateInt, CreateVal, CreateConstIntVal, 
  // GetZero, 
  // PrintDebug
  // Other methods which analysis is probably going to use (and might be changed from NIE to pure virtual):
  // AssumeTrue, AssumeFalse, GetAbstractionStatus

  // --------------------------------------------------------------------------
  // Section A - constant methods, comparison queries
  
  // The first method is an universal method for comparison of two values
  // The others are specialzed helper / convenient methods

  virtual boost::tribool IsCmp  (ValueId first, ValueId second, Type type, CmpFlags flags) const = 0;
  virtual boost::tribool IsEq   (ValueId first, ValueId second, Type type) const 
                                    { return IsCmp(first, second, type, CmpFlags::Eq); }
  virtual boost::tribool IsNeq  (ValueId first, ValueId second, Type type) const 
                                    { return IsCmp(first, second, type, CmpFlags::Neq); }
  virtual boost::tribool IsTrue (ValueId first, Type type) const 
                                    { return IsCmp(first, GetZero(type), type, CmpFlags::Neq); } // !=0
  virtual boost::tribool IsFalse(ValueId first, Type type) const 
                                    { return IsCmp(first, GetZero(type), type, CmpFlags::Eq); }  // ==0
  
  // Compares values' internal representations - typically a bitwise comparison of fixed-length integers
  virtual boost::tribool IsInternalRepEq(ValueId first, ValueId second) const = 0;
  // The value is zero in all possible interpretations 
  virtual boost::tribool IsZero   (ValueId first) const = 0;
  // There are no informations regarding this value
  virtual bool           IsUnknown(ValueId first) const = 0;
  //TODO: relocate, add comment
  virtual AbstractionStatus GetAbstractionStatus(ValueId first) { throw NotImplementedException(); } 

  // --------------------------------------------------------------------------
  // Section B - modifying methods, constraint addition 

  // Creates new boolean (1bit integer) value expressing the constraint
  virtual ValueId Cmp        (ValueId first, ValueId second, Type type, CmpFlags flags) = 0;
  // Sets constraint on both values
  virtual void    Assume     (ValueId first, ValueId second, Type type, CmpFlags flags) = 0;
  // Sets contraint: the value is different from zero in all possible interpetations
  // Sets contraint: first != 0, i.e. == true
  virtual void    AssumeTrue (ValueId first) { throw NotImplementedException(); }
  // Sets contraint: the value is equal     to   zero in all possible interpetations
  // Sets contraint: first == 0, i.e. == false
  virtual void    AssumeFalse(ValueId first) { throw NotImplementedException(); }

  // --------------------------------------------------------------------------
  // Section C - modifying methods, math/bitwise binary and unary operations

  // In following methods, the representation of result of the operation is assigned to the returned value 
  // (need not be a newly created one, e.g. typically in case of constants)

  // Universal arithmetic/bitwise binary operation, configured by struct param  
  virtual ValueId BinOp (ValueId first, ValueId second, Type type, BinaryOpOptions options) = 0;
  virtual ValueId BinOp (ValueId first, ValueId second, Type type, BinaryOpKind kind, ArithFlags flags)
                            { return BinOp(first, second, type, {kind, flags}); }

  virtual ValueId Add   (ValueId first, ValueId second, Type type, ArithFlags flags) 
                            { return BinOp(first, second, type, {BinaryOpKind::Add, flags}); }
  virtual ValueId Sub   (ValueId first, ValueId second, Type type, ArithFlags flags)
                            { return BinOp(first, second, type, {BinaryOpKind::Sub, flags}); }
  virtual ValueId Mul   (ValueId first, ValueId second, Type type, ArithFlags flags)
                            { return BinOp(first, second, type, {BinaryOpKind::Mul, flags}); }
  virtual ValueId Div   (ValueId first, ValueId second, Type type, ArithFlags flags)
                            { return BinOp(first, second, type, {BinaryOpKind::Div, flags}); }
  virtual ValueId Rem   (ValueId first, ValueId second, Type type, ArithFlags flags)
                            { return BinOp(first, second, type, {BinaryOpKind::Rem, flags}); }

  virtual ValueId ShL   (ValueId first, ValueId second, Type type, ArithFlags flags)
                            { return BinOp(first, second, type, {BinaryOpKind::Shl, flags}); }
  virtual ValueId ShR   (ValueId first, ValueId second, Type type, ArithFlags flags)
                            { return BinOp(first, second, type, {BinaryOpKind::Shr, flags}); }

  virtual ValueId BitAnd(ValueId first, ValueId second, Type type)
                            { return BinOp(first, second, type, {BinaryOpKind::And, ArithFlags::Default}); }
  virtual ValueId BitOr (ValueId first, ValueId second, Type type)
                            { return BinOp(first, second, type, {BinaryOpKind::Or,  ArithFlags::Default}); }
  virtual ValueId BitXor(ValueId first, ValueId second, Type type)
                            { return BinOp(first, second, type, {BinaryOpKind::Xor, ArithFlags::Default}); }
  virtual ValueId BitNot(ValueId first, Type type) = 0; // Only unary operation !!! not LLVM operation

  // --------------------------------------------------------------------------
  // Section D - modifying methods, others

  // Following methods are not an LLVM operations
  // When C/C++ is compiled in LLVM, 
  // everything is converted into series of != 0 tests and branch/phi instructions
  // as an effect of expression shorthand evaluation

  virtual ValueId LogAnd(ValueId first, ValueId second, Type type) { throw NotImplementedException(); }
  virtual ValueId LogOr (ValueId first, ValueId second, Type type) { throw NotImplementedException(); }
  virtual ValueId LogNot(ValueId first, Type type)                 { throw NotImplementedException(); }

  // Following methods provides bitwise extension and truncation
  // The returned value need not be a newly created one,
  // i.e. with fixed 64-bit storage implementation, unsigned extensions and truncation would be no-op
  // Sign./unsig. extensions is marked by ArithFlags

  virtual ValueId ExtendInt  (ValueId first, Type sourceType, Type targetType, ArithFlags flags) = 0;
  virtual ValueId TruncateInt(ValueId first, Type sourceType, Type targetType) = 0;

  // Following are in-future to-be-implemented methods
  //virtual ValueId ConvIntToFloat(ValueId first, uint32_t flags) = 0;
  //virtual ValueId ConvFloatToInt(ValueId first, uint32_t flags) = 0;

  // Creates a new unknown value
  virtual ValueId CreateVal(Type type) = 0;

  virtual ValueId CreateConstIntVal  (uint64_t value, Type type) = 0; 
  // To be potentially removed
  virtual ValueId CreateConstIntVal  (uint64_t value           ) { throw NotImplementedException(); }
  // Floating point support is theoretical right now
  virtual ValueId CreateConstFloatVal(float    value, Type type) { throw NotImplementedException(); }
  // Floating point support is theoretical right now
  virtual ValueId CreateConstFloatVal(double   value, Type type) { throw NotImplementedException(); }

  // --------------------------------------------------------------------------
  // Section E - diagnostic methods

  // Prints current state of the container onto console
  virtual void PrintDebug() const { throw NotImplementedException(); }
  //virtual void Print(std::ostream& os) const { throw NotImplementedException(); }

protected:

  // Zero in all possible interpretations 
  virtual ValueId GetZero() const { throw NotImplementedException(); }
  // Zero of specific type/size
  virtual ValueId GetZero(Type type) const = 0;
};

//inline std::ostream& operator<<(std::ostream& os, const IValueContainer& vc) { vc.Print(os); return os; }
