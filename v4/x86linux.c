#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "symbol.h"
#include "x86.h"
#include "reg.h"
#include "x86linux.h"
#include "output.h"

char *ASMTemplate[] = {
#define TEMPLATE(code, str)		str,
#include "x86linux.tpl"
#undef TEMPLATE
};

void PutASMCode(int code, Symbol opds[])
{
	char *fmt = ASMTemplate[code];

	fprintf(ASMFile, "\t");

	while(*fmt){
		switch(*fmt){
			case '%':
				fmt++;
				if(*fmt == '%'){
					fprintf(ASMFile, "%");
				}else{
					int i = *fmt - '0';
					if(opds[i]->reg == NULL){
						fprintf(ASMFile, "%s", GetAccessName(opds[i]));
					}else{
						fprintf(ASMFile, "%s", opds[i]->reg->name);
					}
				}
				break;
			case ';':
				fprintf(ASMFile, "\n\t");
				break;
			default:
				fprintf(ASMFile, "%c", *fmt);
				break;
		}

		fmt++;
	}

	fprintf(ASMFile, "\t");
	fprintf(ASMFile, "%s\n", fmt);	
}

char *GetAccessName(Symbol sym)
{
	if(sym->aname != NULL){
		return sym->aname;
	}

	sym->aname = sym->name;

	return sym->aname;
}

void SetupRegisters()
{
	// 4位
	X86Regs[EAX] = CreateReg("%eax", "(%eax)", EAX);
	X86Regs[EBX] = CreateReg("%ebx", "(%ebx)", EBX);
	X86Regs[ECX] = CreateReg("%ecx", "(%ecx)", ECX);
	X86Regs[EDX] = CreateReg("%edx", "(%edx)", EDX);
	X86Regs[EDI] = CreateReg("%edi", "(%edi)", EDI);
	X86Regs[ESI] = CreateReg("%esi", "(%esi)", ESI);
	// 2位
	X86WordRegs[EAX] = CreateReg("%ax", NULL, EAX);
	X86WordRegs[EBX] = CreateReg("%bx", NULL, EBX);
	X86WordRegs[ECX] = CreateReg("%cx", NULL, ECX);
	X86WordRegs[EDX] = CreateReg("%dx", NULL, EDX);
	X86WordRegs[EDI] = CreateReg("%di", NULL, EDI);
	X86WordRegs[ESI] = CreateReg("%si", NULL, ESI);
	// 1位
	X86ByteRegs[EAX] = CreateReg("%al", NULL, EAX);
	X86ByteRegs[EBX] = CreateReg("%bl", NULL, EBX);
	X86ByteRegs[ECX] = CreateReg("%cl", NULL, ECX);
	X86ByteRegs[EDX] = CreateReg("%dl", NULL, EDX);
}
