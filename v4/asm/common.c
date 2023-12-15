#include "common.h"

// TODO 写完才知道，C语言中有和这个函数的功能相同的内置函数strstr。
// 真是没见识！
// 找机会处理掉我写的这个函数。
// 这个函数可能有存在的必要。
int GetSubStrIndex(char *subStr, char *str, unsigned int strLength)
{
	int index = -1;
	
// str可能是 hello\000\world\000。不能使用strlen计算它的真实长度。
//	unsigned int length = strlen(str);
	unsigned int subStrLength = strlen(subStr);

	for(int i = 0; i < strLength; i++){
		if(strncmp(subStr, str, subStrLength) == 0){
			return i;
		}
		
		str++;
	} 

	return - 1;
}

//Heap CurrentHeap;
//struct heap ProgramHeap;
//HEAP(ProgramHeap);
void StrToUpper(char *str, char *upperStr)
{
	int len = strlen(str);
	for(int i = 0; i < len; i++){
		if(islower(str[i])){
			upperStr[i] = toupper(str[i]);
		}else{
			upperStr[i] = str[i];
		}
	}
}

void StrToLower(char *str, char *lowerStr)
{
	int len = strlen(str);
	for(int i = 0; i < len; i++){
		if(isupper(str[i])){
			lowerStr[i] = tolower(str[i]);
		}else{
			lowerStr[i] = str[i];
		}
	}
}

// 模仿PHP函数命名。
void UcFirst(char *str)
{
	int len = strlen(str);
	if(len == 0)	return;
	str[0] = toupper(str[0]);
}

void *MALLOC(int size)
{
	void *p = (void *)HeapAllocate(CurrentHeap, size);
	memset(p, 0, size);
	return p;
}

int HeapAllocate(Heap heap, int size)
{
	struct mblock *blk = heap->last;

	while(size > blk->end - blk->avail){
		int m = 4096 + sizeof(struct mblock) + size; 
		blk->next = (struct mblock *)malloc(m);
		blk = blk->next;
		if(blk == NULL){
			printf("分配内存失败\n");
			exit(-4);
		}
		blk->begin = blk->avail = (char *)(blk + 1);
//		blk->begin = blk->avail = (char *)(blk);
		blk->next = NULL;
		heap->last = blk;	
		// blk->end = (char *)(blk + m);
		blk->end = (char *)blk + m - 1;
	}

	blk->avail += size;

	// return blk->avail - size;
	return (int)(blk->avail - size);
}


int SkipToNewline()
{
	lexer->currentLine++;
	lexer->index0 = 0;
	if(lexer->currentLine > lexer->lineNum) return END_OF_FILE;

	return lexer->currentLine;
}


int IsWhitespace(char ch)
{
	return (ch == ' ' || ch == '\t');
}


int IsCharDelimeter(char ch)
{
	if(ch == ' ' || ch == ',' || 
		ch == '\n' || ch == '"' ||
		ch == '(' || ch == ')'	|| ch == '*' ||
		ch == '\t' || ch == ':'){
		return True;
	}else{
		return False;
	}
}

int IsCharIdent(char ch)
{
	if(ch == '.' || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z')){
		return True;
	}else{
		return False;
	}
}

int IsCharNumeric(char ch)
{
	if('0' <= ch && ch <= '9'){
		return True;
	}else{
		return False;
	}
}

int IsStringInt(char *str)
{
	int startIndex = 0;
	// 1--十进制，2--二进制，3--八进制，4--十六进制。
	char type = 1;
	// 漏掉了 0x123,0b123, 0123
	if(str[0] == '0'){
		type = 3;
		startIndex++;
		if(str[1] == 'b'){
			type = 2;	
			startIndex++;
		}else if(str[1] == 'x'){
			type = 4;	
			startIndex++;
		}
	}else if(str[0] == '-'){
		startIndex++;
	}
	
	int len = strlen(str);
	if(type == 4){
		// todo 能不能和下面的逻辑合并？我不知道能不能修改IsCharNumeric，不敢随便合并。
		for(int i = startIndex; i < len; i++){
			// 对非字母使用toupper会有问题吗？我验证过，没有问题。
			int upperChar = toupper(str[i]);
			if(!(IsCharNumeric(str[i]) == True) || ('A' <= upperChar && upperChar <= 'F')){
				return False;
			}
		}
	}else{
		for(int i = startIndex; i < len; i++){
			if(IsCharNumeric(str[i]) == False){
				return False;
			}
		}
	}

	return True;
}

