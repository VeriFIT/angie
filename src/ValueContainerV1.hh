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
/** @file ValueContainerV1.hh */

#pragma once

#include "Values.hh"
#include "Exceptions.hh"

#include <map>
#include <vector>


//BinaryConstraint class representing binary constraint between two Value Representations
//--------------------------------------------------------------
class BinaryConstraint {
public:
	ValueId first;
	ValueId second;
	CmpFlags relation;

	BinaryConstraint() : BinaryConstraint(ValueId(0), ValueId(0), CmpFlags::Default) {}
	BinaryConstraint(ValueId _first, ValueId _second, CmpFlags _relation);

	//checks if certain ValueId is in this binary constraint
	bool    Contains(ValueId id) const { return id == first || id == second; }

	//for ValueId returns other ValueId in the constraint
	ValueId GetOther(ValueId id) const
	{
		if (id == first)
			return second;
		else if (id == second)
			return first;
		else throw std::runtime_error("Constraint does not contain ValueId it should");
	}
};
//--------------------------------------------------------------


//ValueContainer class representing storage of value constraints
//--------------------------------------------------------------

typedef uint64_t ConstraintId;

class ValueContainer : public IValueContainer
{
	//fields
private:
	static ConstraintId nextConstraintIdToGive;
	ValueId Zero; //used very often, precached ValueId of 0, simplifies code

	//containers
	std::map<ValueId, uint64_t>						constantContainer;		//contains mapping from ValueId to constant
	std::map<ConstraintId, BinaryConstraint>		constrContainer;		//contains mapping from ConstraintId to Constraint
	std::map< ValueId, std::vector<ConstraintId>>	constrIdContainer;		//contains mapping from ValueId to all used ConstraintIds

	//methods
	static ConstraintId GetNextConstraintId() { return nextConstraintIdToGive++; }
	
	void InsertConstraint(BinaryConstraint);
	void DeleteConstraint(ConstraintId constrId);
	const std::vector<ConstraintId> &GetConstraintIdVector(ValueId id) const;
	bool IsConstant(ValueId first) const { auto res = constantContainer.find(first); return res != constantContainer.end(); }
	inline uint64_t RipBits(size_t numOfBits, uint64_t in) const;
	inline int64_t SignExtend64(size_t numOfBits, uint64_t in) const;
	inline uint64_t BSR(uint64_t val) const; //bit scan reverse - returns position of first set bit from the left side

public:
	//constructors
	ValueContainer() : constantContainer(), constrContainer(), constrIdContainer() { Zero = CreateConstIntVal(0ULL); }
	ValueContainer(const ValueContainer &c) : constantContainer(c.constantContainer), constrContainer(c.constrContainer), constrIdContainer(c.constrIdContainer), Zero(c.Zero) {}

	// Sets constraint on both values
	boost::tribool IsCmp(ValueId first, ValueId second, Type type, CmpFlags flags) const override;

	boost::tribool IsInternalRepEq(ValueId first, ValueId second) const override;
	boost::tribool IsZero(ValueId first) const override;
	bool           IsUnknown(ValueId first) const override;

	// Creates new boolean(1bit integer) value expressing the constraint
	ValueId Cmp(ValueId first, ValueId second, Type type, CmpFlags flags) override;

	void    Assume(ValueId first, ValueId second, Type type, CmpFlags flags) override;
	void    AssumeTrue(ValueId first) override; // Sets contraint: first != 0 ( == true )
	void    AssumeFalse(ValueId first) override; // Sets contraint: first == 0 ( == false)
	
	//operations
	ValueId BinOp(ValueId first, ValueId second, Type type, BinaryOpOptions options) override;
	ValueId BitNot(ValueId first, Type type) override;

	ValueId ExtendInt(ValueId first, Type sourceType, Type targetType, ArithFlags flags) override;
	ValueId TruncateInt(ValueId first, Type sourceType, Type targetType) override;

	ValueId CreateVal(Type type) override { return ValueId::GetNextId(); }
	ValueId CreateConstIntVal(uint64_t value, Type type) override { return CreateConstIntVal(value); }
	ValueId CreateConstIntVal(uint64_t value) override;

	void PrintDebug() const override;

protected:

	ValueId GetZero() const override { return Zero; };
	ValueId GetZero(Type type) const override { return Zero; }
};

//--------------------------------------------------------------