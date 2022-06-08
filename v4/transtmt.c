#include "symbol.h"
#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "exprchk.h"
#include "tranexpr.h"
#include "transtmt.h"

void Translate(AstTranslationUnit transUnit)
{
	printf("%s\n", "Start Translate");

	AstDeclaration p = transUnit->decls;
	VariableSymbol sym;

	while(p){
		if(p->kind == NK_Function){
			printf("%s\n", "Translate function loop");
			TranslateFunction(p);
		}
		p = p->next;
	}
}

FunctionSymbol TranslateFunction(AstFunction function)
{
	printf("%s\n", "Translate function");
	
	FunctionSymbol fsym = FSYM;

	// 思路如下：
	// 1. 创建两个基本块，分别是入口基本块和退出基本块。
	// 2. 把当前基本块设置成入口基本块。
	// 3. 翻译函数体。
	// 4. 把退出基本块放到基本块链表中。
	// 5. 优化IR。暂时不做。
	// 6. 遍历基本块链表，给每个基本块命名。
	BBlock entryBB = CreateBBlock();
	BBlock exitBB = CreateBBlock();

	fsym->entryBB = entryBB;
	fsym->exitBB = exitBB;

	CurrentBBlock = entryBB;

	// 翻译函数体
	TranslateStatement(function->block->stmt);
	
	StartBBlock(exitBB);	

	// 遍历基本块链表
	BBlock bb = entryBB;
	while(bb != NULL){

		bb = bb->next;
	}	

	return fsym;
}

AstStatement (*StmtTranslaters[])(AstStatement) = {
	TranslateExprSwitchStmt,
	TranslateTypeSwitchStmt,
	TranslateSelectCaseStatement,
	TranslateExpressionStmt,
	TranslateShortVarDecl,
	TranslateIfStatement,
	TranslateSwitchStatement,
	TranslateForStmt,
	TranslateGotoStatement,
	TranslateBreakStatement,
	TranslateContinueStatement,
	TranslateReturnStatement,
	TranslateCompoundStatement,
	TranslateIncDecStmt,
	TranslateLabelStmt,
	TranslateDeferStmt,
	TranslateFallthroughStmt,
	TranslateSelectStmt,
	TranslateGoStmt,
	TranslateSendStmt,
	TranslateRecvStmt,
	TranslateAssignmentsStmt
};

void TranslateStatement(AstStatement stmt)
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
	(*StmtTranslaters[stmt->kind - NK_SelectCaseStatement])(stmt);
}

void TranslateExprSwitchStmt(AstStatement stmt)
{

}

void TranslateTypeSwitchStmt(AstStatement stmt)
{

}

void TranslateSelectCaseStatement(AstStatement stmt)
{

}

void TranslateExpressionStmt(AstStatement stmt)
{

}

void TranslateShortVarDecl(AstStatement stmt)
{

}

void TranslateIfStatement(AstStatement stmt)
{
	BBlock trueBB, nextBB, falseBB;

	trueBB = CreateBBlock();
	nextBB = CreateBBlock();

	AstIfStatement ifStmt = AsIf(stmt);
	if(ifStmt->simpleStmt){
		TranslateStatement((AstStatement)ifStmt->simpleStmt);
	}		

	if(ifStmt->elseStmt == NULL){
		TranslateBranch(Not(ifStmt->expr), nextBB, trueBB);

		StartBBlock(trueBB);
		TranslateStatement((AstStatement)ifStmt->thenStmt);

	}else{
		falseBB = CreateBBlock();

		TranslateBranch(Not(ifStmt->expr), falseBB, trueBB);

		StartBBlock(trueBB);
		TranslateStatement((AstStatement)ifStmt->thenStmt);
		GenerateJmp(nextBB);

		StartBBlock(falseBB);
		TranslateStatement(ifStmt->elseStmt);
	}

	StartBBlock(nextBB);
}

void TranslateSwitchStatement(AstStatement stmt)
{

}

void TranslateForStmt(AstStatement stmt)
{

}

void TranslateGotoStatement(AstStatement stmt)
{

}

void TranslateBreakStatement(AstStatement stmt)
{

}

void TranslateContinueStatement(AstStatement stmt)
{

}

void TranslateReturnStatement(AstStatement stmt)
{

}

void TranslateCompoundStatement(AstStatement stmt)
{

}

void TranslateIncDecStmt(AstStatement stmt)
{

}

void TranslateLabelStmt(AstStatement stmt)
{

}

void TranslateDeferStmt(AstStatement stmt)
{

}

void TranslateFallthroughStmt(AstStatement stmt)
{

}

void TranslateSelectStmt(AstStatement stmt)
{

}

void TranslateGoStmt(AstStatement stmt)
{

}

void TranslateSendStmt(AstStatement stmt)
{

}

void TranslateRecvStmt(AstStatement stmt)
{

}

void TranslateAssignmentsStmt(AstStatement stmt)
{

}


