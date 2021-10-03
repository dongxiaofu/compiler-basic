#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "input.h"
#include "decl.h"
#include "lex.h"
#include "ast.h"

int main(int argc, char *argv[])
{
	printf("I am a scanner\n");
	
	ReadSourceFile("1.go");	

	Token token;
	int i = 0;
	
//	while(1){
		get_token();
		StartPeekToken();
		get_token();
		get_token();
		get_token();
		get_token();
		get_token();
		get_token();
		EndPeekToken();
		if(current_token.kind == TK_EOF) return 0;
		ParseTranslationUnit();
//	}

	return 0;
}

AstTranslationUnit ParseTranslationUnit(){
	
	AstTranslationUnit *p;
	// CREATE_AST_NODE(p, "TranslationUnit");
	CREATE_AST_NODE(p, TranslationUnit);

	if(current_token.kind == TK_FUNC){
		LOG("parse func\n");

	}else{
		LOG("parse decl\n");
		p->extDecls = declaration();
	}

	LOG("parse unit over\n");
}


