#ifndef DECL_H
#define DECL_H

#include "ast.h"
#include "lex.h"
#include "stmt.h"
#include "type2.h"
#include "expr.h"

int IsDataType(char *str);
void ExpectDataType();

AstNode declaration();
AstNode ParseConstDecl();

AstNode ParseConstSpec();
AstNode ParseIdentifierList();
AstNode ParseIdentifier();

AstNode ParseVarDecl();
AstDeclaration ParseVarSpec();

AstNode ParseTypeDecl();

// todo 很奇怪，先后顺序可能会导致错误。
AstNode ParseFieldDecl();
AstNode ParseFieldDecl();
AstNode ParseEmbeddedField();
AstNode ParseTag();
AstNode ParseStrintLit();

AstNode ParseParameterDecl();




#endif
