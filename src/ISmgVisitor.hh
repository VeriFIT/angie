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
/** @file ISmgVisitor.hh */

#pragma once

#include "Smg.hh"

class ISmgVisitor {
public:

  virtual void Visit(Smg::HvEdge&) = 0;
  virtual void Visit(Smg::PtEdge&) = 0;
  virtual void Visit(Smg::Object&) = 0;
  virtual void Visit(Smg::Region&) = 0;
  virtual void Visit(Smg::Sls&)    = 0;
  virtual void Visit(Smg::Graph&)  = 0;

};