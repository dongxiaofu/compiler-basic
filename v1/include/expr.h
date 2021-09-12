#ifndef EXPR_H
#define EXPR_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>

AstNodePtr create_ast_node(TokenKind op, Value value, AstNodePtr left, AstNodePtr right);
// 解析基础表达式
AstNodePtr primary_expr();
// 解析乘法表达式
AstNodePtr mul_expr();
// 解析加法表达式
AstNodePtr add_expr();
// 解析表达式
// todo 不想现在设计AST结点，不设计返回值
AstNodePtr expression();

#endif
