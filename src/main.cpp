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
/** @file main.cpp */

#include <cstdio>
#include <memory>

#include <functional>
#include <vector>
#include <queue>
#include <stack>
#include <tuple>

#include <boost/range.hpp>

#include <boost/range.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/utility/string_view.hpp>
#include <gsl/gsl>

#include "Definitions.hpp"
#include "General.hpp"
#include "Values.hpp"
#include "IState.hpp"
#include "IOperation.hpp"
#include "ICfgNode.hpp"
#include "LlvmFrontend.hpp"
#include "FrontedValueMapper.hpp"
#include "StateStorage.hpp"

//#define USE_Z3
#if defined(USE_Z3)
#include "ValuesZ3.hpp"
using ValContT = Z3ValueContainer;
#else
#include "ValueContainerV1.hpp"
using ValContT = ValueContainer;
#endif

#include "ForwardNullAnalysis.hpp"
#include "MemoryGraphAnalysis.hpp"

// queue of states waiting for processing
ref_queue<IState> toProcess{};

ValContT vc;

bool printInterpLlvm = false;

void VerificationLoop()
{
  // loop
  while (!StatesManager::IsWorklistEmpty())
  {
    // load state
    IState& state = StatesManager::WorklistDequeue();

    // process state
    {
      // calculate new states
      if (!state.IsNew())
        continue;

      if (printInterpLlvm) 
        state.GetNode().GetDebugInfo();
      state.GetNode().Execute(state);
    }

  }
}

template<typename FactoryT, typename StateT>
void Verify(boost::string_view fileName)
{
  Mapper mapper;
  FuncMapper fmap;

  auto f = FactoryT{};
  LlvmCfgParser parser{f, vc, mapper, fmap};
  parser.ParseAndOpenIrFile(fileName);//("input-int-conv.ll");
  auto& firstNode = parser.GetEntryPoint();

  auto emptyStateUPtr = std::make_unique<StateT>(firstNode, vc, mapper, fmap);

  firstNode.GetStatesManager().InsertAndEnqueue(move(emptyStateUPtr));

  VerificationLoop();
}

#ifdef _WIN32
#include <Windows.h>
#endif

void main_old(gsl::span<std::string> files)
{
#ifdef _WIN32
  SetConsoleTitleA("NextGen");
#endif

  //lab_main();

  //this is a debugging code for experimenting
  //vc.CreateVal(Type{0});
  for (int i = 0; i < 1001; i++)
  {
    vc.CreateConstIntVal(i);
  }
  for (int i = 1; i < 1000; i++)
  {
    vc.CreateConstIntVal(-i);
  }
  //this is end of experimental code

  //Verify("examples/01_mincase_01_nullptr_dereference[dead].ll");
  for (auto& file : files)
  {
    //Verify<FnaOperationFactory, ForwardNullAnalysisState>(file);
    Verify<MemGraphOpFactory, MemoryGraphAnalysisState>(file);
  }

  //vc.PrintDebug();

  //getchar();
  return;
}

std::string monade; // hello there

// the following files have references which results in repated compilation of all the templates 
// compile them as part of this module to cut compilation time

#if 1 //defined(_MSC_VER) 

#include "SmgCrawler.cpp"
#include "SmgPrinter.cpp"

#endif
