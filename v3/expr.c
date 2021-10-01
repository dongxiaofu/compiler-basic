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

/**
 * Expression = UnaryExpr | Expression binary_op Expression .
UnaryExpr  = PrimaryExpr | unary_op UnaryExpr .

binary_op  = "||" | "&&" | rel_op | add_op | mul_op .
rel_op     = "==" | "!=" | "<" | "<=" | ">" | ">=" .
add_op     = "+" | "-" | "|" | "^" .
mul_op     = "*" | "/" | "%" | "<<" | ">>" | "&" | "&^" .

unary_op   = "+" | "-" | "!" | "^" | "*" | "&" | "<-" .
 */
AstNode ParseExpression(){
	LOG("%s\n", "parse Expression");
//	ParseUnaryExpr() || ParseBinaryExpr();
//	if(TK_POSITIVE <= current_token.kind && current_token.kind <= TK_RECEIVE){
//	if(IsUnaryOp() == 1){
//		ParseUnaryExpr();
//	}else{
//		Token pre_token = current_token;
//		
//
//		ParseBinaryExpr();
//
//	}
	ParseBinaryExpr(4);
}

AstNode ParseBinaryExpr(int prec){
	LOG("%s\n", "parse BinaryExpr");
//	ParseExpression();
//	// todo 如何处理binary_op？
//	ParseBinaryOp();
//	ParseExpression();
	#define HIGHEST_BIN_PREC Prec[TK_MUL - TK_CONDITIONAL_OR]	
	
	if(prec == HIGHEST_BIN_PREC){
		ParseUnaryExpr();
	}else{
		ParseBinaryExpr(prec + 1);
	}

	while(IsBinaryOp() == 1 && (Prec[current_token.kind - TK_CONDITIONAL_OR] == prec)){
		NEXT_TOKEN;
		if(prec == HIGHEST_BIN_PREC){
			ParseUnaryExpr();
		}else{
			ParseBinaryExpr(prec + 1);
		}
	}

	AstNode decl;
	return decl;
}

AstNode ParseUnaryExpr(){
	LOG("%s\n", "parse UnaryExpr");
	// if(TK_POSITIVE <= current_token.kind && current_token.kind <= TK_RECEIVE){
	if(IsUnaryOp() == 1){
		NEXT_TOKEN;
		ParseUnaryExpr();
	}else{
		ParsePrimaryExpr();
	}
}

AstNode ParsePrimaryExpr(){
	LOG("%s\n", "parse PrimaryExpr");
	NEXT_TOKEN;
}


AstNode ParseBinaryOp(){
	LOG("%s\n", "parse binary_op");
	if(TK_CONDITIONAL_AND <= current_token.kind && current_token.kind <= TK_CONDITIONAL_OR){
		NEXT_TOKEN;
	}else{
		ParseRelOp() || ParseAddOp() || ParseMulOp(); 
		// ERROR("expect a binary op, but give %d\n", current_token.kind);
	}
}


AstNode ParseRelOp(){
	LOG("%s\n", "parse rel_op");
	if(TK_EQUAL <= current_token.kind && current_token.kind <= TK_GREATER_OR_EQUAL){
		NEXT_TOKEN;
	}else{
		ERROR("expect a real op, but give %d\n", current_token.kind);
	}
}


AstNode ParseAddOp(){
	LOG("%s\n", "parse add_op");
	if(TK_ADD <= current_token.kind && current_token.kind <= TK_BINARY_BITWISE_XOR){
		NEXT_TOKEN;
	}else{
		ERROR("expect a add op, but give %d\n", current_token.kind);
	}
}


AstNode ParseMulOp(){
	LOG("%s\n", "parse mul_op");
	if(TK_MUL <= current_token.kind && current_token.kind <= TK_BITWISE_AND_NOT){
		NEXT_TOKEN;
	}else{
		ERROR("expect a mul op, but give %d\n", current_token.kind);
	}
}

int IsRelOp(){
	return (TK_EQUAL <= current_token.kind && current_token.kind <= TK_GREATER_OR_EQUAL);
}

int IsAddOp(){
	return (TK_ADD <= current_token.kind && current_token.kind <= TK_BINARY_BITWISE_XOR);
}

int IsMulOp(){
	return (TK_MUL <= current_token.kind && current_token.kind <= TK_BITWISE_AND_NOT);
}

int IsUnaryOp(){
	return (TK_EQUAL <= current_token.kind && current_token.kind <= TK_GREATER_OR_EQUAL);
}

int IsBinaryOp(){
	return ((TK_CONDITIONAL_AND <= current_token.kind && current_token.kind <= TK_CONDITIONAL_OR)
		|| (IsRelOp() == 1)
		|| (IsAddOp() == 1)
		|| (IsMulOp() == 1)
	//	|| current_token.kind == TK_POSITIVE
	//	|| current_token.kind == TK_NEGATIVE
	//	|| current_token.kind == TK_POINTER
	//	|| current_token.kind == TK_ADDRESS
		);
}


