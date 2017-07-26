/*******************************************************************************

Copyright (C) 2017 Michal Charvát
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
/** @file SmgCrawler.hpp */

#pragma once

#include "Definitions.hpp"
#include "ISmgVisitor.hpp"

#include <set>

typedef uint64_t ObjectId;
class SmgCrawler : public ISmgVisitor {
private:
  ISmgVisitor &innerVisitor;
  std::set<Smg::ObjectId> alreadyVisited;

public:
  /*ctr*/SmgCrawler(ISmgVisitor &inner) : innerVisitor{inner}, alreadyVisited{} {}

  void CrawlSmg(Smg::Object); //walk throughout the SMG

  void Visit(Smg::HvEdge) override final;
  void Visit(Smg::PtEdge) override final;
  void Visit(Smg::Object) override final;
  void Visit(Smg::Region) override final;
  void Visit(Smg::Dls)    override final;
  void Visit(Smg::Sls)    override final;
  void Visit(Smg::Graph)  override final;

private:
  ISmgVisitor& GetInnerVisitor() { return innerVisitor; }
};