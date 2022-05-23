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
	UINT8,UINT16,UINT32,UINT64,INT8,INT16,INT32,INT64,FLOAT32,FLOAT64,COMPLEX64,COMPLEX128,BYTE,RUNE,
	INT, POINTER, VOID, ARRAY, STRUCT, INTERFACE,
	FUNCTION, METHOD
};

#define IsObjectPtr(ty)		(ty->categ == POINTER && ty->bty->categ != FUNCTION)
#define IsIntegType(ty)		(BYTE <= ty->categ <= INT)
// #define IsArithType(ty)		IsIntegType
#define IsArithType(ty)		(BYTE <= ty->categ <= INT)

#define BothArithType(ty1, ty2)	(IsArithType(ty1) && IsArithType(ty2))

// TODO 定义数据类型的长度，单位是字节还是位？
#define BYTE_SIZE 1
#define INT_SIZE 4

// Type Types[INT - VOID + 1];
// static struct type Types[INT - VOID + 1];
struct type Types[FUNCTION - BYTE + 1];

// #define T(categ)	Types[VOID + categ]
// #define T(categ)	Types + categ
#define T(categ)	(Types + categ)

enum LITERAL_TYPE{
	EARRAY, ESLICE, ESTRUCT, EMAP, EELEMENT, ENAME 
};

#define TYPE_NAME_TABLE_SIZE 100

typedef struct typeName{
	char *id;
	AstSpecifiers type; 
} TypeName;

typedef struct typeNameTable{
	TypeName *table[TYPE_NAME_TABLE_SIZE];
	int index;
} TypeNameTable;

TypeNameTable tnames;

Type PointerTo(Type ty);
ArrayType ArrayOf(Type ty, int len);

TypeName *LookupTypeName(char *id);
int IsTypeName(char *id);
void AddTypeName(char *id, AstSpecifiers type);
void PreCheckTypeName(AstDeclaration decl);

void SetupTypeSystem();

// 获取数据类型的种类，例如数组、结构体等。
int GetTypeKind();

AstNode ParseMethodName();
AstNode ParseInterfaceTypeName();
AstMethodSpec ParseMethodSpec();


// AstNode ParseType();
AstNode ParseBasicType();
AstNode ParseTypeLit();
AstQualifiedIdent ParseQualifiedIdent();
// AstSpecifiers ParseTypeNameType(char *typeName);
AstArrayTypeSpecifier ParseArrayType();
AstStructSpecifier ParseStructType();
AstPointerDeclarator ParsePointerType();
AstFunctionDeclarator ParseFunctionType();
AstInterfaceSpecifier ParseInterfaceType();
AstSliceType ParseSliceType();
AstMapSpecifier ParseMapType();
AstChannelType ParseChannelType();
AstSpecifiers ParseTypeNameType(char *typeName);

AstVariableElementType ParseVariableElementType();
AstSpecifiers ParseLiteralType();

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
