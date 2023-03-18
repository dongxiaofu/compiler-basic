#include "symbol.h"
#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "exprchk.h"
#include "tranexpr.h"
#include "transtmt.h"
#include "gen.h"

void Translate(AstTranslationUnit transUnit)
{
	printf("%s\n", "Start Translate");

	// AstDeclaration p = transUnit->decls;
	AstNode p = transUnit->decls;
	VariableSymbol sym;

	while(p){
		if(p->kind == NK_Function){
			printf("%s\n", "Translate function loop");
			TranslateFunction((AstFunction)p);
		}
		p = p->next;
	}
}
	
// 调试工具
void PrintBBlock(BBlock bblock)
{
	IRInst irinst = bblock->irinst.next;
	// 怎么遍历双向链表？
	while(irinst != &bblock->irinst){
		if(irinst->opcode == JMP){
			BBlock target = (BBlock)irinst->opds[0];
			printf("%s %s\n", "JMP", target->sym->name);
		}else if(irinst->opcode == RET){
			if(irinst->opds[0]->kind == SK_CONSTANT){
				printf("%s %d\n", "RET", irinst->opds[0]->val.i[0]);
			}else{
				printf("%s %s\n", "RET", irinst->opds[0]->name);
			}
		}else{
		printf("dst name = %s,", irinst->opds[0]->name);
		if(irinst->opds[1] != NULL){
			if(irinst->opds[1]->kind == SK_CONSTANT){
				printf("src val = %d,", irinst->opds[1]->val.i[0]);
			}else{
				printf("src name = %s,", irinst->opds[1]->name);
			}
		}

		if(irinst->opds[2] != NULL){
			if(irinst->opds[2]->kind == SK_CONSTANT){
				printf("src2 val = %d,", irinst->opds[2]->val.i[0]);
			}else{
				printf("src2 name = %s,", irinst->opds[2]->name);
			}
		}
		}

		printf("\n");
		irinst = irinst->next;
	}
}

Symbol CreateLabel()
{
	Symbol sym = (Symbol)MALLOC(sizeof(struct symbol));
	sym->name = (char *)MALLOC(sizeof(char) * MAX_NAME_LEN);
	sprintf(sym->name, "BB%d", BBlockNo++);

	return sym;
}

