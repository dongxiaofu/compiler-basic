#ifndef EMIT_H
#define EMIT_H

#include "lex.h"
#include "stmt.h"
#include "type2.h"
#include "ast.h"
#include "expr.h"

void EmitFunction(FunctionSymbol fsym);
void EmitFunctions(AstTranslationUnit transUnit);
void EmitTranslationUnit(AstTranslationUnit transUnit);

#endif
