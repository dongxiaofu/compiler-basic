#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "lex.h"

//int main(int argc, char *argv[])
//{
//	printf("I am a scanner\n");
//	fp = fopen("test.c", "r");
//	if(fp == NULL){
//		perror("open file error\n");
//		return -1;
//	}
//	
//	Token token;
//	int i = 0;
//	
//	while(1){
//		dump_token(get_token());
////		printf("i = %d\n", i++);
//	}
//
//	printf("over\n");
//
//	return 0;
//}


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
//	printf("get token\n");
//	Token token;
//	token.kind = TK_NAN;
	Token token;
	memset(&token, 0, sizeof(token));
	if(current_char == -1){
		get_next_char();
	}
	// 跳过空白符
	while(is_whitespace(current_char)){
		get_next_char();
	}

try_again:
	if(current_char == TK_EOF){
		token.kind = TK_EOF;
	
	}else if(isalpha(current_char)){ // 是字母
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
	//	if(current_char == EOF){
	//		printf("There is no token any more 100\n");
	//		exit(100);
	//	}
		char name[2];
		name[0] = current_char;
		name[1] = 0;
		token.kind = get_token_kind(name);
		if(token.kind != TK_NAN){
			token.value.value_str[0] = current_char;
			token.value.value_str[1] = 0;

			get_next_char();
		}else{
			printf("There is no token any more 101\n");
			goto try_again;
		}
	}	

	current_token = token;

	dump_token(token);

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
		current_char = TK_EOF;
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

// todo 会出错吗？
// 只担心关键字类型。
void expect_token(TokenKind kind)
{
	TokenKind cur_kind = current_token.kind;
	// todo 又是一个常见问题。此处的token_name设置为多大合适?
	char token_name[30];
	get_token_name(token_name, kind);
	
	if(cur_kind != kind){
		printf("Expect:%s\n", token_name);
		perror("Expect error");
		exit(-2);
	}
	NEXT_TOKEN;
}

// todo 不存在数组元素怎么处理？
// todo 很常见的问题：要通过函数获取返回值，究竟是用指针参数
// 还是使用返回值？
void get_token_name(char *token_name, TokenKind kind){
	strcpy(token_name, token_names[kind]);
}