void TranslateFunction(AstFunction function)
{
	printf("%s\n", "Translate function");
	
	// 这是一个可笑、隐蔽、事出有因的错误。保留它，留个纪念吧。
	// 解释这个错误。在这个函数之外，经过本函数处理之后的函数的FunctionSymbol变量中的entryBB是0x0。
	// 这是一个不正确的数值。为什么会这样？
	// 我在这个函数中正确处理了这个函数，然后，并没有把这种修改保存到AST中，但是，我以为，我已经把
	// 数据保存到AST中了。我以为，我已经把数据保存到了function中。因为，大量函数都是直接操作指针的。
	// 思维定势，让我以为，这个函数也是如此。
	// 还有，当初，我不明白FSYM有啥用。可代码正常运行需要它，为了解决眼前问题写出一些消除错误的代码，可
	// 最后，我又忘记了处理这个稀里糊涂写出来的FSYM。这就叫做“事出有因”。
//	FunctionSymbol fsym = FSYM;
	FSYM = function->fsym;

	// 思路如下：
	// 1. 创建两个基本块，分别是入口基本块和退出基本块。
	// 2. 把当前基本块设置成入口基本块。
	// 3. 翻译函数体。
	// 4. 把退出基本块放到基本块链表中。
	// 5. 优化IR。暂时不做。
	// 6. 遍历基本块链表，给每个基本块命名。
	BBlock entryBB = CreateBBlock();
	BBlock exitBB = CreateBBlock();

	FSYM->entryBB = entryBB;
	FSYM->exitBB = exitBB;

	CurrentBBlock = entryBB;

	// 翻译函数体
	// 这个错误很有意思，它是我错误理解AST的产物，把它保留一段时间。
//	TranslateStatement(function->block->stmt);
	AstBlock block = function->block;
	AstStatement stmt = block->stmt;
	while(stmt != NULL){
		TranslateStatement(stmt);
		stmt = stmt->next;
	}
	
	StartBBlock(exitBB);	

	// 遍历基本块链表
	BBlock bb = entryBB;
	while(bb != NULL){
		bb->sym = CreateLabel();
//		PrintBBlock(bb);
		bb = bb->next;
	}	

//	return fsym;
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

/**
 * 思路是：
 * 1. 把switch(expr)中的expr记作A，把case expr中的expr记作B，把当前case基本块记作C，
 * 把下一个case基本块记作B，把紧跟在switch基本块后面的基本块记作N。
 * 2. 处理第一个case，比较A和B。
 * 3. A == B，进入C，C的最后一条中间码是跳转到N。
 * 4. A != B, 跳转到B。 
 */
void TranslateExprSwitchStmt(AstStatement stmt)
{
	BBlock nextBB,nextCaseBB;
	nextBB = CreateBBlock();

	AstExprSwitchStmt switchStmt = AsExprSwitchStmt(stmt);
	if(switchStmt->simpleStmt){
		TranslateStatement((AstStatement)switchStmt->simpleStmt);
	}

	Symbol caseSym;
	if(switchStmt->expr){
		caseSym = TranslateExpression(switchStmt->expr);
	}else{
		caseSym = (Symbol)MALLOC(sizeof(struct symbol));
		caseSym->kind = SK_BOOLEAN;
		union value val;
		val.boolean = 1;
		caseSym->val = val;
	}

	// 创建了基本块也没有问题，后期应该可以优化基本块。
	StartBBlock(CreateBBlock());

	AstExprCaseClause caseClause = switchStmt->exprCaseClause;

	while(caseClause){
		// 下一个case基本块。
		nextCaseBB = CreateBBlock();

		// 处理case expr中的expr。
		AstExpression expr = caseClause->exprSwitchCase;	
		Symbol sym = TranslateExpression(expr);

		// 如果switch (expr)中的expr不等于当前case中的expr，跳转到下一个case基本块。
		GenerateBranch(T(VOID), nextCaseBB, JNE, caseSym, sym);

		// 处理case expr:{stmts}中的stmts。
		AstStatement statement = caseClause->statementList;
		while(statement){
		    TranslateStatement(statement);
		    statement = statement->next;
		}

		// 执行完当前case基本块后，要跳转到switch基本块的下一个基本块。因为go的switch默认有
		// 其他语言中的break效果。
		GenerateJmp(nextBB);

		// 开启下一个基本块。回到循环开头时，正在处理的case基本块就是上一个case基本块的下一个基本块。
		// 基本块，也和C语言中的指针一样，可以先使用，再填充内容。
		StartBBlock(nextCaseBB);

		caseClause = caseClause->next;
	}

	// 这是翻译语句的惯例。翻译完当前基本块后，开启下一个基本块，开始处理下一个基本块时，
	// 就会把所有中间码存储到这里开启的基本块nextBB中。
	StartBBlock(nextBB);
}

void TranslateTypeSwitchStmt(AstStatement stmt)
{

}

void TranslateSelectCaseStatement(AstStatement stmt)
{

}

void TranslateExpressionStmt(AstStatement stmt)
{
	TranslateExpression((AstExpression)stmt);

	// 虽然只是处理表达式，仍然创建并加入一个基本块。多余的基本块，以后再优化。
	StartBBlock(CreateBBlock());
}

void TranslateShortVarDecl(AstStatement stmt)
{
	AstShortVarDecl shortVarDecl = (AstShortVarDecl)stmt;
	AstExpression expr = shortVarDecl->expr;
	while(expr != NULL){
		TranslateExpression(expr);
		expr = expr->next;
	}

	StartBBlock(CreateBBlock());
}

void TranslateBlock(AstBlock block)
{
	AstStatement stmt = block->stmt;
	while(stmt != NULL){
		TranslateStatement(stmt);
		stmt = stmt->next;
	}
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
		AstBlock block = ifStmt->thenStmt;
		TranslateBlock(block);

	}else{
		falseBB = CreateBBlock();

		TranslateBranch(Not(ifStmt->expr), falseBB, trueBB);

		StartBBlock(trueBB);
		AstBlock thenBlock = ifStmt->thenStmt;
		TranslateBlock(thenBlock);
		GenerateJmp(nextBB);

		StartBBlock(falseBB);
		AstBlock elseBlock = ifStmt->elseStmt;
		TranslateBlock(elseBlock);
	}

	StartBBlock(nextBB);
}

void TranslateSwitchStatement(AstStatement stmt)
{

}

