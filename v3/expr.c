#include "lex.h"
#include "ast.h"
#include "expr.h"

/**
 * ExpressionList = Expression { "," Expression } .
 *
 */
AstNode ParseExpressionList(){
	LOG("%s\n", "parse ExpressionList");
	
	ParseExpression();
	while(current_token.kind==TK_COMMA){
		NEXT_TOKEN;
		ParseExpression();
	}
}

AstNode ParseExpression(){
	LOG("%s\n", "parse Expression");
	NEXT_TOKEN;
}
