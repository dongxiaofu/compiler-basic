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
	// 根据中间码的ty的成员cate和中间码的opcode计算出x86linux.tpl中的汇编指令模板的code。
	// 处理t0:a+b这样的中间码。
	
	int tcode = TypeCode(irinst->ty->categ);
	assert(tcode == U4 || tcode == I4);
	int code = ASM_CODE(irinst->opcode, tcode);

	switch(code){
		case X86_ADDI4:
		case X86_ADDU4:
		case X86_SUBI4:
		case X86_SUBU4:
			{
				AllocateReg(irinst, 1);
				AllocateReg(irinst, 2);
				AllocateReg(irinst, 0);
				if(DST->reg != SRC1->reg){
					Move(X86_MOVI4, DST, SRC1);
				}
				
				PutASMCode(code, irinst->opds);
				break;
			}
		default:
			ERROR("%s\n", "EmitAssignment default");
			break;
	}
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

void PushArgument(Symbol arg)
{
	if(arg->kind == SK_Struct){
		// esi、edi、ecx的值将被修改，在修改前先把这些寄存器中的值回写到内存中。
		SpillReg(X86Regs[ESI]);
		SpillReg(X86Regs[EDI]);
		SpillReg(X86Regs[ECX]);
		// "leal %0, %%esi;subl %2, %%esp;movl %%esp, %%edi;movl %1, %%ecx;rep movsb"
		Symbol opds[3];
		// opds[0]，看了UCC才写出来。
		opds[0] = arg;
		opds[1] = arg->ty->size;
		opds[2] = arg->ty->size;
		PutASMCode(X86_PUSHB, opds);
	}else{
		PutASMCode(X86_PUSH, &arg);
	}
}

void EmitCall(IRInst irinst)
{
	Symbol recv;
	Type rty = irinst->ty;

	int stackSize = 0;
	ArgBucket arg;
	arg = (ArgBucket)(SRC2);
	// 参数入栈
	while(arg != NULL){
		// 参数入栈
		PushArgument(arg->sym);
		stackSize += arg->sym->ty->size;
		arg = arg->link;
	}
	// call 函数名
	int asmCode = SRC1->kind == SK_Function ? X86_CALL : X86_ICALL;
	PutASMCode(asmCode, irinst->opds);
	// 回收参数占用的栈空间
	if(stackSize != 0){
		Symbol opd = IntConstant(stackSize);
		PutASMCode(X86_REDUCEF, &opd);
	}

	recv = irinst->opds[0];
	if(recv == NULL){
		return;
	}

	switch(rty->size){
		case 1:
			Move(X86_MOVI1, DST, X86ByteRegs[EAX]);
			break;
		case 2:
			Move(X86_MOVI2, DST, X86WordRegs[EAX]);
			break;
		case 4:
			Move(X86_MOVI4, DST, X86Regs[EAX]);
			break;
		case 8:
			printf("%s\n", "EmitCall todo");
			break;
		default:
			assert(0);
	}
}

void EmitReturn(IRInst irinst)
{
	Type ty = irinst->ty;
	int size = ty->size;
	switch(size){
		case 1:
			Move(X86_MOVI1, X86ByteRegs[EAX], DST);
			break;
		case 2:
			Move(X86_MOVI2, X86WordRegs[EAX], DST);
			break;
		case 4:
			{
				Move(X86_MOVI4, X86Regs[EAX], DST);	
				break;
			}
		case 8:
			printf("todo\n");
			break;
		default:
			assert(0);
	}
}

void EmitNOP(IRInst irinst)
{

}
