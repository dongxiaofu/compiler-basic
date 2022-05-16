#ifndef __AST_H_
#define __AST_H_

#include <stdlib.h>
#include "grammer.h"
// TODO 这一行代码会导致稀奇古怪的错误。原因不明。
// #include "symbol.h"
//
// 打印日志
// #define LOG	printf
#define LOG	
#define ERROR(fmt, msg)	printf(fmt, msg);\
					exit(-2);

enum SYMBOL_KIND {INTERFACE_SYM, OTHER_SYM};

struct mblock
{
	struct mblock *next;
	char *begin;
	char *avail;
	char *end;
};

typedef struct heap
{
	struct mblock *last;
	struct mblock head;
} *Heap;

union align
{
	double d;
	int (*f)(void);
};

#define ALIGN(size, align)  ((align == 0) ? size: ((size + align - 1) & (~(align - 1))))

#define HEAP(hp)    struct heap  hp = { &hp.head }

int HeapAllocate(Heap heap, int size);

// 打印日志
// #define LOG	printf
// #define LOG	
// #define ERROR	printf
void *MALLOC(int size);
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

	NK_QualifiedIdent,

	NK_SliceMeta,
	NK_Arguments,
	
	NK_Node,	
	// todo 直接加一个元素有问题吗？有没有其他相互关联的地方需要增加对应的东西呢？
	NK_Declarator,
//	NK_FunctionDeclarator,NK_Function,NK_ParameterTypeList,
	NK_ArrayTypeSpecifier,NK_StructTypeSpecifier, NK_MethodSpec, NK_InterfaceSpecifier,
	NK_SliceType, NK_MapSpecifier, NK_ChannelType, NK_VariableElementType,
	NK_ImportSpec, NK_ImportDeclaration, NK_PackageClause, NK_SourceFile, 

	NK_TypeDeclaration, NK_TypeDeclarator, NK_VarDeclarator, NK_VarDeclaration, 
	NK_ConstDeclaration, NK_ConstDeclarator, NK_MethodDeclaration, 

	NK_RecordType,

	// CompositeLit 相关
	NK_KeyedElement, NK_Key, NK_Element, NK_CompositeLit, NK_LiteralValue, 

//	NK_SelectCaseStatement, NK_ExprSwitchStmt,
	NK_ExprCaseClause,
	NK_TypeSwitchGuard, NK_TypeCaseClause,

	NK_FunctionLit, NK_Block, NK_LabeledStmt,
	
	NK_ForClause, NK_RangeClause,

	NK_ShortVarDecl,

	NK_EmptyStmt,

	NK_Expression,


	NK_ExpressionStatement,	// 没用到 
	NK_SelectCaseStatement, NK_ExprSwitchStmt, NK_TypeSwitchStmt,
	NK_IfStatement,        NK_SwitchStatement,		
    NK_ForStmt,		
	NK_GotoStatement,       NK_BreakStmt,     NK_ContinueStatement,		
	NK_ReturnStatement,     NK_CompoundStatement,

	NK_IncDecStmt, NK_LabelStmt, NK_DeferStmt, NK_FallthroughStmt, NK_SelectStmt, 
	NK_GoStmt,NK_SendStmt,NK_RecvStmt,
	NK_AssignmentsStmt,
	NK_Statement
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
	char *id;		\
	char *alias;	\
	int isarray;	\
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
	Field flds;
	Field *tail;	
} *RecordType;

typedef struct arrayType
{
	TYPE_COMMON
	int length;	
} *ArrayType;

typedef struct mapType
{
	TYPE_COMMON
	// 集合的key
	Type key;
	// 集合的value
	Type value;
} *MapType;

#define PARAM_LENGTH 5

typedef struct signatureElement{
	char *id;
	Type ty;
} *SignatureElement;

typedef struct signature
{
	SignatureElement receiver;
	SignatureElement params[PARAM_LENGTH];
	int paramSize;
	SignatureElement results[PARAM_LENGTH];
	int resultSize;
} *Signature;

typedef struct functionType
{
	TYPE_COMMON
	Signature sig;
} *FunctionType;

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

typedef struct astSliceMeta
{
	AST_NODE_COMMON
	AstExpression start;
	AstExpression len;
	AstExpression cap;
} *AstSliceMeta;

typedef struct astNode
{
	AST_NODE_COMMON
} *AstNode;

typedef struct astQualifiedIdent
{
	AST_NODE_COMMON
	char *packageName;
	char *identifier;
} *AstQualifiedIdent;

typedef struct astToken
{
	AST_NODE_COMMON
	int token;
} *AstToken;

#define SPECIFIERS_COMMON \
	AST_DECLARATOR_COMMON \
	AstNode stgClasses;	\
	AstNode tyQuals;	\
	AstNode tySpecs;	\
	char *typeAlias;	\
	Type ty;
	
typedef struct astSpecifiers
{
	SPECIFIERS_COMMON
} *AstSpecifiers;

typedef struct astArguments
{
	AST_NODE_COMMON
	AstSpecifiers type;
	AstExpression args;
	int hasEllipsis;
} *AstArguments;

typedef struct astPointerDeclarator{
	AST_DECLARATOR_COMMON
} *AstPointerDeclarator;

typedef struct astArrayTypeSpecifier{
	SPECIFIERS_COMMON
	AstExpression expr;
	AstNode type;
} *AstArrayTypeSpecifier;

typedef struct astStructDeclarator{
	SPECIFIERS_COMMON
} *AstStructDeclarator;

