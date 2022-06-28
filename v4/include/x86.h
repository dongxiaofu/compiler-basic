#ifndef X86_H
#define X86_H

#include "lex.h"
#include "stmt.h"
#include "type2.h"
#include "ast.h"
#include "expr.h"

void AddVarToReg(Symbol reg, Symbol v);
void AllocateReg(IRInst inst, int index);
void ModifyVar(Symbol p);

void EmitPrologue();
void EmitEpilogue();

void EmitAssignment(IRInst irinst);
void EmitBinary(IRInst irinst);
void EmitDeref(IRInst irinst);
void EmitBranch(IRInst irinst);
void EmitIncDec(IRInst irinst);
void EmitAddress(IRInst irinst);
void EmitJump(IRInst irinst);
void EmitCall(IRInst irinst);
void EmitReturn(IRInst irinst);
void EmitNOP(IRInst irinst);

void EmitIRInst(IRInst irinst);
void EmitBBlock(BBlock bblock);

#endif
