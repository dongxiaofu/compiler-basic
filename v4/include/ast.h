#ifndef __AST_H_
#define __AST_H_

#include <stdlib.h>
#include "grammer.h"

// 打印日志
// #define LOG	printf
#define LOG	
#define ERROR	printf

/**
	NK:		Node Kind
*/
enum nodeKind 
{ 
	NK_TranslationUnit,     NK_Function,           NK_Declaration,
	NK_TypeName,            NK_Specifiers,         NK_Token,				
	NK_TypedefName,         NK_EnumSpecifier,      NK_Enumerator,			
	NK_StructSpecifier,     NK_UnionSpecifier,     NK_StructDeclaration,	
	NK_StructDeclarator,    NK_PointerDeclarator,  NK_ArrayDeclarator,		
	NK_FunctionDeclarator,  NK_ParameterTypeList,  NK_ParameterDeclaration,
	NK_NameDeclarator,      NK_InitDeclarator,     NK_Initializer,
	
	NK_Node,	
	// todo 直接加一个元素有问题吗？有没有其他相互关联的地方需要增加对应的东西呢？
	NK_Declarator,
//	NK_FunctionDeclarator,NK_Function,NK_ParameterTypeList,

	NK_Expression,

	NK_Statement,

	NK_ExpressionStatement, NK_LabelStatement,     NK_CaseStatement,		
	NK_DefaultStatement,    NK_IfStatement,        NK_SwitchStatement,		
	NK_WhileStatement,      NK_DoStatement,        NK_ForStatement,		
	NK_GotoStatement,       NK_BreakStatement,     NK_ContinueStatement,		
	NK_ReturnStatement,     NK_CompoundStatement
};

static char TypeNames[][16] = {
	"bool",
	"uint8","uint16","uint32","uint64","int8","int16","int32","int64",
	"float32", "float64", "complex64", "complex128",
	"byte","rune","uint","int","uintptr"
};


/**
	kind:	the kind of node
	next:	pointer to next node
	coord:	the coordinate of node
*/
#define AST_NODE_COMMON   \
    int kind;             \
    struct astNode *next; 
//    struct coord coord;

#define AST_DECLARATOR_COMMON   \
    AST_NODE_COMMON             \
    struct astDeclarator *dec;  \
    char *id;          		\         
    int variable_count;
//    TypeDerivList tyDrvList;

// Token Value
union value
{
	int i[2];
	float f;
	double d;
	void *p;
};

typedef struct astDeclarator
{
	AST_DECLARATOR_COMMON
} *AstDeclarator;

#define TYPE_COMMON \
    int categ : 8;  \
    int qual  : 8;  \
    int align : 16; \
    int size;       \
    struct type *bty;
	
typedef struct type
{
	TYPE_COMMON
} *Type;


struct astExpression
{
	AST_NODE_COMMON
	Type ty;
	int op : 16;
	int isarray : 1;
	int isfunc  : 1;
	int lvalue  : 1;
	int bitfld  : 1;
	int inreg   : 1;
	int unused  : 11;
	struct astExpression *kids[2];
	union value val;
	int variable_count;
};

typedef struct astExpression      *AstExpression;

typedef struct astNode
{
	AST_NODE_COMMON
} *AstNode;

typedef struct astToken
{
	AST_NODE_COMMON
	int token;
} *AstToken;

typedef struct astSpecifiers
{
	AST_NODE_COMMON
	AstNode stgClasses;
	AstNode tyQuals;
	AstNode tySpecs;
} *AstSpecifiers;

// todo 暂时只支持初始化int类型变量。
typedef  struct initData{
	int offset;
	AstExpression expr;
	struct initData *next;
} *InitData;

// todo 想不到更好的命名。
union Data {
	AstNode initials;		// when lbrace is 1,	initializer-list
	AstExpression expr;		// when lbrace is 0, assignment-expression
};

typedef struct astInitializer{
	AST_NODE_COMMON
	//  left brace  {			1/0		has or not
	int lbrace;
//	union Data data;
//	todo 在struct中使用union受阻，There is no member named data，先简化成不使用union。
	AstExpression expr;		// when lbrace is 0, assignment-expression
	// todo 语法解析时创建语法树并未使用到此成员。
	InitData idata;
} *AstInitializer;

typedef struct astInitDeclarator{
	AST_NODE_COMMON
	// declarator
	AstDeclarator dec;
	// initializer
	AstInitializer init;
} *AstInitDeclarator;

/**
 * 前面的astInitDeclarator用到了AstDeclarator，把AstDeclarator放在下面符合语法吗？
 * 本来，我不会这样写，但在操作系统项目中，这样写无问题，所以，我不知道在本项目中这样写有没有问题。
 */
// typedef struct astDeclarator
// {
// 	AST_DECLARATOR_COMMON
// } *AstDeclarator;

typedef struct astDeclaration
{
	AST_NODE_COMMON
	// declaration-specifiers:	(staic | int | const | ...) +
	AstSpecifiers specs;
	// init-declarator-list:		id, id=300,
	AstNode initDecs;
} *AstDeclaration;

typedef struct astTypedefName{
	AST_NODE_COMMON
	char *id;
} *AstTypedefName;


typedef struct astParameterDeclaration
{
        AST_NODE_COMMON
        AstSpecifiers specs;
        AstDeclarator dec;
} *AstParameterDeclaration;

typedef struct astParameterTypeList
{
        AST_NODE_COMMON
        AstSpecifiers specs;
        AstParameterDeclaration paramDecls;
        int ellipsis;
} *AstParameterTypeList;

typedef struct astFunctionDeclarator
{
        AST_DECLARATOR_COMMON
        AstParameterTypeList paramTyList;
        AstParameterTypeList sig;
} *AstFunctionDeclarator;

#define AST_STATEMENT_COMMON AST_NODE_COMMON

struct astStatement
{
        AST_STATEMENT_COMMON
};

typedef struct astStatement *AstStatement;

typedef struct astFunction
{
        AST_NODE_COMMON
        AstFunctionDeclarator fdec;
        // compound-statement
        AstStatement stmt;
        int hasReturn;
} *AstFunction;

struct astTranslationUnit
{
	AST_NODE_COMMON
	/**
		ExternalDeclarations
	 */
	AstNode extDecls;
};

typedef struct astTranslationUnit AstTranslationUnit;

#define CREATE_AST_NODE(p, k) \
    p = (void *)malloc(sizeof(*p));              \
    p->kind = NK_##k;         

AstTranslationUnit ParseTranslationUnit();
int IsDataType(char *str);

#endif

