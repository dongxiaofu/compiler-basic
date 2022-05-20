#include "symbol.h"
#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "stmtchk.h"

AstStatement (*StmtCheckers[])(AstStatement) = {
	CheckExprSwitchStmt,
	CheckTypeSwitchStmt,
	CheckSelectCaseStatement,
	CheckExpressionStmt,
	CheckShortVarDecl,
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
	CheckSendStmt,
	CheckRecvStmt,
	CheckAssignmentsStmt
};

AstStatement CheckStatement(AstStatement stmt)
{
	if(stmt == NULL){
		printf("empty stmt\n");
		return NULL;
	}

	if(stmt->kind - NK_SelectCaseStatement < 0 || stmt->kind - NK_SelectCaseStatement > 21){
		printf("1stmt->kind = %d\n", stmt->kind);
		return NULL;
	}
	
	printf("stmt index = %d\n", stmt->kind - NK_SelectCaseStatement);
	return (*StmtCheckers[stmt->kind - NK_SelectCaseStatement])(stmt);
}

AstStatement CheckExprSwitchStmt(AstStatement stmt)
{
	AstExprSwitchStmt switchStmt = AsExprSwitchStmt(stmt);
	if(switchStmt->simpleStmt){
		switchStmt->simpleStmt = CheckSimpleStmt(switchStmt->simpleStmt);
	}

	if(switchStmt->expr){
		switchStmt->expr = CheckExpression(switchStmt->expr);
	}

	AstExprCaseClause caseClause = switchStmt->exprCaseClause;
	while(caseClause){
		AstExpression expr = caseClause->exprSwitchCase;	
		caseClause->exprSwitchCase = CheckExpressionList(expr);
		
		AstStatement statement = caseClause->statementList;	
		while(statement){
			CheckStatement(statement);
			statement = statement->next;
		}
		caseClause = caseClause->next;
	}

	return stmt;
}

AstStatement CheckTypeSwitchStmt(AstStatement stmt)
{
	AstTypeSwitchStmt switchStmt = AsTypeSwitchStmt(stmt);
	if(switchStmt->simpleStmt){
		switchStmt->simpleStmt = CheckSimpleStmt(switchStmt->simpleStmt);
	}

	// TODO 检查guard，待实现。
	switchStmt->guard = switchStmt->guard;	

	// 检查AstTypeCaseClause typeCaseClause
	AstTypeCaseClause typeCaseClause = switchStmt->typeCaseClause;
	while(typeCaseClause){
		// TODO typeSwitchCase，不检查。我不知道是否要对数据类型做语义分析。
		// AstStatement statementList
		AstStatement statement = typeCaseClause->statementList;
		while(statement){
			CheckStatement(statement);	
			statement = statement->next;
		}
		typeCaseClause = typeCaseClause->next;
	}

	return stmt;
}

AstStatement CheckSelectCaseStatement(AstStatement stmt)
{
	AstSelectCaseStatement selectCaseStmt = AsSwitch(stmt);
	if(selectCaseStmt->caseStmt){
		selectCaseStmt->caseStmt = CheckStatement(selectCaseStmt->caseStmt);	
	}
	AstStatement stmts = selectCaseStmt->stmt;
	AstStatement oneStmt = stmts;
	while(oneStmt){
		CheckStatement(oneStmt);
		oneStmt = oneStmt->next;
	}
	// TODO 这个赋值并不是必须的。
	selectCaseStmt->stmt = stmts;

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
	AstSwitchStatement switchStmt = AsSwitchStmt(stmt);
	if(switchStmt->expr){
		switchStmt->expr = CheckStatement((AstStatement)switchStmt->expr);
	}

	if(switchStmt->type){
		switchStmt->type = CheckStatement((AstStatement)switchStmt->type);
	}

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
	AstGotoStatement gotoStmt = AsGoto(stmt);
	// TODO 检查label，待实现。
	gotoStmt->target = CheckStatement(gotoStmt->target);

	return stmt;
}

