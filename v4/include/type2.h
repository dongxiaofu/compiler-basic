#ifndef TYPE_H 
#define TYPE_H

#include "ast.h"
#include "lex.h"

// todo 寻机测试一下，函数的顺序是否必要。
// AstNode ParseParameterList();
// AstNode ParseSignature();
// AstNode ParseParameters();
// AstNode ParseResult();

AstNode ParseMethodName();
AstNode ParseInterfaceTypeName();
AstNode ParseMethodSpec();
AstNode ParseInterfaceType();


// AstNode ParseType();
AstTypedefName ParseTypeName();
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

AstNode ParseType();

 static AstNode (*TypeListParsers[])() = {
 	#define TYPEINFO(kind,name)	Parse##name##Type,
 	#include "typeinfo.txt"
 	#undef TYPEINFO 
 };
// static AstExpression (* ExprCheckers[])(AstExpression)
// static AstNode (* TypeListParsers[])() = {
// 	ParseArrayType,
// 	ParseStructType,
// 	ParsePointerType,
// 	ParseFunctionType,
// 	ParseInterfaceType,
// 	ParseSliceType,
// 	ParseMapType,
// 	ParseChannelType
// };



#endif
