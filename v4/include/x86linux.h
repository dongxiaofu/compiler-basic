#ifndef X86LINUX_H
#define X86LINUX_H

#include "lex.h"
#include "stmt.h"
#include "type2.h"
#include "ast.h"
#include "expr.h"
#include "symbol.h"

void PutASMCode(int code, Symbol opds[]);

#endif
