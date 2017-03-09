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
/** @file SmgCrawler.cc */

#include "SmgCrawler.hh"

void SmgCrawler::CrawlSmg(Object &o)
{
	alreadyVisited.clear();
	for (auto edge : o.GetOutEdges())
	{
		edge.Accept(*this);
	}
}

void SmgCrawler::Visit(HvEdge &hve)
{
	GetInnerVisitor().Visit(hve);
}

void SmgCrawler::Visit(PtEdge &pte)
{
	GetInnerVisitor().Visit(pte);

	auto &object = pte.GetTargetObject();
	if (alreadyVisited.find == alreadyVisited.end)
	{
		//have not been here yet
		alreadyVisited.emplace(object);
		object.Accept(*this);
	}
}

void SmgCrawler::Visit(Object &o)
{
	GetInnerVisitor().Visit(o);

	for (auto edge : o.GetOutEdges())
		edge.Accept(*this);
}

void SmgCrawler::Visit(Region &r)
{
	GetInnerVisitor().Visit(r);

	for (auto edge : r.GetOutEdges())
		edge.Accept(*this);
}
void SmgCrawler::Visit(Sls &s)
{
	GetInnerVisitor().Visit(s);

	for (auto edge : s.GetOutEdges())
		edge.Accept(*this);
}
void SmgCrawler::Visit(Graph &g)
{
	GetInnerVisitor().Visit(g);
}