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
/** @file LlvmGlobals.cc */

#include <llvm/IR/Module.h>

namespace llvm
{
  class DataLayout;
  class LLVMContext;
}

thread_local const llvm::Module* llvmModule = nullptr;
thread_local       llvm::DataLayout llvmDataLayout{"e-m:w-i64:64-f80:128-n8:16:32:64-S128"};
thread_local const llvm::LLVMContext* llvmContext = nullptr;

void setLlvmGlobalVars(llvm::Module* module)
{
  llvmModule = module;
  llvmDataLayout = module->getDataLayout();
  llvmContext = &module->getContext();
}

void initEmptyContext()
{
  auto context = new llvm::LLVMContext{};
  llvmModule = new llvm::Module("EmptyModule", *context);
  llvmDataLayout = llvmModule->getDataLayout();
  llvmContext = context;
}