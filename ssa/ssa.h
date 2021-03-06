#ifndef VAIVEN_VISITOR_HEADER_SSA
#define VAIVEN_VISITOR_HEADER_SSA

#include "../asmjit/src/asmjit/asmjit.h"
#include "../type_info.h"
#include "../value.h"
#include "../functions.h"
#include "../runtime_error.h"

#include <string>
#include <vector>
#include <set>

using std::string;
using std::vector;
using std::set;

namespace vaiven { namespace ssa {

enum InstructionType {
  INSTR_PHI,
  INSTR_ARG,
  INSTR_CONSTANT,
  INSTR_CALL,
  INSTR_TYPECHECK,
  INSTR_BOX,
  INSTR_UNBOX,
  INSTR_TO_DBL,
  INSTR_INT_TO_DBL,
  INSTR_ADD,
  INSTR_INT_ADD,
  INSTR_DBL_ADD,
  INSTR_STR_ADD,
  INSTR_SUB,
  INSTR_INT_SUB,
  INSTR_DBL_SUB,
  INSTR_MUL,
  INSTR_INT_MUL,
  INSTR_DBL_MUL,
  INSTR_DIV,
  INSTR_NOT,
  INSTR_CMPEQ,
  INSTR_INT_CMPEQ,
  INSTR_DBL_CMPEQ,
  INSTR_CMPINEQ,
  INSTR_INT_CMPINEQ,
  INSTR_DBL_CMPINEQ,
  INSTR_CMPGT,
  INSTR_INT_CMPGT,
  INSTR_DBL_CMPGT,
  INSTR_CMPGTE,
  INSTR_INT_CMPGTE,
  INSTR_DBL_CMPGTE,
  INSTR_CMPLT,
  INSTR_INT_CMPLT,
  INSTR_DBL_CMPLT,
  INSTR_CMPLTE,
  INSTR_INT_CMPLTE,
  INSTR_DBL_CMPLTE,
  INSTR_DYNAMIC_ACCESS,
  INSTR_DYNAMIC_STORE,
  INSTR_LIST_ACCESS,
  INSTR_LIST_STORE,
  INSTR_LIST_INIT,
  INSTR_DYNAMIC_OBJECT_ACCESS,
  INSTR_DYNAMIC_OBJECT_STORE,
  INSTR_OBJECT_ACCESS,
  INSTR_OBJECT_STORE,
  INSTR_ERR,
  INSTR_RET,
  INSTR_JMPCC,
};

class SsaVisitor;
class Block;

class Instruction {

  public:
  Instruction(InstructionType tag, VaivenStaticType type, bool isBoxed)
    : tag(tag), type(type), isBoxed(isBoxed), next(NULL), block(NULL) {};

  virtual void accept(SsaVisitor& visitor)=0;

  void replaceInput(Instruction* oldInstr, Instruction* newInstr);
  void replaceUsagesWith(Instruction* newInstr);
  void append(Instruction* toAppend);
  virtual ~Instruction();

  InstructionType tag;
  VaivenStaticType type;
  bool isBoxed;
  vector<Instruction*> inputs;
  set<Instruction*> usages;
  Instruction* next;

  // TODO unionize these? store an X86Operand?
  asmjit::X86Gp out;
  asmjit::X86Xmm xmmout;

