#ifndef STMT_H
#define STMT_H

#include <stdlib.h>
#include <assert.h>

// 默认语句
// todo 后期可能要丢弃
void default_stmt();

// 解析一个token是不是表达式的前缀
int check_is_prefix_of_stmt(TokenKind kind);

// 解析表达式语句
void expr_stmt();

// 解析if语句
void if_stmt();
// 解析while语句
void while_stmt();
// 解析语句
void stmt();
// 解析复杂语句
void compound_stmt();

#endif
