#ifndef EXPR_H
#define EXPR_H

#include <assert.h>




AstNode ParseExpressionList();
AstNode ParseExpression();
AstNode ParseBinaryExpr(int prec);
AstNode ParseUnaryExpr();

AstNode ParseConversion();
int IsPostfix(TokenKind kind);
AstNode ParseSelectorTypeAssertion();
AstNode ParseArguments();

AstNode ParsePrimaryExpr();

// todo 这些运算符的解析函数是否应该存在？
AstNode ParseBinaryOp();
AstNode ParseRelOp();
AstNode ParseAddOp();
AstNode ParseMulOp();

int IsRelOp();
int IsAddOp();
int IsMulOp();
int IsUnaryOp();
int IsBinaryOp();

#endif