typedef struct astStructSpecifier{
	SPECIFIERS_COMMON
	AstStructDeclarator stDecls;
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

typedef struct keyValue{
	AstExpression key;
	AstExpression value;
} *KeyValue;

// todo 暂时只支持初始化int类型变量。
typedef  struct initData{
	int offset;
	int kind;	// 0---expr;1--keyValue
	union{
		AstExpression expr;
		KeyValue kv;
	};
	struct initData *next;
	// type INT int 中的INT。
	char *typeAlias;
} *InitData;

#define SYM_HASH_MASK	127
#define SEARCH_OUTER_TABLE 1

#define SYMBOL_COMMON	\
	int kind;	\
	char *name;	\
	Type ty;	\
	int level;	\
	union value val;	\
	struct symbol *link;	\
	struct symbol *next;

typedef struct symbol{
	SYMBOL_COMMON
} *Symbol;

typedef struct functionSymbol{
	SYMBOL_COMMON
	Symbol params;
	Symbol results;
	Symbol locals;
} *FunctionSymbol;

typedef struct variableSymbol{
	SYMBOL_COMMON
	InitData idata;	
	// 接口变量的数据类型别名。
	char *typeAlias;
} *VariableSymbol;

typedef struct methodSymbol{
	SYMBOL_COMMON
	char *interfaceName;
	Type receiverType;
} *MethodSymbol;

typedef struct bucketLinker{
	Symbol sym;
	struct bucketLinker *link;
} *BucketLinker;

typedef struct table{
	Symbol *buckets;
//	Symbol buckets[128];
	int level;
	struct table *outer;
} *Table;

void InitSymbolTable();

// todo 想不到更好的命名。
union Data {
	AstNode initials;		// when lbrace is 1,	initializer-list
	AstExpression expr;		// when lbrace is 0, assignment-expression
};

typedef struct astKey{
	AST_NODE_COMMON
	int lbrace;
	union{
		AstNode literalValue;
		AstNode expr;
	};
} *AstKey;

typedef struct astElement{
	AST_NODE_COMMON
	int lbrace;
	union{
		AstNode literalValue;
		AstNode expr;
	};
} *AstElement;

typedef struct AstKeyedElement{
	AST_NODE_COMMON
	AstKey key;
	AstElement element;
} *AstKeyedElement;

typedef struct astLiteralValue{
	AST_NODE_COMMON
	int hasKey;
	AstKeyedElement keyedElement;
} *AstLiteralValue;

typedef struct astCompositeLit{
	AST_NODE_COMMON
	AstNode literalType;
	AstLiteralValue literalValue;
} *AstCompositeLit;

typedef struct astInitializer{
	AST_NODE_COMMON
	int isCompositeLit;
	union{
		AstExpression expr;		// when isCompositeLit is 0, assignment-expression
		AstCompositeLit compositeLit;
	};
	// todo 语法解析时创建语法树并未使用到此成员。
	InitData idata;
} *AstInitializer;

typedef struct astInitDeclarator{
	AST_NODE_COMMON
	// declarator
	AstDeclarator dec;
	// initializer
	// AstInitializer init;
	AstNode init;
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

// TODO 不理解我当初为什么设计这个结构？
typedef struct astFunctionDeclarator
{
        AST_DECLARATOR_COMMON
        AstParameterTypeList receiver;
        AstParameterTypeList paramTyList;
        AstParameterTypeList result;
} *AstFunctionDeclarator;

typedef struct astMethodSpec{
	AST_NODE_COMMON
	AstDeclarator funcName;
    AstParameterTypeList paramTyList;
    AstParameterTypeList sig;
} *AstMethodSpec;

typedef struct interfaceVariableSymbol{
	SYMBOL_COMMON
	InitData initData;	
	AstMethodSpec currentMethod;
} *InterfaceVariableSymbol;

typedef struct astInterfaceSpecifier{
	AST_NODE_COMMON
	AstNode interfaceDecs;

	// TODO 语义分析后使用。
	AstMethodSpec method;
} *AstInterfaceSpecifier;

typedef struct astSliceType{
	AST_NODE_COMMON
	AstNode node;
} *AstSliceType;

typedef struct astMapSpecifier{
	SPECIFIERS_COMMON
	AstNode keyType;
	AstNode elementType;
} *AstMapSpecifier;

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

#define VECTOR_SIZE	100
// TODO 语句容器
typedef struct stmtVector{
	AstStatement params[VECTOR_SIZE];
	int index;
} *StmtVector;

typedef struct astFunction
{
        AST_NODE_COMMON
        AstFunctionDeclarator fdec;
        // compound-statement
		AstBlock block;
        int hasReturn;
		FunctionSymbol fsym;

		StmtVector breakable;
		StmtVector loops;		
} *AstFunction;

typedef struct interfaceType{
	TYPE_COMMON
	AstFunction methodDecl;
	AstFunction methodDeclTail;
	AstMethodSpec methods;
} *InterfaceType;

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

typedef struct astMethodDeclaration{
        AST_NODE_COMMON
        AstFunctionDeclarator fdec;
        // compound-statement
		AstBlock	block;
        int hasReturn;
//		FunctionSymbol fsym;
		StmtVector breakable;
		StmtVector loops;		
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
    p = (void *)MALLOC(sizeof(*p));              \
    p->kind = NK_##k;        \
    p->next = NULL; 

#define PRINTF printf

// static AstFunction CURRENT;
AstFunction CURRENT;
FunctionSymbol FSYM;
AstMethodDeclaration CURRENT_METHOD;
MethodSymbol MSYM;

static InterfaceType INTERFACE_LIST = NULL;
InterfaceType INTERFACE_CURRENT;
static AstFunction FUNCTION_LIST = NULL;
AstFunction FUNCTION_CURRENT;

AstTranslationUnit ParseTranslationUnit();
int IsDataType(char *str);

#endif

