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

Symbol TranslateStatement(AstStatement stmt)
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
	return (*StmtTranslaters[stmt->kind - NK_SelectCaseStatement])(stmt);
}

Symbol TranslateExprSwitchStmt(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateTypeSwitchStmt(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateSelectCaseStatement(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateExpressionStmt(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateShortVarDecl(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateIfStatement(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateSwitchStatement(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateForStmt(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateGotoStatement(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateBreakStatement(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateContinueStatement(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateReturnStatement(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateCompoundStatement(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateIncDecStmt(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateLabelStmt(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateDeferStmt(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateFallthroughStmt(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateSelectStmt(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateGoStmt(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateSendStmt(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateRecvStmt(AstStatement stmt)
{
    Symbol sym;
    return sym;
}

Symbol TranslateAssignmentsStmt(AstStatement stmt)
{
    Symbol sym;
    return sym;
}
