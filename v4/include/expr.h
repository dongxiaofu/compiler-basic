#ifndef EXPR_H
#define EXPR_H

#include <assert.h>

AstKeyedElement ParseKeyedElement();
AstLiteralValue ParseLiteralValue();
AstCompositeLit ParseCompositeLit();

AstExpression ParseExpressionList();
AstExpression ParseExpression();

AstExpression ParseFactor();
AstExpression ParseTerm();
AstExpression ParseBinaryExpr(int prec);
AstExpression ParseUnaryExpr();

// AstExpression ParseDecimalDigit();
// AstExpression ParseDecimalDigits();
// AstExpression ParseDecimalLit();
// AstExpression Parse
// AstExpression Parse
// AstExpression Parse
// AstExpression Parse
AstFunctionLit ParseFunctionLit();
AstExpression ParseOperandName();
AstExpression ParseIntLit();
AstExpression ParseBasicLit();
int IsSwitch();
unsigned char IsCompositeLit();
AstExpression ParseLiteral();
AstExpression ParseOperand();
AstExpression ParseMethodExpr();

AstExpression ParseConversion();
int IsPostfix(TokenKind kind);
void ParseSelectorTypeAssertion(AstExpression expr);
AstArguments ParseArguments();
AstNode ParseIndexSlice();
AstExpression ParseDotExpr();
AstExpression ParsePrimaryExpr();

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
