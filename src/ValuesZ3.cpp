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
/** @file ValuesZ3.cpp */

#if !defined(NOT_COMPILE_Z3)

#include "ValuesZ3.hpp"

#include <z3++.h>

using namespace z3;

z3::context                 Z3ValueContainer::ctx;
std::map<ValueId, z3::expr> Z3ValueContainer::idsToExprs;

boost::tribool constexpr Z3ResultToTribool(z3::check_result result)
{
  return
    result == z3::unsat ? boost::tribool(false) :
    (result == z3::sat ? boost::tribool(true) :
      boost::indeterminate);
}

void Z3ValueContainer::AddValueInfoToSolver(z3::solver& s, ValueId val) const
{
  // add value's AST
  s.add(idsToExprs.at(val));
  // add all constraints
  for (
    auto it = constraints.find(val), end = constraints.cend();
    it != end;
    ++it)
  {
    if (it->first != val)
      break;
    s.add(it->second);
  }
}

z3::expr Z3ValueContainer::CreateCmpExpression(ValueId first, ValueId second, Type type, CmpFlags flags) const
{
  const auto& lhs = idsToExprs.at(first);
  const auto& rhs = idsToExprs.at(second);

  // check that the size I assume the value/expression have
  // psedo-code: sizeof(first) == sizeof(second) == type.size
  assert(lhs.get_sort().bv_size() == rhs.get_sort().bv_size());
  assert(lhs.get_sort().bv_size() == type.GetBitWidth());

  // Based on >= operator :
#if 0
  check_context(lhs, rhs);
  Z3_ast r = 0;
  if (lhs.is_arith() && rhs.is_arith()) {
    r = Z3_mk_ge(lhs.ctx(), lhs, rhs);
  }
  else if (lhs.is_bv() && rhs.is_bv()) {
    r = Z3_mk_bvsge(lhs.ctx(), lhs, rhs);
  }
  else {
    // operator is not supported by given arguments.
    assert(false);
  }
  lhs.check_error();
  return expr(lhs.ctx(), r);
#endif

  check_context(lhs, rhs);
  Z3_ast r;
  switch (flags)
  {
  case CmpFlags::Eq:
    r = Z3_mk_eq(ctx, lhs, rhs);
    break;
  case CmpFlags::Neq:
  {
    Z3_ast args[2] = {lhs, rhs};
    r = Z3_mk_distinct(ctx, 2, args);
    break;
  }
  case CmpFlags::UnsigGt:
    r = Z3_mk_bvugt(ctx, lhs, rhs);
    break;
  case CmpFlags::UnsigGtEq:
    r = Z3_mk_bvugt(ctx, lhs, rhs);
    break;
  case CmpFlags::UnsigLt:
    r = Z3_mk_bvugt(ctx, lhs, rhs);
    break;
  case CmpFlags::UnsigLtEq:
    r = Z3_mk_bvugt(ctx, lhs, rhs);
    break;
  case CmpFlags::SigGt:
    r = Z3_mk_bvsgt(ctx, lhs, rhs);
    break;
  case CmpFlags::SigGtEq:
    r = Z3_mk_bvsgt(ctx, lhs, rhs);
    break;
  case CmpFlags::SigLt:
    r = Z3_mk_bvsgt(ctx, lhs, rhs);
    break;
  case CmpFlags::SigLtEq:
    r = Z3_mk_bvsgt(ctx, lhs, rhs);
    break;
  case CmpFlags::Default:
    throw exception("spatna hodnota"); //TODO: create new exception class?
    break;
  default:
    throw NotSupportedException();
    break;
  }
  ctx.check_error();
  return expr{ctx, r};
}



boost::tribool Z3ValueContainer::IsCmp(ValueId first, ValueId second, Type type, CmpFlags flags) const
{
  solver s{ctx};

  // add value's info / expression (AST, constraints)
  AddValueInfoToSolver(s, first);
  AddValueInfoToSolver(s, second);

  // Cmp
  s.add(CreateCmpExpression(first, second, type, flags));

  // get result
  return Z3ResultToTribool(s.check());
}

boost::tribool Z3ValueContainer::IsEq(ValueId first, ValueId second, Type type) const
{
  return IsCmp(first, second, type, CmpFlags::Eq);
}

boost::tribool Z3ValueContainer::IsNeq(ValueId first, ValueId second, Type type) const
{
  return IsCmp(first, second, type, CmpFlags::Neq);
}

boost::tribool Z3ValueContainer::IsTrue(ValueId first, Type type) const
{
  solver s{ctx};
  const auto& lhs = idsToExprs.at(first);

  // check that the size I assume the value/expression have
  // psedo-code: sizeof(first) == sizeof(second) == type.size
  assert(lhs.get_sort().bv_size() == type.GetBitWidth());

  // add value's info / expression (AST, constraints)
  AddValueInfoToSolver(s, first);

  // IsZero opposite
  s.add(lhs != 0);

  // get result
  return Z3ResultToTribool(s.check());
}

