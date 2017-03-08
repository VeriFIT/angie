/*******************************************************************************

Copyright (C) 2017 Michal Charvát
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
/** @file ValueContainerV1.cc */

#include "ValueContainerV1.hh"

#include <climits>
#include <iostream>
#include <algorithm>
#define CHECK_FLAG(var, flag) ((var & flag) == flag)


//BinaryConstraint helper methods
//--------------------------------------------------------------
BinaryConstraint::BinaryConstraint(ValueId _first, ValueId _second, CmpFlags _relation)
{
	//makes sure that first ValueId is smaller than second.
	if (first > second)
	{
		std::swap(first, second);
		switch (_relation)
		{
		case CmpFlags::Eq:
		case CmpFlags::Neq:
			//missing break intended
			break;

		case CmpFlags::SigGt:
			_relation = CmpFlags::SigLt;
			break;
		case CmpFlags::SigGtEq:
			_relation = CmpFlags::SigLtEq;
			break;
		case CmpFlags::SigLt:
			_relation = CmpFlags::SigGt;
			break;
		case CmpFlags::SigLtEq:
			_relation = CmpFlags::SigGtEq;
			break;
		case CmpFlags::UnsigGt:
			_relation = CmpFlags::UnsigLt;
			break;
		case CmpFlags::UnsigGtEq:
			_relation = CmpFlags::UnsigLtEq;
			break;
		case CmpFlags::UnsigLt:
			_relation = CmpFlags::UnsigGt;
			break;
		case CmpFlags::UnsigLtEq:
			_relation = CmpFlags::UnsigGtEq;
			break;
		default:
			throw std::runtime_error("Creating binaryconstraint with odd relation");
		}
	}

	this->first = _first;
	this->second = _second;
	this->relation = _relation;
}
//--------------------------------------------------------------


//ValueContainer helper methods
//--------------------------------------------------------------

//masks out specific amount of bits
inline uint64_t ValueContainer::RipBits(size_t numOfBits, uint64_t in) const
{
	if (numOfBits < sizeof(uint64_t)*CHAR_BIT)
		in &= (1ULL << numOfBits) - 1;
	return in;
}

//sign extends from number of bits specified to 64
inline int64_t ValueContainer::SignExtend64(size_t numOfBits, uint64_t in) const
{
	in = RipBits(numOfBits, in);
	uint64_t mask = (1ULL << (numOfBits - 1));
	return  static_cast<int64_t>(static_cast<uint64_t>(-(static_cast<int64_t>(in & mask))) | in);
}

//bit scan reverse - returns position of first set bit from the left side, tweaked for mul overflow verification on special cases 0,1,2
inline uint64_t ValueContainer::BSR(uint64_t val) const
{
	if (val >= 0 && val <= 1)
		return val;
	else if (val == 2)
		return 1;

	uint64_t pos = 1;
	while (val >>= 1)
		pos++;
	return pos;
}