  Block* block;
};

class ArgInstr;
class PhiInstr;
class ConstantInstr;
class CallInstr;
class TypecheckInstr;
class BoxInstr;
class UnboxInstr;
class ToDoubleInstr;
class IntToDoubleInstr;
class AddInstr;
class IntAddInstr;
class DoubleAddInstr;
class StrAddInstr;
class SubInstr;
class IntSubInstr;
class DoubleSubInstr;
class MulInstr;
class IntMulInstr;
class DoubleMulInstr;
class DivInstr;
class NotInstr;
class CmpEqInstr;
class IntCmpEqInstr;
class DoubleCmpEqInstr;
class CmpIneqInstr;
class IntCmpIneqInstr;
class DoubleCmpIneqInstr;
class CmpGtInstr;
class IntCmpGtInstr;
class DoubleCmpGtInstr;
class CmpGteInstr;
class IntCmpGteInstr;
class DoubleCmpGteInstr;
class CmpLtInstr;
class IntCmpLtInstr;
class DoubleCmpLtInstr;
class CmpLteInstr;
class IntCmpLteInstr;
class DoubleCmpLteInstr;
class DynamicAccessInstr;
class DynamicStoreInstr;
class ListAccessInstr;
class ListStoreInstr;
class ListInitInstr;
class DynamicObjectAccessInstr;
class DynamicObjectStoreInstr;
class ObjectAccessInstr;
class ObjectStoreInstr;
class ErrInstr;
class RetInstr;
class JmpCcInstr;

// from cfg.h
class Block;
class UnconditionalBlockExit;
class ConditionalBlockExit;

class SsaVisitor {
  public:
  virtual void visitPhiInstr(PhiInstr& instr)=0;
  virtual void visitArgInstr(ArgInstr& instr)=0;
  virtual void visitConstantInstr(ConstantInstr& instr)=0;
  virtual void visitCallInstr(CallInstr& instr)=0;
  virtual void visitTypecheckInstr(TypecheckInstr& instr)=0;
  virtual void visitBoxInstr(BoxInstr& instr)=0;
  virtual void visitUnboxInstr(UnboxInstr& instr)=0;
  virtual void visitToDoubleInstr(ToDoubleInstr& instr)=0;
  virtual void visitIntToDoubleInstr(IntToDoubleInstr& instr)=0;
  virtual void visitAddInstr(AddInstr& instr)=0;
  virtual void visitIntAddInstr(IntAddInstr& instr)=0;
  virtual void visitDoubleAddInstr(DoubleAddInstr& instr)=0;
  virtual void visitStrAddInstr(StrAddInstr& instr)=0;
  virtual void visitSubInstr(SubInstr& instr)=0;
  virtual void visitIntSubInstr(IntSubInstr& instr)=0;
  virtual void visitDoubleSubInstr(DoubleSubInstr& instr)=0;
  virtual void visitMulInstr(MulInstr& instr)=0;
  virtual void visitIntMulInstr(IntMulInstr& instr)=0;
  virtual void visitDoubleMulInstr(DoubleMulInstr& instr)=0;
  virtual void visitDivInstr(DivInstr& instr)=0;
  virtual void visitNotInstr(NotInstr& instr)=0;
  virtual void visitCmpEqInstr(CmpEqInstr& instr)=0;
  virtual void visitIntCmpEqInstr(IntCmpEqInstr& instr)=0;
  virtual void visitDoubleCmpEqInstr(DoubleCmpEqInstr& instr)=0;
  virtual void visitCmpIneqInstr(CmpIneqInstr& instr)=0;
  virtual void visitIntCmpIneqInstr(IntCmpIneqInstr& instr)=0;
  virtual void visitDoubleCmpIneqInstr(DoubleCmpIneqInstr& instr)=0;
  virtual void visitCmpGtInstr(CmpGtInstr& instr)=0;
  virtual void visitIntCmpGtInstr(IntCmpGtInstr& instr)=0;
  virtual void visitDoubleCmpGtInstr(DoubleCmpGtInstr& instr)=0;
  virtual void visitCmpGteInstr(CmpGteInstr& instr)=0;
  virtual void visitIntCmpGteInstr(IntCmpGteInstr& instr)=0;
  virtual void visitDoubleCmpGteInstr(DoubleCmpGteInstr& instr)=0;
  virtual void visitCmpLtInstr(CmpLtInstr& instr)=0;
  virtual void visitIntCmpLtInstr(IntCmpLtInstr& instr)=0;
  virtual void visitDoubleCmpLtInstr(DoubleCmpLtInstr& instr)=0;
  virtual void visitCmpLteInstr(CmpLteInstr& instr)=0;
  virtual void visitIntCmpLteInstr(IntCmpLteInstr& instr)=0;
  virtual void visitDoubleCmpLteInstr(DoubleCmpLteInstr& instr)=0;
  virtual void visitDynamicAccessInstr(DynamicAccessInstr& instr)=0;
  virtual void visitDynamicStoreInstr(DynamicStoreInstr& instr)=0;
  virtual void visitListAccessInstr(ListAccessInstr& instr)=0;
  virtual void visitListStoreInstr(ListStoreInstr& instr)=0;
  virtual void visitListInitInstr(ListInitInstr& instr)=0;
  virtual void visitDynamicObjectAccessInstr(DynamicObjectAccessInstr& instr)=0;
  virtual void visitDynamicObjectStoreInstr(DynamicObjectStoreInstr& instr)=0;
  virtual void visitObjectAccessInstr(ObjectAccessInstr& instr)=0;
  virtual void visitObjectStoreInstr(ObjectStoreInstr& instr)=0;
  virtual void visitErrInstr(ErrInstr& instr)=0;
  virtual void visitRetInstr(RetInstr& instr)=0;
  virtual void visitJmpCcInstr(JmpCcInstr& instr)=0;