int IsStringFloat(char *str)
{
	return False;
}

int IsStringIdent(char *str)
{
	int len = strlen(str);

	for(int i = 0; i < len; i++){
		char ch = str[i];
		if(!(IsCharIdent(ch) || IsCharNumeric(ch) || ch == '.')){
			return False;
		}
	}

	if(IsCharNumeric(str[0]))	return False;

	return True;
}

int IsRegister(char *str)
{
	int len = strlen(str);
	if(str[0] != '%')	return False;
	for(int i = 1; i < len; i++){
		if(!IsCharIdent(str[i])) 	return False;
	}

	return True;
}

int IsImmediateOperand(char *str)
{
	int len = strlen(str);
	if(str[0] != '$')	return False;
	str++;
	if(!IsStringInt(str))    return False;

	return True;
}

void TrimWhitespace(char *str)
{
	int paddingLeft = 0;
	int length = strlen(str);
	// 清除左边的空白字符。
	// 计算出左边有多少个空白字符。
	for(int i = 0; i < length; i++){
		if(!IsWhitespace(str[i])) break;
		paddingLeft++;
	}
	// 把全部字符往左移动，不留空白字符。
	for(int i = paddingLeft; i < length; i++){
		str[i - paddingLeft] = str[i];
	}
	// 全部字符左移后，原字符占据的一些空间应该被设置成空字符。
	for(int i = length - paddingLeft; i < length; i++){
		str[i] = ' ';
	}
	// 清除右边的空白字符。
	// 从字符串的最右边开始往左寻找第一个非空白字符。找到后，把这个字符后面的那个字符设置成`0`。
	// for(int i = length - 1; i >= 0; i--){
	for(int i = length - 1; i > 0; i--){
		if(!IsWhitespace(str[i])){
//			str[i+1] = '\n';
//			str[i+2] = '0';
			str[i] = '\n';
			// str[i+1] = '0';
			str[i+1] = 0;
			break;
		}
	}

}

char GetLookAheadChar()
{
	int currentIndex = lexer->index1;
	int currentLine = lexer->currentLine;

	// TODO 需要在这里检查吗？
	if(sourceCode[currentLine] == 0x0){
		return END_OF_FILE;
	}

	if(lexer->string_state != STATE_IN_STRING){
		while(1){
			char *line = sourceCode[currentLine];
			if(currentIndex >= strlen(line)){
				currentLine++;
				// TODO 换行遇到文件结束怎么处理？像下面这样处理。
				if(sourceCode[currentLine] == 0x0) return END_OF_FILE;
			}

			if(!IsWhitespace(line[currentIndex]))	break;

			currentIndex++;
		}
	}

	return sourceCode[currentLine][currentIndex];
}

int GetLookAheadToken()
{
	Lexer copyLexer = (Lexer)MALLOC(sizeof(struct _lexer));
	*copyLexer = *lexer;
	int token = GetNextTokenExceptNewLine();
	*lexer = *copyLexer;

	return token;
}


