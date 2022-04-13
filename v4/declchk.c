#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"

void CheckTranslationUnit(AstTranslationUnit transUnit)
{
	printf("%s\n", "Start Check");

	AstNode p = transUnit->decls;

	while(p){
		if(p->kind == NK_Function){
			printf("%s\n", "Check function");
		}else{
			printf("%s\n", "Check global declaration");
		}
		p = p->next;
	}

	printf("%s\n", "End Check");
}
