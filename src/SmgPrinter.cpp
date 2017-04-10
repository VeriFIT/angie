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
/** @file SmgPrinter.cpp */

#include "SmgPrinter.hpp"
#include "Smg/Wrappers.hpp"
#include "../memgraph/src/plotter.h"

using namespace Smg; //TODO: replace with namespace Smg {

SmgPrinter::SmgPrinter() : graph{ new memgraph::Graph() }, plotter{ new memgraph::Plotter(graph) } 
{
  graph
    ->setAttr("label", "MemGraph")
    ->setAttr("clusterrank", "local")
    ->setAttr("labelloc", "t");

}

void SmgPrinter::Visit(HvEdge hve){}
void SmgPrinter::Visit(PtEdge pte){}

void SmgPrinter::Visit(Object o)
{
  graph->addNode(std::to_string(o.GetId()))
    ->setAttr("shape", "box")
    ->setAttr("color", "blue")
    ->setAttr("fontcolor", "blue")
    ->setAttr("label", "O#" + std::to_string(o.GetId()) + " ");

  for (HvEdge edge : o.GetHvOutEdges())
  {
    graph->addNode("V" + std::to_string(edge.GetValue()))
      ->setAttr("shape", "ellipse")
      ->setAttr("color", "black")
      ->setAttr("fontcolor", "green")
      ->setAttr("label", "V#" + std::to_string(edge.GetValue()) + "]");
    graph->addEdge(std::to_string(o.GetId()), "V" + std::to_string(edge.GetValue()))
      ->setAttr("color", "blue")
      ->setAttr("label", std::to_string(edge.GetSourceOffset()));
  }

  for (PtEdge edge : o.GetPtOutEdges())
  {
    graph->addNode("V" + std::to_string(edge.GetValue()))
      ->setAttr("shape", "ellipse")
      ->setAttr("color", "black")
      ->setAttr("fontcolor", "green")
      ->setAttr("label", "V#" + std::to_string(edge.GetValue()) + "]");
    graph->addEdge(std::to_string(o.GetId()), "V" + std::to_string(edge.GetValue()))
      ->setAttr("color", "black")
      ->setAttr("label", std::to_string(edge.GetSourceOffset()));
    graph->addEdge("V" + std::to_string(edge.GetValue()), std::to_string(edge.GetTargetObject().GetId()))
      ->setAttr("color", "blue")
      ->setAttr("label", std::to_string(edge.GetTargetOffset()));
  }

}
void SmgPrinter::Visit(Region r)
{
  memgraph::Subgraph *s;
  s = graph->addSubgraph("R" + std::to_string(r.GetId()));
  s->setAttr("rank", "same")
    ->setAttr("label", "")
    ->setAttr("color", "black")
    ->setAttr("fontcolor", "black")
    ->setAttr("bgcolor", "white")
    ->setAttr("penwidth", 1.0)
    ->setAttr("style", "dashed");
  s->addNode(std::to_string(r.GetId()))
    ->setAttr("shape", "box")
    ->setAttr("color", "black")
    ->setAttr("fontcolor", "black")
    ->setAttr("label", "R#" + std::to_string(r.GetId()) + " [SC_ON_HEAP, size = " + std::to_string(r.GetSize()) + " B, valid = " + std::to_string(r.IsValid()) + "]");

  for (HvEdge edge : r.GetHvOutEdges())
  {
    s->addNode("R" + std::to_string(r.GetId()) + std::to_string(edge.GetValue()))
      ->setAttr("shape", "box")
      ->setAttr("color", "black")
      ->setAttr("fontcolor", "black")
      ->setAttr("label", "F#" + std::to_string(edge.GetValue()) + "]");
    s->addEdge(std::to_string(r.GetId()), std::string("R") + std::to_string(r.GetId()) + std::to_string(edge.GetValue()))
      ->setAttr("color", "black")
      ->setAttr("fontcolor", "blue")
      ->setAttr("label", std::to_string(edge.GetSourceOffset()));
  }

  for (PtEdge edge : r.GetPtOutEdges())
  {
    s->addNode("R" + std::to_string(r.GetId()) + std::to_string(edge.GetValue()))
      ->setAttr("shape", "box")
      ->setAttr("color", "black")
      ->setAttr("fontcolor", "black")
      ->setAttr("label", "F#" + std::to_string(edge.GetValue()) + "]");
    s->addEdge(std::to_string(r.GetId()), "R" + std::to_string(r.GetId()) + std::to_string(edge.GetValue()))
      ->setAttr("color", "black")
      ->setAttr("label", std::to_string(edge.GetSourceOffset()));
    s->addEdge("R" + std::to_string(r.GetId()) + std::to_string(edge.GetValue()), std::to_string(edge.GetTargetObject().GetId()))
      ->setAttr("color", "blue")
      ->setAttr("label", std::to_string(edge.GetTargetOffset()));
  }
}
void SmgPrinter::Visit(Sls s)
{
  memgraph::Subgraph *sub;
  sub = graph->addSubgraph("SLS" + std::to_string(s.GetId()));
  sub->setAttr("rank", "same")
    ->setAttr("label", "SLS " + std::to_string(s.GetRank()))
    ->setAttr("color", "red")
    ->setAttr("fontcolor", "red")
    ->setAttr("bgcolor", "white")
    ->setAttr("penwidth", 3.0)
    ->setAttr("style", "dashed");
  sub->addNode(std::to_string(s.GetId()))
    ->setAttr("shape", "box")
    ->setAttr("color", "orange")
    ->setAttr("fontcolor", "orange")
    ->setAttr("label", "SLS#" + std::to_string(s.GetId()) + " [SC_ON_HEAP, size = " + std::to_string(s.GetSize()) + " B]");

  for (HvEdge edge : s.GetHvOutEdges())
  {
    sub->addNode("S" + std::to_string(s.GetId()) + std::to_string(edge.GetValue()))
      ->setAttr("shape", "box")
      ->setAttr("color", "red")
      ->setAttr("fontcolor", "red")
      ->setAttr("style", "dashed")
      ->setAttr("label", "F#" + std::to_string(edge.GetValue()) + "]");
    sub->addEdge(std::to_string(s.GetId()), "S" + std::to_string(s.GetId()) + std::to_string(edge.GetValue()))
      ->setAttr("color", "black")
      ->setAttr("fontcolor", "black")
      ->setAttr("label", std::to_string(edge.GetSourceOffset()));
  }

  for (PtEdge edge : s.GetPtOutEdges())
  {
    sub->addNode("S" + std::to_string(s.GetId()) + std::to_string(edge.GetValue()))
      ->setAttr("shape", "box")
      ->setAttr("color", "red")
      ->setAttr("fontcolor", "red")
      ->setAttr("style", "dashed")
      ->setAttr("label", "F#" + std::to_string(edge.GetValue()) + "]");
    sub->addEdge(std::to_string(s.GetId()), "S" + std::to_string(s.GetId()) + std::to_string(edge.GetValue()))
      ->setAttr("color", "black")
      ->setAttr("label", std::to_string(edge.GetSourceOffset()));
    sub->addEdge("S" + std::to_string(s.GetId()) + std::to_string(edge.GetValue()), std::to_string(edge.GetTargetObject().GetId()))
      ->setAttr("color", "blue")
      ->setAttr("label", std::to_string(edge.GetTargetOffset()));
  }

}
void SmgPrinter::Visit(Graph g)
{

}

std::string SmgPrinter::GetDot() const
{
  return plotter->getDot();
}
