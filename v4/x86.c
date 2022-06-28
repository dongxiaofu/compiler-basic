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

void AddVarToReg(Symbol reg, Symbol v)
{
	v->link = reg->link;
	reg->link = v;

	v->reg = reg;
}

void AllocateReg(IRInst inst, int index)
{
	Symbol reg, p;
	
	p = inst->opds[index];
	// 目标操作数的kind不是SK_Temp，不分配寄存器。
	if(p->kind != SK_Temp){
		return;
	}	
	// 目标操作数已经在寄存器中，不分配寄存器。
	if(p->reg != NULL){
		return;
	}
	// 获取一个寄存器，把目标操作数存储到寄存器中。
	reg = GetReg();
	AddVarToReg(reg, p);
}

void ModifyVar(Symbol p)
{
	Symbol reg = p->reg;

	if(reg == NULL){
		return;
	}

	p->needwb = 0;
	SpillReg(reg);

	AddVarToReg(reg, p);

	p->needwb = 1;
}

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
	// leal 寄存器，内存地址
	// leal的dst必须是寄存器，src必须是内存地址。
	AllocateReg(irinst, 0);
	PutASMCode(X86_ADDR, irinst->opds);
	// 为什么必须有这一句？
	ModifyVar(DST);
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
