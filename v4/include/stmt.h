#ifndef __STMT_H_
#define __STMT_H_

#include "ast.h"
#include "lex.h"
#include "type2.h"
#include "expr.h"
#include <string.h>

// #define AST_STATEMENT_COMMON AST_NODE_COMMON

#define AST_LOOP_STATEMENT_COMMON \
    AST_STATEMENT_COMMON          \
    AstExpression expr;           \
    AstStatement stmt;            
//    BBlock loopBB;                \
//    BBlock contBB;                \
//    BBlock nextBB;

// struct astStatement
// {
// 	AST_STATEMENT_COMMON
// };
/**
	do-while
	while-do
		astLoopStatement
	for:
		astForStatement
 */
typedef struct astLoopStatement
{
	AST_LOOP_STATEMENT_COMMON
} *AstLoopStatement;
//	[expression] ;
typedef struct astExpressionStatement
{
	AST_STATEMENT_COMMON
	AstExpression expr;
} *AstExpressionStatement;
/**
	label-statement:
		identifier:	statement
		case	constant-expression:	statement
		default:		statement
		
	However,	in UCC,
		case-statement and default-statement is specially treated.
		see astCaseStatement and astDefaultStatement.

	
 */
// identifier:	statement	
typedef struct astLabelStatement
{
	AST_STATEMENT_COMMON
	char *id;
	AstStatement stmt;
//	Label label;
} *AstLabelStatement;
// case	constant-expression:	statement
typedef struct astCaseStatement
{
	AST_STATEMENT_COMMON
	AstExpression expr;
	AstStatement  stmt;
	struct astCaseStatement *nextCase;
//	BBlock respBB;
} *AstCaseStatement;

typedef struct astSelectCaseStatement
{
	AST_STATEMENT_COMMON
	AstStatement  caseStmt;
	AstStatement  stmt;
// todo 这句是重复的吗？
// AST_STATEMENT_COMMON 有next成员。
//	struct astCaseStatement *nextCase;
//	BBlock respBB;
} *AstSelectCaseStatement;
// default:		statement
typedef struct astDefaultStatement
{
	AST_STATEMENT_COMMON
	AstStatement stmt;
//	BBlock respBB;
} *AstDefaultStatement;
/**
	if(expression)	statement
	if(expression)	statement	else statement
 */
typedef struct astIfStatement
{
	AST_STATEMENT_COMMON
	// 本想把simpleStmt放到expr中，但不怎么方便。
	AstStatement simpleStmt;
	AstExpression expr;
	AstStatement  thenStmt;
	AstStatement  elseStmt;
} *AstIfStatement;

typedef struct astTypeSwitchGuard{
	AST_STATEMENT_COMMON
	AstExpression identifier;	
	AstExpression expr;
} *AstTypeSwitchGuard;

typedef struct astTypeCaseClause{
	AST_STATEMENT_COMMON
	AstNode typeSwitchCase;
	AstStatement statementList;
} *AstTypeCaseClause;

typedef struct astTypeSwitchStmt{
	AST_STATEMENT_COMMON
	AstStatement simpleStmt;
	AstTypeSwitchGuard guard;
	AstTypeCaseClause typeCaseClause;
} *AstTypeSwitchStmt;

/**
	@ncase		number of case
	@minVal	minium value
	@maxVal	maximum value
	@cases		list of case-statement
	@tail		the end of the list of case-statement
	@prev		
 */
typedef struct switchBucket
{
	int ncase;
	int	minVal;
	int maxVal;
	AstCaseStatement cases;
	AstCaseStatement *tail;
	struct switchBucket *prev;
} *SwitchBucket;
/**
	@expr		expression
	@stmt		statement
	@cases		list of case-statements
	@defStmt	default-statement
	@buckets	list of switchBucket
	@nbucket	number of nodes in the list of switchBucket
	@nextBB
	@defBB
 */
// switch(expression) statement
typedef struct astSwitchStatement
{
	AST_STATEMENT_COMMON
	AstExpression expr;
	AstStatement  stmt;
	AstCaseStatement cases;
	AstDefaultStatement defStmt;
	SwitchBucket buckets;
	int nbucket;
//	BBlock nextBB;
//	BBlock defBB;
} *AstSwitchStatement;
/**
	@expr	in AST_LOOP_STATEMENT_COMMON
			expression2
	@initExpr	expression1
	@incrExpr	expression3
	@testBB
 */
// for([expression1];[expression2];[expression3]) statement
typedef struct astForStatement
{
	AST_LOOP_STATEMENT_COMMON
	AstExpression initExpr;
	AstExpression incrExpr;
//	BBlock testBB;
} *AstForStatement;
// goto identifier
typedef struct astGotoStatement
{
	AST_STATEMENT_COMMON
	char *id;
//	Label label;
	char *label;
	AstStatement target;
} *AstGotoStatement;
//	break;
typedef struct astBreakStatement
{
	AST_STATEMENT_COMMON
	AstStatement target;
	char *label;
} *AstBreakStatement;
// continue;
typedef struct astContinueStatement
{   
	AST_STATEMENT_COMMON
	AstLoopStatement target;
	char *label;
} *AstContinueStatement;
// return [expression];
typedef struct astReturnStatement
{
	AST_STATEMENT_COMMON
	AstExpression expr;
} *AstReturnStatement;
/**
	@decls		list of declarations
	@stmts		list of statements
	@ilocals	id of local variables  ?
 */
