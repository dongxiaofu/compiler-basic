#ifndef __AST_H_
#define __AST_H_

#include <stdlib.h>

// 打印日志
#define LOG	printf

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
	
	NK_Expression,

	NK_ExpressionStatement, NK_LabelStatement,     NK_CaseStatement,		
	NK_DefaultStatement,    NK_IfStatement,        NK_SwitchStatement,		
	NK_WhileStatement,      NK_DoStatement,        NK_ForStatement,		
	NK_GotoStatement,       NK_BreakStatement,     NK_ContinueStatement,		
	NK_ReturnStatement,     NK_CompoundStatement
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

typedef struct astNode
{
	AST_NODE_COMMON
} *AstNode;

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
    p = (void *)malloc(sizeof(p));              \
    p->kind = NK_##k;         

AstTranslationUnit ParseTranslationUnit();

#endif

