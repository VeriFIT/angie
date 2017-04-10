/*******************************************************************************

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
/** @file laboratory.cpp */

#include <vector>
#include <iostream>


#if 0 //defined(_MSC_VER)

#include <range/v3/all.hpp>
using namespace ::ranges;

#include <filesystem>
using namespace ::std::experimental;
using namespace filesystem;

std::vector<std::string> GetExamples()
{
  auto examplesFiles = range<directory_iterator>{
    directory_iterator{current_path().append("examples")},
    directory_iterator{}
  };
  auto llFiles = examplesFiles | view::filter([](auto dir) { return dir.path().has_extension() && dir.path().extension() == ".ll"; });
  return llFiles | view::transform([](auto file) { return file.path().generic_string(); });
}

#else

std::vector<std::string> GetExamples()
{
  return {
  "examples/01_mincase_01_nullptr_dereference[dead].ll",
  "examples/01_mincase_02_non_init_dereference[dead].ll",
  "examples/01_mincase_03_normal_dereference[dead].ll",
  "examples/01_mincase_04_all_conditional.ll",
  "examples/01_mincase_05_all_conditional_with_function_calls.ll",
  };
}

#endif
#include <string>
#include <vector>
#include <iostream>

class ImplVisitor;
class WrapVisitor;
class DirectVisitor;

class Context {
public:
  std::string name = "default";
  std::string ToString() { return name; }
  int cid;
  Context(int cid) : cid{cid} {}
};

class ImplA {
public:
  int subclassId;

  int id;
  virtual std::string Join() { return "Base class"; }
  void JoinX(Context* ctx) { std::cout << Join() + " with context " + ctx->ToString(); }

  virtual void accept(ImplVisitor& v);
  virtual void accept(WrapVisitor& v, Context& c);
};
class ImplB : public ImplA {
  virtual std::string Join() override { return "A->B"; }
  virtual void accept(ImplVisitor& v) override;
  virtual void accept(WrapVisitor& v, Context& c) override;
};
class ImplC : public  ImplA {
  virtual std::string Join() override { return "A->C"; }
  virtual void accept(ImplVisitor& v) override;
  virtual void accept(WrapVisitor& v, Context& c) override;
};
class ImplD : public ImplC {
  virtual std::string Join() override { return "A->C->D"; }
  virtual void accept(ImplVisitor& v) override;
  virtual void accept(WrapVisitor& v, Context& c) override;
};
class ImplX : public ImplA {
  virtual std::string Join() override { return "A-X"; }
  virtual void accept(ImplVisitor& v) override;
  virtual void accept(WrapVisitor& v, Context& c) override;
};

class WrapA {
public:
  ImplA* ptr;
  Context* ctx;
  WrapA(ImplA* i, Context* c) : ptr{i}, ctx{c} {}

  int GetSubclassID() { return ptr->subclassId; }
  int GetId() { return ptr->id; }

  void Join () const { std::cout << ptr->Join() + " with context " + ctx->ToString(); }
  void JoinX() const { ptr->JoinX(ctx); }
  std::string JoinDir() { return ptr->Join(); }

  void accept(WrapVisitor& v);
};
class WrapB : public WrapA {
public:
  WrapB(ImplA* i, Context* c) : WrapA{i, c} {}
};
class WrapC : public WrapA {
public:
  WrapC(ImplA* i, Context* c) : WrapA{i, c} {}
};
class WrapD : public WrapC {
public:
  WrapD(ImplA* i, Context* c) : WrapC{i, c} {}
};
class WrapX : public WrapA {
public:
  WrapX(ImplA* i, Context* c) : WrapA{i, c} {}
};

class IImplVisitor {
public:
  virtual void visit(ImplA& it) = 0;
  virtual void visit(ImplB& it) = 0;
  virtual void visit(ImplC& it) = 0;
  virtual void visit(ImplD& it) = 0;
};
class IWrapVisitor {
public:
  virtual void visit(WrapA it) = 0;
  virtual void visit(WrapB it) = 0;
  virtual void visit(WrapC it) = 0;
  virtual void visit(WrapD it) = 0;
};
class ImplVisitor : public IImplVisitor {
public:
  virtual void visit(ImplA& it) override { std::cout << "internall visitor A: " << it.id << std::endl; }
  virtual void visit(ImplB& it) override { std::cout << "internall visitor B: " << it.id << std::endl; }
  virtual void visit(ImplC& it) override { std::cout << "internall visitor C: " << it.id << std::endl; }
  virtual void visit(ImplD& it) override { std::cout << "internall visitor D: " << it.id << std::endl; }
};
class WrapVisitor : public IWrapVisitor {
public:
  virtual void visit(WrapA it) override { std::cout << "wrapper visitor A: " << it.GetId() << std::endl; }
  virtual void visit(WrapB it) override { std::cout << "wrapper visitor B: " << it.GetId() << std::endl; }
  virtual void visit(WrapC it) override { std::cout << "wrapper visitor C: " << it.GetId() << std::endl; }
  virtual void visit(WrapD it) override { std::cout << "wrapper visitor D: " << it.GetId() << std::endl; }
};
class DirectVisitor {
public:
  void visit(WrapA it)
  {
    switch (it.GetSubclassID())
    {
    case 1:
      visitB(static_cast<WrapB&>(it));
      break;
    case 2:
      visitC(static_cast<WrapC&>(it));
      break;
    case 3:
      visitD(static_cast<WrapD&>(it));
      break;
    case 0:
    default:
      visitA(static_cast<WrapB&>(it));
      break;
    }
  }
  void visitA(WrapA& it) 
  { std::cout << "switched wrapper (id#"<< it.GetSubclassID() <<"): " << it.GetId() << std::endl; }
  void visitB(WrapB& it) { visitA(it); } //default
  void visitC(WrapC& it) { visitA(it); } //default
  void visitD(WrapD& it) { visitC(it); } //default
};