  virtual void visitBlock(Block& block)=0;
  virtual void visitUnconditionalBlockExit(UnconditionalBlockExit& exit)=0;
  virtual void visitConditionalBlockExit(ConditionalBlockExit& exit)=0;
};

class PhiInstr : public Instruction {
  public:
  PhiInstr() : Instruction(INSTR_PHI, VAIVEN_STATIC_TYPE_UNKNOWN, true) {
  };
  PhiInstr(Instruction* optionA, Instruction* optionB)
      : Instruction(INSTR_PHI, VAIVEN_STATIC_TYPE_UNKNOWN, true) {
    inputs.push_back(optionA);
    inputs.push_back(optionB);
    optionA->usages.insert(this);
    optionB->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitPhiInstr(*this);
  }
};

class ArgInstr : public Instruction {
  public:
  ArgInstr(int argi, VaivenStaticType basedOnUsage)
      : argi(argi), Instruction(INSTR_ARG, basedOnUsage, true) {};

  int argi;

  void accept(SsaVisitor& visitor) {
    visitor.visitArgInstr(*this);
  }
};

class ConstantInstr : public Instruction {
  public:
  ConstantInstr(Value val) : val(val), Instruction(INSTR_CONSTANT, val.getStaticType(), false) {};

  Value val;

  void accept(SsaVisitor& visitor) {
    visitor.visitConstantInstr(*this);
  }
};

class CallInstr : public Instruction {
  public:
  CallInstr(string funcName, Function& func)
      : funcName(funcName), func(func), Instruction(INSTR_CALL, VAIVEN_STATIC_TYPE_UNKNOWN, true) {};

  string funcName;
  Function& func;

  void accept(SsaVisitor& visitor) {
    visitor.visitCallInstr(*this);
  }
};

class TypecheckInstr : public Instruction {
  public:
  TypecheckInstr(Instruction* what, VaivenStaticType checkType)
      : Instruction(INSTR_TYPECHECK, checkType, false), safelyDeletable(false) {
    inputs.push_back(what);
    what->usages.insert(this);
  };

  bool safelyDeletable;

  void accept(SsaVisitor& visitor) {
    visitor.visitTypecheckInstr(*this);
  }
};

class BoxInstr : public Instruction {
  public:
  BoxInstr(Instruction* what) : Instruction(INSTR_BOX, what->type, true) {
    inputs.push_back(what);
    what->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitBoxInstr(*this);
  }
};

class UnboxInstr : public Instruction {
  public:
  // currently only doubles have an unboxing step
  UnboxInstr(Instruction* what) : Instruction(INSTR_UNBOX, VAIVEN_STATIC_TYPE_DOUBLE, false) {
    inputs.push_back(what);
    what->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitUnboxInstr(*this);
  }
};

class ToDoubleInstr : public Instruction {
  public:
  // currently only doubles have an unboxing step
  ToDoubleInstr(Instruction* what)
      : Instruction(INSTR_TO_DBL, VAIVEN_STATIC_TYPE_DOUBLE, false),
      safelyDeletable(false) {
    inputs.push_back(what);
    what->usages.insert(this);
  };

  bool safelyDeletable;

