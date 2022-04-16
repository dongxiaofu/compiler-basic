#ifndef __AST_H_
#define __AST_H_

#include <stdlib.h>
#include "grammer.h"
// #include "symbol.h"

// 打印日志
// #define LOG	printf
// #define LOG	
// #define ERROR	printf

/**
	NK:		Node Kind
*/
enum nodeKind 
{ 
	NK_Function,
	NK_TranslationUnit,     NK_FunctionTest,           NK_Declaration,
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
	NK_ArrayTypeSpecifier,NK_StructTypeSpecifier, NK_MethodSpec, NK_InterfaceDeclaration,
	NK_SliceType, NK_MapType, NK_ChannelType, NK_VariableElementType,
	NK_ImportSpec, NK_ImportDeclaration, NK_PackageClause, NK_SourceFile, 

	NK_TypeDeclaration, NK_TypeDeclarator, NK_VarDeclarator, NK_VarDeclaration, 
	NK_ConstDeclaration, NK_ConstDeclarator, NK_MethodDeclaration, 

	NK_RecordType,

	NK_SelectCaseStatement, NK_ExprSwitchStmt,
	NK_ExprCaseClause,
	NK_TypeSwitchGuard, NK_TypeCaseClause, NK_TypeSwitchStmt,

	NK_FunctionLit, NK_Block, NK_LabeledStmt, NK_BreakStmt,
	
	NK_ForClause, NK_RangeClause, NK_ForStmt,

	NK_ShortVarDecl,

	NK_EmptyStmt,

	NK_Expression,

	NK_Statement,
	NK_SendStmt, NK_IncDecStmt, NK_LabelStmt, NK_DeferStmt, NK_FallthroughStmt, NK_SelectStmt, 
	NK_GoStmt,

	NK_AssignmentsStmt,

	NK_ExpressionStatement, NK_LabelStatement,     NK_CaseStatement,		
	NK_DefaultStatement,    NK_IfStatement,        NK_SwitchStatement,		
	NK_WhileStatement,      NK_DoStatement,        NK_ForStatement,		
	NK_GotoStatement,       NK_BreakStatement,     NK_ContinueStatement,		
	NK_ReturnStatement,     NK_CompoundStatement
};

// 在AstChannelType中的成员中使用。
enum ChannelType{
	CT_Send, CT_Receive, CT_SendReceive
};

static char TypeNames[][16] = {
	"bool",
	"uint8","uint16","uint32","uint64","int8","int16","int32","int64",
	"float32", "float64", "complex64", "complex128",
	"byte","rune","uint","int","uintptr",

	"error","string"
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
	AST_NODE_COMMON \
    int categ : 8;  \
    int qual  : 8;  \
    int align : 16; \
    int size;       \
    struct type *bty;
	
typedef struct type
{
	TYPE_COMMON
} *Type;

typedef struct field
{
	char *id;
	int offset;
	Type ty;
	struct field *next;	
} *Field;

typedef struct recordType
{
	TYPE_COMMON
	char *id;
	Field flds;
	Field *tail;	
} *RecordType;

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

#define SPECIFIERS_COMMON \
	AST_NODE_COMMON		\
	AstNode stgClasses;	\
	AstNode tyQuals;	\
	AstNode tySpecs;	\
	Type ty;
	
typedef struct astSpecifiers
{
	SPECIFIERS_COMMON
} *AstSpecifiers;

typedef struct astPointerDeclarator{
	AST_DECLARATOR_COMMON
} *AstPointerDeclarator;

typedef struct astArrayTypeSpecifier{
	AST_DECLARATOR_COMMON
	AstExpression expr;
	AstNode type;
} *AstArrayTypeSpecifier;

typedef struct astStructDeclarator{
	SPECIFIERS_COMMON
	char *id;
} *AstStructDeclarator;

typedef struct astStructSpecifier{
	SPECIFIERS_COMMON
	AstStructDeclarator stDecls;
	char *id;
} *AstStructSpecifier;

typedef struct fieldDecl{
	// TODO 暂时保留。目前，没有作用。
  	int type;		// 0--IdentifierList Type；1--EmbeddedField
  	// 存储IdentifierList Type链表
  	AstStructDeclarator member;
  	AstStructDeclarator tail;
  	// 存储EmbeddedField
  	// 也用AstStructDeclarator存储
} *FieldDecl;

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

typedef struct astArrayDeclarator{
	AST_DECLARATOR_COMMON
	AstExpression expr;
} *AstArrayDeclarator;

typedef struct astStructDeclaration
{
	AST_NODE_COMMON
	AstSpecifiers specs;
	AstNode stDecs;
} *AstStructDeclaration;

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
        AstParameterTypeList receiver;
        AstParameterTypeList paramTyList;
        AstParameterTypeList sig;
} *AstFunctionDeclarator;

