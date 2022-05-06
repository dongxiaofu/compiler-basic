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

void dump_token_number(){
	printf("token no:%d####", token_number);
	token_number++;
	Token token = current_token;
	dump_token(token);
}

void dump_token(Token token)
{
	if(is_dump_token == 1){
		return;
	}

	if(token.kind == TK_NUM){
		printf("token = %d, type = %d\n", token.value.value_num, token.kind);
	}else{// if(token.token_kind == T_)
		// 不打印结束符。
		if(token.kind == TK_EOF) return;
		if(token.kind == TK_SEMICOLON){
		 	printf("token = %s, type = %d\n", ";", token.kind);
		}else{
			printf("token = %s, type = %d\n", token.value.value_str, token.kind);
		}
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
		dump_token(current_token);
//	todo 暂时不打印token
//	if(is_dump_token == 1){
//		dump_token(current_token);
//	}
//	printf("get token\n");
//	Token token;
//	token.kind = TK_NAN;
	Token token;
	//memset(&token, 0, sizeof(token));
	if(current_char == -1){
		get_next_char();
	}
try_again:
	// 跳过空白符
	while(is_whitespace(current_char)){
		get_next_char();
	}

// try_again:
	// TK_EOF的值是82，R的值也是82，导致奇怪的问题。
	// if(current_char == TK_EOF){
	if(current_char == CH_EOF){
		token.kind = TK_EOF;
	}else if(current_char == '\n'){
		// token.kind = TK_LINE_BREAK;
		get_next_char();
		if(IsInsertSemicolon() == 1){
			token.kind = TK_SEMICOLON;
		}else{
			// get_next_char();
			goto try_again;
		}
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
//		int len = 0;
//		do{
//			
//			token.value.value_str[len] = current_char;
//			char ch = current_char;
//			get_next_char();
//			// todo 没想到更好的方法，只能用这种方式修修补补。
//			if(ch == '[' && current_char == '*') break;
//			len++;
//		// }while(is_operator(current_char));
//		}while(len == 0);

			
		//TODO current_char 不是运算符时怎么办？
		// int token_kind = scanners[current_char];
//		int token_kind = scanners[current_char]();
//		token.kind = token_kind;	
//		strcpy(token.value.value_str, token_names[token_kind]);

		// 处理运算符等token。
		token = *(ScanToken());

//		token.kind = get_token_kind(token.value.value_str);
			if(token.kind != TK_NAN){
				//get_next_char();
			}else{
				if(is_dump_token == 0){
					printf("There is no token any more 101\n");
				}
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
//		return 1;
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
//	if(is_dump_token == 0){
//		dump_token(current_token);
//	}

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
		// current_token_tail->token = &current_token;
		// *(current_token_tail->token) = current_token;
		memcpy(current_token_tail->token, &current_token, sizeof(current_token));
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

	// 注意这行代码的位置，必须在此函数返回前。
//	is_dump_token = 0;

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

	// is_dump_token = 0;
}


void setupScanner(){
	
	scanners['='] = ScanEqual;
	scanners['*'] = ScanStar;
	scanners['+'] = ScanPlus;
	scanners['-'] = ScanMinus;
	scanners['%'] = ScanPercent;
	scanners['/'] = ScanSlash;
	scanners['<'] = ScanLess;
	scanners['>'] = ScanGreat;
	scanners['"'] = ScanStrintLiterals;
	scanners[':'] = ScanColon;
	scanners['|'] = ScanBar;
	scanners['!'] = ScanExclamation;
	scanners['&'] = ScanAmpersand;
	scanners['^'] = ScanCaret;
	scanners['.'] = ScanDot;
}

int ScanEqual(){
	get_next_char();	
	if(current_char == '='){
		get_next_char();	
		return TK_EQUAL;	// ==
	}else{
		return TK_ASSIGN;		// =
	}
}

int ScanStar(){
	get_next_char();	
	if(current_char == '='){
		get_next_char();	
		return TK_MUL_ASSIGN;	// *=
	}else{
		return TK_MUL;		// *
	}
}

int ScanPlus(){
	get_next_char();	
	if(current_char == '='){
		get_next_char();	
		return TK_ADD_ASSIGN;	// +=
	}else if(current_char == '+'){
		get_next_char();	
		return TK_INC;		// ++
	}else{
		return TK_ADD;		// +
	}
}

int ScanMinus(){
	get_next_char();	
	if(current_char == '='){
		get_next_char();	
		return TK_MINUS_ASSIGN;	// -=
	}else if(current_char == '-'){
		get_next_char();	
		return TK_DEC;	// --
	}else{
		return TK_MINUS;		// -
	}
}

int ScanPercent(){
	get_next_char();	
	if(current_char == '='){
		get_next_char();	
		return TK_MOD_ASSIGN;	// %=
	}else{
		return TK_MOD;		// %
	}
}

int ScanSlash(){
	get_next_char();	
	if(current_char == '='){
		get_next_char();	
		return TK_DIV_ASSIGN;	// /=
	}else{
		return TK_DIV;		// /
	}
}

int ScanLess(){
	get_next_char();	
	if(current_char == '='){
		get_next_char();	
		return TK_LESS_OR_EQUAL;	// <=
	}else if(current_char == '<'){	
		int token = TK_LEFT_SHIFT;	// <<
		get_next_char();	
		if(current_char == '='){
			get_next_char();	
			token = TK_LEFT_SHIFT_ASSIGN;	// <<= 
		}
		return token;
	}else if(current_char == '-'){
		get_next_char();	
		return TK_RECEIVE;
	}else{
		return TK_LESS;		// <
	}
}

int ScanGreat(){
	get_next_char();	
	if(current_char == '='){
		get_next_char();	
		return TK_GREATER_OR_EQUAL;	// >=
	}else if(current_char == '>'){
		get_next_char();	
		int token = TK_RIGHT_SHIFT;	// >>
		if(current_char == '='){
			token = TK_RIGHT_SHIFT_ASSIGN;	// >>= 
			get_next_char();	
		} 
		return token;
	}else{
		return TK_GREATER;		// >
	}
}

// TODO 全局变量有点可怕。需进入函数才知道是怎么回事。
// void ScanToken(){
Token *ScanToken(){
	Token *token = (Token *)MALLOC(sizeof(Token));

	char flag = 0;
	for(int i = 0; i < 256; i++){
		// if(scanners[i] != 0){
		if(i == current_char && scanners[i] != 0){
			flag = 1;
			break;
		}	
	}

	if(flag == 1){
		//TODO current_char 不是运算符时怎么办？
		// int token_kind = scanners[current_char];
		int token_kind = scanners[current_char]();
		token->kind = token_kind;	
		// TODO 寻机优化这种特殊处理的方式。
		if(token_kind != TK_STRING){
			strcpy(token->value.value_str, token_names[token_kind]);
		}else{
			strcpy(token->value.value_str, current_token_value.value_str);
		}
	}else{
		int len = 0;
		do{
			token->value.value_str[len] = current_char;
			char ch = current_char;
			get_next_char();
			// todo 没想到更好的方法，只能用这种方式修修补补。
			if(ch == '[' && current_char == '*') break;
			len++;
		// }while(is_operator(current_char));
		}while(len == 0);
		token->kind = get_token_kind(token->value.value_str);
	}

	return token;
}

int ScanStrintLiterals(){
	get_next_char();
	char temp[512];
	memset(temp, 0, 512);
	int len = -1;

//	while(current_char != TK_DOUBLE_QUOTATION_MARK){
	while(current_char != '"'){
//	while(*CURSOR != '"'){
		// if(*CURSOR == '\n'){
		if(current_char == '\n'){
			// temp[++len] = current_char;	
			// temp[++len] = *CURSOR;	
			// get_next_char();
			break;
		}
		temp[++len] = current_char;	
		// temp[++len] = *CURSOR;	
		get_next_char();
	}

	// if(current_char != TK_DOUBLE_QUOTATION_MARK){
	if(current_char != '"'){
//	if(*CURSOR != '"'){
		EXPECT(TK_DOUBLE_QUOTATION_MARK);
	}	
	
	get_next_char();
	
	strcpy(current_token_value.value_str, temp);

	return TK_STRING;
}

// :
int ScanColon(){
	get_next_char();
	if(current_char == '='){
		get_next_char();
		return TK_INIT_ASSIGN;
	}
	
	return TK_COLON;
}

// &
int ScanAmpersand(){
	get_next_char();
	if(current_char == '='){
		get_next_char();
		return TK_AND_ASSIGN;
	}else if(current_char == '&'){
		get_next_char();
		return TK_CONDITIONAL_AND;
	}else if(current_char == '^'){
		get_next_char();
		int token = TK_BITWISE_AND_NOT;
		if(current_char == '='){
			get_next_char();
			token = TK_BIT_CLEAR_ASSIGN;
		} 
		return token;
	}

	return TK_BITWISE_AND;
}

// !
int ScanExclamation(){
	get_next_char();
	if(current_char == '='){
		get_next_char();
		return TK_NOT_EQUAL;
	}
	
	return TK_NOT;
}

// |
int ScanBar(){
	get_next_char();
	if(current_char == '|'){
		get_next_char();
		return TK_CONDITIONAL_OR;
	}else if(current_char == '='){
		get_next_char();
		return TK_OR_ASSIGN;
	}
	
	return TK_BINARY_BITWISE_OR;	
}

// ^
int ScanCaret(){
	get_next_char();
	if(current_char == '='){
		get_next_char();
		return TK_XOR_ASSIGN;
	}
	
	return TK_BINARY_BITWISE_XOR;
}

// .
int ScanDot(){
	get_next_char();
	if(current_char == '.'){
		get_next_char();
		if(current_char == '.'){
			get_next_char();
			return TK_ELLIPSIS;
		}
	}
	
	return TK_DOT;
}

// 是否加上分号
// TODO 可以用数组来把代码写简洁一些。
int IsInsertSemicolon(){
	if(current_token.kind == TK_ID){
		return 1;
	}

	if(current_token.kind == TK_STRING){
		return 1;
	}

	if(current_token.kind == TK_NUM){
		return 1;
	}

	if(current_token.kind == TK_INC || current_token.kind == TK_DEC
		|| current_token.kind == TK_RPARENTHESES || current_token.kind == TK_RBRACE
		|| current_token.kind == TK_RBRACKET
	){
		return 1;
	}

	if(current_token.kind == TK_BREAK || current_token.kind == TK_CONTINUE
		|| current_token.kind == TK_FALLTHROUGH || current_token.kind == TK_RETURN
	){
		return 1;
	}

	return 0;
}
