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
/** @file SmgPrinter.hh */

#pragma once

#include "ISmgVisitor.hh"

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

	void Visit(HvEdge&)	override;
	void Visit(PtEdge&)	override;
	void Visit(Object&)	override;
	void Visit(Region&)	override;
	void Visit(Sls&)	override;
	void Visit(Graph&)	override;

	std::string GetDot() const { return plotter->getDot(); }

};