typedef struct astMethodSpec{
	AST_NODE_COMMON
	AstDeclarator funcName;
    AstParameterTypeList paramTyList;
    AstParameterTypeList sig;
} *AstMethodSpec;

typedef struct astInterfaceDeclaration{
	AST_NODE_COMMON
	// TODO 这个成员没有作用。
	AstSpecifiers specs;
	AstNode interfaceDecs;
} *AstInterfaceDeclaration;

typedef struct astSliceType{
	AST_NODE_COMMON
	AstNode node;
} *AstSliceType;

typedef struct astMapType{
	AST_NODE_COMMON
	AstNode keyType;
	AstNode elementType;
} *AstMapType;

typedef struct astChannelType{
	AST_NODE_COMMON
	// ChannelType type;
	enum ChannelType type;
	AstNode elementType;
} *AstChannelType;

typedef struct astVariableElementType{
	AST_NODE_COMMON
	AstNode node;
} *AstVariableElementType;

typedef struct astTypeDeclarator{
	AST_DECLARATOR_COMMON
	// declaration-specifiers:	(staic | int | const | ...) +
	AstSpecifiers specs;
	// init-declarator-list:		id, id=300,
	AstNode initDecs;
} *AstTypeDeclarator;

typedef struct astTypeDeclaration{
	AST_DECLARATOR_COMMON
	AstTypeDeclarator decs;
} *AstTypeDeclaration;

typedef struct astVarDeclarator{
	AST_DECLARATOR_COMMON
	// declaration-specifiers:	(staic | int | const | ...) +
	AstSpecifiers specs;
	// init-declarator-list:		id, id=300,
	AstNode initDecs;
} *AstVarDeclarator;

typedef struct astVarDeclaration{
	AST_DECLARATOR_COMMON
	AstVarDeclarator decs;
} *AstVarDeclaration;

typedef struct astConstDeclarator{
	AST_DECLARATOR_COMMON
	// declaration-specifiers:	(staic | int | const | ...) +
	AstSpecifiers specs;
	// init-declarator-list:		id, id=300,
	AstNode initDecs;
} *AstConstDeclarator;

typedef struct astConstDeclaration{
	AST_DECLARATOR_COMMON
	AstVarDeclarator decs;
} *AstConstDeclaration;

#define AST_STATEMENT_COMMON AST_NODE_COMMON

struct astStatement
{
        AST_STATEMENT_COMMON
};

typedef struct astStatement *AstStatement;

typedef struct astBlock{
	AST_NODE_COMMON
	AstStatement stmt;
} *AstBlock;

typedef struct astFunctionLit{
	AST_NODE_COMMON
	AstFunctionDeclarator fdecl;
	AstBlock body;
} *AstFunctionLit;

typedef struct astExprCaseClause{
	AST_NODE_COMMON
	AstExpression exprSwitchCase;
	AstStatement statementList;
} *AstExprCaseClause;

typedef struct astExprSwitchStmt{
	AST_STATEMENT_COMMON
	AstStatement simpleStmt;
	AstExpression expr;
	AstExprCaseClause exprCaseClause;
} *AstExprSwitchStmt;

typedef struct astShortVarDecl{
	AST_NODE_COMMON
	AstExpression identifierList;
	AstExpression expressionList;
} *AstShortVarDecl;

typedef struct astFunction
{
        AST_NODE_COMMON
        AstFunctionDeclarator fdec;
        // compound-statement
		AstBlock block;
        int hasReturn;
} *AstFunction;

typedef struct astMethodDeclaration{
        AST_NODE_COMMON
        AstFunctionDeclarator fdec;
        // compound-statement
		AstBlock	block;
        int hasReturn;
} *AstMethodDeclaration;

typedef struct astPackageClause{
	AST_NODE_COMMON
	AstExpression packageName;
} *AstPackageClause;

typedef struct astImportSpec{
	AST_NODE_COMMON
	AstDeclarator packageName;
	AstDeclarator importPath;
} *AstImportSpec;

typedef struct astImportDeclaration{
	AST_NODE_COMMON
	AstImportSpec importSpec;
} *AstImportDeclaration;

// typedef struct astSourceFile{
typedef struct astTranslationUnit{
	AST_NODE_COMMON
	AstPackageClause packageClause;
	AstImportDeclaration importDecls;
	AstNode decls;
} *AstTranslationUnit;

#define CREATE_AST_NODE(p, k) \
    p = (void *)malloc(sizeof(*p));              \
    p->kind = NK_##k;        \
    p->next = NULL; 

#define PRINTF printf

AstTranslationUnit ParseTranslationUnit();
int IsDataType(char *str);

#endif