void ValueContainer::InsertConstraint(BinaryConstraint constr)
{
	//invalidate old constraints
	//--------------------------
	bool isConstraintWithConstant = IsConstant(constr.first) || IsConstant(constr.second);
	if (isConstraintWithConstant)
	{
		//relation with a constant -> update intervals
		if (IsConstant(constr.first))
		{
			//first valueId is a constant
			for (auto oldConstraintId : GetConstraintIdVector(constr.second))
			{
				//for each constraint of nonconstatnt valueId
				auto oldConstraint = constrContainer.find(oldConstraintId);
				if (oldConstraint == constrContainer.end())
					throw std::runtime_error("ConstraintId references constraint that does not exist.");

				switch (constr.relation)
				{
				case CmpFlags::Eq:
					//new constraint is equal to a constant -> delete all previous
          //TODO@charvin FIX: invalidates iterator you are using!, 3x in this function
          //HACK for now just do not delete, see TODO
          // DeleteConstraint(oldConstraintId); 
					break;
					//TODO::add cases for intervals
				default:
					break;
				}

			}

		}
		else
		{
			//second valueId is a constant
			for (auto oldConstraintId : GetConstraintIdVector(constr.first))
			{
				//for each constraint of nonconstatnt valueId
				auto oldConstraint = constrContainer.find(oldConstraintId);
				if (oldConstraint == constrContainer.end())
					throw std::runtime_error("ConstraintId references constraint that does not exist.");

				switch (constr.relation)
				{
				case CmpFlags::Eq:
					//new constraint is equal to a constant -> delete all previous
          //TODO@charvin FIX: invalidates iterator you are using!, 3x in this function
          //HACK for now just do not delete, see TODO
          // DeleteConstraint(oldConstraintId); 
					break;
					//TODO::add cases for intervals
				default:
					break;
				}

			}

		}
	}


	//invalidate constraints with each other
	for (auto oldConstraintId : GetConstraintIdVector(constr.first))
	{
		if (constr.second == constrContainer.at(oldConstraintId).GetOther(constr.first))
		{
			//delete mutual constraints
      //TODO@charvin FIX: invalidates iterator you are using!, 3x in this function
      //HACK for now just do not delete, see TODO
      // DeleteConstraint(oldConstraintId); 
		}

	}

	//create and insert new constraint
	//--------------------------------
	ConstraintId newConstraintId = GetNextConstraintId();
	constrContainer[newConstraintId] = constr;

	//add constraint to both ValueIds
	constrIdContainer[constr.first].push_back(newConstraintId);
	constrIdContainer[constr.second].push_back(newConstraintId);
}

void ValueContainer::DeleteConstraint(ConstraintId constrId)
{
	auto constraint = constrContainer.find(constrId);
	if (constraint == constrContainer.end())
		throw std::runtime_error("Trying to delete constraint that does not exist");

	auto lhs = constrIdContainer.find(constraint->second.first); //first valueId
	auto rhs = constrIdContainer.find(constraint->second.second); //second valueId

	if (lhs == constrIdContainer.end() || rhs == constrIdContainer.end())
		throw std::runtime_error("ValueId contained in a constraint does not reference the constraint");

	lhs->second.erase(std::remove(lhs->second.begin(), lhs->second.end(), constrId), lhs->second.end());
	rhs->second.erase(std::remove(rhs->second.begin(), rhs->second.end(), constrId), rhs->second.end());

	constrContainer.erase(constrId);
}

const std::vector<ConstraintId>& ValueContainer::GetConstraintIdVector(const ValueId id) const
{
	static std::vector<ConstraintId> emptyvec;
	auto lhsConstrId = constrIdContainer.find(id);

	//is there a record of any constraints for the id
	if (lhsConstrId == constrIdContainer.end())
		return emptyvec;

	//return reference to the vector of constraintIds corresponding to the id
	return lhsConstrId->second;
}


//--------------------------------------------------------------


//ValueContainer interface methods implementations
ConstraintId ValueContainer::nextConstraintIdToGive = 0;

