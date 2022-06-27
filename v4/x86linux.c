#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "symbol.h"
#include "x86.h"
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
