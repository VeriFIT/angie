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
/** @file SmgPrinter.cc */

#include "SmgPrinter.hh"
#include "../memgraph/src/plotter.h"

using namespace Smg; //TODO: replace with namespace Smg {

SmgPrinter::SmgPrinter() : graph{ new memgraph::Graph() }, plotter{ new memgraph::Plotter(graph) } 
{
  graph
    ->setAttr("label", "MemGraph")
    ->setAttr("clusterrank", "local")
    ->setAttr("labelloc", "t");

}

void SmgPrinter::Visit(HvEdge& hve){}
void SmgPrinter::Visit(PtEdge &pte){}

void SmgPrinter::Visit(Object &o)
{
  graph->addNode(o.GetId)
    ->setAttr("shape", "box")
    ->setAttr("color", "blue")
    ->setAttr("fontcolor", "blue")
    ->setAttr("label", "O#" + o.GetId() + " ");

  for (HvEdge &edge : o.GetHvOutEdges())
  {
    graph->addNode("V" + edge.GetValue())
      ->setAttr("shape", "ellipse")
      ->setAttr("color", "black")
      ->setAttr("fontcolor", "green")
      ->setAttr("label", "V#" + edge.GetValue + "]");
    graph->addEdge(o.GetId(), "V" + edge.GetValue())
      ->setAttr("color", "blue")
      ->setAttr("label", edge.GetSourceOffset());
  }

  for (PtEdge &edge : o.GetPtOutEdges())
  {
    graph->addNode("V" + edge.GetValue())
      ->setAttr("shape", "ellipse")
      ->setAttr("color", "black")
      ->setAttr("fontcolor", "green")
      ->setAttr("label", "V#" + edge.GetValue + "]");
    graph->addEdge(o.GetId(), "V" + edge.GetValue())
      ->setAttr("color", "black")
      ->setAttr("label", edge.GetSourceOffset());
    graph->addEdge("V" + edge.GetValue(), edge.GetTargetObject())
      ->setAttr("color", "blue")
      ->setAttr("label", edge.GetTargetOffset());
  }

}
void SmgPrinter::Visit(Region &r)
{
  memgraph::Subgraph *s;
  s = graph->addSubgraph("R" + r.GetId());
  s->setAttr("rank", "same")
    ->setAttr("label", "")
    ->setAttr("color", "black")
    ->setAttr("fontcolor", "black")
    ->setAttr("bgcolor", "white")
    ->setAttr("penwidth", 1.0)
    ->setAttr("style", "dashed");
  s->addNode(r.GetId())
    ->setAttr("shape", "box")
    ->setAttr("color", "black")
    ->setAttr("fontcolor", "black")
    ->setAttr("label", "R#" + r.GetId() + " [SC_ON_HEAP, size = " + r.GetSize() + " B, valid = " + r.IsValid() + "]");

  for (HvEdge &edge : r.GetHvOutEdges())
  {
    s->addNode("R" + r.GetId() + edge.GetValue())
      ->setAttr("shape", "box")
      ->setAttr("color", "black")
      ->setAttr("fontcolor", "black")
      ->setAttr("label", "F#" + edge.GetValue + "]");
    s->addEdge(r.GetId(), "R" + r.GetId() + edge.GetValue())
      ->setAttr("color", "black")
      ->setAttr("fontcolor", "blue")
      ->setAttr("label", edge.GetSourceOffset());
  }

  for (PtEdge &edge : r.GetPtOutEdges())
  {
    s->addNode("R" + r.GetId() + edge.GetValue())
      ->setAttr("shape", "box")
      ->setAttr("color", "black")
      ->setAttr("fontcolor", "black")
      ->setAttr("label", "F#" + edge.GetValue + "]");
    s->addEdge(r.GetId(), "R" + r.GetId() + edge.GetValue())
      ->setAttr("color", "black")
      ->setAttr("label", edge.GetSourceOffset());
    s->addEdge("R" + r.GetId() + edge.GetValue(), edge.GetTargetObject())
      ->setAttr("color", "blue")
      ->setAttr("label", edge.GetTargetOffset());
  }
}
void SmgPrinter::Visit(Sls &s)
{
  memgraph::Subgraph *sub;
  sub = graph->addSubgraph("SLS" + s.GetId());
  sub->setAttr("rank", "same")
    ->setAttr("label", "SLS " + s.GetRank())
    ->setAttr("color", "red")
    ->setAttr("fontcolor", "red")
    ->setAttr("bgcolor", "white")
    ->setAttr("penwidth", 3.0)
    ->setAttr("style", "dashed");
  sub->addNode(s.GetId())
    ->setAttr("shape", "box")
    ->setAttr("color", "orange")
    ->setAttr("fontcolor", "orange")
    ->setAttr("label", "SLS#" + s.GetId() + " [SC_ON_HEAP, size = " + s.GetSize() + " B]");

  for (HvEdge &edge : s.GetHvOutEdges())
  {
    sub->addNode("S" + s.GetId() + edge.GetValue())
      ->setAttr("shape", "box")
      ->setAttr("color", "red")
      ->setAttr("fontcolor", "red")
      ->setAttr("style", "dashed")
      ->setAttr("label", "F#" + edge.GetValue + "]");
    sub->addEdge(s.GetId(), "S" + s.GetId() + edge.GetValue())
      ->setAttr("color", "black")
      ->setAttr("fontcolor", "black")
      ->setAttr("label", edge.GetSourceOffset());
  }

  for (PtEdge &edge : s.GetPtOutEdges())
  {
    sub->addNode("S" + s.GetId() + edge.GetValue())
      ->setAttr("shape", "box")
      ->setAttr("color", "red")
      ->setAttr("fontcolor", "red")
      ->setAttr("style", "dashed")
      ->setAttr("label", "F#" + edge.GetValue + "]");
    sub->addEdge(s.GetId(), "S" + s.GetId() + edge.GetValue())
      ->setAttr("color", "black")
      ->setAttr("label", edge.GetSourceOffset());
    sub->addEdge("S" + s.GetId() + edge.GetValue(), edge.GetTargetObject())
      ->setAttr("color", "blue")
      ->setAttr("label", edge.GetTargetOffset());
  }

}
void SmgPrinter::Visit(Graph &g)
{
  //no info about g yet
}

std::string SmgPrinter::GetDot() const
{
  return plotter->getDot();
}