boost::tribool ValueContainer::IsCmp(ValueId first, ValueId second, Type type, CmpFlags flags) const
{
	//NOTE::first and second must be sorted

	if (IsConstant(first) && IsConstant(second))
	{
		//both are constants
		int64_t val1 = SignExtend64(type.GetBitWidth(), constantContainer.at(first));
		int64_t val2 = SignExtend64(type.GetBitWidth(), constantContainer.at(second));
		uint64_t uval1 = RipBits(type.GetBitWidth(), constantContainer.at(first));
		uint64_t uval2 = RipBits(type.GetBitWidth(), constantContainer.at(second));

		switch (flags)
		{
		case CmpFlags::Eq:
			return uval1 == uval2;
		case CmpFlags::Neq:
			return uval1 != uval2;
		case CmpFlags::SigGt:
			return  val1 > val2;
		case CmpFlags::SigGtEq:
			return  val1 >= val2;
		case CmpFlags::SigLt:
			return  val1 < val2;
		case CmpFlags::SigLtEq:
			return  val1 <= val2;
		case CmpFlags::UnsigGt:
			return  uval1 > uval2;
		case CmpFlags::UnsigGtEq:
			return  uval1 >= uval2;
		case CmpFlags::UnsigLt:
			return  uval1 < uval2;
		case CmpFlags::UnsigLtEq:
			return  uval1 <= uval2;
		default:
			throw std::runtime_error("Unexpected binary constraint");
		}

	}


	else if (IsConstant(first) || IsConstant(second))
	{
		//only one of ValueIds is a constant

		if (IsConstant(second))
			std::swap(first, second);

		//first is a constant
		int64_t val1 = SignExtend64(type.GetBitWidth(), constantContainer.at(first));
		uint64_t uval1 = RipBits(type.GetBitWidth(), constantContainer.at(first));

		//go through constraints of second ValueId and check for constraints with other constants
		for (auto constraintId : GetConstraintIdVector(second))
		{
			auto constraint = constrContainer.at(constraintId);
			ValueId other = constraint.GetOther(second);
			if (IsConstant(other))
			{
				//constaint with a constant
				int64_t val2 = SignExtend64(type.GetBitWidth(), constantContainer.at(other));
				uint64_t uval2 = RipBits(type.GetBitWidth(), constantContainer.at(other));

				switch (flags)
				{
				case CmpFlags::Eq:
					if (constraint.relation == CmpFlags::Eq)
						return uval1 == uval2;
					if (constraint.relation == CmpFlags::Neq && uval1 == uval2)
						return false;
					//add intervals -- needs even more layers first<>second in constraint...
					break;
				case CmpFlags::Neq:
					if (constraint.relation == CmpFlags::Eq)
						return uval1 != uval2;
					if (constraint.relation == CmpFlags::Neq && uval1 == uval2)
						return true;
					//intervals require more layers
				case CmpFlags::Gt:
				case CmpFlags::GtEq:
				case CmpFlags::Lt:
				case CmpFlags::LtEq:
				default:
					break;
				}

			}

		}
	}
	else
	{
		//neither is a constant

		//loop through constraintIds corresponding to the first ValueId
		for (auto constrId : GetConstraintIdVector(first))
		{
			auto constraint = constrContainer.find(constrId);
			if (constraint == constrContainer.end())
				throw std::runtime_error("ConstraintId references constraint that does not exist.");
			if (second == constraint->second.GetOther(first))
			{
				//this constraint contains both valueIds
				switch (flags)
				{
				case CmpFlags::Eq:
					if (constraint->second.relation == CmpFlags::Eq)
						return true;
					if (constraint->second.relation == CmpFlags::Neq || constraint->second.relation == CmpFlags::Gt || constraint->second.relation == CmpFlags::Lt)
						return false;
					break;
				case CmpFlags::Neq:
					if (constraint->second.relation == CmpFlags::Eq)
						return false;
					if (constraint->second.relation == CmpFlags::Neq || constraint->second.relation == CmpFlags::Gt || constraint->second.relation == CmpFlags::Lt)
						return true;
					break;
				case CmpFlags::Gt:
					if (constraint->second.relation == CmpFlags::Gt)
						return true;
					if (constraint->second.relation == CmpFlags::Eq || constraint->second.relation == CmpFlags::Lt || constraint->second.relation == CmpFlags::LtEq)
						return false;
					break;
				case CmpFlags::Lt:
					if (constraint->second.relation == CmpFlags::Lt)
						return true;
					if (constraint->second.relation == CmpFlags::Eq || constraint->second.relation == CmpFlags::Gt || constraint->second.relation == CmpFlags::GtEq)
						return false;
					break;
				case CmpFlags::GtEq:
					if (constraint->second.relation == CmpFlags::Gt || constraint->second.relation == CmpFlags::GtEq || constraint->second.relation == CmpFlags::Eq)
						return true;
					if (constraint->second.relation == CmpFlags::Lt)
						return false;
					break;
				case CmpFlags::LtEq:
					if (constraint->second.relation == CmpFlags::Lt || constraint->second.relation == CmpFlags::LtEq || constraint->second.relation == CmpFlags::Eq)
						return true;
					if (constraint->second.relation == CmpFlags::Gt)
						return false;
					break;
				default:
					break;
				}
			}
		}

	}

	return boost::indeterminate;
}


