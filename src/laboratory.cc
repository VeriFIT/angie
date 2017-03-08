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
/** @file laboratory.cc */

#include <vector>
#include <range/v3/all.hpp>
#include <iostream>

using namespace ::ranges;

#if defined(_MSC_VER)

#include <filesystem>
using namespace ::std::experimental;
using namespace filesystem;

std::vector<std::string> GetExamples()
{
  auto examplesFiles = range<directory_iterator>{
    directory_iterator{current_path().append("examples")},
    directory_iterator{}
  };
  auto llFiles = examplesFiles | view::filter([](auto dir) { return dir.path().has_extension() && dir.path().extension() == ".ll"; });
  return llFiles | view::transform([](auto file) { return file.path().generic_string(); });
}

#else

std::vector<std::string> GetExamples()
{
  return {
  "examples/01_mincase_01_nullptr_dereference[dead].ll",
  "examples/01_mincase_02_non_init_dereference[dead].ll",
  "examples/01_mincase_03_normal_dereference[dead].ll",
  "examples/01_mincase_04_all_conditional.ll",
  "examples/01_mincase_05_all_conditional_with_function_calls.ll",
  };
}

#endif

void lab_main()
{
  for (auto i : GetExamples())
  {
    std::cout << i << std::endl;
  }

  //for(auto dir : )

  exit(0);
}
