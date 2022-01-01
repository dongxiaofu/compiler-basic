#ifndef DECL_H
#define DECL_H

// #include "grammer.h"
#include "ast.h"
#include "lex.h"
// #include "grammer.h"
#include "stmt.h"
#include "type2.h"
#include "expr.h"
// #include "grammer.h"


//  to allow    struct{ ; },    empty struct declaration

#define TOKEN_SET_END_FLAG -1

static int FIRST_StructDeclaration[]   = { FIRST_DECLARATION, TK_SEMICOLON, TOKEN_SET_END_FLAG};
static int FF_StructDeclaration[]      = { FIRST_DECLARATION, TK_SEMICOLON,TK_RBRACE, TOKEN_SET_END_FLAG};
//
static int FIRST_Function[]            = { FIRST_DECLARATION, TK_LBRACE, TOKEN_SET_END_FLAG};
// to allow ";"
static int FIRST_ExternalDeclaration[] = { FIRST_DECLARATION, TK_MUL, TK_LPARENTHESES, TK_SEMICOLON,TOKEN_SET_END_FLAG};

static int FIRST_Declaration[] = { FIRST_DECLARATION, TOKEN_SET_END_FLAG};
static int FIRST_Expression[] = { FIRST_EXPRESSION, TOKEN_SET_END_FLAG};


int CurrentTokenIn(int *toks);

int IsDataType(char *str);
void ExpectDataType();

// AstNode declaration();
AstNode ParseConstDecl();

AstDeclaration ParseConstSpec();
AstDeclarator ParseIdentifierList();
AstDeclarator ParseIdentifier();
// TODO 找机会做得更完善。
AstNode ParseShortVarDecl();
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
AstStatement ParseFunctionBody();
AstNode ParseFunctionDecl();
AstNode ParseMethodDecl();
AstNode ParseMethodDeclOrFunctionDecl();

// todo 很奇怪，先后顺序可能会导致错误。
AstNode ParseFieldDecl();
AstNode ParseEmbeddedField();
AstNode ParseTag();
AstNode ParseStrintLit();

AstNode declaration();



#endif