  void accept(SsaVisitor& visitor) {
    visitor.visitToDoubleInstr(*this);
  }
};

class IntToDoubleInstr : public Instruction {
  public:
  // currently only doubles have an unboxing step
  IntToDoubleInstr(Instruction* what) : Instruction(INSTR_INT_TO_DBL, VAIVEN_STATIC_TYPE_DOUBLE, false) {
    inputs.push_back(what);
    what->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitIntToDoubleInstr(*this);
  }
};

class AddInstr : public Instruction {
  public:
  AddInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_ADD, VAIVEN_STATIC_TYPE_UNKNOWN, true), safelyDeletable(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  // Impure, due to typechecking which can throw, unless shown otherwise.
  bool safelyDeletable;

  void accept(SsaVisitor& visitor) {
    visitor.visitAddInstr(*this);
  }
};

class IntAddInstr : public Instruction {
  public:
  IntAddInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_INT_ADD, VAIVEN_STATIC_TYPE_INT, false), hasConstRhs(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  bool hasConstRhs;
  int constRhs;

  void accept(SsaVisitor& visitor) {
    visitor.visitIntAddInstr(*this);
  }
};

class DoubleAddInstr : public Instruction {
  public:
  DoubleAddInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_DBL_ADD, VAIVEN_STATIC_TYPE_DOUBLE, false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitDoubleAddInstr(*this);
  }
};

class StrAddInstr : public Instruction {
  public:
  StrAddInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_STR_ADD, VAIVEN_STATIC_TYPE_STRING, false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitStrAddInstr(*this);
  }
};

class SubInstr : public Instruction {
  public:
  SubInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_SUB, VAIVEN_STATIC_TYPE_UNKNOWN, true), safelyDeletable(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  // Impure, due to typechecking which can throw, unless shown otherwise.
  bool safelyDeletable;

  void accept(SsaVisitor& visitor) {
    visitor.visitSubInstr(*this);
  }
};

class IntSubInstr : public Instruction {
  public:
  IntSubInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_INT_SUB, VAIVEN_STATIC_TYPE_INT, false), hasConstLhs(false), isInverse(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  bool isInverse;
  bool hasConstLhs;
  int constLhs;

  void accept(SsaVisitor& visitor) {
    visitor.visitIntSubInstr(*this);
  }
};

class DoubleSubInstr : public Instruction {
  public:
  DoubleSubInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_DBL_SUB, VAIVEN_STATIC_TYPE_DOUBLE, false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitDoubleSubInstr(*this);
  }
};

class MulInstr : public Instruction {
  public:
  MulInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_MUL, VAIVEN_STATIC_TYPE_UNKNOWN, true), safelyDeletable(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  // Impure, due to typechecking which can throw, unless shown otherwise.
  bool safelyDeletable;

  void accept(SsaVisitor& visitor) {
    visitor.visitMulInstr(*this);
  }
};

class IntMulInstr : public Instruction {
  public:
  IntMulInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_INT_MUL, VAIVEN_STATIC_TYPE_INT, false), hasConstRhs(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  bool hasConstRhs;
  int constRhs;

  void accept(SsaVisitor& visitor) {
    visitor.visitIntMulInstr(*this);
  }
};

class DoubleMulInstr : public Instruction {
  public:
  DoubleMulInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_DBL_MUL, VAIVEN_STATIC_TYPE_DOUBLE, false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitDoubleMulInstr(*this);
  }
};

class DivInstr : public Instruction {
  public:
  DivInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_DIV, VAIVEN_STATIC_TYPE_DOUBLE, false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitDivInstr(*this);
  }
};

class NotInstr : public Instruction {
  public:
  NotInstr(Instruction* expr) : Instruction(INSTR_NOT, VAIVEN_STATIC_TYPE_BOOL, false) {
    inputs.push_back(expr);
    expr->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitNotInstr(*this);
  }
};

class CmpEqInstr : public Instruction {
  public:
  CmpEqInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_CMPEQ, VAIVEN_STATIC_TYPE_BOOL, false), safelyDeletable(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  bool safelyDeletable;

  void accept(SsaVisitor& visitor) {
    visitor.visitCmpEqInstr(*this);
  }
};

