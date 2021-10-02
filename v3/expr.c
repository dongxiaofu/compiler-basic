#include "lex.h"
#include "ast.h"
#include "decl.h"
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

AstNode ParseConversion(){
	LOG("%s\n", "parse Conversion");
	NEXT_TOKEN;
	expect_token(TK_LPARENTHESES);
	ParseExpression();
	if(current_token.kind == TK_COMMA) expect_token(TK_COMMA);
	expect_token(TK_RPARENTHESES);
}

int IsPostfix(TokenKind kind){
	return (kind == TK_DOT || kind == TK_LBRACKET || kind == TK_LPARENTHESES);
} 

/**
 * Selector       = "." identifier .
 * TypeAssertion  = "." "(" Type ")" .
 */
AstNode ParseSelectorTypeAssertion(){
	NEXT_TOKEN;
	if(current_token.kind == TK_LPARENTHESES){
		NEXT_TOKEN;
		ExpectDataType();
		expect_token(TK_RPARENTHESES);
	}else{
		expect_token(TK_ID);
	}
}

/**
 * Arguments      = "(" [ ( ExpressionList | Type [ "," ExpressionList ] ) [ "..." ] [ "," ] ] ")" .
 * ()
 * (abc, def)
 * (uint abc, def,name)
 * (uint abc, def,name) ...
 */
AstNode ParseArguments(){
	expect_token(TK_LPARENTHESES);
	
	while(current_token.kind != TK_RPARENTHESES){
		if(IsDataType(current_token.value.value_str) == 1){
			NEXT_TOKEN;
		}else{
			ParseExpressionList();
		}

		// 第一个循环
		while(current_token.kind == TK_COMMA){
			NEXT_TOKEN;
			ParseExpressionList();
		}

		// 第二个循环
		while(current_token.kind == TK_ELLIPSIS){
			NEXT_TOKEN;
		}

		// 第三个循环
		while(current_token.kind == TK_COMMA){
			NEXT_TOKEN;
		}
	}	

	expect_token(TK_RPARENTHESES);
}

/**
 * PrimaryExpr =
	Operand |
	Conversion |
	MethodExpr |
	PrimaryExpr Selector |
	PrimaryExpr Index |
	PrimaryExpr Slice |
	PrimaryExpr TypeAssertion |
	PrimaryExpr Arguments .

Selector       = "." identifier .
Index          = "[" Expression "]" .
Slice          = "[" [ Expression ] ":" [ Expression ] "]" |
                 "[" [ Expression ] ":" Expression ":" Expression "]" .
TypeAssertion  = "." "(" Type ")" .
Arguments      = "(" [ ( ExpressionList | Type [ "," ExpressionList ] ) [ "..." ] [ "," ] ] ")" .
 */
AstNode ParsePrimaryExpr(){
	LOG("%s\n", "parse PrimaryExpr");

	if(IsDataType(current_token.value.value_str) == 1){
		ParseConversion();
	}else{
		NEXT_TOKEN;
	}

	while(IsPostfix(current_token.kind)){
		switch(current_token.kind){
			case TK_DOT:
				ParseSelectorTypeAssertion();
				break; 
			case TK_LPARENTHESES:
				ParseArguments();
				break; 
			default:
				break;
		}		
	}
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


