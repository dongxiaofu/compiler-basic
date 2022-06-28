#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "symbol.h"
#include "gen.h"
#include "x86.h"
#include "x86linux.h"
#include "output.h"

enum ASMCODE {
#define TEMPLATE(code, str)		code,
#include "x86linux.tpl"
#undef TEMPLATE
};

void Move(int code, Symbol dst, Symbol src)
{
	Symbol opds[2];
	opds[0] = dst;
	opds[1] = src;
	PutASMCode(code, opds);
}

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

Symbol PutInReg(Symbol v)
{
	// 如果v在寄存器中，直接返回这个寄存器。
	Symbol reg;
	if(v->reg != NULL){
		return v->reg;
	}
	// v不在这个寄存器中
	// 分配一个寄存器
	reg = GetReg();
	// 生成Mov指令
	Move(X86_MOVI4, reg, v);
	// 返回这个寄存器
	return reg;
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

void EmitMove(IRInst irinst)
{
	Symbol reg;
	// 只处理INT类型数据。
	// SRC1是常量，只生成一条Move指令。
	// SRC1不是常量
	if(SRC1->kind == SK_CONSTANT){
		Move(X86_MOVI4, DST, SRC1);
		return;
	}
	// 给DST和SRC1分配寄存器，先给SRC1分配寄存器
	AllocateReg(irinst, 1);
	AllocateReg(irinst, 0);
	// 如果DST和SRC1都在内存中，生成两条MOV指令。
	if(DST->reg == NULL && SRC1->reg == NULL){
		reg = GetReg();
		Move(X86_MOVI4, reg, SRC1);
		Move(X86_MOVI4, DST, reg);
	}else{
		// 如果是其他情况，只生成一条MOV指令。
		Move(X86_MOVI4, DST, SRC1);
	}
}

void EmitAssignment(IRInst irinst)
{

}

void EmitBinary(IRInst irinst)
{

}

void EmitDeref(IRInst irinst)
{
	Symbol reg;
	// 把SRC1放到寄存器中
	reg = PutInReg(SRC1);
	// 修改当前中间码：opcode修改成MOV，SRC1修改成寄存器的next
	irinst->opcode = MOV;
	SRC1 = reg->next;
	// 生成MOV指令
	EmitMove(irinst);
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
