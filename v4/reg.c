#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "symbol.h"
#include "x86.h"
#include "x86linux.h"
#include "output.h"
#include "reg.h"
#include <limits.h>

Symbol X86Regs[ESI + 1];
Symbol X86WordRegs[ESI + 1];
Symbol X86ByteRegs[ESI + 1];

int UsedRegs = 0;

Symbol CreateReg(char *name, char *iname, int no)
{
	Symbol reg;

	reg = (Symbol)MALLOC(sizeof(struct symbol));
	reg->kind = SK_REGISTER;
	reg->name = reg->aname = name;
	reg->val.i[0] = no;

	if(iname != NULL){
		reg->next = (Symbol)MALLOC(sizeof(struct symbol));
		reg->next->name = reg->next->aname = name;
		reg->next->kind = SK_IREGISTER;
	}

	return reg;
}

void SpillReg(Symbol reg)
{
	Symbol p;

	p = reg->link;

	while(p != NULL){
		p->reg = NULL;
		if(p->needwb == 1 && p->ref > 0){
			p->needwb = 0;
			// TODO 把寄存器中的数据回写到内存中。
		}
		
		p = p->link;
	}

	reg->link = NULL;
}

void ClearRegs()
{
	for(int i = EAX; i <= ESI; i++){
		if(X86Regs[i]){
			SpillReg(X86Regs[i]);
		}
	}
}

int FindEmptyReg(int endReg)
{
	for(int i = EAX; i <= endReg; i++){
		Symbol reg = X86Regs[i];
		if(reg != NULL && reg->link == NULL && (!((1 << i) & UsedRegs)) ){
			return i;
		}
	}

	return NO_REG;
}

Symbol GetRegInternal(int width)
{
	int endReg;
	Symbol *regs;
	int regNo;

	switch(width){
		case 1:
			endReg = EDX;
			regs = X86ByteRegs;
			break;
		case 2:
			endReg = EDX;
			regs = X86WordRegs;
			break;
		case 4:
			endReg = ESI;
			regs = X86Regs;
			break;
	}

	regNo = FindEmptyReg(endReg);

	if(regNo == NO_REG){
		// 找出一个最近最少使用的寄存器
		regNo = SelectSpillReg(endReg);
		// 把这个寄存器中的数据写回到内存中
		// SpillReg(regs[regNo]);
		SpillReg(X86Regs[regNo]);
	}

	UsedRegs |= 1 << regNo;
	
	return regs[regNo];
}

int SelectSpillReg(int endReg)
{
	int index = NO_REG;
	int refMin = INT_MAX;
	int ref = 0;
	Symbol reg, p;

	for(int i = EAX; i <= endReg; i++){
		// 排除esp、ebp和在一条中间码中被使用了的寄存器。
		reg = X86Regs[i];
		if(reg == NULL || (1 << i & UsedRegs)){
			continue;
		}

		// 找出使用次数最少的寄存器。
		p = reg->link;
		while(p != NULL){
			if(p->needwb && p->ref > 0){
				ref += p->ref;
			}

			p = p->link;

			if(refMin > ref){
				refMin = ref;
				index = i;
			}
		}
	}

	return index;
}

Symbol GetReg()
{
	return GetRegInternal(4);
}

Symbol GetWordReg()
{
	return GetRegInternal(2);
}

Symbol GetByteReg()
{
	return GetRegInternal(1);
}