// 能解析出换行符，但是能自动换行。
int GetNextToken()
{
	if(sourceCode[lexer->currentLine] == 0){
		return TYPE_TOKEN_INVALID;
	}

	int length = strlen(sourceCode[lexer->currentLine]);
	// TODO 不应该有这行代码。
	// lexer->index0 = lexer->index1;	

	if(lexer->string_state == STATE_END_STRING){
		lexer->string_state = STATE_NO_STRING;
	}

	// 这个循环的作用是什么？跳过空白行。
	while(1){
		// SkipToNewline();
		// TODO 我觉得这里是一个明显的错误。currentLine超过lineNum后，无法获得
		// 正常的token。如果在循环外面没有进行容错处理，就出现错误了。
		// 看了许久，我才敢认为：这是一个错误。
		if(lexer->currentLine > lexer->lineNum)	break;
		// TODO 这里也是一个错误吗？
		// 不理解这个条件是什么意思。
		// 当lexer->index0是0时，说明在一行数据的开头。
		// 这个流程，真是一点都不好！我重看了很多次，才勉强认为它是没有问题的。
		// 牵涉的代码还挺多的，还需要执行循环一两次后才有效，太不直观了。
		if(lexer->index0 != 0)	break;
		if(sourceCode[lexer->currentLine] && sourceCode[lexer->currentLine][lexer->index0] != '\n'){
			 break;
		}

		int line = SkipToNewline();
		if(line == END_OF_FILE){
			printf("read file over2\n");
//			exit(-1);
//			break;
			return TYPE_TOKEN_INVALID;
		}
	}

	length = strlen(sourceCode[lexer->currentLine]);

	if(lexer->string_state != STATE_IN_STRING){
		for(int i = lexer->index0; i < length; i++){
			if(!IsWhitespace(sourceCode[lexer->currentLine][i])){
				lexer->index0 = i;
				break;
			}
		}
	}

	lexer->index1 = lexer->index0;
	for(int i = lexer->index1; i < length; i++){
		if(lexer->string_state != STATE_IN_STRING){
			if(IsCharDelimeter(sourceCode[lexer->currentLine][i])){
				lexer->index1 = i;
				break;	
			}
		}else{
			if(sourceCode[lexer->currentLine][i] == '"'){
				lexer->index1 = i;
				break;
			}
		}
	}
	
	// index0和index1之间是一个单词。
	int end = lexer->index1;
	if(lexer->index1 == lexer->index0){
		end = lexer->index1 + 1;
		lexer->index1++;
	}
	// lexer->index0 = lexer->index1;	

	int count = 0;
	memset(lexer->lexeme, 0, sizeof(char) * MAX_SIZE);
	for(int i = lexer->index0; i < end; i++){
		// printf("line:%s\n", sourceCode[lexer->currentLine]);
		char currentChar = sourceCode[lexer->currentLine][i];

		if(currentChar == '\\'){
			// todo 暂时没有使用这个变量。
			// 是不是转义字符。
			char isEscapeCharacter = 1;
			int currentLineStrIndex = i;
			while(currentChar == '\\'){
					currentLineStrIndex++;
					char nextChar = sourceCode[lexer->currentLine][currentLineStrIndex];
					currentLineStrIndex++;
					char newChar;
					switch(nextChar){
						case 'a':
							newChar = '\a';
							break;
						case 'b':
							newChar = '\b';
							break;
						case 'f':
							newChar = '\f';
							break;
						case 'n':
							newChar = '\n';
							break;
						case 'r':
							newChar = '\r';
							break;
						case 't':
							newChar = '\t';
							break;
						case 'v':
							newChar = '\v';
							break;
						case '\'':
							newChar = '\'';
							break;
						case '\"':
							newChar = '\"';
							break;
						case '\\':
							newChar = '\\';
							break;
						default:
							{
								// if(nextChar == 2 || nextChar == 3){
								if(nextChar == '2' || nextChar == '3'){
									newChar = (nextChar - 48) << 6;
									for(int j = 0; j < 2; j++){
										currentChar = \
											sourceCode[lexer->currentLine][currentLineStrIndex];
										if(currentChar == 2 || currentChar == 3)	break;
										// if(currentChar == '2' || currentChar == '3')	break;
										newChar += ((currentChar - 48) << ((1-j) * 3));
										currentLineStrIndex++;
									}
								}else{
									lexer->lexeme[count++] = '\\';
									newChar = nextChar;
									isEscapeCharacter = 0;
								}
//								currentLineStrIndex++;
							}
					}
				lexer->lexeme[count++] = newChar;
				currentChar = sourceCode[lexer->currentLine][currentLineStrIndex];
			}

			if(currentLineStrIndex > end)	break;
			if(currentLineStrIndex != i){
				i = currentLineStrIndex - 1;
			}
		}else{
			lexer->lexeme[count++] = sourceCode[lexer->currentLine][i];
		}
	}
	lexer->lexeme[count] = 0;
	lexer->index0 = lexer->index1;	


	if(strlen(lexer->lexeme) > 1){
		if(lexer->string_state == STATE_IN_STRING){
			lexer->token_type = TYPE_TOKEN_STRING;
			return lexer->token_type;
		}
	}

	lexer->token_type = TYPE_TOKEN_INDENT;

	if(strlen(lexer->lexeme) == 1){
		if(lexer->lexeme[0] == '"'){
			switch(lexer->string_state){
				case STATE_NO_STRING:
					lexer->string_state = STATE_IN_STRING;
					break;
				case STATE_IN_STRING:
					lexer->string_state = STATE_END_STRING;
					break;
				default:
					// TODO
					break;
			}
			lexer->token_type = TYPE_TOKEN_QUOT;	
		}else if(lexer->lexeme[0] == ','){
			lexer->token_type = TYPE_TOKEN_COMMA;
		}else if(lexer->lexeme[0] == ':'){
			lexer->token_type = TYPE_TOKEN_COLON;
		}else if(lexer->lexeme[0] == '('){
			lexer->token_type = TYPE_TOKEN_OPEN_PARENTHESES;
		}else if(lexer->lexeme[0] == ')'){
			lexer->token_type = TYPE_TOKEN_CLOSE_PARENTHESES;
		}else if(lexer->lexeme[0] == '\n'){
			lexer->token_type = TYPE_TOKEN_NEWLINE;
		}else if(lexer->lexeme[0] == '%'){
			lexer->token_type = TYPE_TOKEN_PERCENT_SIGN;
		}else if(lexer->lexeme[0] == '*'){
			lexer->token_type = TYPE_TOKEN_STAR;
		}
	}

	if(IsStringInt(lexer->lexeme)){
		lexer->token_type = TYPE_TOKEN_INT;
	}

	if(IsStringIdent(lexer->lexeme)){
		lexer->token_type = TYPE_TOKEN_INDENT;
	}
	
	if(IsRegister(lexer->lexeme)){
		lexer->token_type = TYPE_TOKEN_REGISTER;
	}

	if(IsImmediateOperand(lexer->lexeme)){
		lexer->token_type = TYPE_TOKEN_IMMEDIATE;
	}

	if(strncmp(lexer->lexeme, ".", 1) == 0){
	//	lexer->token_type = TYPE_TOKEN_DOT_INDENT;
	}
	
	if(strcmp(lexer->lexeme, ".long") == 0){
		lexer->token_type = TYPE_TOKEN_LONG;
	}
	
	if(strcmp(lexer->lexeme, ".byte") == 0){
		lexer->token_type = TYPE_TOKEN_BYTE;
	}
	
	if(strcmp(lexer->lexeme, ".data") == 0){
		lexer->token_type = TYPE_TOKEN_DATA;
	}
	
	if(strcmp(lexer->lexeme, ".globl") == 0){
		lexer->token_type = TYPE_TOKEN_GLOBL;
	}
	
	if(strcmp(lexer->lexeme, ".text") == 0){
		lexer->token_type = TYPE_TOKEN_TEXT;
	}
	
	if(strcmp(lexer->lexeme, ".string") == 0){
		lexer->token_type = TYPE_TOKEN_KEYWORD_STRING;
	}

	if(strcmp(lexer->lexeme, ".align") == 0){
		lexer->token_type = TYPE_TOKEN_ALIGN;
	}

	if(strncmp(lexer->lexeme, "@", 1) == 0){
		lexer->token_type = TYPE_TOKEN_ALIGN;
	}

	if(strcmp(lexer->lexeme, ".section") == 0){
		lexer->token_type = TYPE_TOKEN_SECTION;
	}

	if(strcmp(lexer->lexeme, ".rodata") == 0){
		lexer->token_type = TYPE_TOKEN_RODATA;
	}

	if(strcmp(lexer->lexeme, ".type") == 0){
		lexer->token_type = TYPE_TOKEN_TYPE;
	}

	if(strcmp(lexer->lexeme, ".size") == 0){
		lexer->token_type = TYPE_TOKEN_SIZE;
	}

	if(strcmp(lexer->lexeme, "@object") == 0){
		lexer->token_type = TYPE_TOKEN_OBJECT;
	}

	if(strcmp(lexer->lexeme, ".local") == 0){
		lexer->token_type = TYPE_TOKEN_LOCAL;
	}

	if(strcmp(lexer->lexeme, ".comm") == 0){
		lexer->token_type = TYPE_TOKEN_COMM;
	}

	if(strcmp(lexer->lexeme, "@function") == 0){
		// TODO 应该使用 TYPE_TOKEN_FUNCTION 还是 TYPE_TOKEN_AT_FUNCTION ？
		lexer->token_type = TYPE_TOKEN_FUNCTION;
	}

	if(strcmp(lexer->lexeme, "call") == 0){
		lexer->token_type = TYPE_TOKEN_CALL;
	}

	// 检查是否要换行。
	// if(lexer->index0 == length - 1){
	if(lexer->index0 + 1 >= length){
	// if(lexer->index0 >= length){
		int line = SkipToNewline();
		if(line == END_OF_FILE){
			printf("read file over\n");
			return TYPE_TOKEN_INVALID;
//			exit(-1);
		}
	}

	return lexer->token_type;
}