boost::tribool Z3ValueContainer::IsFalse(ValueId first, Type type) const
{
  solver s{ctx};
  const auto& lhs = idsToExprs.at(first);

  // check that the size I assume the value/expression have
  // psedo-code: sizeof(first) == sizeof(second) == type.size
  assert(lhs.get_sort().bv_size() == type.GetBitWidth());

  // add value's info / expression (AST, constraints)
  AddValueInfoToSolver(s, first);

  // IsZero equivalent
  s.add(lhs == 0);

  // get result
  return Z3ResultToTribool(s.check());
}

boost::tribool Z3ValueContainer::IsInternalRepEq(ValueId first, ValueId second) const
{
  throw NotImplementedException();
}

bool Z3ValueContainer::IsUnknown(ValueId first) const
{
  throw NotImplementedException();
}

boost::tribool Z3ValueContainer::IsZero(ValueId first) const
{
  solver s{ctx};

  // add value's info / expression (AST, constraints)
  AddValueInfoToSolver(s, first);

  // IsZero
  s.add(idsToExprs.at(first) == 0);

  // get result
  return Z3ResultToTribool(s.check());
}

ValueId Z3ValueContainer::Cmp(ValueId first, ValueId second, Type type, CmpFlags flags)
{
  auto id = ValueId::GetNextId();
  expr ex = CreateCmpExpression(first, second, type, flags);
  idsToExprs.insert({id, ex});

  return id;
}
void Z3ValueContainer::Assume(ValueId first, ValueId second, Type type, CmpFlags flags)
{
  expr ex = CreateCmpExpression(first, second, type, flags);

  constraints.insert({first, ex});
  constraints.insert({second, ex});
}

void Z3ValueContainer::AssumeTrue(ValueId first)
{
  const auto& lhs = idsToExprs.at(first);
  constraints.insert({first, lhs != 0});
}

void Z3ValueContainer::AssumeFalse(ValueId first)
{
  const auto& lhs = idsToExprs.at(first);
  constraints.insert({first, lhs == 0});
}

ValueId Z3ValueContainer::Add(ValueId first, ValueId second, Type type, ArithFlags flags)
{
  auto id = ValueId::GetNextId();
  const auto& lhs = idsToExprs.at(first);
  const auto& rhs = idsToExprs.at(second);

  // check that the size I assume the value/expression have
  // psedo-code: sizeof(first) == sizeof(second) == type.size
  assert(lhs.get_sort().bv_size() == rhs.get_sort().bv_size());
  assert(lhs.get_sort().bv_size() == type.GetBitWidth());

  expr ex = lhs + rhs;
  idsToExprs.insert({id, ex});
  //TODO: dále podle NSW a NUW (a signed) flagů je třeba přidat ještě no_under* a no_over* predikaty

  return id;
}

ValueId Z3ValueContainer::Sub(ValueId first, ValueId second, Type type, ArithFlags flags)
{
  return CreateVal(type);
}

// kopie ValueId Z3ValueContainer::Add(ValueId first, ValueId second, Type type, ArithFlags flags)
ValueId Z3ValueContainer::Mul(ValueId first, ValueId second, Type type, ArithFlags flags)
{
  auto id = ValueId::GetNextId();
  const auto& lhs = idsToExprs.at(first);
  const auto& rhs = idsToExprs.at(second);

  // check that the size I assume the value/expression have
  // psedo-code: sizeof(first) == sizeof(second) == type.size
  assert(lhs.get_sort().bv_size() == rhs.get_sort().bv_size());
  assert(lhs.get_sort().bv_size() == type.GetBitWidth());

  expr ex = lhs * rhs;
  idsToExprs.insert({id, ex});
  //TODO: dále podle NSW a NUW (a signed) flagů je třeba přidat ještě no_under* a no_over* predikaty

  return id;
}

ValueId Z3ValueContainer::Div(ValueId first, ValueId second, Type type, ArithFlags flags)
{
  return CreateVal(type);
}

ValueId Z3ValueContainer::Rem(ValueId first, ValueId second, Type type, ArithFlags flags)
{
  return CreateVal(type);
}

ValueId Z3ValueContainer::ShL(ValueId first, ValueId second, Type type, ArithFlags flags)
{
  return CreateVal(type);
}

ValueId Z3ValueContainer::ShR(ValueId first, ValueId second, Type type, ArithFlags flags)
{
  return CreateVal(type);
}

ValueId Z3ValueContainer::LogAnd(ValueId first, ValueId second, Type type)
{
  return CreateVal(type);
}