// { [declaration-list] 	[statement-list] }
typedef struct astCompoundStatement
{
	AST_STATEMENT_COMMON
	AstNode decls;
	AstNode stmts;
	//	local variables that has initializer-list.
//	Vector ilocals;
} *AstCompoundStatement;

typedef struct astSendStmt{
	AST_STATEMENT_COMMON
	AstExpression channel;
	AstExpression expr;
} *AstSendStmt;

typedef struct astRecvStmt{
	AST_STATEMENT_COMMON
	AstExpression channel;
	AstExpression receiver;
} *AstRecvStmt;

typedef struct astIncDecStmt{
	AST_STATEMENT_COMMON
	AstExpression expr;
	// todo token kind用int表示是最好的方式吗？
	int op;	
} *AstIncDecStmt;

// 这是一个很重要的数据结构，差点漏掉。
typedef struct astLabeledStmt{
	AST_STATEMENT_COMMON
	char *label;
	AstStatement stmt;
} *AstLabeledStmt;

typedef struct astDeferStmt{
	AST_STATEMENT_COMMON
	AstStatement stmt;
} *AstDeferStmt;

typedef struct astGoStmt
{
	AST_STATEMENT_COMMON
	AstExpression expr;
} *AstGoStmt;

typedef struct astFallthroughStmt{
	AST_STATEMENT_COMMON
} *AstFallthroughStmt;

typedef struct astAssignmentsStmt{
	AST_STATEMENT_COMMON
	AstExpression expr;
} *AstAssignmentsStmt;

typedef struct astSelectStmt{
	AST_STATEMENT_COMMON
	AstSelectCaseStatement stmt;
} *AstSelectStmt;

// 简化代码，应学习。
#define AsExpr(stmt)   ((AstExpressionStatement)stmt)
#define AsLabel(stmt)  ((AstLabelStatement)stmt)
#define AsCase(stmt)   ((AstCaseStatement)stmt)
#define AsDef(stmt)    ((AstDefaultStatement)stmt)
#define AsIf(stmt)     ((AstIfStatement)stmt)
#define AsSwitch(stmt) ((AstSwitchStatement)stmt)
#define AsLoop(stmt)   ((AstLoopStatement)stmt)
#define AsFor(stmt)    ((AstForStatement)stmt)
#define AsGoto(stmt)   ((AstGotoStatement)stmt)
#define AsCont(stmt)   ((AstContinueStatement)stmt)
#define AsBreak(stmt)  ((AstBreakStatement)stmt)
#define AsRet(stmt)    ((AstReturnStatement)stmt)
#define AsComp(stmt)   ((AstCompoundStatement)stmt)

AstStatement CheckCompoundStatement(AstStatement stmt);

int IsAssignOp(int token_kind);
// 解析AstAssignmentsStmt中的运算符。
int ParseAssignmentsOp();
AstAssignmentsStmt ParseAssignmentsStmt(); 
AstFallthroughStmt ParseFallthroughStmt();
AstGoStmt ParseGoStmt();
AstBreakStatement ParseBreakStatement();
AstContinueStatement ParseContinueStatement();
AstGotoStatement ParseGotoStatement();
AstRecvStmt ParseRecvStmt();
AstSendStmt ParseSendStmt();
AstIncDecStmt ParseIncDecStmt();
AstLabeledStmt ParseLabeledStmt();
AstDeferStmt ParseDeferStmt();

int getStmtType();
AstStatement ParseStatementList();
AstSelectCaseStatement ParseAstSelectCaseStmt();
AstSelectStmt ParseSelectStmt();

// TODO 寻机改把返回值改成更合适的数据类型。
AstExpression ParseRangeExpr();
AstExpression ParseCondition();
AstStatement ParseForClause();
AstStatement ParseRangeClause();
AstStatement ParseForStmt();

// switch
// AstStatement Parse();
// AstStatement Parse();
// TODO 这是一个很特殊的产生式。暂时先放在这里。
AstNode ParseTypeList();
AstNode ParseTypeSwitchCase();
AstTypeCaseClause ParseTypeCaseClause();
AstTypeSwitchGuard ParseTypeSwitchGuard();

AstStatement ParseExprSwitchCase();
AstExprCaseClause ParseExprCaseClause();
AstStatement ParseTypeSwitchStmt();
AstExprSwitchStmt ParseExprSwitchStmt();
AstStatement ParseSwitchStmt();

// AstStatement ParseSimpleStatement();
// AstStatement ParseSimpleStatement();
AstStatement ParseBreakStmt();
AstReturnStatement ParseReturnStatement();
AstStatement ParseSimpleStatement();
AstIfStatement ParseIfStatement();
AstCompoundStatement ParseCompoundStatement();
AstStatement ParseLabelStatement();
AstStatement ParseStatement();

#endif

