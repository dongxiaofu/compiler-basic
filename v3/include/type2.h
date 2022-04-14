#ifndef TYPE_H
#define TYPE_H

#include "ast.h"
#include "lex.h"
#include "stmt.h"
#include "expr.h"

AstNode ParseType();
AstNode ParseTypeName();
AstNode ParseTypeLit();
AstNode ParseQualifiedIdent();
AstNode ParseArrayType();
AstNode ParseStructType();
AstNode ParsePointerType();
AstNode ParseFunctionType();
AstNode ParseInterfaceType();
AstNode ParseSliceType();
AstNode ParseMapType();
AstNode ParseChannelType();

// static AstNode (*TypeListParsers[])() = {
// 	#define TYPEINFO(kind,name)	Parse##name##Type,
// 	#include "typeinfo.txt"
// 	#undef TYPEINFO 
// };
// static AstExpression (* ExprCheckers[])(AstExpression)
 static AstNode (* TypeListParsers[])() = {
 	ParseArrayType,
 	ParseStructType,
 	ParsePointerType,
 	ParseFunctionType,
 	ParseInterfaceType,
 	ParseSliceType,
 	ParseMapType,
 	ParseChannelType
 };



#endif