AstStatement CheckBreakStatement(AstStatement stmt)
{
	AstBreakStmt breakStmt = AsBreak(stmt);
	if(breakStmt->id){
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
	if(continueStmt->id){
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
	FunctionType fty = FSYM->ty;	
	Signature sig = fty->sig;
	SignatureElement results = sig->results;
	int resultSize = sig->resultSize;
	int index = resultSize - 1;

	AstReturnStatement returnStmt = AsRet(stmt); 	
	AstExpression expr = returnStmt->expr;

	while(index >= 0 && expr){
		// 
		Type ty = results[index--].ty;	
		CheckExpression(expr);
		// TODO 检查返回值的数据类型和函数要求的返回值类型是否一致。
		expr = expr->next;
		resultSize--;
	}

	if(resultSize > 0 || expr){
		ERROR("函数的返回值和函数定义不匹配\n", "");
	}

	return stmt;
}

AstStatement CheckCompoundStatement(AstStatement stmt)
{
	AstCompoundStatement compoundStmt = AsComp(stmt);

	if(compoundStmt->decls){
		AstDeclaration decl = (AstDeclaration)compoundStmt->decls;
		while(decl){
			CheckDeclaration(decl);
			decl = (AstDeclaration)decl->next;
		}
	}

	if(compoundStmt->labeledStmt){
		compoundStmt->labeledStmt = CheckStatement(compoundStmt->labeledStmt);
	}

	if(compoundStmt->stmts){
		AstStatement stmt = (AstStatement)compoundStmt->stmts;	
		compoundStmt->stmts = CheckStatement(stmt);
	}

	return stmt;
}

AstStatement CheckIncDecStmt(AstStatement stmt)
{
	AstIncDecStmt incDecStmt = AsIncDec(stmt);
	incDecStmt->expr = CheckExpression(incDecStmt->expr);
	// TODO 不知道怎么检测是不是加号或减号，暂时不处理。
	return stmt;
}

AstStatement CheckLabelStmt(AstStatement stmt)
{
	AstLabelStmt labelStmt = AsLabel(stmt);;
	// TODO 还没有处理label
	labelStmt->stmt = CheckStatement(labelStmt->stmt);

	return stmt;
}

// TODO 这个函数远远没有完成。
// 请阅读https://go.dev/ref/spec#DeferStmt实现其他检查。
AstExpression CheckDeferExpr(AstExpression expr)
{
	AstExpression deferExpr = CheckExpression(expr);

	return deferExpr;
}

AstStatement CheckDeferStmt(AstStatement stmt)
{
	AstDeferStmt deferStmt = AsDefer(stmt);
	deferStmt->expr = CheckDeferExpr(deferStmt->expr);

	return stmt;
}

AstStatement CheckFallthroughStmt(AstStatement stmt)
{

return stmt;
}

AstStatement CheckSelectStmt(AstStatement stmt)
{
	AstSelectStmt selectStmt = AsSelect(stmt);
	AstSelectCaseStatement selectCaseStmt = selectStmt->stmt;	
	while(selectCaseStmt){
		CheckSelectCaseStatement(selectCaseStmt);
		selectCaseStmt = selectCaseStmt->next;
	}	
	// TODO 我认为，这里的赋值不是必须的。因为CheckStatement并不会修改参数的值，只会修改
	// 参数指向的内存空间中的数据。但为了形式上统一或未来某种未知的需要，还是写上赋值。
	selectStmt->stmt = selectCaseStmt;

	return stmt;
}

AstStatement CheckGoStmt(AstStatement stmt)
{
	AstGoStmt goStmt = AsGo(stmt);
	if(goStmt->expr == NULL){
		ERROR("GoStmt的expr不能是空", "");
	}
	// TODO 只完成了最基本的检查。
	// 还有许多其他检查，例如，expr是不是函数调用等。
	goStmt->expr = CheckExpression(goStmt->expr);
	
	return stmt;
}

int CheckAssignOp(int op)
{
	
	return op;
}

AstStatement CheckAssignmentsStmt(AstStatement stmt)
{
	AstAssignmentsStmt assignStmt = AsAssign(stmt);

	assignStmt->expr->kids[0] = CheckExpressionList(assignStmt->expr->kids[0]);
	assignStmt->expr->op = CheckAssignOp(assignStmt->expr->op);
	assignStmt->expr->kids[1] = CheckExpressionList(assignStmt->expr->kids[1]);

	return stmt;
}

AstStatement CheckSimpleStmt(AstStatement stmt)
{

	return stmt;
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

AstStatement CheckSendStmt(AstStatement stmt)
{
	AstSendStmt sendStmt = AsSend(stmt);
	sendStmt->channel = CheckExpression(sendStmt->channel);
	sendStmt->expr = CheckExpression(sendStmt->expr);

	return stmt;
}

AstStatement CheckRecvStmt(AstStatement stmt)
{
	AstRecvStmt recvStmt = AsRecv(stmt);

	if(recvStmt->expressionList){
		recvStmt->expressionList = CheckExpressionList(recvStmt->expressionList);
	}

	if(recvStmt->identifierList){
		recvStmt->identifierList = CheckIdentifierList(recvStmt->identifierList);	
	}

	recvStmt->receiver = CheckExpression(recvStmt->receiver);

	return stmt;
}

AstStatement CheckExpressionStmt(AstStatement stmt)
{
	stmt = CheckExpression((AstExpression)stmt);

	return stmt;
}

AstStatement CheckShortVarDecl(AstStatement stmt)
{
	AstShortVarDecl shortVarDecl = (AstShortVarDecl)stmt;
	AstExpression identifierList = shortVarDecl->identifierList;
	AstExpression expressionList = shortVarDecl->expressionList;
	AstExpression identifier = identifierList;
	while(identifier != NULL){
		CheckExpression(identifier);
		identifier = identifier->next;
	}

	AstExpression expr = expressionList;
	while(expr != NULL){
		CheckExpression(expr);
		expr = expr->next;
	}
	return stmt;
}
