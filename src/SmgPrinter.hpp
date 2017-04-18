/*******************************************************************************

Copyright (C) 2017 Michal Charvát

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
/** @file SmgPrinter.hpp */

#pragma once

#include "ISmgVisitor.hpp"
#include <string>

// do not include memgraph\plotter.h to prevent header poisoning
namespace memgraph {
class Graph;
class Plotter;
}

class SmgPrinter : public ISmgVisitor
{
private:
  memgraph::Graph *graph;
  memgraph::Plotter *plotter;

public:
  /*ctr*/SmgPrinter();
  ~SmgPrinter() { delete plotter; } //plotter deletes graph it contains (for some reason)

  void Visit(Smg::HvEdge) override;
  void Visit(Smg::PtEdge) override;
  void Visit(Smg::Object) override;
  void Visit(Smg::Region) override;
  void Visit(Smg::Dls)    override;
  void Visit(Smg::Sls)    override;
  void Visit(Smg::Graph)  override;

  std::string GetDot() const;

};
