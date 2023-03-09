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
	// 我用CheckBlock的返回值来返回函数是否有return语句的数据，但这样不能返回处理之后的AST。
	// 幸运的是，并不需要返回处理之后的AST，因为，我操作的是指针。
//	ifStmt->thenStmt = CheckBlock(ifStmt->thenStmt);
	CheckBlock(ifStmt->thenStmt);

// TODO AstIfStatement的结构已经发生变化，不再把elseIfStmt存储到单独的成员中。
//	if(ifStmt->elseIfStmt){
//		ifStmt->elseIfStmt = CheckIfStatement(ifStmt->elseIfStmt);
//	}

	if(ifStmt->elseStmt){
		if(ifStmt->elseStmt->kind == NK_Block){
			// ifStmt->elseStmt = CheckBlock(ifStmt->elseStmt);
			// 由于CheckBlock的参数是指针，因此，不需要依靠它的返回值获取修改之后的返回结果。
			CheckBlock(ifStmt->elseStmt);
		}else{
			ifStmt->elseStmt = CheckStatement(ifStmt->elseStmt);
		}
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

//	forStmt->body = CheckBlock(forStmt->body);
	CheckBlock(forStmt->body);

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
	AstIncDecStmt incDecStmt = (AstIncDecStmt)stmt;	
	assert(incDecStmt->expr != NULL);
//	assert(incDecStmt->expr->kids[0] != NULL);
	// incDecStmt->expr = CheckExpression(incDecStmt->expr->kids[0]);
	incDecStmt->expr = CheckExpression(incDecStmt->expr);

	return (AstStatement)incDecStmt;
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
	AstExpression rightExpr;
	// 做如下检查：
	// 1. 变量名和变量值是否一一对应。
	// 2. 待定。
	// 最终结果是一个语句单链表。
	AstAssignmentsStmt assignStmt = AsAssign(stmt);
	AstStatement nextStmt = (AstStatement)assignStmt->next;

	AstExpression leftExpr = CheckExpressionList(assignStmt->expr->kids[0]);
	rightExpr = assignStmt->expr->kids[1];
	if(rightExpr->op == OP_CALL){
		rightExpr = CheckExpressionList(rightExpr);
		rightExpr->receiver = leftExpr;
		stmt = (AstStatement)rightExpr;
		stmt->kind = NK_Expression;
		stmt->next = nextStmt;
		return stmt;
	}

	rightExpr = CheckExpressionList(rightExpr);

	int leftExprCount = 0;
	int	rightExprCount = 0;

	AstAssignmentsStmt currentAssignStmt = NULL;
	AstAssignmentsStmt preAssignStmt = NULL;
			assignStmt->expr->kids[0] = leftExpr;
			assignStmt->expr->kids[1] = rightExpr;

//	while(leftExpr && rightExpr){
//
//		if(currentAssignStmt == NULL){
//			assignStmt->expr->kids[0] = leftExpr;
//			assignStmt->expr->kids[1] = rightExpr;
//			preAssignStmt = assignStmt;
//
//		}else{
//			leftExpr = CheckExpressionList(leftExpr);
//			rightExpr = CheckExpressionList(rightExpr);
//
//			CREATE_AST_NODE(currentAssignStmt, AssignmentsStmt);
//			currentAssignStmt->expr->kids[0] = leftExpr;
//			currentAssignStmt->expr->kids[1] = rightExpr;
//
//			preAssignStmt->next = currentAssignStmt;
//			preAssignStmt = currentAssignStmt;
//		}
//
//		leftExprCount++;
//		rightExprCount++;
//		leftExpr = leftExpr->next;
//		rightExpr = rightExpr->next;
//	}
//
//	// 使用assert能让我写出正确的代码。
//	assert(leftExpr == NULL || rightExpr == NULL);
//	assert(leftExprCount == rightExprCount);
//	
//	// 把a,b,c = 1,2,3这样的语句拆成了a = 1、b = 2、c = 3这样三个语句。要把
//	// 这些语句加到原来的AST中。
//	if(currentAssignStmt != NULL){
//		currentAssignStmt->next = nextStmt;
//	}

	// 返回stmt或assignStmt都可以。因为，操作的是指针。
	return stmt;
}

AstStatement CheckSimpleStmt(AstStatement stmt)
{
	return CheckStatement(stmt);
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

	AstExpression expr = expressionList;
	
	if(expr->op == OP_CALL){
		char *funcName = (char *)(expr->kids[0]->val.p);
		FunctionSymbol fsym = LookupID(funcName);
//		VariableSymbol head = (VariableSymbol)MALLOC(sizeof(struct variableSymbol));
//		VariableSymbol *ptr = &(head->next);
//		VariableSymbol p;
		AstExpression head = (AstExpression)MALLOC(sizeof(struct astExpression));
		AstExpression *ptr = &(head->next);
		Symbol p;
		Type ty;
		AstExpression identifier = identifierList;
		VariableSymbol receiver = fsym->receivers;	
		int receiverCount = fsym->receiverCount;
		for(int i = 0; i < receiverCount; i++){
			char *name = (char *)(identifier->val.p);
			ty = receiver->ty;
			p = AddVariable(name, ty);
	//		*ptr = p;
	//		ptr = &(p->next);
			identifier->val.p = p;

			receiver = receiver->next;
			identifier = identifier->next;
		}
		expr->receiver = identifierList;
		CheckExpression(expr);
		
		shortVarDecl->expr = expr;

		return stmt; 
	}

	while(identifier != NULL){
		CheckExpression(identifier);
		identifier = identifier->next;
	}

	while(expr != NULL){
		CheckExpression(expr);
		expr = expr->next;
	}

	AstExpression dst = identifierList;
	AstExpression src = expressionList;
	AstExpression head = NULL;
	AstExpression cur, pre;
	
	while(dst != NULL && src != NULL){

		CREATE_AST_NODE(cur, Expression);
		cur->op = OP_ASSIGN;
		cur->kids[0] = dst;
		cur->kids[1] = src;

		if(head == NULL){
			head = cur;
			pre = cur;
		}else{
			pre->next = cur;	
			pre = cur;
		}

		dst = dst->next;
		src = src->next;
	}

	shortVarDecl->expr = head;

	return stmt;
}
