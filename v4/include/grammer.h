#ifndef __GRAMMER_H_
#define __GRAMMER_H_
//  tokens that may be first token in a declaration
#define FIRST_DECLARATION                                           \
    TK_CONST,    \
    TK_VAR,	TK_TYPE, TK_FUNC
//    TK_STRUCT,   TK_ID,	TK_VAR,	TK_TYPE, TK_FUNC
// fisrt token of an expression
// 第二行是一元运算符，unary_op   = "+" | "-" | "!" | "^" | "*" | "&" | "<-" .
// 第三行是(T).Mv(t, 7)。测试中偶然发现的。
#define FIRST_EXPRESSION                                                          \
    TK_ID,	TK_STRING, TK_NUM,	\
	TK_ADD,	TK_MINUS,	TK_NOT,	TK_BINARY_BITWISE_XOR,	TK_MUL,	TK_BITWISE_AND,	TK_RECEIVE,	\
	TK_LPARENTHESES,	\
	TK_STRING
	
// first token of statement
#define FIRST_STATEMENT                                                                   \
    TK_BREAK, TK_CASE,   TK_CONTINUE, TK_DEFAULT, TK_DO,    TK_FOR,       TK_GOTO,        \
    TK_IF,    TK_LBRACE, TK_RETURN,   TK_SWITCH,  TK_WHILE, TK_SEMICOLON, FIRST_EXPRESSION

#endif


