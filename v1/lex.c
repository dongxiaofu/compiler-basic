#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

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

char *token_names[] = {
	#define TOKEN(kind, name)	name,
	#include "tokens.txt"
	#undef TOKEN
};

typedef struct{
	TokenKind kind;
	char name[MAX_NAME_LEN];
}TokenInfo;

TokenInfo keywords[] = {
	{TK_INT, "int"},
	{TK_IF, "if"},
	{TK_WHILE, "while"},
	{TK_ELSE, "else"}
};

typedef struct{
	Value value;
	TokenKind kind;
}Token;

Token current_token;
char current_char = -1;
FILE *fp;

int is_whitespace(char ch);

void get_next_char();

int get_token_kind(char *token_name);

// TokenKind get_keyword_kind(char *keyword);
// todo 这样修改，不知道有没有问题。
int get_keyword_kind(char *keyword);

Token get_token();

void dump_token(Token token);


int main(int argc, char *argv[])
{
	printf("I am a scanner\n");
	fp = fopen("test.c", "r");
	if(fp == NULL){
		perror("open file error\n");
		return -1;
	}
	
	Token token;
	int i = 0;
	
	while(1){
		dump_token(get_token());
//		printf("i = %d\n", i++);
	}

	printf("over\n");

	return 0;
}


void dump_token(Token token)
{
	if(token.kind == TK_NUM){
		printf("token = %d, type = %d\n", token.value.value_num, token.kind);
	}else{// if(token.token_kind == T_)
		printf("token = %s, type = %d\n", token.value.value_str, token.kind);
	}
}

Token get_token()
{
	Token token;
	token.kind = TK_NAN;
	if(current_char == -1){
		get_next_char();
	}
	// 跳过空白符
	while(is_whitespace(current_char)){
		get_next_char();
	}

	// 是字母
	if(isalpha(current_char)){
		int len = 0;
		do{
			token.value.value_str[len] = current_char;
			len++;
			get_next_char();
		}while( isalnum(current_char)  );	// 是字母或数字

		token.value.value_str[len] = 0;
		token.kind = get_keyword_kind(token.value.value_str);	

	}else if(isdigit(current_char)){ // 是数字
		int num = 0;
		do{
			num = num * 10 + current_char - '0';
			get_next_char();
		}while(isdigit(current_char));	// 是数字
		
		token.value.value_num = num;
		token.kind = TK_NUM;

	}else{	// 是其他
		// todo 暂时没有想到好方法。
		if(current_char == EOF){
			exit(3);
		}
		char name[2];
		name[0] = current_char;
		name[1] = 0;
		token.kind = get_token_kind(name);
		if(token.kind != TK_NAN){
			token.value.value_str[0] = current_char;
			token.value.value_str[1] = 0;

			get_next_char();
		}else{
			exit(3);
		}
	}	

	return token;
}


int is_whitespace(char ch)
{
	if(ch == ' '){
		return 1;
	}

	if(ch == '\n'){
		return 1;
	}

	if(ch == '\r'){
		return 1;
	}

	if(ch == '\t'){
		return 1;
	}

	return 0;
}


void get_next_char()
{
	char ch = fgetc(fp);
	if(ch != EOF){
		current_char = ch;
	}else{
		current_char = EOF;
	}
}



int get_token_kind(char *token_name)
{
	int kind = -1;
	for(int i = 0; i < sizeof(token_names) / sizeof(token_names[0]); i++){
		if(strcmp(token_names[i], token_name) == 0){
			// return token_names[i].kind;
			return i;
		}
	}

	return kind;
}



int get_keyword_kind(char *keyword)
{
	int kind = TK_ID;
	for(int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++){
		if(strcmp(keywords[i].name, keyword) == 0){
			return keywords[i].kind;
		}
	}

	return kind;
}
