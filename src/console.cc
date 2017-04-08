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
/** @file console.cc */

#include <llvm/Support/CommandLine.h>
#include <string>
#include <boost/utility/string_view.hpp>
#include <gsl/gsl>

#include "Type.hh"

using namespace ::std;
using namespace ::llvm::cl;

OptionCategory MyCategory("Angie options");
//opt<string> OutputFilename("o", cat(MyCategory), desc("Specify output filename"), value_desc("filename"), init("-"));
opt<string> InputFilename("f", cat(MyCategory), desc("LLVM IR file to perfrom analysis on"), value_desc("filename"), init(""));
opt<bool>   Test("t", cat(MyCategory), desc("Enable test analysis"));
opt<bool>   Lab ("l", cat(MyCategory), desc("Enable laboratory code"));

// laboratory.cc
extern int lab_main();
extern std::vector<std::string> GetExamples();

// main.cc
extern void main_old(gsl::span<std::string> files);

// smg.cc
extern void playground();

int main(int argc, char** argv)
{
  HideUnrelatedOptions(MyCategory);
  ParseCommandLineOptions(argc, argv);

  if (Lab)
  {
    lab_main();
    return 0;
  }
  else if (Test)
  {
    auto files = GetExamples();
    main_old(files);
    return 0;
  }
  else if (InputFilename != "")
  {
    auto files = std::array<std::string,1>{InputFilename};
    main_old(files);
    return 0;
  }
  else
  {
    Type::InitTypeSystem();
    playground();
    return 0;
  }
}