boost::tribool ValueContainer::IsInternalRepEq(ValueId first, ValueId second) const
{
	if (first == second)
		return true;

	//if both constants but different, return false
	if (IsConstant(first) && IsConstant(second))
		return false;


	//check for equality in constraint container
	//------------------------------------------
	if (first > second)
		std::swap(first, second);


	//loop through vector of constraintIds corresponding to the first ValueId
	for (auto constrId : GetConstraintIdVector(first))
	{
		auto constr = constrContainer.find(constrId);
		if (constr == constrContainer.end())
			throw std::runtime_error("Referenced constraint does not exist!");

		if (constr->second.Contains(second))
		{
			//constraint between both ValueIds
			switch (constr->second.relation)
			{
			case CmpFlags::Eq:
				return true;
			case CmpFlags::Neq:
			case CmpFlags::Gt:
			case CmpFlags::Lt:
				return false;
			default:
				break;
			}
		}

		//TODO::check intervals
	}


	return boost::indeterminate;
}

boost::tribool ValueContainer::IsZero(ValueId first) const
{
	if (first == Zero)
		return true;

	//is a constant?
	auto lhs = constantContainer.find(first);
	if (lhs != constantContainer.end())
	{
		uint64_t value = lhs->second;
		return value == 0;
	}

	//checks in constraint container
	//------------------------------------------
	//loop through vector of constraintIds
	for (const auto constrId : GetConstraintIdVector(first))
	{
		auto constr = constrContainer.find(constrId);
		if (constr == constrContainer.end())
			throw std::runtime_error("Referenced constraint does not exist!");

		//check direct relation
		ValueId second = constr->second.GetOther(first);

		if (IsConstant(second))
		{
			//both first and second are constants
			int64_t Value = static_cast<int64_t>(constantContainer.at(second));

			switch (constr->second.relation)
			{
			case CmpFlags::Eq:
				return Value == 0;
			case CmpFlags::Neq:
				if (Value == 0)
					return false;
				break;
			case CmpFlags::Gt:
				if (Value >= 0)
					return false;
				break;
			case CmpFlags::GtEq:
				if (Value > 0)
					return false;
				break;
			case CmpFlags::Lt:
				if (Value <= 0)
					return false;
				break;
			case CmpFlags::LtEq:
				if (Value < 0)
					return false;
				break;
			default:
				break;
			}

		}

		//TODO::Check in other layers, maybe recursion?
	}

	return boost::indeterminate;
}

bool ValueContainer::IsUnknown(ValueId first) const
{
	return constantContainer.find(first) == constantContainer.end();
}


ValueId ValueContainer::Cmp(ValueId first, ValueId second, Type type, CmpFlags flags)
{
	boost::tribool result = IsCmp(first, second, type, flags);
	if (result == boost::indeterminate)
		throw std::runtime_error("Fuck if I know how to create boolean from indeterminate relation.");
	
	return CreateConstIntVal(static_cast<uint64_t>(result.value));
}

void ValueContainer::Assume(ValueId first, ValueId second, Type type, CmpFlags flags)
{
	InsertConstraint(BinaryConstraint(first, second, flags));
}

void ValueContainer::AssumeTrue(ValueId first)
{
	InsertConstraint(BinaryConstraint(Zero, first, CmpFlags::Neq)); //equal to 1?
}

void ValueContainer::AssumeFalse(ValueId first)
{
	InsertConstraint(BinaryConstraint(Zero, first, CmpFlags::Eq));
}