class IntCmpEqInstr : public Instruction {
  public:
  IntCmpEqInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_INT_CMPEQ, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };
  IntCmpEqInstr(Instruction* lhs, int rhsI32)
      : Instruction(INSTR_INT_CMPEQ, VAIVEN_STATIC_TYPE_BOOL, false),
      hasConstRhs(true), constI32Rhs(rhsI32) {
    inputs.push_back(lhs);
    lhs->usages.insert(this);
  };

  int constI32Rhs;
  bool hasConstRhs;

  void accept(SsaVisitor& visitor) {
    visitor.visitIntCmpEqInstr(*this);
  }
};

class DoubleCmpEqInstr : public Instruction {
  public:
  DoubleCmpEqInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_DBL_CMPEQ, VAIVEN_STATIC_TYPE_BOOL, false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitDoubleCmpEqInstr(*this);
  }
};

class CmpIneqInstr : public Instruction {
  public:
  CmpIneqInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_CMPINEQ, VAIVEN_STATIC_TYPE_BOOL, false), safelyDeletable(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  bool safelyDeletable;

  void accept(SsaVisitor& visitor) {
    visitor.visitCmpIneqInstr(*this);
  }
};

class IntCmpIneqInstr : public Instruction {
  public:
  IntCmpIneqInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_INT_CMPINEQ, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };
  IntCmpIneqInstr(Instruction* lhs, int rhsI32)
      : Instruction(INSTR_INT_CMPINEQ, VAIVEN_STATIC_TYPE_BOOL, false),
    hasConstRhs(true), constI32Rhs(rhsI32) {
    inputs.push_back(lhs);
    lhs->usages.insert(this);
  };

  int constI32Rhs;
  bool hasConstRhs;

  void accept(SsaVisitor& visitor) {
    visitor.visitIntCmpIneqInstr(*this);
  }
};

class DoubleCmpIneqInstr : public Instruction {
  public:
  DoubleCmpIneqInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_DBL_CMPINEQ, VAIVEN_STATIC_TYPE_BOOL, false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitDoubleCmpIneqInstr(*this);
  }
};

class CmpGtInstr : public Instruction {
  public:
  CmpGtInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_CMPGT, VAIVEN_STATIC_TYPE_BOOL, false), safelyDeletable(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  bool safelyDeletable;

  void accept(SsaVisitor& visitor) {
    visitor.visitCmpGtInstr(*this);
  }
};

class IntCmpGtInstr : public Instruction {
  public:
  IntCmpGtInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_INT_CMPGT, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };
  IntCmpGtInstr(Instruction* lhs, int rhs)
      : Instruction(INSTR_INT_CMPGT, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(true), constRhs(rhs) {
    inputs.push_back(lhs);
    lhs->usages.insert(this);
  };

  int constRhs;
  bool hasConstRhs;

  void accept(SsaVisitor& visitor) {
    visitor.visitIntCmpGtInstr(*this);
  }
};

class DoubleCmpGtInstr : public Instruction {
  public:
  DoubleCmpGtInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_DBL_CMPGT, VAIVEN_STATIC_TYPE_BOOL, false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitDoubleCmpGtInstr(*this);
  }
};

class CmpGteInstr : public Instruction {
  public:
  CmpGteInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_CMPGTE, VAIVEN_STATIC_TYPE_BOOL, false), safelyDeletable(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  bool safelyDeletable;

  void accept(SsaVisitor& visitor) {
    visitor.visitCmpGteInstr(*this);
  }
};

class IntCmpGteInstr : public Instruction {
  public:
  IntCmpGteInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_INT_CMPGTE, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };
  IntCmpGteInstr(Instruction* lhs, int rhs)
      : Instruction(INSTR_INT_CMPGTE, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(true), constRhs(rhs) {
    inputs.push_back(lhs);
    lhs->usages.insert(this);
  };

  int constRhs;
  bool hasConstRhs;

  void accept(SsaVisitor& visitor) {
    visitor.visitIntCmpGteInstr(*this);
  }
};

