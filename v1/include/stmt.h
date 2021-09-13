#ifndef STMT_H
#define STMT_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>
//#include "decl.h"


typedef struct astStmtNode{
	TokenKind op;
	Value value;
	struct astNode *kids[2];

	struct astNode *expr;
	struct astStmtNode *then_stmt;
	struct astStmtNode *else_stmt;
	struct astStmtNode *next;
} *AstStmtNodePtr;

int new_label();

AstNodePtr new_label_node();

AstStmtNodePtr create_ast_stmt_node(TokenKind op);

// 默认语句
// todo 后期可能要丢弃
void default_stmt();

// 解析一个token是不是表达式的前缀
int check_is_prefix_of_stmt(TokenKind kind);

// 解析表达式语句
 AstStmtNodePtr expr_stmt();

// 解析if语句
 AstStmtNodePtr if_stmt();
// 解析while语句
 AstStmtNodePtr while_stmt();
// 解析语句
 AstStmtNodePtr stmt();
// 解析复杂语句
 AstStmtNodePtr compound_stmt();

void visit_stmt_node(AstStmtNodePtr stmt);
#endif
