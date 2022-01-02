#ifndef LEX_H
#define LEX_H

#include <stdio.h>
#include <assert.h>
//#include <string.h>
//#include <ctype.h>
//#include <stdlib.h>

#define MAX_NAME_LEN	200
// 打印日志
// #define LOG	printf
#define LOG	
#define ERROR	printf
#define NO_TOKEN	if(current_token.kind == TK_EOF){break;}
// #define NO_TOKEN 

#define BINARY 		TokenOps[current_token.kind - TK_ASSIGN].bop	
#define UNARY 		TokenOps[current_token.kind - TK_ASSIGN].uop	

// 扫描token的函数指针数组
typedef int (*Scanner)(void);
static Scanner scanners[256];

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

static int Prec[] = {
	#define OPINFO(op, prec, name, func, opcode)	prec,
	#include "opinfo.h"
	#undef OPINFO
};

enum OP
{
#define OPINFO(op, prec, name, func, opcode) op,
#include "opinfo.h"
#undef OPINFO
};

struct tokenOp {
	int bop:16;
	int uop:16;
};

// todo 不明白，照抄的。
static struct tokenOp TokenOps[] =
{
#define TOKENOP(tok, bop, uop) {bop, uop},
#include "tokenop.h"
#undef  TOKENOP
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

typedef struct token{
	Value value;
	TokenKind kind;
}Token;

typedef struct token_link{
	Token *token;
	struct token_link *pre;
	struct token_link *next;
}*TOKEN_LINK;

Token current_token;
// ScanStringLiterals中需要这样做。
Value current_token_value;
Token start_token;
char *start_cursor;
char start_char;

typedef struct start_cursor_link{
	char *start_cursor;
	struct start_cursor_link *pre;
	struct start_cursor_link *next;
}*START_CURSOR_LINK;

typedef struct start_char_link{
	char *start_char;
	struct start_char_link *pre;
	struct start_char_link *next;
}*START_CHAR_LINK;

// static START_CURSOR_LINK cursor_tail;
// static START_CHAR_LINK char_tail;
// static TOKEN_LINK current_token_tail;
START_CURSOR_LINK cursor_tail;
START_CHAR_LINK char_tail;
TOKEN_LINK current_token_tail;

static char current_char = -1;
FILE *fp;
char is_dump_token;

//#define NEXT_TOKEN	get_token
#define NEXT_TOKEN	get_token()
#define EXPECT(token_kind)	expect_token(token_kind)	

// todo 寻机把这个宏放到更合适的位置。
#define expect_semicolon if(current_token.kind == TK_SEMICOLON) expect_token(TK_SEMICOLON);
#define expect_ellipsis if(current_token.kind == TK_ELLIPSIS) expect_token(TK_ELLIPSIS);
#define expect_comma if(current_token.kind == TK_COMMA) expect_token(TK_COMMA);

#define BINARY_OP       TokenOps[current_token.kind - TK_ASSIGN].bop
#define UNARY_OP        TokenOps[current_token.kind - TK_ASSIGN].uop

// todo 这个值不能是任何正常字符。
#define CH_EOF 255

void StartPeekToken();
void EndPeekToken();

int is_whitespace(char ch);

void get_next_char();

int get_token_kind(char *token_name);

// TokenKind get_keyword_kind(char *keyword);
// todo 这样修改，不知道有没有问题。
int get_keyword_kind(char *keyword);

// 检查是不是运算符
int is_operator(char ch);
int IsInsertSemicolon();
Token get_token();

// 调试表达式使用。
static int token_number = 0;
void dump_token_number();
void dump_token(Token token);

// char *get_token_name(TokenKind kind);
void get_token_name(char *token_name, TokenKind kind);

void expect_token(TokenKind kind);

int ScanEqual();
int ScanStar();
int ScanPlus();
int ScanMinus();
int ScanPercent();
int ScanSlash();
int ScanLess();
int ScanGreat();
int ScanStrintLiterals();
int ScanColon();
int ScanBar();
int ScanExclamation();
int ScanAmpersand();
int ScanCaret();
int ScanDot();
Token *ScanToken();
void setupScanner();


typedef struct astNode2{
	TokenKind op;
	Value value;
	struct astNode *kids[2];
} *AstNodePtr;

#endif
