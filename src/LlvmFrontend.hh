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
/** @file LlvmFrontend.hh */

#pragma once

#include "Definitions.hh"
#include "IOperation.hh"
#include "Values.hh"

#include "FrontedValueMapper.hh"
#include "CfgNodeBase.hh"

#include <map>
#include <boost/utility/string_view.hpp>

class LlvmCfgNode;

namespace llvm
{
  class BasicBlock;
  class Module;
  class Function;
  class Instruction;
  class Value;
  class Type;
  class Constant;
}

class LlvmCfgParser {
  IOperationFactory& opFactory;
  IValueContainer& vc;
  Mapper& mapper;
  FuncMapper& fmap;

  std::map<const llvm::BasicBlock*, LlvmCfgNode*> copyMapping;
  std::queue<std::tuple<const llvm::BasicBlock*, LlvmCfgNode*, unsigned int>> parseAndLinkTogether;

  ICfgNode* entryPointCfg = nullptr;
  ICfgNode* mainCfg = nullptr;

public:
  LlvmCfgParser(IOperationFactory& opFactory, IValueContainer& vc, Mapper& mapper, FuncMapper& fmap) : opFactory{opFactory}, vc{vc}, mapper{mapper}, fmap{fmap} {}

private:
  IOperation& GetOperationFor(const llvm::Instruction& instruction) const;

  //non null!!
  std::vector<const llvm::Constant*> constantValuesToBeCreated;
  void constantValuesToBeCreatedInsert(const llvm::Constant* c);

  static Type GetValueType(llvm::Type* type);

  static FrontendValueId GetValueId(uint64_t id);
  static FrontendValueId GetValueId(const llvm::Value* value);
  static FrontendValueId GetValueId(const llvm::Value& value);

  FrontendIdTypePair ToIdTypePair(const llvm::Value* value);
  FrontendIdTypePair ToIdTypePair(const llvm::Value& value);

  static OperArg ToOperArg(const llvm::Value* value);
  static OperArg ToOperArg(const llvm::Value& value);
  static OperArg GetEmptyOperArg();
  static OperArg GetFlagsOperArg(CmpFlags flags);
  static OperArg GetFlagsOperArg(ArithFlags flags);
  static OperArg GetFlagsOperArg(CastOpKind kind, ArithFlags flags);

  OperationArgs GetOperArgsForInstr(const llvm::Instruction& instr);

  bool TryGetMappedCfgNode(const llvm::BasicBlock* bb, LlvmCfgNode** outNode);

  void LinkWithOrPlanProcessing(
    LlvmCfgNode* currentNode,
    const llvm::BasicBlock* nextBlock,
    unsigned targetIndex
  );

  LlvmCfgNode& ParseBasicBlock(const llvm::BasicBlock* entryBlock);

  void DealWithConstants();

  ICfgNode& ParseFunction(const llvm::Function& func);

public:
  void ParseModule(llvm::Module& module);

  uptr<llvm::Module> OpenIrFile(std::string fileName);

  void ParseAndOpenIrFile(boost::string_view fileName);

  ICfgNode& GetEntryPoint() { return *entryPointCfg; }
  ICfgNode& GetMain() { return *mainCfg; }
};
