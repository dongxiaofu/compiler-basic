#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "symbol.h"
#include "x86.h"
#include "x86linux.h"
#include "output.h"

enum ASMCODE {
#define TEMPLATE(code, str)		code,
#include "x86linux.tpl"
#undef TEMPLATE
};


void EmitPrologue()
{
	PutASMCode(X86_PROLOGUE, NULL);
}

void EmitEpilogue()
{
	PutASMCode(X86_EPILOGUE, NULL);
}

void EmitBBlock(BBlock bblock)
{
	IRInst head = &(bblock->irinst);
	IRInst inst = head->next;

	while(inst != head){
		EmitIRInst(inst);
		inst = inst->next;
	}
}

static void (*Emitter[])(IRInst) = {
#define OPCODE(opcode, opcodeName, func) Emit##func,
#include "opcode.h"
#undef OPCODE
};

void EmitIRInst(IRInst irinst)
{
	struct irinst irinstCopy = *irinst;

	(*Emitter[irinst->opcode])(&irinstCopy);

	return;
}

void EmitAssignment(IRInst irinst)
{

}

void EmitBinary(IRInst irinst)
{

}

void EmitDeref(IRInst irinst)
{

}

void EmitBranch(IRInst irinst)
{

}

void EmitIncDec(IRInst irinst)
{

}

void EmitAddress(IRInst irinst)
{

}

void EmitJump(IRInst irinst)
{
	BBlock block = (BBlock)DST;
	DST = block->sym;
	PutASMCode(X86_JMP, irinst->opds);
}

void EmitCall(IRInst irinst)
{

}

void EmitReturn(IRInst irinst)
{

}

void EmitNOP(IRInst irinst)
{

}
