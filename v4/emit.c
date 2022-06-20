#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "symbol.h"
#include "emit.h"
#include "x86.h"
#include "output.h"

void EmitTranslationUnit(AstTranslationUnit transUnit)
{
	ASMFile = fopen("as.asm", "w+");

	EmitFunctions(transUnit);

	fclose(ASMFile);
}

void EmitFunctions(AstTranslationUnit transUnit)
{
	AstNode p = transUnit->decls;
	VariableSymbol sym;

	while(p){
		if(p->kind == NK_Function){
			AstFunction func = (AstFunction)p;
			EmitFunction(func->fsym);
		}
		p = p->next;
	}
}

void EmitFunction(FunctionSymbol fsym)
{
	// 函数名	
	fprintf(ASMFile, "%s:\n", fsym->name);
	// 对齐
	// 序言
	EmitPrologue();
	// 处理函数的基本块
	BBlock bblock = fsym->entryBB;
	while(bblock != NULL){
		EmitBBlock(bblock);
		bblock = bblock->next;
	}
	// 后记
	EmitEpilogue();
}
