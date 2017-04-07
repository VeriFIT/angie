
#include "Wrappers.hh"
#include "../ISmgVisitor.hh"

void Smg::Object::Accept(ISmgVisitor& visitor) { visitor.Visit(*this); }
void Smg::HvEdge::Accept(ISmgVisitor& visitor) { visitor.Visit(*this); }
void Smg::PtEdge::Accept(ISmgVisitor& visitor) { visitor.Visit(*this); }
