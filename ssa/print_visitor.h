#ifndef VAIVEN_VISITOR_HEADER_SSA_PRINT_VISITOR
#define VAIVEN_VISITOR_HEADER_SSA_PRINT_VISITOR

#include "forward_visitor.h"
#include <map>

namespace vaiven { namespace ssa {

class PrintVisitor : public ForwardVisitor {
  public:
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
  void visitJmpCcInstr(JmpCcInstr& instr);
  void visitBlock(Block& block);
  void visitUnconditionalBlockExit(UnconditionalBlockExit& exit);
  void visitConditionalBlockExit(ConditionalBlockExit& exit);

  std::map<Instruction*, int> varIds;

  void printInstruction(std::string prefix, Instruction* instr);
};

}}

#endif
