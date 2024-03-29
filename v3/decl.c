#include "ast.h"
#include "decl.h"
#include "expr.h"


AstNode declaration(){
	LOG("%s\n", "enter declaration");	
	TokenKind kind = current_token.kind;
	Value value = current_token.value;

	AstNode decl;

	switch(kind){
		case TK_VAR:
			LOG("%s\n", "parse var");
			ParseVarDecl();
			break;
		case TK_TYPE:
			LOG("%s\n", "parse type");
			break;
		case TK_CONST:
			LOG("%s\n", "parse const");
			ParseConstDecl();
			break;
		default:
			LOG("%s\n", "parse decl error");
			break;
	}

	return decl;
}

// todo 寻机把这个宏放到更合适的位置。
#define expect_semicolon if(current_token.kind == TK_SEMICOLON) expect_token(TK_SEMICOLON);
/**
 * 	ConstDecl      = "const" ( ConstSpec | "(" { ConstSpec ";" } ")" ) .
	ConstSpec      = IdentifierList [ [ Type ] "=" ExpressionList ] .
 */
AstNode ParseConstDecl(){
	LOG("%s\n", "parse const decl");

	AstNode decl;
	expect_token(TK_CONST);
	if(current_token.kind == TK_LPARENTHESES){
	NEXT_TOKEN;	
//	ParseConstSpec();
//	expect_semicolon;
	// if(current_token.kind == TK_SEMICOLON) expect_token(TK_SEMICOLON);
	// todo 耗费了很多很多时间。
	// while(current_token.kind == TK_SEMICOLON){
	while(current_token.kind == TK_ID){
		// todo 不需要这个NEXT_TOKEN，留给下面的函数解析。花了很多很多时间才找出这个问题。
		//NEXT_TOKEN;	
		ParseConstSpec();
		expect_semicolon;
		// if(current_token.kind == TK_SEMICOLON) expect_token(TK_SEMICOLON);
		//expect_token(TK_SEMICOLON);
	}
	expect_token(TK_RPARENTHESES);
}else{
	ParseConstSpec();
}

	return decl;
}

AstNode ParseConstSpec(){
	LOG("%s\n", "parse const spec");

	ParseIdentifierList();
//	if(current_token.kind == TK_TYPE){
	if(IsDataType(current_token.value.value_str) == 1){
		NEXT_TOKEN;
		assert(current_token.kind == TK_ASSIGN);
		expect_token(TK_ASSIGN);
		ParseExpressionList();
	}else if(current_token.kind == TK_ASSIGN){
		NEXT_TOKEN;
		ParseExpressionList();
	}else{

		NEXT_TOKEN;
	}

	// todo 
	AstNode decl;
	return decl;
}

// IdentifierList = identifier { "," identifier }
AstNode ParseIdentifierList(){
	LOG("%s\n", "parse IdentifierList");

	ParseIdentifier();
	while(current_token.kind == TK_COMMA ){
		NEXT_TOKEN;
		ParseIdentifier();
	}

	AstNode decl;

	return decl;
}

AstNode ParseIdentifier(){
	LOG("%s\n", "parse Identifier");

	NEXT_TOKEN;

	AstNode decl;

	return decl;

}

void ExpectDataType(){
	if(IsDataType(current_token.value.value_str) == 1){
		NEXT_TOKEN;
	}else{
		ERROR("%s\n", "expect a data type");
		exit(-1);	
	}
}

int IsDataType(char *str){
	int count = sizeof(TypeNames) / sizeof(TypeNames[0]);
	for(int i = 0; i < count; i++){
		if(strcmp(str, TypeNames[i]) == 0){
			return 1;	
		}
	}

	return 0;
}

/**
 * VarDecl     = "var" ( VarSpec | "(" { VarSpec ";" } ")" ) .
VarSpec     = IdentifierList ( Type [ "=" ExpressionList ] | "=" ExpressionList ) .
 */
AstNode ParseVarDecl(){
	LOG("%s\n", "parse VarDec");
	NEXT_TOKEN;
	if(current_token.kind == TK_LPARENTHESES){
		NEXT_TOKEN;
		while(current_token.kind != TK_RPARENTHESES){
			ParseVarSpec();
			expect_semicolon;	
		}
		expect_token(TK_RPARENTHESES);
	}else{
		ParseVarSpec();
	}
}

/**
 * VarSpec     = IdentifierList ( Type [ "=" ExpressionList ] | "=" ExpressionList ) .
 */
AstNode ParseVarSpec(){
	LOG("%s\n", "parse VarDec");
	ParseIdentifierList();
	if(current_token.kind == TK_ASSIGN){
		NEXT_TOKEN;
		ParseExpressionList();
	}else{
		// 跳过Type
		NEXT_TOKEN;
		if(current_token.kind == TK_ASSIGN){
			NEXT_TOKEN;
			ParseExpressionList();
		}
	}
}

//AstNode ParseVarDecl(){
//	LOG("%s\n", "parse VarDec");
//
//}
//
//
//AstNode ParseVarDecl(){
//	LOG("%s\n", "parse VarDec");
//
//}
//
//
//AstNode ParseVarDecl(){
//	LOG("%s\n", "parse VarDec");
//
//}
//
//
//AstNode ParseVarDecl(){
//	LOG("%s\n", "parse VarDec");
//
//}