ValueId Z3ValueContainer::LogOr(ValueId first, ValueId second, Type type)
{
  return CreateVal(type);
}

ValueId Z3ValueContainer::LogNot(ValueId first, Type type)
{
  return CreateVal(type);
}

ValueId Z3ValueContainer::BitAnd(ValueId first, ValueId second, Type type)
{
  return CreateVal(type);
}

ValueId Z3ValueContainer::BitOr(ValueId first, ValueId second, Type type)
{
  return CreateVal(type);
}

ValueId Z3ValueContainer::BitXor(ValueId first, ValueId second, Type type)
{
  return CreateVal(type);
}

ValueId Z3ValueContainer::BitNot(ValueId first, Type type)
{
  return CreateVal(type);
}

ValueId Z3ValueContainer::ExtendInt(ValueId first, Type sourceType, Type targetType, ArithFlags flags)
{
  auto id = ValueId::GetNextId();
  const auto& lhs = idsToExprs.at(first);

  assert(lhs.get_sort().bv_size() == sourceType.GetBitWidth());

  expr ex = has_flag(flags, ArithFlags::Signed) ?
    sext(lhs, (unsigned)targetType.GetBitWidth() - lhs.get_sort().bv_size()) :
    zext(lhs, (unsigned)targetType.GetBitWidth() - lhs.get_sort().bv_size());
  idsToExprs.insert({id, ex});

  return id;
}

ValueId Z3ValueContainer::TruncateInt(ValueId first, Type sourceType, Type targetType)
{
  auto id = ValueId::GetNextId();
  const auto& lhs = idsToExprs.at(first);

  assert(lhs.get_sort().bv_size() == sourceType.GetBitWidth());

  expr ex = lhs.extract((unsigned)targetType.GetBitWidth(), 0);
  idsToExprs.insert({id, ex});

  return id;
}

ValueId Z3ValueContainer::CreateVal(ValueId newId, Type type)
{
  auto id = newId;
  auto ex = ctx.constant(ctx.int_symbol(static_cast<uint64_t>(id)), ctx.bv_sort((unsigned)type.GetBitWidth()));
  idsToExprs.insert({id, ex});

  return id;
}
ValueId Z3ValueContainer::CreateVal(Type type)
{
  auto id = ValueId::GetNextId();
  auto ex = ctx.constant(ctx.int_symbol(static_cast<uint64_t>(id)), ctx.bv_sort((unsigned)type.GetBitWidth()));
  idsToExprs.insert({id, ex});

  return id;

#if 0
  //ctx.constant(ctx.int_symbol(static_cast<uint64_t>(id)), ctx.bv_sort(64));
  try {
    std::cout << "find_model_example1\n";
    context ctx;

    expr x = ctx.constant(ctx.int_symbol(0), ctx.bv_sort(64)); //type.GetBitWidth()
    expr y = ctx.constant(ctx.int_symbol(1), ctx.bv_sort(64));
    expr vyraz1 = x >= 1;
    expr vyraz2 = y < x + 3;

    solver s(ctx);

    s.add(vyraz1);
    s.add(vyraz2);
    std::cout << s.check() << "\n";

    model m = s.get_model();
    std::cout << m << "\n";
    // traversing the model
    for (unsigned i = 0; i < m.size(); i++) {
      func_decl v = m[i];
      // this problem contains only constants
      assert(v.arity() == 0);
      std::cout << v.name() << " = " << m.get_const_interp(v) << "\n";
    }
    // we can evaluate expressions in the model.
    std::cout << "x + y + 1 = " << m.eval(x + y + 1) << "\n";
  }
  catch (exception & ex) {
    std::cout << "unexpected error: " << ex << "\n";
  }
  return ValueId::GetNextId();
#endif
}

ValueId Z3ValueContainer::CreateConstIntVal(uint64_t value, Type type)
{
  auto id = ValueId::GetNextId();
  auto ex = ctx.bv_val(value, (unsigned)type.GetBitWidth());
  idsToExprs.insert({id, ex});

  return id;
}

ValueId Z3ValueContainer::CreateConstIntVal(uint64_t value)
{
  auto id = ValueId::GetNextId();
  auto ex = ctx.bv_val(value, 64u);
  idsToExprs.insert({id, ex});

  return id;
}

ValueId Z3ValueContainer::CreateConstFloatVal(float value, Type type)
{
  throw NotImplementedException();
}

ValueId Z3ValueContainer::CreateConstFloatVal(double value, Type type)
{
  throw NotImplementedException();
}

void Z3ValueContainer::PrintDebug() const
{
  for (auto& pair : idsToExprs)
  {
    printf("\n%llu ", (unsigned long long)(size_t)pair.first);
    std::cout << pair.second;
  }
}

#endif // #if !defined(NOT_COMPILE_Z3)
