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
	fprintf(ASMFile, "%s\n", fmt);	
}