class DoubleCmpGteInstr : public Instruction {
  public:
  DoubleCmpGteInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_DBL_CMPGTE, VAIVEN_STATIC_TYPE_BOOL, false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitDoubleCmpGteInstr(*this);
  }
};

class CmpLtInstr : public Instruction {
  public:
  CmpLtInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_CMPLT, VAIVEN_STATIC_TYPE_BOOL, false), safelyDeletable(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  bool safelyDeletable;

  void accept(SsaVisitor& visitor) {
    visitor.visitCmpLtInstr(*this);
  }
};

class IntCmpLtInstr : public Instruction {
  public:
  IntCmpLtInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_INT_CMPLT, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };
  IntCmpLtInstr(Instruction* lhs, int rhs)
      : Instruction(INSTR_INT_CMPLT, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(true), constRhs(rhs) {
    inputs.push_back(lhs);
    lhs->usages.insert(this);
  };

  int constRhs;
  bool hasConstRhs;

  void accept(SsaVisitor& visitor) {
    visitor.visitIntCmpLtInstr(*this);
  }
};

class DoubleCmpLtInstr : public Instruction {
  public:
  DoubleCmpLtInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_DBL_CMPLT, VAIVEN_STATIC_TYPE_BOOL, false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitDoubleCmpLtInstr(*this);
  }
};

class CmpLteInstr : public Instruction {
  public:
  CmpLteInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_CMPLTE, VAIVEN_STATIC_TYPE_BOOL, false), safelyDeletable(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  bool safelyDeletable;

  void accept(SsaVisitor& visitor) {
    visitor.visitCmpLteInstr(*this);
  }
};

class IntCmpLteInstr : public Instruction {
  public:
  IntCmpLteInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_INT_CMPLTE, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };
  IntCmpLteInstr(Instruction* lhs, int rhs)
      : Instruction(INSTR_INT_CMPLTE, VAIVEN_STATIC_TYPE_BOOL, false), hasConstRhs(true), constRhs(rhs) {
    inputs.push_back(lhs);
    lhs->usages.insert(this);
  };

  int constRhs;
  bool hasConstRhs;

  void accept(SsaVisitor& visitor) {
    visitor.visitIntCmpLteInstr(*this);
  }
};

class DoubleCmpLteInstr : public Instruction {
  public:
  DoubleCmpLteInstr(Instruction* lhs, Instruction* rhs)
      : Instruction(INSTR_DBL_CMPLTE, VAIVEN_STATIC_TYPE_BOOL, false) {
    inputs.push_back(lhs);
    inputs.push_back(rhs);
    lhs->usages.insert(this);
    rhs->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitDoubleCmpLteInstr(*this);
  }
};

class DynamicAccessInstr : public Instruction {
  public:
  DynamicAccessInstr(Instruction* subject, Instruction* property)
      : Instruction(INSTR_DYNAMIC_ACCESS, VAIVEN_STATIC_TYPE_UNKNOWN, true), safelyDeletable(false) {
    inputs.push_back(subject);
    inputs.push_back(property);
    subject->usages.insert(this);
    property->usages.insert(this);
  };

  bool safelyDeletable;

  void accept(SsaVisitor& visitor) {
    visitor.visitDynamicAccessInstr(*this);
  }
};

class DynamicStoreInstr : public Instruction {
  public:
  DynamicStoreInstr(Instruction* subject, Instruction* property, Instruction* rhs)
      : Instruction(INSTR_DYNAMIC_STORE, rhs->type, rhs->isBoxed), safelyDeletable(false) {
    inputs.push_back(subject);
    inputs.push_back(property);
    inputs.push_back(rhs);
    subject->usages.insert(this);
    property->usages.insert(this);
    rhs->usages.insert(this);
  };

  bool safelyDeletable;

  void accept(SsaVisitor& visitor) {
    visitor.visitDynamicStoreInstr(*this);
  }
};