int GetNextTokenExceptNewLine()
{
	int token = GetNextToken();
	while(token == TYPE_TOKEN_NEWLINE){
		token = GetNextToken();
	}

	return token;
}

char *GetCurrentTokenLexeme()
{
	if(lexer->lexeme[0] == 0)	return NULL;
	char *name = (char *)MALLOC(sizeof(char) * MAX_SIZE);
	strcpy(name,  lexer->lexeme);
	return name;
}

void StartPeekToken()
{
	int size = sizeof(struct _lexer);
	snapshotLexer = (Lexer)MALLOC(size);
	// 不能这样做。有问题。
	// snapshotLexer = lexer;
	memcpy(snapshotLexer, lexer, size);
}

void EndPeekToken()
{
	lexer = snapshotLexer;
}

int StrToNumber(char *str)
{
	if(!IsStringInt(str))	return 0;

	// todo 这是一段重复代码，从IsStringInt中复制而来。
	// 我不能立刻想到优化这段代码的方法，干脆直接复制过来用。找时间再优化吧。
	int startIndex = 0;
	// 1--十进制，2--二进制，3--八进制，4--十六进制。
	char type = 1;
	// 漏掉了 0x123,0b123, 0123
	if(str[0] == '0'){
		type = 3;
		startIndex++;
		if(str[1] == 'b'){
			type = 2;	
			startIndex++;
		}else if(str[1] == 'x'){
			type = 4;	
			startIndex++;
		}
	}else if(str[0] == '-'){
		startIndex++;
	}

	int len = strlen(str);
	int num = 0;
	
	for(int i = startIndex; i < len; i++){
		char ch = toupper(str[i]);
		int m = (len - 1) - i;
		if(ch >= 'A'){
			ch = 10 + (ch - 'A');
		}else{
			ch = ch - '0';
		}

		int n = 1;
		switch(type){
			case 1:			// 十进制
				for(int k = 0; k < m; k++){
					n *= 10;
				}
				break;
			case 2:			// 二进制
				n = m;
				break;
			case 3:			// 八进制
				n = 3*m;
				break;
			case 4:			// 十六进制
				n = 4*m;
				break;
			default:
				printf("there is an error in line %d\n", __LINE__);
				exit(__LINE__);
				break;
		}

		if(type == 1){
			num += ch * n;
		}else{
			num += ch << n;
		}
	}

	return num;
}