ValueId ValueContainer::BinOp(ValueId first, ValueId second, Type type, BinaryOpOptions options)
{
	auto lhs = constantContainer.find(first);
	auto rhs = constantContainer.find(second);

	//not constants, return unknown
	if (lhs == constantContainer.end() || rhs == constantContainer.end())
		return CreateVal(type);


	//both constants - perform operation
	size_t numOfBits = type.GetBitWidth();
	uint64_t uValueLhs = RipBits(numOfBits, lhs->second);
	uint64_t uValueRhs = RipBits(numOfBits, rhs->second);
	int64_t sValueLhs = SignExtend64(numOfBits, lhs->second);
	int64_t sValueRhs = SignExtend64(numOfBits, rhs->second);

	uint64_t uMax = RipBits(type.GetBitWidth(), ULLONG_MAX);
	int64_t sMax = static_cast<int64_t>(RipBits(type.GetBitWidth() - 1, ULLONG_MAX));
	int64_t sMin = static_cast<int64_t>(1ULL << (type.GetBitWidth() - 1));


	switch (options.opKind)
	{
	case BinaryOpKind::Add:{
		//check for overflow
		if (CHECK_FLAG(options.flags, ArithFlags::NoUnsignedWrap))
		{
			if (uMax - uValueLhs < uValueRhs)	//unsigned overflow
				return CreateVal(type);
		}
		else if (CHECK_FLAG(options.flags, ArithFlags::NoSignedWrap))
		{
			if (sValueLhs * sValueRhs > 0) //<-potential overflow?
			{
				if (sValueLhs >= 0)
				{
					if (sMax - sValueLhs < sValueRhs) //signed overflow
						return CreateVal(type);
				}
				else
					if (sMin - sValueLhs > sValueRhs) //signed underflow
						return CreateVal(type);
			}
		}


		uint64_t result = uValueLhs + uValueRhs;
		return CreateConstIntVal(result, type);
	}
	case BinaryOpKind::Sub: {
		//check for overflow
		if (CHECK_FLAG(options.flags, ArithFlags::NoUnsignedWrap))
		{
			if ( uValueRhs > uValueLhs) //unsigned underflow
				return CreateVal(type);
		}
		else if (CHECK_FLAG(options.flags, ArithFlags::NoSignedWrap))
		{
			if (sValueLhs * sValueRhs < 0)//<-potential overflow?
			{
				if (sValueLhs >= 0)
				{
					if (sMax + sValueRhs < sValueLhs) //signed overflow
						return CreateVal(type);
				}
				else
					if (sMin + sValueRhs > sValueLhs) //signed underflow
						return CreateVal(type);
			}
		}


		uint64_t result = uValueLhs - uValueRhs;
		return CreateConstIntVal(result, type);
	}
	case BinaryOpKind::Mul: {
		//check for overflow
		if (CHECK_FLAG(options.flags, ArithFlags::NoUnsignedWrap))
		{
			//on how many bits we need to represent the result?
			uint64_t bitsNeeded = BSR(uValueLhs) + BSR(uValueRhs);

			//overflow
			if (bitsNeeded > type.GetBitWidth())
				return CreateVal(type);
		}
		else if (CHECK_FLAG(options.flags, ArithFlags::NoSignedWrap))
		{
			uint64_t bitsNeeded = BSR( static_cast<uint64_t>(sValueLhs < 0 ? -sValueLhs : sValueLhs)) + BSR( static_cast<uint64_t>(sValueRhs < 0 ? -sValueRhs : sValueRhs));

			//overflow
			if (bitsNeeded > type.GetBitWidth())
				return CreateVal(type);
		}


		uint64_t result = static_cast<uint64_t>(sValueLhs * sValueRhs);
		return CreateConstIntVal(result, type);
	}
	case BinaryOpKind::Div: {
		uint64_t result;

		if (uValueRhs == 0)
			return CreateVal(type);

		switch (options.flags)
		{
		case ArithFlags::Unsigned:
			result = uValueLhs / uValueRhs;
			break;
		case ArithFlags::Signed:
			result = static_cast<uint64_t>(sValueLhs / sValueRhs);
			break;
		default:
			throw std::runtime_error("Undefined type of division.");
		}

		return CreateConstIntVal(result, type);
	}
	case BinaryOpKind::Rem: {
		uint64_t result;

		if (uValueRhs == 0)
			return CreateVal(type);

		switch (options.flags)
		{
		case ArithFlags::Unsigned:
			result = uValueLhs % uValueRhs;
			break;
		case ArithFlags::Signed:
			result = static_cast<uint64_t>(sValueLhs % sValueRhs);
			break;
		default:
			throw std::runtime_error("Undefined type of remainder.");
		}

		return CreateConstIntVal(result, type);
	}
	case BinaryOpKind::And: {
		return CreateConstIntVal(uValueLhs & uValueRhs, type);
	}
	case BinaryOpKind::Or: {
		return CreateConstIntVal(uValueLhs | uValueRhs, type);
	}
	case BinaryOpKind::Xor: {
		return CreateConstIntVal(uValueLhs ^ uValueRhs, type);
	}
	case BinaryOpKind::Shl: {
		return CreateConstIntVal(uValueLhs << uValueRhs, type);
	}
	case BinaryOpKind::Shr: {
		uint64_t result = uValueLhs >> uValueRhs;
		bool isNegative = (bool)(uValueLhs & (1ULL << (numOfBits - 1)));

		switch (options.flags)
		{
		case ArithFlags::Unsigned:
			break;
		case ArithFlags::Signed:

			if (isNegative)
			{
				int64_t origBits = numOfBits - uValueRhs;
				if (origBits < 0)
					result = (uint64_t)(-1LL);
				else
					result |= ~((1ULL << origBits) - 1);
			}
			break;

		default:
			throw std::runtime_error("Undefined type of shift.");
		}
		return CreateConstIntVal(result, type);

	}
	default:
		throw std::runtime_error("Operation not specified!");
		break;
	}
}

