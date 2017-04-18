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

using std::to_string;

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
  graph->addNode(to_string(o.GetId()))
    ->setAttr("shape", "box")
    ->setAttr("color", "blue")
    ->setAttr("fontcolor", "blue")
    ->setAttr("label", "O#" + to_string(o.GetId()) + " ");

  for (HvEdge edge : o.GetHvOutEdges())
  {
    graph->addNode("V" + to_string(edge.GetValue()))
      ->setAttr("shape", "ellipse")
      ->setAttr("color", "black")
      ->setAttr("fontcolor", "green")
      ->setAttr("label", "V#" + to_string(edge.GetValue()));
    graph->addEdge(to_string(o.GetId()), "V" + to_string(edge.GetValue()))
      ->setAttr("color", "blue")
      ->setAttr("label", to_string(edge.GetSourceOffset()));
  }

  for (PtEdge edge : o.GetPtOutEdges())
  {
    graph->addNode("V" + to_string(edge.GetValue()))
      ->setAttr("shape", "ellipse")
      ->setAttr("color", "black")
      ->setAttr("fontcolor", "green")
      ->setAttr("label", "V#" + to_string(edge.GetValue()));
    graph->addEdge(to_string(o.GetId()), "V" + to_string(edge.GetValue()))
      ->setAttr("color", "black")
      ->setAttr("label", to_string(edge.GetSourceOffset()));
    graph->addEdge("V" + to_string(edge.GetValue()), to_string(edge.GetTargetObject().GetId()))
      ->setAttr("color", "blue")
      ->setAttr("label", to_string(edge.GetTargetOffset()));
  }

}
void SmgPrinter::Visit(Region r)
{
  memgraph::Subgraph *s;
  s = graph->addSubgraph("R" + to_string(r.GetId()));
  s //->setAttr("rank", "same")
    ->setAttr("label", "")
    ->setAttr("color", "black")
    ->setAttr("fontcolor", "black")
    ->setAttr("bgcolor", "white")
    ->setAttr("penwidth", 1.0)
    ->setAttr("style", "dashed");
  s->addNode(to_string(r.GetId()))
    ->setAttr("shape", "box")
    ->setAttr("color", "black")
    ->setAttr("fontcolor", "black")
    ->setAttr("label", "R#" + to_string(r.GetId()) + "\n[sz=" + to_string(r.GetSize()) + "B,va=" + to_string(r.IsValid()) + "]");

  for (HvEdge edge : r.GetHvOutEdges())
  {
    s->addNode("R" + to_string(r.GetId()) + to_string(edge.GetValue()))
      ->setAttr("shape", "box")
      ->setAttr("color", "black")
      ->setAttr("fontcolor", "black")
      ->setAttr("label", "F#" + to_string(edge.GetValue()));
    s->addEdge(to_string(r.GetId()), std::string("R") + to_string(r.GetId()) + to_string(edge.GetValue()))
      ->setAttr("color", "black")
      ->setAttr("fontcolor", "blue")
      ->setAttr("label", to_string(edge.GetSourceOffset()));
  }

  for (PtEdge edge : r.GetPtOutEdges())
  {
    s->addNode("R" + to_string(r.GetId()) + to_string(edge.GetValue()))
      ->setAttr("shape", "box")
      ->setAttr("color", "black")
      ->setAttr("fontcolor", "black")
      ->setAttr("label", "F#" + to_string(edge.GetValue()));
    s->addEdge(to_string(r.GetId()), "R" + to_string(r.GetId()) + to_string(edge.GetValue()))
      ->setAttr("color", "black")
      ->setAttr("label", to_string(edge.GetSourceOffset()));
    s->addEdge("R" + to_string(r.GetId()) + to_string(edge.GetValue()), to_string(edge.GetTargetObject().GetId()))
      ->setAttr("color", "blue")
      ->setAttr("label", to_string(edge.GetTargetOffset()));
  }
}
void SmgPrinter::Visit(Dls s)
{
  Visit(*reinterpret_cast<Sls*>(&s));
}
void SmgPrinter::Visit(Sls s)
{
  memgraph::Subgraph *sub;
  sub = graph->addSubgraph("SLS" + to_string(s.GetId()));
  sub->setAttr("rank", "same")
    ->setAttr("label", "SLS " + to_string(s.GetRank()))
    ->setAttr("color", "red")
    ->setAttr("fontcolor", "red")
    ->setAttr("bgcolor", "white")
    ->setAttr("penwidth", 3.0)
    ->setAttr("style", "dashed");
  sub->addNode(to_string(s.GetId()))
    ->setAttr("shape", "box")
    ->setAttr("color", "orange")
    ->setAttr("fontcolor", "orange")
    ->setAttr("label", "SLS#" + to_string(s.GetId()) + " [sz=" + to_string(s.GetSize()) + " B]");

  for (HvEdge edge : s.GetHvOutEdges())
  {
    sub->addNode("S" + to_string(s.GetId()) + to_string(edge.GetValue()))
      ->setAttr("shape", "box")
      ->setAttr("color", "red")
      ->setAttr("fontcolor", "red")
      ->setAttr("style", "dashed")
      ->setAttr("label", "F#" + to_string(edge.GetValue()));
    sub->addEdge(to_string(s.GetId()), "S" + to_string(s.GetId()) + to_string(edge.GetValue()))
      ->setAttr("color", "black")
      ->setAttr("fontcolor", "black")
      ->setAttr("label", to_string(edge.GetSourceOffset()));
  }

  for (PtEdge edge : s.GetPtOutEdges())
  {
    sub->addNode("S" + to_string(s.GetId()) + to_string(edge.GetValue()))
      ->setAttr("shape", "box")
      ->setAttr("color", "red")
      ->setAttr("fontcolor", "red")
      ->setAttr("style", "dashed")
      ->setAttr("label", "F#" + to_string(edge.GetValue()));
    sub->addEdge(to_string(s.GetId()), "S" + to_string(s.GetId()) + to_string(edge.GetValue()))
      ->setAttr("color", "black")
      ->setAttr("label", to_string(edge.GetSourceOffset()));
    sub->addEdge("S" + to_string(s.GetId()) + to_string(edge.GetValue()), to_string(edge.GetTargetObject().GetId()))
      ->setAttr("color", "blue")
      ->setAttr("label", to_string(edge.GetTargetOffset()));
  }

}
void SmgPrinter::Visit(Graph g)
{

}

std::string SmgPrinter::GetDot() const
{
  return plotter->getDot();
}
