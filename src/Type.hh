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
/** @file Type.hh */

#pragma once

#include <string>

#include "ValueId.hh"
class IValueContainer;

#define TYPE_KIND_LLVM  0
#define TYPE_KIND_ANGIE 1

#if !defined(TYPE_KIND)
#define TYPE_KIND TYPE_KIND_LLVM
#endif

#if TYPE_KIND == TYPE_KIND_LLVM

namespace llvm
{
class Type;
}

typedef llvm::Type* FrontendTypeId;    // unique GCC id (or LLVM representation)
class LlvmType;
typedef LlvmType Type;


// Type -- reference to type representation (copyable, assignable)
// Might be later changed to interface(fully abstract class) when multiple frontends are in mind
class LlvmType {
private:
  llvm::Type* frontedId;

public:
  constexpr explicit /*ctr*/ LlvmType(llvm::Type* frontedId) : frontedId{frontedId} {}
  llvm::Type* GetFrontendId() const { return frontedId; }

  static void InitTypeSystem();

  static Type CreateVoidType();
  static Type CreateCharPointerType();
  static Type CreateIntegerType(unsigned bitwidth);
  static Type CreatePointerTo(Type target);
  static Type CreateArrayOf(Type target, uint64_t size);

  bool operator==(const Type& rhs) const
  {
    return GetFrontendId() == rhs.GetFrontendId();
  }

  void        ToString(std::string& str) const;
  std::string ToString() const { std::string s{}; ToString(s); return s; }

  size_t GetSizeOf() const; // Returns type's allocation size
  size_t GetBitWidth() const; // Returns type's bit size

  ValueId GetSizeOfV(IValueContainer& vc) const; // Returns type's allocation size
  ValueId GetBitWidthV(IValueContainer& vc) const; // Returns type's bit size

  bool IsInteger() const;
  bool IsInteger(unsigned bitwidth) const;
  bool IsBool() const { return IsInteger(1u); }
  bool IsReal() const;
  bool IsPointer() const;
  bool IsFunction() const;
  bool IsSimdVector() const;
  bool IsAggregateType() const; // LLVM type group for Arrays and Structs
  bool IsArray() const;
  bool IsStruct() const;
  // bool IsUnion() const; // LLVM does not support Union type
  bool IsVoid() const;

  Type GetPointerToType() const;
  Type GetPointerElementType() const;
  Type GetStructElementType(unsigned index) const;
  size_t GetStructElementOffset(unsigned index) const;

  ValueId GetStructElementOffsetV(unsigned index, IValueContainer& vc) const;
};

#elif TYPE_KIND == TYPE_KIND_ANGIE

class AngieTypeImpl;

typedef const AngieTypeImpl* FrontendTypeId;
class AngieType;
typedef AngieType Type;

class AngieType {
private:
  const AngieTypeImpl* frontedId;

public:
  constexpr explicit /*ctr*/ AngieType(const AngieTypeImpl* frontedId) : frontedId{frontedId} {}
  const AngieTypeImpl* GetFrontendId() const { return frontedId; }

  static void InitTypeSystem() {}

  static Type CreateVoidType();
  static Type CreateCharPointerType();
  static Type CreateIntegerType(unsigned bitwidth);
  static Type CreatePointerTo(Type target);

  bool operator==(const Type& rhs) const
  {
    return GetFrontendId() == rhs.GetFrontendId();
  }

  void        ToString(std::string& str) const;
  std::string ToString() const { std::string s{}; ToString(s); return s; }

  size_t GetSizeOf() const; // Returns type's allocation size
  size_t GetBitWidth() const; // Returns type's bit size

  bool IsInteger() const;
  bool IsInteger(unsigned bitwidth) const;
  bool IsBool() const { return IsInteger(1u); }
  bool IsReal() const;
  bool IsPointer() const;
  bool IsFunction() const;
  bool IsSimdVector() const;
  bool IsAggregateType() const; // LLVM type group for Arrays and Structs
  bool IsArray() const;
  bool IsStruct() const;
  // bool IsUnion() const; // LLVM does not support Union type
  bool IsVoid() const;

  Type GetPointerToType() const;
  Type GetPointerElementType() const;
  Type GetStructElementType(unsigned index) const;
  size_t GetStructElementOffset(unsigned index) const;
};

#endif

#define PTR_TYPE Type::CreateCharPointerType()
