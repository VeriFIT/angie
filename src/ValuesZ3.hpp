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
/** @file ValuesZ3.hpp */

#pragma once

#if !defined(NOT_COMPILE_Z3)

#include "Definitions.hpp"
#include "Type.hpp"

#include "Values.hpp"

#include <boost/logic/tribool.hpp>
#include <map>
#include <z3++.h>

class Z3ValueContainer : public IValueContainer {
private:
  static z3::context ctx;
  static std::map<ValueId, z3::expr> idsToExprs;
  std::multimap<ValueId, z3::expr> constraints;

  z3::expr CreateCmpExpression(ValueId first, ValueId second, Type type, CmpFlags flags) const;
  void     AddValueInfoToSolver(z3::solver& s, ValueId val) const;
public:
  virtual boost::tribool IsCmp     (ValueId first, ValueId second, Type type, CmpFlags flags) const override;
  virtual boost::tribool IsEq      (ValueId first, ValueId second, Type type) const override;
  virtual boost::tribool IsNeq     (ValueId first, ValueId second, Type type) const override;
  virtual boost::tribool IsTrue    (ValueId first, Type type) const override; // !=0
  virtual boost::tribool IsFalse   (ValueId first, Type type) const override; // ==0

  virtual boost::tribool IsInternalRepEq(ValueId first, ValueId second) const override;
  virtual boost::tribool IsZero   (ValueId first) const override;
  virtual bool           IsUnknown(ValueId first) const override;
  // Creates new boolean (1bit integer) value expressing the constraint
  virtual ValueId Cmp        (ValueId first, ValueId second, Type type, CmpFlags flags) override;
  // Sets constraint on both values
  virtual void    Assume     (ValueId first, ValueId second, Type type, CmpFlags flags) override;
  virtual void    AssumeTrue (ValueId first) override; // Sets contraint: first != 0 ( == true )
  virtual void    AssumeFalse(ValueId first) override; // Sets contraint: first == 0 ( == false)

  virtual ValueId BinOp (ValueId first, ValueId second, Type type, BinaryOpOptions options) { throw NotImplementedException(); }

  virtual ValueId Add   (ValueId first, ValueId second, Type type, ArithFlags flags) override;
  virtual ValueId Sub   (ValueId first, ValueId second, Type type, ArithFlags flags) override;
  virtual ValueId Mul   (ValueId first, ValueId second, Type type, ArithFlags flags) override;
  virtual ValueId Div   (ValueId first, ValueId second, Type type, ArithFlags flags) override;
  virtual ValueId Rem   (ValueId first, ValueId second, Type type, ArithFlags flags) override;

  virtual ValueId ShL   (ValueId first, ValueId second, Type type, ArithFlags flags) override;
  virtual ValueId ShR   (ValueId first, ValueId second, Type type, ArithFlags flags) override;

  virtual ValueId BitAnd(ValueId first, ValueId second, Type type) override;
  virtual ValueId BitOr (ValueId first, ValueId second, Type type) override;
  virtual ValueId BitXor(ValueId first, ValueId second, Type type) override;
  virtual ValueId BitNot(ValueId first, Type type) override;

  //following are not an LLVM operations
  //when C/C++ is compiled in LLVM, everything is series of != 0 tests and branch/phi instructions
  virtual ValueId LogAnd(ValueId first, ValueId second, Type type) override;
  virtual ValueId LogOr (ValueId first, ValueId second, Type type) override;
  virtual ValueId LogNot(ValueId first, Type type) override;

  virtual ValueId ExtendInt(ValueId first, Type sourceType, Type targetType, ArithFlags flags) override;
  virtual ValueId TruncateInt (ValueId first, Type sourceType, Type targetType) override;

  //virtual ValueId ConvIntToFloat(ValueId first, uint32_t flags) override;
  //virtual ValueId ConvFloatToInt(ValueId first, uint32_t flags) override;
;
  virtual ValueId CreateVal(Type type) override;
  virtual ValueId CreateVal(ValueId newId, Type type) override;

  virtual ValueId CreateConstIntVal  (uint64_t value, Type type) override;
  virtual ValueId CreateConstIntVal  (uint64_t value           ) override; // To be potentially removed
  virtual ValueId CreateConstFloatVal(float    value, Type type) override;
  virtual ValueId CreateConstFloatVal(double   value, Type type) override;

  virtual void PrintDebug() const override;

  virtual uptr<IValueContainer> Clone() override { return std::make_unique<Z3ValueContainer>(*this); }
protected:

  virtual ValueId GetZero(Type type) const override { throw NotImplementedException(); }
};

#endif // #if !defined(NOT_COMPILE_Z3)