void TranslateForStmt(AstStatement stmt)
{
	BBlock initBB, testBB, loopBB, contBB, nextBB;

//	initBB = CreateBBlock();
	testBB = CreateBBlock();
	loopBB = CreateBBlock();
	contBB = CreateBBlock();
	nextBB = CreateBBlock();

	AstForStmt forStmt = AsFor(stmt);

	// for语句并非总是由这些基本块组成，但我简单处理成这样。
	forStmt->testBB = testBB;
	forStmt->loopBB = loopBB;
	forStmt->contBB = contBB;
	forStmt->nextBB = nextBB;

	if(forStmt->condition){
		StartBBlock(testBB);	
		TranslateBranch(Not(forStmt->condition), nextBB, loopBB);
		StartBBlock(loopBB);
		TranslateStatement((AstStatement)forStmt->body);
		GenerateJmp(testBB);
	}

	if(forStmt->forClause){
		AstForClause forClause = forStmt->forClause;
		TranslateStatement(forClause->initStmt);		

		StartBBlock(testBB);
		TranslateBranch(Not(forClause->condition), nextBB, loopBB);

		StartBBlock(loopBB);	
//		TranslateStatement((AstStatement)forStmt->body);
		AstBlock block = forStmt->body;
		TranslateBlock(block);

		StartBBlock(contBB);
		TranslateStatement(forClause->postStmt);
		GenerateJmp(testBB);
	}

	if(forStmt->rangeClause){
		ERROR("%s\n", "不支持翻译range类型的for语句");
	}

	StartBBlock(nextBB);
}

void TranslateGotoStatement(AstStatement stmt)
{

}

void TranslateBreakStatement(AstStatement stmt)
{
	AstBreakStmt breakStmt = AsBreak(stmt);
	GenerateJmp(AsFor(breakStmt->target)->nextBB);

	StartBBlock(CreateBBlock());
}

void TranslateContinueStatement(AstStatement stmt)
{
	AstContinueStatement continueStmt = AsCont(stmt);
	GenerateJmp(AsFor(continueStmt->target)->contBB);

	// 此处新建基本块，有什么用？
	StartBBlock(CreateBBlock());
}

void TranslateReturnStatement(AstStatement stmt)
{
	AstReturnStatement returnStmt = AsRet(stmt);
	AstExpression expr = returnStmt->expr;

	Symbol dstHead = (Symbol)MALLOC(sizeof(struct symbol));
	Symbol dst;
	Symbol *dstPtr = &(dstHead->next);

	while(expr != NULL){
		dst = TranslateExpression(expr);
		*dstPtr = dst;
		dstPtr = &(dst->next);

		expr = expr->next;
	}

	GenerateReturn(FSYM->ty, dstHead->next);

	GenerateJmp(FSYM->exitBB);
	// 结束一个基本块后，要马上开启另一个基本块。我只能这样理解这个问题。
	StartBBlock(CreateBBlock());
}

void TranslateCompoundStatement(AstStatement stmt)
{
	AstCompoundStatement compoundStmt = (AstCompoundStatement)stmt;
//	while(compoundStmt != NULL){
		if(compoundStmt->decls){
			if(compoundStmt->decls->kind == NK_VarDeclaration){
				AstVarDeclaration decl = (AstVarDeclaration)compoundStmt->decls;
				while(decl){
					AstVarDeclarator declarator = (AstVarDeclarator)decl->decs;
					while(declarator){
						AstInitDeclarator initDec = (AstInitDeclarator)declarator->initDecs;
						while(initDec){
							if(initDec->init){
								InitData idata = ((AstInitializer)initDec->init)->idata;
								// TODO 先简化处理吧。
								Symbol dst = LookupID(initDec->dec->id);
								assert(dst != NULL);
								Symbol src = TranslateExpression(idata->expr);
								Type ty = dst->ty;
								GenerateMov(ty, dst, src);

							}
							initDec = (AstInitDeclarator)initDec->next;
						}

						declarator = declarator->next;
					}

					decl = decl->next;
				}
			}
		}
	
		if(compoundStmt->labeledStmt != NULL){
			// TODO 不知道怎么处理。
		}
	
		if(compoundStmt->stmts != NULL){
			TranslateStatement((AstStatement)compoundStmt->stmts);
		}

		// 许多AstCompoundStatement语句连接在一起（是这样吗），但AstCompoundStatement语句的下一个
		// 并非都是AstCompoundStatement语句。
		if(compoundStmt->kind != NK_CompoundStatement){
//			break;
		}
//		compoundStmt = compoundStmt->next;
//	}
}

void TranslateIncDecStmt(AstStatement stmt)
{
	AstIncDecStmt incDecStmt = (AstIncDecStmt)stmt;
	TranslateExpression(incDecStmt->expr);

	StartBBlock(CreateBBlock());
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
	while(stmt != NULL && stmt->kind == NK_AssignmentsStmt){
		TranslateOneAssignmentsStmt(stmt);
		stmt = stmt->next;
	}

	assert(stmt == NULL);
}

void TranslateOneAssignmentsStmt(stmt)
{
	// 暂时只处理op是=这样的赋值语句，不处理+=这样的语句。
	AstAssignmentsStmt asStmt = AsAssign(stmt);
	TranslateAssignmentExpression(asStmt->expr);
}
