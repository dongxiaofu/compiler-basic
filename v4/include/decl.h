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

AstDeclaration ParseConstSpec();
AstDeclarator ParseIdentifierList();
AstDeclarator ParseIdentifier();

AstNode ParseVarDecl();
AstDeclaration ParseVarSpec();

AstDeclaration ParseTypeSpec();
AstNode ParseTypeDecl();

AstParameterDeclaration ParseParameterDecl(int *count);
AstParameterDeclaration ParseParameterList();
AstParameterDeclaration ParseParameters();
AstParameterDeclaration ParseResult();
AstNode ParseFunctionName();
AstNode ParseSignature();
AstNode ParseFunctionBody();
AstNode ParseFunctionDecl();

// todo 很奇怪，先后顺序可能会导致错误。
AstNode ParseFieldDecl();
AstNode ParseEmbeddedField();
AstNode ParseTag();
AstNode ParseStrintLit();




#endif
