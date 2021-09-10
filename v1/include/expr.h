#ifndef EXPR_H
#define EXPR_H

#include <stdlib.h>
#include <assert.h>

// 解析基础表达式
void primary_expr();
//
// 解析乘法表达式
void mul_expr();

// 解析加法表达式
void add_expr();
// 解析表达式
// todo 不想现在设计AST结点，不设计返回值
void expression();

#endif
