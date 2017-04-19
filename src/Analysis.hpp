#pragma once

#include "Definitions.hpp"
#include "ICfgNode.hpp"

//TODO@michkot: should there be the Handle word in the class names?

class LlvmFunctionWrapper {


};

//TODO@michkot: rename to Function
using FunctionWrapper = LlvmFunctionWrapper;

class FunctionHandle {
public:
  FunctionWrapper       function;
  gsl::owner<ICfgNode*> entryPoint;

  //TODO@michkot: destory CFG on destructor call, add custom move semantics
};

class FrontendHandle {
  virtual ~FrontendHandle() = default;

  //TODO@michkot: implement module handler for LLLVM
};

class Program{
public:
  std::vector<FunctionHandle> functions;
  FrontendHandle              frontendModule;
};

class AnalysisResult {

};

class Analysis {
public:
  Program program;
  std::vector<IState*> states;
  AnalysisResult result;
};

//TODO@michlot: add more fields based on the meetings 11.4, 12.4.
