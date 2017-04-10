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
/** @file SmgCrawler.hh */

#pragma once

#include "Definitions.hh"
#include "ISmgVisitor.hh"

#include <set>

// TEMPLATE STRUCT less
template<class _Ty = void>
struct ref_less
{ // functor for operator<
  typedef _Ty first_argument_type;
  typedef _Ty second_argument_type;
  typedef bool result_type;

  constexpr bool operator()(const ref_wr<_Ty> _Left, const ref_wr<_Ty> _Right) const
  { // apply operator< to operands
    return (&static_cast<const _Ty&>(_Left) < &static_cast<const _Ty&>(_Right));
  }
};

typedef uint64_t ObjectId;
class SmgCrawler : public ISmgVisitor {
private:
  ISmgVisitor &innerVisitor;
  std::set<ref_wr<Smg::Object>,ref_less<Smg::Object>> alreadyVisited;

public:
  /*ctr*/SmgCrawler(ISmgVisitor &inner) : innerVisitor{inner}, alreadyVisited{} {}

  void CrawlSmg(Smg::Object); //walk throughout the SMG

  void Visit(Smg::HvEdge) override;
  void Visit(Smg::PtEdge) override;
  void Visit(Smg::Object) override;
  void Visit(Smg::Region) override;
  void Visit(Smg::Sls)    override;
  void Visit(Smg::Graph)  override;

private:
  ISmgVisitor& GetInnerVisitor() { return innerVisitor; }
};