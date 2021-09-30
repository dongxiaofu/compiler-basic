#include "ast.h"
#include "decl.h"


AstNode declaration(){
	LOG("%s\n", "enter declaration");	
	TokenKind kind = current_token.kind;
	Value value = current_token.value;

	AstNode decl;

	switch(kind){
		case TK_VAR:
			LOG("%s\n", "parse var");
			break;
		case TK_TYPE:
			LOG("%s\n", "parse type");
			break;
		case TK_CONST:
			LOG("%s\n", "parse const");
			break;
		default:
			LOG("%s\n", "parse decl error");
			break;
	}

	return decl;
}

