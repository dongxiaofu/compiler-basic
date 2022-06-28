#ifndef X86LINUX_H
#define X86LINUX_H

#include "lex.h"
#include "stmt.h"
#include "type2.h"
#include "ast.h"
#include "expr.h"
#include "symbol.h"

char *GetAccessName(Symbol sym);
void PutASMCode(int code, Symbol opds[]);
void SetupRegisters();

#endif
