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
	AstBlock	thenStmt;
	// AstStatement elseIfStmt;
	AstBlock   elseStmt;
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
 * ForClause = [ InitStmt ] ";" [ Condition ] ";" [ PostStmt ] .
 * InitStmt = SimpleStmt .
 * PostStmt = SimpleStmt .
 */
typedef struct astForClause{
	AST_NODE_COMMON
	AstStatement initStmt;
	AstExpression condition;
	AstStatement postStmt;
} *AstForClause;

/**
 * RangeClause = [ ExpressionList "=" | IdentifierList ":=" ] "range" Expression .
 */
typedef struct astRangeClause{
	AST_NODE_COMMON
	AstExpression expressionList;
	AstExpression identifierList;
	AstExpression expr;
} *AstRangeClause;

typedef struct astForStmt{
	AST_STATEMENT_COMMON
	AstExpression condition;
	AstForClause forClause;
	AstRangeClause rangeClause;
	AstStatement body;

	BBlock nextBB;
} *AstForStmt;

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

typedef struct astSwitchStatement
{
	AST_STATEMENT_COMMON
	AstStatement expr;
	AstStatement type;
} *AstSwitchStatement;

// goto identifier
typedef struct astGotoStatement
{
	AST_STATEMENT_COMMON
	char *id;
//	Label label;
	AstStatement target;
} *AstGotoStatement;

// continue;
typedef struct astContinueStatement
{   
	AST_STATEMENT_COMMON
	AstLoopStatement target;
	char *id;
} *AstContinueStatement;
// return [expression];
typedef struct astReturnStatement
{
	AST_STATEMENT_COMMON
	AstExpression expr;
} *AstReturnStatement;

typedef struct astLabelStmt{
	AST_STATEMENT_COMMON
	char *id;
	AstStatement stmt;
} *AstLabelStmt;

/**
 * Declaration | LabeledStmt | SimpleStmt
 */
typedef struct astCompoundStatement{
	AST_STATEMENT_COMMON
	AstNode decls;
	AstLabelStmt labeledStmt;
	AstNode stmts;
} *AstCompoundStatement;

typedef struct astSendStmt{
	AST_STATEMENT_COMMON
	AstExpression channel;
	AstExpression expr;
} *AstSendStmt;

typedef struct astRecvStmt{
	AST_STATEMENT_COMMON
// TODO 这个命名，含义更好，但不适合存储对应的产生式。
//	AstExpression channel;
	AstExpression expressionList;
	AstExpression identifierList;
	AstExpression receiver;
} *AstRecvStmt;

typedef struct astIncDecStmt{
	AST_STATEMENT_COMMON
	AstExpression expr;
} *AstIncDecStmt;

typedef struct astDeferStmt{
	AST_STATEMENT_COMMON
	AstExpression expr;
} *AstDeferStmt;

typedef struct astGoStmt
{
	AST_STATEMENT_COMMON
	AstExpression expr;
} *AstGoStmt;

typedef struct astFallthroughStmt{
	AST_STATEMENT_COMMON
} *AstFallthroughStmt;

typedef struct astBreakStmt{
	AST_STATEMENT_COMMON
	AstStatement target;
	char *id;
} *AstBreakStmt;

typedef struct astEmptyStmt{
	AST_STATEMENT_COMMON
} *AstEmptyStmt;

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
#define AsLabel(stmt)  ((AstLabelStmt)stmt)
#define AsCase(stmt)   ((AstCaseStatement)stmt)
#define AsDef(stmt)    ((AstDefaultStatement)stmt)
#define AsIf(stmt)     ((AstIfStatement)stmt)
#define AsSwitch(stmt) ((AstSelectCaseStatement)stmt)
#define AsSwitchStmt(stmt) ((AstSwitchStatement)stmt)
#define AsTypeSwitchStmt(stmt) ((AstTypeSwitchStmt)stmt)
#define AsExprSwitchStmt(stmt) ((AstExprSwitchStmt)stmt)
#define AsSelect(stmt) ((AstSelectStmt)stmt)
#define AsDefer(stmt)  ((AstDeferStmt)stmt)
#define AsSend(stmt)   ((AstSendStmt)stmt)
#define AsRecv(stmt)   ((AstRecvStmt)stmt)
#define AsAssign(stmt) ((AstAssignmentsStmt)stmt)
#define AsLoop(stmt)   ((AstLoopStatement)stmt)
#define AsFor(stmt)    ((AstForStmt)stmt)
#define AsGoto(stmt)   ((AstGotoStatement)stmt)
#define AsGo(stmt)     ((AstGoStmt)stmt)
#define AsCont(stmt)   ((AstContinueStatement)stmt)
#define AsBreak(stmt)  ((AstBreakStmt)stmt)
#define AsRet(stmt)    ((AstReturnStatement)stmt)
#define AsComp(stmt)   ((AstCompoundStatement)stmt)

int IsAssignOp(int token_kind);
// 解析AstAssignmentsStmt中的运算符。
int ParseAssignmentsOp();
AstAssignmentsStmt ParseAssignmentsStmt(); 
AstFallthroughStmt ParseFallthroughStmt();
AstGoStmt ParseGoStmt();
AstBreakStmt ParseBreakStmt();
AstContinueStatement ParseContinueStatement();
AstGotoStatement ParseGotoStatement();
AstRecvStmt ParseRecvStmt();
AstSendStmt ParseSendStmt();
AstIncDecStmt ParseIncDecStmt();
AstDeferStmt ParseDeferStmt();

int getStmtType();
AstStatement ParseStatementList();
AstSelectCaseStatement ParseAstSelectCaseStmt();
AstSelectStmt ParseSelectStmt();

// TODO 寻机改把返回值改成更合适的数据类型。
AstExpression ParseRangeExpr();
AstExpression ParseCondition();
AstForClause ParseForClause();
AstRangeClause ParseRangeClause();
AstForStmt ParseForStmt();

// switch
// AstStatement Parse();
// AstStatement Parse();
// TODO 这是一个很特殊的产生式。暂时先放在这里。
AstNode ParseTypeList();
AstNode ParseTypeSwitchCase();
AstTypeCaseClause ParseTypeCaseClause();
AstTypeSwitchGuard ParseTypeSwitchGuard();

AstExpression ParseExprSwitchCase();
AstExprCaseClause ParseExprCaseClause();
AstTypeSwitchStmt ParseTypeSwitchStmt();
AstExprSwitchStmt ParseExprSwitchStmt();
AstSwitchStatement ParseSwitchStmt();

// AstStatement ParseSimpleStatement();
// AstStatement ParseSimpleStatement();
AstReturnStatement ParseReturnStatement();
AstStatement ParseSimpleStatement();
AstIfStatement ParseIfStatement();
AstCompoundStatement ParseCompoundStatement();
AstLabelStmt ParseLabelStatement();
AstStatement ParseStatement();

#endif

