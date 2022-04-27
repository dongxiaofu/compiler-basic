#include "symbol.h"
#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "stmtchk.h"

AstStatement (*StmtCheckers[])(AstStatement) = {
	CheckExprSwitchStmt,
	CheckSelectCaseStatement,
	CheckIfStatement,
	CheckSwitchStatement,
	CheckForStmt,
	CheckGotoStatement,
	CheckBreakStatement,
	CheckContinueStatement,
	CheckReturnStatement,
	CheckCompoundStatement,
	CheckIncDecStmt,
	CheckLabelStmt,
	CheckDeferStmt,
	CheckFallthroughStmt,
	CheckSelectStmt,
	CheckGoStmt,
	CheckAssignmentsStmt
};

AstStatement CheckStatement(AstStatement stmt)
{
	return (*StmtCheckers[stmt->kind - NK_SelectCaseStatement])(stmt);
}

AstStatement CheckExprSwitchStmt(AstStatement stmt)
{

	return stmt;
}

AstStatement CheckSelectCaseStatement(AstStatement stmt)
{

return stmt;
}

AstStatement CheckIfStatement(AstStatement stmt)
{
	AstIfStatement ifStmt = AsIf(stmt);
	if(ifStmt->simpleStmt){
		ifStmt->simpleStmt = CheckSimpleStmt(ifStmt->simpleStmt);
	}

	ifStmt->expr = CheckExpression(ifStmt->expr);
	ifStmt->thenStmt = CheckBlock(ifStmt->thenStmt);

	if(ifStmt->elseIfStmt){
		ifStmt->elseIfStmt = CheckIfStatement(ifStmt->elseIfStmt);
	}

	if(ifStmt->elseStmt){
		ifStmt->elseStmt = CheckBlock(ifStmt->elseStmt);
	}

	return stmt;
}

AstStatement CheckSwitchStatement(AstStatement stmt)
{

return stmt;
}

AstForClause CheckForClause(AstForClause forClause)
{
	forClause->initStmt = CheckSimpleStmt(forClause->initStmt);
	forClause->condition = CheckExpression(forClause->condition);
	forClause->postStmt = CheckSimpleStmt(forClause->postStmt);

	return forClause;
}

AstRangeClause CheckRangeClause(AstRangeClause rangeClause)
{
	if(rangeClause->expressionList){
		rangeClause->expressionList = CheckExpressionList(rangeClause->expressionList);
	}

	if(rangeClause->identifierList){
		rangeClause->identifierList = CheckIdentifierList(rangeClause->identifierList);
	}

	rangeClause->expr = CheckExpression(rangeClause->expr);
	
	return rangeClause;
}

AstStatement CheckForStmt(AstStatement stmt)
{
	PushStatement(stmt, CURRENT->loops);
	PushStatement(stmt, CURRENT->breakable);
	
	AstForStmt forStmt = AsFor(stmt);
	if(forStmt->condition){
		forStmt->condition = CheckExpression(forStmt->condition);
	}

	if(forStmt->forClause){
		forStmt->forClause = CheckForClause(forStmt->forClause);
	}

	if(forStmt->rangeClause){
		forStmt->rangeClause = CheckRangeClause(forStmt->rangeClause);
	}

	forStmt->body = CheckBlock(forStmt->body);

	PopStatement(CURRENT->loops);
	PopStatement(CURRENT->breakable);

	return stmt;
}

AstStatement CheckGotoStatement(AstStatement stmt)
{

return stmt;
}

AstStatement CheckBreakStatement(AstStatement stmt)
{
	AstBreakStmt breakStmt = AsBreak(stmt);
	if(breakStmt->label){
		// TODO 不知道怎么实现标签？	

		return stmt;
	}

	AstStatement target = PopStatement(CURRENT->breakable);
	if(target == NULL){
		// TODO 打印错误信息，还没有实现。
		PRINTF("break必须出现在for、switch、select语句中\n");
		exit(-1);
	}

	breakStmt->target = target;
	
	return stmt;
}

AstStatement CheckContinueStatement(AstStatement stmt)
{
	AstContinueStatement continueStmt = AsCont(stmt);
	if(continueStmt->label){
		// TODO 实现continue label
		return stmt;
	}

	AstStatement target = PopStatement(CURRENT->loops);
	if(target){
		continueStmt->target = target;
	}else{
		// TODO 打印错误信息，还没有实现。
		PRINTF("continue必须出现在for语句中\n");
		exit(-1);
	}

	return stmt;
}

AstStatement CheckReturnStatement(AstStatement stmt)
{

return stmt;
}

AstStatement CheckCompoundStatement(AstStatement stmt)
{

return stmt;
}

AstStatement CheckIncDecStmt(AstStatement stmt)
{

return stmt;
}

AstStatement CheckLabelStmt(AstStatement stmt)
{

return stmt;
}

AstStatement CheckDeferStmt(AstStatement stmt)
{

return stmt;
}

AstStatement CheckFallthroughStmt(AstStatement stmt)
{

return stmt;
}

AstStatement CheckSelectStmt(AstStatement stmt)
{

return stmt;
}

AstStatement CheckGoStmt(AstStatement stmt)
{

return stmt;
}

AstStatement CheckAssignmentsStmt(AstStatement stmt)
{

return stmt;
}

AstExpression CheckExpression(AstExpression expr)
{

	return expr;
}

AstStatement CheckSimpleStmt(AstStatement stmt)
{

	return stmt;
}

AstExpression CheckExpressionList(AstExpression expr)
{

	return expr;
}

AstExpression CheckIdentifierList(AstExpression expr)
{

	return expr;
}

void PushStatement(AstStatement stmt, StmtVector v)
{
	if(v->index >= VECTOR_SIZE){
		return;
	}

	v->params[++v->index] = stmt;
}

AstStatement PopStatement(StmtVector v)
{
	if(v->index >= 0){
		return v->params[v->index--];
	}

	return NULL;
}
