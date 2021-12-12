#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "input.h"
#include "lex.h"

int main2(int argc, char *argv[])
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
		if(current_token.kind == TK_EOF) break;
		get_token();
	//	dump_token(get_token());
//		if(i++ > 100) break;
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
		// 不打印结束符。
		if(token.kind == TK_EOF) return;
		printf("token = %s, type = %d\n", token.value.value_str, token.kind);
	}
}


int is_operator(char ch)
{
	// char operators[] = {'*','/', '%', '<', '>',  '&',  '+',  '-',  '|',  '^',  '='};
	char operators[] = {'*','/', '%', '<', '>',  '&',  '+',  '-',  '|',  '^',  '=', '.'};
	
	for(int i = 0; i < sizeof(operators) / sizeof(char); i++){
		if(ch == operators[i]){
			return 1;
		}
	}
	return 0;
//	return (!isdigit(ch) && !isalpha(ch) && !is_whitespace(ch));
}

Token get_token()
{
//	todo 暂时不打印token
	dump_token(current_token);
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
	// TK_EOF的值是82，R的值也是82，导致奇怪的问题。
	// if(current_char == TK_EOF){
	if(current_char == CH_EOF){
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
		int len = 0;
		do{
			
			token.value.value_str[len] = current_char;
			char ch = current_char;
			get_next_char();
			// todo 没想到更好的方法，只能用这种方式修修补补。
			if(ch == '[' && current_char == '*') break;
			len++;
		// }while(is_operator(current_char));
		}while(len == 0);

		token.kind = get_token_kind(token.value.value_str);
			if(token.kind != TK_NAN){
				//get_next_char();
			}else{
				printf("There is no token any more 101\n");
				token.kind = TK_EOF;
			//	char is_go_to_try = 1;
			//	char arr[] = {TK_LBRACE, TK_RBRACE};
			//	for(int i = 0; i < sizeof(arr) / sizeof(char); i++){
			//		if(token.kind == arr[i]){
			//			is_go_to_try = 0;
			//			break;
			//		} 
			//	}
			//	if(is_go_to_try == 1){
			//		goto try_again;
			//	}
				
			}
	}	

	current_token = token;

	//dump_token(token);

	return token;
}


int is_whitespace(char ch)
{
	if(ch == ' ' || ch == '\t' || ch == '\r'){
	//	CURSOR++;
		return 1;
	}

	if(ch == '\n'){
	//	CURSOR++;
		LINE++;
		return 1;
	}

	return 0;
}


void get_next_char()
{
//	char ch = fgetc(fp);
	char ch = *CURSOR;
	if(ch != EOF){
		current_char = ch;
	}else{
		current_char = TK_EOF;
	}
	CURSOR++;
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

// 回退token
void StartPeekToken(){
//	start_token = current_token;
//	start_cursor = CURSOR;
//	start_char = current_char;


	if(cursor_tail->start_cursor == NULL){
		cursor_tail->start_cursor = CURSOR;
		char_tail->start_char = current_char;
		current_token_tail->token = &current_token;
	}else{
	//	START_CURSOR_LINK cursor_tail_cur = (START_CURSOR_LINK)malloc(sizeof(*START_CURSOR_LINK));
	//	START_CHAR_LINK char_tail_cur = (START_CHAR_LINK)malloc(sizeof(*START_CHAR_LINK));		
	//	TOKEN_LINK current_token_tail_cur = (TOKEN_LINK)malloc(*TOKEN_LINK);
//		START_CURSOR_LINK cursor_tail_cur = (START_CURSOR_LINK)malloc(sizeof(cursor_tail));
//		START_CHAR_LINK char_tail_cur = (START_CHAR_LINK)malloc(sizeof(char_tail));		
//		TOKEN_LINK current_token_tail_cur = (TOKEN_LINK)malloc(current_token_tail);

		START_CURSOR_LINK cursor_tail_cur = (START_CURSOR_LINK)malloc(sizeof(struct start_cursor_link));
		START_CHAR_LINK char_tail_cur = (START_CHAR_LINK)malloc(sizeof(struct start_char_link));		
		TOKEN_LINK current_token_tail_cur = (TOKEN_LINK)malloc(sizeof(struct token_link));

		cursor_tail_cur->start_cursor = CURSOR;
		cursor_tail_cur->pre = cursor_tail;
		cursor_tail->next = cursor_tail_cur;
		cursor_tail = cursor_tail_cur;

		char_tail_cur->start_char = current_char;
		char_tail_cur->pre = char_tail;
		char_tail->next = char_tail_cur;
		char_tail = char_tail_cur;

		// TODO 注意这里，耗时很久！
		// 创建双链表不是很难，难点在于这几行代码。
		// current_token_tail_cur->token = &current_token;
		// *(current_token_tail_cur->token) = current_token;
		Token *token = (Token *)malloc(sizeof(Token));
		*token = current_token;
		current_token_tail_cur->token = token;	
		current_token_tail_cur->pre = current_token_tail;
		current_token_tail->next = current_token_tail_cur;
		current_token_tail = current_token_tail_cur;
	}
}

void EndPeekToken(){
//	current_token = start_token;
//	CURSOR = start_cursor;
//	current_char = start_char;


//	if(cursor_tail->pre == NULL){
//		return;
//	}	

	if(cursor_tail->start_cursor == NULL){
		ERROR("%s\n", "Must use StartPeekToken at first");
		return;
	}
	current_token = *(current_token_tail->token);	
	CURSOR = cursor_tail->start_cursor;
	current_char = char_tail->start_char;

	if(cursor_tail->pre == NULL){
		return;
	}	

	current_token_tail = current_token_tail->pre;
	free(current_token_tail->next);
	current_token_tail->next = NULL;
	
	cursor_tail = cursor_tail->pre;
	free(cursor_tail->next);
	cursor_tail->next = NULL;

	char_tail = char_tail->pre;
	free(char_tail->next);
	char_tail->next = NULL;
}
