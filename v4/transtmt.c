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
	
	FunctionSymbol fsym;

	return fsym;
}
