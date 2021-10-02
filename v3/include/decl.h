#ifndef DECL_H
#define DECL_H

#include "ast.h"
#include "lex.h"
#include "stmt.h"
#include "expr.h"

int IsDataType(char *str);
void ExpectDataType();

AstNode declaration();
AstNode ParseConstDecl();

AstNode ParseConstSpec();
AstNode ParseIdentifierList();
AstNode ParseIdentifier();









#endif
