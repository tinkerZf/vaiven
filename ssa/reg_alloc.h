#ifndef VAIVEN_VISITOR_HEADER_SSA_REG_ALLOC
#define VAIVEN_VISITOR_HEADER_SSA_REG_ALLOC
#include "ssa.h"

#include "../asmjit/src/asmjit/asmjit.h"

namespace vaiven { namespace ssa {

class RegAlloc : public SsaVisitor {

  public:
  RegAlloc(asmjit::X86Compiler& cc) : cc(cc) {};

  void visitPhiInstr(PhiInstr& instr);
  void visitArgInstr(ArgInstr& instr);
  void visitConstantInstr(ConstantInstr& instr);
  void visitCallInstr(CallInstr& instr);
  void visitTypecheckInstr(TypecheckInstr& instr);
  void visitBoxInstr(BoxInstr& instr);
  void visitAddInstr(AddInstr& instr);
  void visitSubInstr(SubInstr& instr);
  void visitMulInstr(MulInstr& instr);
  void visitDivInstr(DivInstr& instr);
  void visitNotInstr(NotInstr& instr);
  void visitCmpEqInstr(CmpEqInstr& instr);
  void visitCmpIneqInstr(CmpIneqInstr& instr);
  void visitCmpGtInstr(CmpGtInstr& instr);
  void visitCmpGteInstr(CmpGteInstr& instr);
  void visitCmpLtInstr(CmpLtInstr& instr);
  void visitCmpLteInstr(CmpLteInstr& instr);
  void visitErrInstr(ErrInstr& instr);
  void visitRetInstr(RetInstr& instr);

  private:
  asmjit::X86Compiler& cc;
  void reuseInputRegIfPossible(Instruction& instr);
};

}}

#endif