inline void ImplA::accept(ImplVisitor & v) { v.visit(*this); };
inline void ImplA::accept(WrapVisitor & v, Context & c) { v.visit(WrapA{this, &c}); };

inline void ImplB::accept(ImplVisitor & v) { v.visit(*this); };
inline void ImplB::accept(WrapVisitor & v, Context & c) { v.visit(WrapB{this, &c}); };

inline void ImplC::accept(ImplVisitor & v) { v.visit(*this); };
inline void ImplC::accept(WrapVisitor & v, Context & c) { v.visit(WrapC{this, &c}); };

inline void ImplD::accept(ImplVisitor & v) { v.visit(*this); };
inline void ImplD::accept(WrapVisitor & v, Context & c) { v.visit(WrapD{this, &c}); };

inline void ImplX::accept(ImplVisitor & v) { v.visit(*this); };
inline void ImplX::accept(WrapVisitor & v, Context & c) { v.visit(WrapX{this, &c}); };

inline void WrapA::accept(WrapVisitor & v) { ptr->accept(v, *ctx); }

ImplA objA;
ImplB objB;
ImplC objC;
ImplD objD;
ImplX objX;
std::vector<ImplA*>* objs;

ImplA* lab_init()
{
  objA.id = 0;  objA.subclassId = 0; // Base class instance, normaly would be abstract class
  objB.id = 11; objB.subclassId = 1; // Derived class B
  objC.id = 22; objC.subclassId = 2; // Derived class C
  objD.id = 33; objD.subclassId = 3; // Derived class D derived from C
  objX.id = 99; objX.subclassId = 9; // Derived class X, no support in visitors

  objs = new std::vector<ImplA*>{&objA, &objB, &objC, &objD, &objX};
  return (*objs)[0];
}
int lab_main()
{
  Context ctx{0};

  auto visitor1 = ImplVisitor();
  auto visitor2 = WrapVisitor();
  auto visitor3 = DirectVisitor();

  ImplA* obj1 = lab_init();
  ImplA* obj2 = lab_init();

  //   {
  //       obj1->Join(); // use the object diretcly
  //       obj2->Join(); // use the object diretcly
  //   }
  {
    for(int i = 0; i < 40; ++i)
    {
      WrapA{obj1+i, &ctx}.Join();    // Called wrapper method uses its field -> materialized on stack
      // GCC will inline the function on inline/O3, clang never
      // mov     qword ptr [rsp + 88], r15
      // mov     qword ptr [rsp + 96], rbp
      // mov     rdi, r13
      // call    WrapA::Join() const
      WrapA{obj1+i, &ctx}.JoinDir(); // Inlines:Forwards the call to object's Join()
      WrapA{obj1+i, &ctx}.JoinX();   // Inlines:Forwards the call to object's JoinX(ctx)
    }
  }

  // double solution
  {
    obj1->accept(visitor1); // use the object diretcly
    obj2->accept(visitor1); // use the object diretcly
  }

  {
    WrapA{obj1, &ctx}.accept(visitor2); // create r-value wrapper and call its function
    WrapA{obj2, &ctx}.accept(visitor2); // create r-value wrapper and call its function
  }

  {

    visitor3.visit(WrapA{obj1, &ctx}); // call directly the visitor and use switch
    visitor3.visit(WrapA{obj2, &ctx}); // call directly the visitor and use switch
  }  
  // vector solution
  {
    auto visitor = ImplVisitor();
    for (ImplA* obj : *objs)
      obj->accept(visitor); // use the object diretcly
  }

  {
    auto visitor = WrapVisitor();
    for (ImplA* obj : *objs)
      WrapA{obj, &ctx}.accept(visitor); // create r-value wrapper and call its function
  }

  {
    auto visitor = DirectVisitor();
    for (ImplA* obj : *objs)
      visitor.visit(WrapA{obj, &ctx}); // call directly the visitor and use switch
  }

  exit(0);
}