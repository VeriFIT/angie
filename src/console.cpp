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
/** @file console.cpp */

#include <llvm/Support/CommandLine.h>
#include <string>
#include <boost/utility/string_view.hpp>
#include <gsl/gsl>

#include "Type.hpp"

// If on windows, use SetConsoleTitleA function to change the name of console window
#ifdef _WIN32
#include <Windows.h>
#endif

using namespace ::std;
using namespace ::llvm::cl;

OptionCategory MyCategory("Angie options");
//opt<string> OutputFilename("o", cat(MyCategory), desc("Specify output filename"), value_desc("filename"), init("-"));
opt<string> InputFilename("f", cat(MyCategory), desc("LLVM IR file to perfrom analysis on"), value_desc("filename"), init(""));
opt<bool>   Print("p", cat(MyCategory), desc("Enable printing of intepreted LLVM IR on stderr"));

// laboratory.cpp
extern int lab_main();
extern std::vector<std::string> GetExamples();

// main.cpp
extern void main_verify_files(gsl::span<std::string> files);
extern bool printInterpLlvm;

// smg.cpp
extern void playground();


int main(int argc, char** argv)
{
  // If on windows, use SetConsoleTitleA function to change the name of console window
#ifdef _WIN32
  SetConsoleTitleA("Angie");
#endif

  HideUnrelatedOptions(MyCategory);
  ParseCommandLineOptions(argc, argv);

  if (Print)
    printInterpLlvm = true;

  if (InputFilename != "")
  {
    auto files = std::array<std::string,1>{InputFilename};
    try
    {
      main_verify_files(files);
    }
    catch (std::exception err)
    {
      std::cerr << "The application encountered an unexpected exception:" << std::endl;
      std::cerr << err.what() << std::endl;
      return 10;
    }
    return 0;
  }
  else
  {
    std::cout << "No command specified. Use --help" << std::endl;
    return 0;
  }
}