ValueId ValueContainer::BitNot(ValueId first, Type type)
{
	auto lhs = constantContainer.find(first);

	//if not a constant, return unkown
	if (lhs == constantContainer.end())
		return CreateVal(type);

	uint64_t value = ~(lhs->second);

	return CreateConstIntVal(value, type);
}


ValueId ValueContainer::ExtendInt(ValueId first, Type sourceType, Type targetType, ArithFlags flags)
{
	auto val = constantContainer.find(first);

	//if unknown return new unkown
	if (val == constantContainer.end())
		return CreateVal(targetType);

	uint64_t value = val->second;

	//if signed flag perform sign extention
	if (CHECK_FLAG(flags, ArithFlags::Signed))
		value = SignExtend64(sourceType.GetBitWidth(), value);
	else
		value = RipBits(sourceType.GetBitWidth(), value);

	//find or create ValueId
	return CreateConstIntVal(value, targetType);
}

ValueId ValueContainer::TruncateInt(ValueId first, Type sourceType, Type targetType)
{
	auto val = constantContainer.find(first);

	//if not already in const container return new unknown
	if (val == constantContainer.end())
		return CreateVal(targetType);

	else
		return first;
}

ValueId ValueContainer::CreateConstIntVal(uint64_t value)
{
	//if exists return existing
	for (auto &item : constantContainer)
	{
		if (item.second == value)
			return item.first;
	}

	//else create new
	ValueId ret = ValueId::GetNextId();
	constantContainer[ret] = value;

	return ret;
}

void ValueContainer::PrintDebug() const
{
	using namespace std;
	cout << "Constants:\n___________________________________\n";
	for (auto constant : constantContainer)
	{
		cout << "#" << static_cast<uint64_t>(constant.first) << " = " << constant.second << std::endl;
	}

	cout << "\n\nConstraints:\n__________________________________________\n";
	for (auto constraint : constrContainer)
	{
		if (IsConstant(constraint.second.first))
			cout << constantContainer.at(constraint.second.first);
		else
			cout << "#" << static_cast<uint64_t>(constraint.second.first);

		cout << " " << static_cast<uint64_t>(constraint.second.relation) << " ";
		
		if (IsConstant(constraint.second.second))
			cout << constantContainer.at(constraint.second.second);
		else 
			cout << "#" << static_cast<uint64_t>(constraint.second.second);

		cout << std::endl;
	}
}
