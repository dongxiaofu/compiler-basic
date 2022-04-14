#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "symbol.h"

void CheckTranslationUnit(AstTranslationUnit transUnit)
{
	printf("%s\n", "Start Check");

	AstDeclaration p = transUnit->decls;

	while(p){
		if(p->kind == NK_Function){
			printf("%s\n", "Check function");
		}else{
			printf("%s\n", "Check global declaration");
			CheckGlobalDeclaration(p);
		}
		p = p->next;
	}

	printf("%s\n", "End Check");
}

// TODO 必须加static吗？
// static void CheckGlobalDeclaration(AstDeclaration decl)
void CheckGlobalDeclaration(AstDeclaration decls)
{
	if(decls->kind == NK_VarDeclaration){
		AstVarDeclaration decl = (AstVarDeclaration)decls;
		while(decl){
			AstVarDeclarator declarator = (AstVarDeclarator)decl->decs;	
			while(declarator){
				// 处理说明符

				Symbol sym;
				// TODO var a,b int = 2,4
				AstInitDeclarator initDec = (AstInitDeclarator)declarator->initDecs;
				// CG 处理a,b
				while(initDec){
					AstDeclarator dec = initDec->dec;
					if((sym = LookupID(dec->id)) == NULL){
						sym = (Symbol)AddVariable(dec->id);
					}	
					// 变量的初始值
					if(initDec->init){
						AstInitializer init = initDec->init;
					}

					initDec = (AstInitDeclarator)initDec->next;
				}
				declarator = declarator->next;
			}
			decl = decl->next;
		}	
	}else if(decls->kind == NK_ConstDeclaration){

	}else{
		printf("%s\n", "todo");
	}
}