InstructionSet FindInstrCode(char *instr)
{
	int index = -1;
	int count = INSTRUCTION_SETS_SIZE;
	char *instrUpper = (char *)MALLOC(strlen(instr));
	StrToUpper(instr, instrUpper);

	for(int i = 0; i < count; i++){
		if(strcmp(instructionSets[i], instrUpper) == 0){
			index = i;
			break;
		}
	}

	InstructionSet instrCode = (index == -1) ? I_INSTR_INVALID:(InstructionSet)index;

	return instrCode;
}

char HaveParenthesis(char *str){
	unsigned char leftParenthesis = 0;
	unsigned char rightParenthesis = 0;

	int len = strlen(str);

	for(int i = 0; i < len; i++){
		if(str[i] == '('){
			leftParenthesis = 1; 
		}else if(str[i] == ')'){
			rightParenthesis = 1;
		}
	}

	return (leftParenthesis & rightParenthesis);
}

char IsMemoryAddress(int token, char *str)
{
	if(token == TYPE_TOKEN_INT){
		return True;
	}

	if(token == TYPE_TOKEN_INDENT){
		return True;
	}

	return HaveParenthesis(str);
}

char IsSibOrOtherMemory(int token, char *str)
{
	if(!HaveParenthesis(str)){
		return False;
	}

	// 根据括号中的逗号来识别是不是SIB。	
	unsigned char commaCount = 0;

	int len = strlen(str);

	for(int i = 0; i < len; i++){
		if(str[i] == ','){
			commaCount++;
		}
	}

	// 一定不能使用2和1这种值。对这种值，有个术语，叫啥？我想不起来了。
	// 这种判断成立的条件是SIB一定是 0x7234(%ebx,%eax,2) 这种格式。
	if(commaCount == 2)	return 2;

	if(commaCount == 0)	return 1;
}

