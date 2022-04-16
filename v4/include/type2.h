#ifndef TYPE_H 
#define TYPE_H

// #include "ast.h"
// #include "lex.h"

// todo 寻机测试一下，函数的顺序是否必要。
// AstNode ParseParameterList();
// AstNode ParseSignature();
// AstNode ParseParameters();
// AstNode ParseResult();


// TODO 暂时这样。
enum{
	VOID, BYTE, INT
};

// TODO 定义数据类型的长度，单位是字节还是位？
#define BYTE_SIZE 1
#define INT_SIZE 4

// Type Types[INT - VOID + 1];
static struct type Types[INT - VOID + 1];

// #define T(categ)	Types[VOID + categ]
// #define T(categ)	Types + categ
#define T(categ)	(Types + categ)

enum LITERAL_TYPE{
	ARRAY, SLICE, STRUCT, MAP, ELEMENT, NAME 
};

void SetupTypeSystem();

// 获取数据类型的种类，例如数组、结构体等。
int GetTypeKind();

AstNode ParseMethodName();
AstNode ParseInterfaceTypeName();
AstMethodSpec ParseMethodSpec();


// AstNode ParseType();
AstNode ParseBasicType();
AstNode ParseTypeLit();
AstNode ParseQualifiedIdent();
AstArrayTypeSpecifier ParseArrayType();
AstStructSpecifier ParseStructType();
AstPointerDeclarator ParsePointerType();
AstFunctionDeclarator ParseFunctionType();
AstInterfaceDeclaration ParseInterfaceType();
AstSliceType ParseSliceType();
AstMapType ParseMapType();
AstChannelType ParseChannelType();

AstVariableElementType ParseVariableElementType();
AstNode ParseLiteralType();

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
