#ifndef LEX_H
#define LEX_H

#include <stdio.h>
#include <assert.h>
//#include <string.h>
//#include <ctype.h>
//#include <stdlib.h>

#define MAX_NAME_LEN	15

typedef union{
	char value_str[MAX_NAME_LEN];
	int value_num;
}Value;

typedef enum{
	#define TOKEN(kind, name)	kind,
	#include "tokens.txt"
	#undef TOKEN
}TokenKind;

static char *token_names[] = {
	#define TOKEN(kind, name)	name,
	#include "tokens.txt"
	#undef TOKEN
};

typedef struct{
	TokenKind kind;
	char name[MAX_NAME_LEN];
}TokenInfo;

static TokenInfo keywords[] = {
	#define TOKEN(kind, name)	{kind, name},
	#include "tokens.txt"
	#undef TOKEN
};

//static TokenInfo keywords[] = {
//	{TK_INT, "int"},
//	{TK_IF, "if"},
//	{TK_WHILE, "while"},
//	{TK_ELSE, "else"}
//};

//static TokenKind prefix_of_stmt_token_kinds[] = {
//	TK_INT, TK_ID, TK_IF, TK_WHILE,TK_LBRACE
//};

typedef struct{
	Value value;
	TokenKind kind;
}Token;

Token current_token;
static char current_char = -1;
FILE *fp;

//#define NEXT_TOKEN	get_token
#define NEXT_TOKEN	get_token()
#define EXPECT(token_kind)	expect_token(token_kind)	


int is_whitespace(char ch);

void get_next_char();

int get_token_kind(char *token_name);

// TokenKind get_keyword_kind(char *keyword);
// todo 这样修改，不知道有没有问题。
int get_keyword_kind(char *keyword);

// 检查是不是运算符
int is_operator(char ch);

Token get_token();

void dump_token(Token token);

// char *get_token_name(TokenKind kind);
void get_token_name(char *token_name, TokenKind kind);

void expect_token(TokenKind kind);

typedef struct astNode{
	TokenKind op;
	Value value;
	struct astNode *kids[2];
} *AstNodePtr;

#endif