InstructionType GetInstructionType(InstructionSet instrCode)
{
	InstructionType type = (InstructionType)MALLOC(sizeof(struct instructionType));
	// FPU指令
	if(I_FCHS <= instrCode && instrCode <= I_FMULL){
		type->fpuType = FPU;
	}else{
		type->fpuType = NOT_FPU;
		// FPU指令
		if(I_CDQ <= instrCode && instrCode <= I_RET){
			// 无操作数指令
			type->oprandCount = Zero;
		}else if(I_MULL <= instrCode && instrCode <= I_CALL){
			// 单操作数指令
			type->oprandCount = One;
		}else{
			// 双操作数指令
			type->oprandCount = Two;
		}
	}

	return type;
}

RegInfo FindRegIndex(char *regName)
{
	RegInfo reg = (RegInfo)MALLOC(sizeof(struct regInfo));

	for(int i = 0; i < 8; i++){
		char *reg8 = registers[i].reg8; 
		char *reg16 = registers[i].reg16; 
		char *reg32 = registers[i].reg32; 
		// 分成三个if语句是因为我不想写一个条件判断太长的语句。
		if(strcmp(regName, reg8) == 0){
			reg->index = i;
			reg->size = 8;
			reg->name = regName;
			return reg;
		}

		if(strcmp(regName, reg16) == 0){
			reg->index = i;
			reg->size = 16;
			reg->name = regName;
			return reg;
		}

		if(strcmp(regName, reg32) == 0){
			reg->index = i;
			reg->size = 32;
			reg->name = regName;
			return reg;
		}
	}

	return NULL;
}
