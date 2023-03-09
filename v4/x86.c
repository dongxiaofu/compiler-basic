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
//	opds[1] = dst;
//	opds[0] = src;
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

/**
 * 1. 一个寄存器reg中的数据可能是多个变量的值。
 * 2. 换句话说，多个值相同的变量把这个值存储在reg中。
 * 3. 这些变量存储在reg的link中。准确地说，reg的link指向由这样的变量组成的链表。
 * 4. 现在要修改这个变量，具体操作是：
 * 4.1. 把reg的数据回写到对应的变量中。
 * 4.2. 把当前变量p存储到reg中。
 *
 */
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

void LayoutFrame(FunctionSymbol fsym, int firstParamPos)
{
	int functionParamCount = fsym->paramCount;
	int paramCount = 0;
	int functionReceiverCount = fsym->receiverCount;
	int receiverCount = 0;
	// 处理函数的参数
	int offset = STACK_SIZE * firstParamPos;
	VariableSymbol param = AsVar(fsym->params);
//	while(param != NULL){
	while(paramCount < functionParamCount){
		param->offset = offset;
		offset += param->ty->size;	
		param = AsVar(param->next);
		paramCount++;
	}

	// 处理函数的返回值。
	VariableSymbol result = AsVar(fsym->results);
//	while(result != NULL){
	while(receiverCount < functionReceiverCount){
		result->offset = offset;
		offset += result->ty->size;
		result = AsVar(result->next);
		receiverCount++;
	}

	// 处理局部变量
	offset = 0;
	VariableSymbol localVariable = AsVar(fsym->locals);
	while(localVariable != NULL){
		offset -= localVariable->ty->size;
		localVariable->offset = offset;
		localVariable = AsVar(localVariable->next);
	}
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
	fprintf(ASMFile, "%s:\n", bblock->sym->name);

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

void EmitIndirectMove(IRInst irinst)
{
	UsedRegs = 0;
	Symbol reg = PutInReg(DST);
	irinst->opcode = MOV;
	DST = reg->next;
	EmitMove(irinst);
}

void EmitAssignment(IRInst irinst)
{
	// 根据中间码的ty的成员cate和中间码的opcode计算出x86linux.tpl中的汇编指令模板的code。
	// 处理t0:a+b这样的中间码。
	
	int tcode = TypeCode(irinst->ty->categ);
	assert(tcode == U4 || tcode == I4);
	int code = ASM_CODE(irinst->opcode, tcode);

	switch(code){
		case X86_NEGI4: case X86_NEGU4:
		case X86_COMPI4:case X86_COMPU4:
			{
				AllocateReg(irinst, 1);
				goto put_code;
				break;
			}
		case X86_MULU4: 
		case X86_DIVI4: case X86_DIVU4:
		case X86_MODI4: case X86_MODU4:
			{
				AllocateReg(irinst, 1);
				AllocateReg(irinst, 2);
				AllocateReg(irinst, 0);
				// 被乘数应该放在EAX中。
				SpillReg(X86Regs[EAX]);
				if(SRC1->reg != X86Regs[EAX]){
					Move(X86_MOVI4, X86Regs[EAX], SRC1);
				}

				SpillReg(X86Regs[EDX]);

				PutASMCode(code, irinst->opds);

				if(code == X86_MODI4 || code == X86_MODU4){
					// Move(X86_MOVI4, DST, X86Regs[EDX]);
					AddVarToReg(X86Regs[EDX], DST);
				}else{
					// Move(X86_MOVI4, DST, X86Regs[EAX]);
					AddVarToReg(X86Regs[EDX], DST);
				}
				break;
			}
		case X86_ADDI4:
		case X86_ADDU4:
		case X86_SUBI4:
		case X86_SUBU4:
			{
				AllocateReg(irinst, 1);
				AllocateReg(irinst, 2);
				goto put_code;
				break;
			}
		case X86_LSHI4: case X86_RSHI4:
		case X86_LSHU4: case X86_RSHU4:
			{
				AllocateReg(irinst, 1);
				if(SRC2->kind != SK_CONSTANT){
					if(SRC2->reg != X86Regs[ECX]){
						SpillReg(X86Regs[ECX]);
						Move(X86_MOVI4, X86Regs[ECX], SRC2);
						UsedRegs |= 1 << ECX;
					}
					SRC2 = X86ByteRegs[ECX];
				}
				goto put_code;
				break;
			}
		default:
			AllocateReg(irinst, 1);
			AllocateReg(irinst, 2);
			goto put_code;
			break;
		put_code:
			AllocateReg(irinst, 0);
			if(DST->reg != SRC1->reg){
				Move(X86_MOVI4, DST, SRC1);
			}
			
			PutASMCode(code, irinst->opds);
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
	// 1. DST被分配了寄存器reg。
	// 2. leal会存储数据到reg中。
	// 3. reg中的旧数据怎么处理？
	// 4. 不能直接丢弃reg中的旧数据。可能某个变量把数据存储在reg中，还经过了一些计算。
	// 5. 在往reg中写入新数据时，要把reg中的旧数据回写到变量中。
	// 6. 我举不出这样的例子。留心。看看今后能不能遇到这样的例子。 
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
	fprintf(ASMFile, "Call start\n");
	Symbol recv;
	Type rty = irinst->ty;

	int stackSize = 0;
	Symbol arg;

	// 接收返回值的参数入栈。
//	arg = (ArgBucket)(rty->sig->recv);
//	while(arg != NULL){
//		// 参数入栈
//		PushArgument(arg->sym);
//		stackSize += arg->sym->ty->size;
//		arg = arg->link;
//	}

	FunctionSymbol fsym = (FunctionSymbol)SRC1;
	// 参数入栈
	// int functionParamCount = FSYM->paramCount;
	int functionParamCount = fsym->paramCount;
	int paramCount = 0; 
	arg = SRC2;
	while(arg != NULL){
		UsedRegs = 0;
		// 只处理真正的参数。
		if(paramCount == functionParamCount) break;
		// 参数入栈
		PushArgument(arg);
		stackSize += arg->ty->size;
		paramCount++;
		arg = arg->next;
	}

	Symbol tempReceiver = arg;	
	// 接收返回值的参数入栈。
	{
		// VariableSymbol result = FSYM->results;
//		VariableSymbol receiver = FSYM->receivers;
//		Symbol tempReceiver = arg;	
		int functionReceiverCount = fsym->receiverCount;
		int receiverCount = 0;
		while(arg != NULL){
			if(receiverCount == functionReceiverCount) break;
			stackSize += arg->ty->size;
			UsedRegs = 0;
			Symbol opds[2];
			opds[0] = GetReg();
			opds[1] = (Symbol)arg;
			PutASMCode(X86_ADDR, opds);
			PutASMCode(X86_PUSH, opds);
		
			receiverCount++;
			arg = arg->next;
		}
	}	
	// call 函数名
	int asmCode = SRC1->kind == SK_Function ? X86_CALL : X86_ICALL;
	PutASMCode(asmCode, irinst->opds);
	// 回收参数占用的栈空间
	if(stackSize != 0){
		Symbol opd = IntConstant(stackSize);
		PutASMCode(X86_REDUCEF, &opd);
	}

	// 接收返回值
	// result 接收函数返回值的变量。
	// tempReceiver 函数返回值声明。
	Symbol result = fsym->results; 
	int receiverCount = fsym->receiverCount;
//	while(tempReceiver != NULL){
	for(int i = 0; i < receiverCount; i++){
		IRInst irinst;
		
//		irinst = (IRInst)MALLOC(sizeof(struct irinst));;
//		Symbol reg = GetReg();
//		DST = reg;
//		SRC1 = tempReceiver;
//		EmitDeref(irinst);

		irinst = (IRInst)MALLOC(sizeof(struct irinst));	
		irinst->opcode = MOV;
		SRC1 = tempReceiver;
		DST = result;
//		SRC1 = result;
//		DST = tempReceiver;
	//	EmitIndirectMove(irinst);	
		EmitMove(irinst);

		tempReceiver = tempReceiver->next;
		result = result->next;
	}
	fprintf(ASMFile, "Call end\n");

	return;

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
	// 需要获取函数的参数。从哪里获取？
//	FunctionType ty = (FunctionType)(irinst->ty);
//	Symbol firstParam = FSYM->params;
	VariableSymbol result = FSYM->results;
	VariableSymbol src = AsVar(DST);
	int receiverCount = FSYM->receiverCount;
	for(int i = 0; i < receiverCount; i++){
//	while(result != NULL){
		irinst->opcode = IMOV;
		SRC1 = src;
		DST = (Symbol)result;
		EmitIndirectMove(irinst);

		result = result->next;
		src = src->next;
	}
}

// void EmitReturn(IRInst irinst)
// {
// 	Type ty = irinst->ty;
// 	int size = ty->size;
// 	switch(size){
// 		case 1:
// 			Move(X86_MOVI1, X86ByteRegs[EAX], DST);
// 			break;
// 		case 2:
// 			Move(X86_MOVI2, X86WordRegs[EAX], DST);
// 			break;
// 		case 4:
// 			{
// 				Move(X86_MOVI4, X86Regs[EAX], DST);	
// 				break;
// 			}
// 		case 8:
// 			printf("todo\n");
// 			break;
// 		default:
// 			assert(0);
// 	}
// }

void EmitNOP(IRInst irinst)
{

}

void EmitFunction(FunctionSymbol fsym)
{
	// 函数名	
	fprintf(ASMFile, "%s:\n", fsym->name);
	// 对齐
	LayoutFrame(fsym, PRESERVE_REGS + 1);
	// 序言
	EmitPrologue();
	// 处理函数的基本块
	BBlock bblock = fsym->entryBB;
	while(bblock != NULL){
		EmitBBlock(bblock);
		bblock = bblock->next;
	}
	// 后记
	EmitEpilogue();
}

void EmitString()
{
	Symbol p = Strings->next;

	while(p != NULL){
		fprintf(ASMFile, ".%s:\t", p->name);
		fprintf(ASMFile, ".string \"%s\"",  ((String)(p->val.p))->str);
		fprintf(ASMFile, "\n");
		
		p = p->next;
	} 

	fprintf(ASMFile, "\n");
	fprintf(ASMFile, "\n");
	fprintf(ASMFile, "\n");
}