class ListAccessInstr : public Instruction {
  public:
  ListAccessInstr(Instruction* subject, Instruction* index)
      : Instruction(INSTR_LIST_ACCESS, VAIVEN_STATIC_TYPE_UNKNOWN, true) {
    inputs.push_back(subject);
    inputs.push_back(index);
    subject->usages.insert(this);
    index->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitListAccessInstr(*this);
  }
};

class ListStoreInstr : public Instruction {
  public:
  ListStoreInstr(Instruction* subject, Instruction* index, Instruction* rhs)
      : Instruction(INSTR_LIST_STORE, rhs->type, rhs->isBoxed) {
    inputs.push_back(subject);
    inputs.push_back(index);
    inputs.push_back(rhs);
    subject->usages.insert(this);
    index->usages.insert(this);
    rhs->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitListStoreInstr(*this);
  }
};

class ListInitInstr : public Instruction {
  public:
  ListInitInstr() : Instruction(INSTR_LIST_INIT, VAIVEN_STATIC_TYPE_LIST, true) {
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitListInitInstr(*this);
  }
};

class DynamicObjectAccessInstr : public Instruction {
  public:
  DynamicObjectAccessInstr(Instruction* subject, Instruction* index)
      : Instruction(INSTR_DYNAMIC_OBJECT_ACCESS, VAIVEN_STATIC_TYPE_UNKNOWN, true) {
    inputs.push_back(subject);
    inputs.push_back(index);
    subject->usages.insert(this);
    index->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitDynamicObjectAccessInstr(*this);
  }
};

class DynamicObjectStoreInstr : public Instruction {
  public:
  DynamicObjectStoreInstr(Instruction* subject, Instruction* index, Instruction* rhs)
      : Instruction(INSTR_DYNAMIC_OBJECT_STORE, rhs->type, rhs->isBoxed),
      safelyDeletable(false) {
    inputs.push_back(subject);
    inputs.push_back(index);
    inputs.push_back(rhs);
    subject->usages.insert(this);
    index->usages.insert(this);
    rhs->usages.insert(this);
  };

  bool safelyDeletable;

  void accept(SsaVisitor& visitor) {
    visitor.visitDynamicObjectStoreInstr(*this);
  }
};

class ObjectAccessInstr : public Instruction {
  public:
  ObjectAccessInstr(Instruction* subject, string* property) : property(property),
      Instruction(INSTR_OBJECT_ACCESS, VAIVEN_STATIC_TYPE_UNKNOWN, true) {
    inputs.push_back(subject);
    subject->usages.insert(this);
  };

  string* property;

  void accept(SsaVisitor& visitor) {
    visitor.visitObjectAccessInstr(*this);
  }
};

class ObjectStoreInstr : public Instruction {
  public:
  ObjectStoreInstr(Instruction* subject, string* property, Instruction* rhs)
      : Instruction(INSTR_OBJECT_STORE, rhs->type, rhs->isBoxed), property(property) {
    inputs.push_back(subject);
    inputs.push_back(rhs);
    subject->usages.insert(this);
    rhs->usages.insert(this);
  };

  string* property;

  void accept(SsaVisitor& visitor) {
    visitor.visitObjectStoreInstr(*this);
  }
};

class ErrInstr : public Instruction {
  public:
  ErrInstr(vaiven::ErrorCode error) : Instruction(INSTR_ERR, VAIVEN_STATIC_TYPE_VOID, false), error(error) {};
  vaiven::ErrorCode error;

  void accept(SsaVisitor& visitor) {
    visitor.visitErrInstr(*this);
  }
};

class RetInstr : public Instruction {
  public:
  RetInstr(Instruction* val) : Instruction(INSTR_RET, VAIVEN_STATIC_TYPE_VOID, false) {
    inputs.push_back(val);
    val->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitRetInstr(*this);
  }
};

class JmpCcInstr : public Instruction {
  public:
  JmpCcInstr(Instruction* val) : Instruction(INSTR_JMPCC, VAIVEN_STATIC_TYPE_VOID, false) {
    inputs.push_back(val);
    val->usages.insert(this);
  };

  void accept(SsaVisitor& visitor) {
    visitor.visitJmpCcInstr(*this);
  }
};

}}

#endif
