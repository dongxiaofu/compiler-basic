#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf.h"
#include "assembler.h"

Heap CurrentHeap;
struct heap ProgramHeap;
HEAP(ProgramHeap);

char **sourceCode;
Lexer lexer;
unsigned int instrStreamIndex;
Label currentLabel;
FILE *fp;

int GetDataTypeSize(int variableDataTypeToken)
{
	int length = 0;
	if(variableDataTypeToken == TYPE_TOKEN_BYTE){
		length = sizeof(char);
	}else if(variableDataTypeToken == TYPE_TOKEN_LONG){
		length = sizeof(int);
	}


	return length;
}

void GetInstrByMnemonic(char *mnemonic, InstrLookup *instrPtr)
{
	for(int i = 0; i < MAX_INSTR_NUM; i++){
		if(strcmp(mnemonic, InstrTable[i].mnemonic) == 0){
			*instrPtr = InstrTable[i];
			break;
		}
	}
}

int AddInstr(char *mnemonic, int opcode, int oprandNum)
{
	static int index = 0;

	strcpy(InstrTable[index].mnemonic, mnemonic);
	InstrTable[index].opcode = opcode;
	InstrTable[index].oprandNum = oprandNum;

	int currentIndex = index;

	index++;
	
	return currentIndex;
}

void SetInstrOprandType(int instrIndex, int operandIndex, OpType type)
{
	InstrTable[instrIndex].oprand[operandIndex] = type;
}

void InitInstrTable()
{
	int index;
	// Addl
	index = AddInstr("addl", OPCODE_ADD, 2);
	SetInstrOprandType(index, 0, OP_FLAG_TYPE_INT | OP_FLAG_TYPE_MEM_REF | OP_FLAG_TYPE_REG); 
	SetInstrOprandType(index, 1, OP_FLAG_TYPE_MEM_REF | OP_FLAG_TYPE_REG); 
	// Subl
	index = AddInstr("subl", OPCODE_SUBL, 2);
	SetInstrOprandType(index, 0, OP_FLAG_TYPE_INT | OP_FLAG_TYPE_MEM_REF | OP_FLAG_TYPE_REG); 
	SetInstrOprandType(index, 1, OP_FLAG_TYPE_MEM_REF | OP_FLAG_TYPE_REG); 
	// pushl
	index = AddInstr("pushl", OPCODE_PUSHL, 1);
	SetInstrOprandType(index, 0, OP_FLAG_TYPE_INT | OP_FLAG_TYPE_MEM_REF | OP_FLAG_TYPE_REG); 
	// popl
	index = AddInstr("popl", OPCODE_POPL, 1);
	SetInstrOprandType(index, 0, OP_FLAG_TYPE_MEM_REF | OP_FLAG_TYPE_REG); 

	// Movl
	index = AddInstr("movl", OPCODE_MOVL, 2);
	SetInstrOprandType(index, 0, OP_FLAG_TYPE_INT | OP_FLAG_TYPE_MEM_REF | OP_FLAG_TYPE_REG); 
	SetInstrOprandType(index, 1, OP_FLAG_TYPE_MEM_REF | OP_FLAG_TYPE_REG); 
	
	index = AddInstr("jmp", OPCODE_JMP, 1);
	SetInstrOprandType(index, 0, OP_FLAG_TYPE_LINE_LABEL);
	
	index = AddInstr("call", OPCODE_CALL, 1);
	SetInstrOprandType(index, 0, OP_FLAG_TYPE_FUNC_NAME);

	index = AddInstr("ret", OPCODE_RET, 0);
//	SetInstrOprandType(index, 0, OP_FLAG_TYPE_FUNC_NAME);
}

void Init()
{
	instrStreamIndex = 0;
	StringTable = (LinkedList)MALLOC(sizeof(struct _LinkedList));
	SymbolTable = (LinkedList)MALLOC(sizeof(struct _LinkedList));
	LineLabelTable = (LinkedList)MALLOC(sizeof(struct _LinkedList));
	FunctionTable = (LinkedList)MALLOC(sizeof(struct _LinkedList));
	VariableTable = (LinkedList)MALLOC(sizeof(struct _LinkedList));

	// 是否要像这样初始化数组？不初始化，我担心数组元素是奇怪的初始值。
//	memset(InstrTable, 0, sizeof(InstrTable));
	lexer->currentFuncIndex = NO_FUNCTION;

	currentLabel = NULL;

	InitInstrTable();

	// 初始化文件头。
	gScriptHeader = (ScriptHeader)MALLOC(sizeof(struct _ScriptHeader));
	strcpy(gScriptHeader->ID, "CEXE");
	gScriptHeader->majorVersion = 0;
	gScriptHeader->minorVerson = 1;
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
	int len = strlen(str);
	for(int i = 0; i < len; i++){
		if(!(IsCharNumeric(str[i]) == True || str[i] == '-')){
			return False;
		}
	}

	for(int i = 1; i < len; i++){
		if(str[i] == '-')	return False;
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

int SkipToNewline()
{
	lexer->currentLine++;
	lexer->index0 = 0;
	if(lexer->currentLine > lexer->lineNum) return END_OF_FILE;

	return lexer->currentLine;
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

int AddNode(LinkedList table, void *pData)
{
	ListNode node = (ListNode)MALLOC(sizeof(struct _ListNode));
	node->pData = pData;

	if(table->pHead == NULL){
		table->pHead = node;
		table->pTail = node;
	}else{
		table->pTail->next = node;
		table->pTail = node;
	}	

	table->num++;

	return (table->num - 1);
}

int AddString(char *name, char *val)
{
	String str = (String)MALLOC(sizeof(struct _String));
	strcpy(str->name, name);
	strcpy(str->val, val);

	int index = AddNode(StringTable, (void *)str);
	str->index = index;

	return index;
}

int AddVariable(char *name, int address)
{
	Variable var = (Variable)MALLOC(sizeof(struct _Variable));
	strcpy(var->name, name);
	var->address = address;

	int index = AddNode(VariableTable, (void *)var);
	var->index = index;

	return index;
}

String GetString(char *name)
{
	ListNode node = StringTable->pHead;
	int num = StringTable->num;

	String str;
	for(int i = 0; i < num; i++){
		str = (String)node->pData;
		if(strcmp(name, str->name) == 0) return str;
		
		node = node->next;
	}

	return NULL;
}

Variable GetVariable(char *name)
{
	ListNode node = VariableTable->pHead;
	int num = VariableTable->num;

	Variable var;
	for(int i = 0; i < num; i++){
		var = (Variable)node->pData;
		if(strcmp(name, var->name) == 0)	return var;	
		node = node->next;
	}	

	return NULL;
}

Function GetFunction(char *name)
{
	int num = FunctionTable->num;
	if(num == 0)	return NULL;

	ListNode funcNode = FunctionTable->pHead;
	for(int i = 0; i < num; i++){
		Function func = (Function)funcNode->pData;
		if(strcmp(func->name, name) == 0){
			return func;
		}	

		funcNode = funcNode->next;
	}

	return NULL;
}

Function GetFunctionByIndex(int index)
{
	Function func = NULL;
	ListNode node = FunctionTable->pHead;

	for(int i = 0; i <= index; i++){
		func = (Function)node->pData;
		node = node->next;
	}

	return func;
}

int AddFunction(char *name)
{
	Function func = (Function)MALLOC(sizeof(struct _Function));
	func->entryPoint = INVALID_INSTR_STREAM_INDEX;
	strcpy(func->name, name);

	int index = AddNode(FunctionTable, (void *)func);
	func->index = index;

	return index;
}

Label GetLabel(char *name)
{
	int num = LineLabelTable->num;
	if(num == 0)	return NULL;

	ListNode labelNode = LineLabelTable->pHead;
	for(int i = 0; i < num; i++){
		Label label = (Label)labelNode->pData;
		if(strcmp(label->name, name) == 0){
			return label;
		}

		labelNode = labelNode->next;
	}	

	return NULL;
}

Label GetLabelByIndex(int index)
{
	int num = LineLabelTable->num;
	if(num == 0)	return NULL;

	ListNode labelNode = LineLabelTable->pHead;
	for(int i = 0; i < num; i++){
		Label label = (Label)labelNode->pData;
		if(index == i){
			return label;
		}

		labelNode = labelNode->next;
	}	

	return NULL;
}

int AddLabel(char *name, int functionIndex, int targetIndex)
{
	Label label = (Label)MALLOC(sizeof(struct _Label));
	strcpy(label->name, name);
	label->functionIndex = functionIndex;
	label->targetIndex = targetIndex;

	int index = AddNode(LineLabelTable, (void *)label);
	label->index = index;

	return index;
}

int AddSymbol(char *name, int isString, int dataType, SymbolVal val)
{
	Symbol symbol = (Symbol)MALLOC(sizeof(struct _Symbol));
	strcpy(symbol->name, name);
	symbol->isString = isString;
	symbol->dataType = dataType;
	symbol->val = val;

	int index = AddNode(SymbolTable, (void *)symbol);

	return index;
} 

Symbol GetSymbol(char *name)
{
	ListNode node = SymbolTable->pHead;
	int num = SymbolTable->num;

	Symbol sym;
	for(int i = 0; i < num; i++){
		sym = (Symbol)node->pData;
		if(strcmp(sym->name, name) == 0)	return sym;

		node = node->next;
	}
	
	return NULL;
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

	// 移动到后面去了。
	// 检查是否要换行。
	// if(lexer->index0 == length - 1){
//	if(lexer->index0 >= length){
//		int line = SkipToNewline();
//		if(line == END_OF_FILE){
//			printf("read file over\n");
//			return TYPE_TOKEN_INVALID;
////			exit(-1);
//		}
//	}

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
		lexer->lexeme[count++] = sourceCode[lexer->currentLine][i];
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
	// TODO 不确定这一行代码有没有问题。
//	if(strcmp(lexer->lexeme, 0) == 0)	return NULL;
	if(lexer->lexeme[0] == 0)	return NULL;
	char *name = (char *)MALLOC(sizeof(char) * MAX_SIZE);
	strcpy(name,  lexer->lexeme);
	return name;
}

int GetImmediateVal(char *immediateOprand)
{
	int len = strlen(immediateOprand);
	if(len == 0)	return 0;
	if(immediateOprand[0] != '$')	return 0;

	char *ptr = immediateOprand + 1;
	int val = atoi(ptr);

	return val;	
}

int GetRegIndex(char *regName)
{
	char *regs[8];

	regs[REG_EAX] = "%eax";
	regs[REG_EBX] = "%ebx";
	regs[REG_ECX] = "%ecx";
	regs[REG_EDX] = "%edx";
	regs[REG_ESI] = "%esi";
	regs[REG_EDI] = "%edi";
	regs[REG_ESP] = "%esp";
	regs[REG_EBP] = "%ebp";

	for(int i = 0; i < 8; i++){
		if(strcmp(regs[i], regName) == 0)	return i;
	}

	return REG_INVALID;
}

void RestLexer()
{
	lexer->currentFuncIndex = 0;
	lexer->currentLine = 1;
	lexer->index0 = lexer->index1 = 0;
//	lexer->lineNum = 0;
	lexer->string_state = STATE_NO_STRING;
	lexer->function_state = STATE_OUT_FUNCTION;
	lexer->currentFuncIndex = NO_FUNCTION;
}


void FindFunctionOrLabel()
{
	int token;
	char ch;
	char label[MAX_SIZE];
	while(True){
		token = GetNextToken();
		if(token == TYPE_TOKEN_INVALID)	break;
		strcpy(label, lexer->lexeme);
		if(token == END_OF_FILE)	break;
		
		ch = GetLookAheadChar();
		// TODO 不明白，前面对token的判断为什么没有阻止程序执行到这里。
		if(ch == END_OF_FILE)	break; 
		if(ch == ':'){
			GetNextToken();
			ch = GetLookAheadChar();
			if(ch == '\n'){
				// 行标签或函数。
				if(lexer->function_state == STATE_IN_FUNCTION){
					int index = AddLabel(label, lexer->currentFuncIndex, 0);
					// int index = 0;
					printf("label-index = %d, %s\n", index, label);
				}else{
					lexer->function_state = STATE_IN_FUNCTION;
					int index = AddFunction(label);
					// int index = 0;
					lexer->currentFuncIndex = index;
					printf("function-index = %d, %s\n", index, label);
				
					// 识别main函数。
					if(strcmp("main", label) == 0){
						gScriptHeader->hasMainFunction = 1;
						gScriptHeader->mainFunctionIndex = index;
					}
				}

				continue;
			}

			token = GetNextToken();
			// 识别字符串。
			if(token == TYPE_TOKEN_KEYWORD_STRING){
				token = GetNextToken();
				if(token != TYPE_TOKEN_QUOT){
					printf("expect %s\n", "\"");
					exit(-1);
				}
				token = GetNextToken();
				if(token != TYPE_TOKEN_STRING){
					printf("expect a string\n");
					exit(-1);
				}

				int index = AddString(label, lexer->lexeme);
				//	int index = 0;
				printf("index = %d, val = %s\n", index, lexer->lexeme);

				token = GetNextToken();
				if(token != TYPE_TOKEN_QUOT){
					printf("expect %s\n", "\"");
					exit(-1);
				}
			}else if(token == TYPE_TOKEN_LONG || token == TYPE_TOKEN_BYTE){
				// 识别全局变量。
				int isString = 0;
				token = GetNextToken();
				SymbolVal symbolVal = (SymbolVal)MALLOC(sizeof(union _SymbolVal));
				int dataType = DATA_TYPE_INVALID;
				if(token == TYPE_TOKEN_INT){
					dataType = DATA_TYPE_INT;
					symbolVal->val = atoi(lexer->lexeme);
				}else if(token == TYPE_TOKEN_INDENT){
					isString = 1;
					dataType = DATA_TYPE_INT;	
					char *variableName = GetCurrentTokenLexeme();
					int indexOfTable = -1;
					
					String str = GetString(variableName);
					if(str == NULL){
						Symbol sym = GetSymbol(variableName);
						isString = 0;
						if(sym != NULL){
							indexOfTable = sym->index;
						}

						// 这段检查代码虽然冗余，但从逻辑上更好理解。
						// 直观，没有转折。
						if(indexOfTable == -1){
							printf("undefined variable\n");
							exit(-1);
						}
					}else{
						indexOfTable = str->index;
						if(indexOfTable == -1){
							printf("undefined string\n");
							exit(-1);
						}
					}

					symbolVal->val = indexOfTable; 
				}else{
					printf("Invalid data type\n");
					exit(-1);
				}
				int index = AddSymbol(label, isString, dataType, symbolVal);
			//		int index = 0;
				printf("variable name = %s, index = %d, val = %s\n", label, index, lexer->lexeme);
			}else{
				// 行标签或函数。
			}

		}else{
			// 指令。
			if(strcmp("ret", lexer->lexeme) == 0){
				lexer->function_state = STATE_OUT_FUNCTION;
			}
		}
	}
}

int CheckTokenType(int token)
{
	if(token == TYPE_TOKEN_INVALID){
		return TOKEN_INVALID;
	}

	char *name = GetCurrentTokenLexeme();
	if(name == NULL){ 
		return TOKEN_INVALID;
	}

	Function func = GetFunction(name);
	if(func != NULL){
		return TOKEN_FUNC;
	}

	Label label = GetLabel(name);
	if(label != NULL){
		return TOKEN_LABEL;
	}

	InstrLookup instrLookup;
	memset(&instrLookup, 0, sizeof(InstrLookup));
	instrLookup.opcode = -1;
	GetInstrByMnemonic(name, &instrLookup);
	if(instrLookup.opcode -= -1){
		return TOKEN_INSTR;
	}

	return TOKEN_IDENT;
}

void DealWithOprandImmediate(InstrLookup instrLookup, Op opList, char *oprandLexeme, int opIndex)
{
	int i = opIndex;
	OpType opType = instrLookup.oprand[i];

	if(!(opType & OP_FLAG_TYPE_INT))	return;

	int num = GetImmediateVal(oprandLexeme);
	opList[i].iType = OP_TYPE_INT;
	opList[i].iIntLiteral = num;
}

void DealWithOprandRegister(InstrLookup instrLookup, Op opList, char *oprandLexeme, int opIndex)
{
	OpType opType = instrLookup.oprand[opIndex];

	if((opType & OP_FLAG_TYPE_REG) == 0) return;

    int regIndex = GetRegIndex(oprandLexeme);
    if(regIndex == REG_INVALID){
        printf("invalid register\n");
        exit(-1);
    }

    opList[opIndex].iType = OP_TYPE_REG;
    opList[opIndex].iReg = regIndex;
}

void DealWithOprandIndent(InstrLookup instrLookup, Op opList, char *oprandLexeme, int opIndex)
{
	OpType opType = instrLookup.oprand[opIndex];
	// TODO 懒得把下面的i修改成opIndex。
	int i = opIndex;
	Function func = GetFunction(oprandLexeme);
	if(func != NULL) {
	    if (opType & OP_FLAG_TYPE_FUNC_NAME) {
	        opList[i].iType = OP_TYPE_FUNC_INDEX;
	        opList[i].iFuncIndex = func->index;
	    }

		return;
	}
	
	// 行标签
	Label label = GetLabel(oprandLexeme);
	if(label != NULL) {
	    if(opType & OP_FLAG_TYPE_LINE_LABEL) {
	        if(label == NULL) {
	            printf("%s label undefined\n", oprandLexeme);
	            exit(-1);
	        }
	        opList[i].iType = OP_TYPE_LABEL_INDEX;
	        opList[i].iLabelIndex = label-> index;
	    }

		return;
	}
	
	if(func == NULL && label == NULL) {
	    printf("oprandLexeme = %s\n", oprandLexeme);
	    // TODO 操作数是一个identifier，但不是函数也不是标签，会是字符串吗？
	    // 可能是全局变量。
	    // 找了很久才找到这个地方，在这里处理全局变量。
	    opList[i].iType = OP_TYPE_MEM;
	    int length = strlen(oprandLexeme);
	    opList[i].iMemory.length = length;
	    // opList[i].iMemory.length = length - 1;
	    opList[i].iMemory.name = oprandLexeme;
		
		return;
	}
}

// TODO 这个函数并不是处理Int数据。先这样吧。
// 它处理-4(%ebp)这样的操作数。
void DealWithOprandInt(char * oprandLexeme, Op opList, int opIndex)
{
	char ch;
	int iOffsetIndex = atoi(oprandLexeme);
	ch = GetLookAheadChar();
	if(ch != '(') {
	    printf("need a (\n");
	    exit(-1);
	}
	GetNextToken(); // 跳过(
	int token = GetNextToken();
	if(token != TYPE_TOKEN_REGISTER) {
	    printf("need a register\n");
	    exit(-1);
	}
	//						oprandLexeme = GetCurrentTokenLexeme();
	strcpy(oprandLexeme, GetCurrentTokenLexeme());
	ch = GetLookAheadChar();
	if(ch != ')') {
	    printf("need a (\n");
	    exit(-1);
	}
	GetNextToken(); // 跳过)
	
	opList[opIndex].iType = OP_TYPE_REG_MEM;
	opList[opIndex].iReg = GetRegIndex(oprandLexeme);
	opList[opIndex].iOffsetIndex = iOffsetIndex;
}

void DealWithOprand(InstrLookup instrLookup)
{
	// TODO instrLookup是非法数据怎么办？
	int oprandNum = instrLookup.oprandNum;
	InstrStream[instrStreamIndex].opcode = instrLookup.opcode;
	InstrStream[instrStreamIndex].oprandNum = oprandNum;
	InstrStream[instrStreamIndex].opList = (Op) MALLOC(oprandNum * sizeof(struct _Op));
	Op opList = InstrStream[instrStreamIndex].opList;
	Op oprand;
	OpType opType;
	for (int i = 0; i < oprandNum; i++) {
		int token = GetNextToken();
		char *oprandLexeme = GetCurrentTokenLexeme();
		
		switch(token){
			case TYPE_TOKEN_IMMEDIATE:
				DealWithOprandImmediate(instrLookup, opList, oprandLexeme, i);
				break;
			case TYPE_TOKEN_REGISTER:
				DealWithOprandRegister(instrLookup, opList, oprandLexeme, i);
				break;
			case TYPE_TOKEN_INDENT:
				DealWithOprandIndent(instrLookup, opList, oprandLexeme, i);
				break;
			case TYPE_TOKEN_INT:
				DealWithOprandInt(oprandLexeme, opList, i);
				break;
		}
	}
}

void DealWithInstruction()
{
	int token = TYPE_TOKEN_INVALID;

	while(True){
		token = GetNextToken();

		if(token == TYPE_TOKEN_INVALID) break;

		char *name = GetCurrentTokenLexeme();

		Function func = GetFunction(name);
		if(func != NULL){
        	printf("deal with func:%s\n", name);
        	lexer->currentFuncIndex = func->index;

			return;
        }

		Label label = GetLabel(name);
		if(label != NULL){
			printf("deal with label:%s\n", name);
			currentLabel = label;
			currentLabel->targetIndex = INVALID_INSTR_STREAM_INDEX;

			return;
        }

		// 处理指令。
		InstrLookup instrLookup;
		memset(&instrLookup, 0, sizeof(InstrLookup));
		instrLookup.opcode = -1;
		GetInstrByMnemonic(name, & instrLookup);
		if(instrLookup.opcode != -1){
		    printf("deal with instr:%s\n", name);
		
		    // 吃两个不知道名字的红色水果时想到这个方法。
		    // 这没有什么领域知识，见招拆招而已，具体问题具体分析。
		    if(currentLabel != NULL && currentLabel-> targetIndex == INVALID_INSTR_STREAM_INDEX) {
		        currentLabel->targetIndex = instrStreamIndex;
		    }
		
		    if(lexer->currentFuncIndex == NO_FUNCTION){
		        printf("instr should be in a function\n");
		        exit(-1);
		    }
		
		    Function currentFunction = GetFunctionByIndex(lexer->currentFuncIndex);
		    if(currentFunction != NULL &&
		        currentFunction->entryPoint == INVALID_INSTR_STREAM_INDEX) {
		        currentFunction->entryPoint = instrStreamIndex;
		    }

			DealWithOprand(instrLookup);
		}
	}	
}


void AssembleInstruction()
{
	int isOver = 0;
	while(True){
		int token = GetNextToken();
		printf("token = %d\n", token);
		if(token == TYPE_TOKEN_INVALID)	break; 
		char *name = GetCurrentTokenLexeme();
		printf("name = %s\n", name);
		int tokenType = CheckTokenType(token);
		printf("tokenType = %d\n", tokenType);
	}

	return;

	while(True){
		int token = GetNextToken();
		char *name = GetCurrentTokenLexeme();
		printf("name = %s\n", name);
		int tokenType = CheckTokenType(token);
		printf("tokenType = %d\n", tokenType);
		switch(tokenType){
			case TOKEN_INVALID:
				{
					isOver = 1;
					break;
				}
			case TOKEN_FUNC:
				{
					printf("deal with func:%s\n", name);
					Function func = GetFunction(name);
					lexer->currentFuncIndex = func->index;
					break;
				}
			case TOKEN_LABEL:
				{
					printf("deal with label:%s\n", name);
					currentLabel = GetLabel(name);
					currentLabel->targetIndex = INVALID_INSTR_STREAM_INDEX;
					break;
				}
			case TOKEN_INSTR:
				{
					DealWithInstruction();
					break;
				}
			case TOKEN_IDENT:
				{
					// do nothing
					break;
				}
			default:
				{
					printf("there is an error in %d\n", __LINE__);
					isOver = 1;
					break;
				}
		}

		if(isOver == 1)	break;
	}
}

void ReplaceLabelWithInstrStreamIndex()
{
	// 把指令流中的标签索引换成标签的targetIndex。
	// 我想不到更好的方法，先这样做吧。
	for(int i = 0; i < instrStreamIndex; i++) {
	    Instr instr = InstrStream[i];
	    int oprandNum = instr.oprandNum;
	    for(int j = 0; j < oprandNum; j++) {
	        Op oprand = &(instr.opList[j]);
	        if(oprand->iType != OP_TYPE_LABEL_INDEX)	continue;

			int index = oprand->iLabelIndex;
			Label label = GetLabelByIndex(index);
			if(label != NULL) {
			    int targetIndex = label->targetIndex;
			    if(targetIndex == INVALID_INSTR_STREAM_INDEX){
			        printf("label %s point to invalid instruction\n", label->name);
			        exit(-1);
			    }
			    oprand->iInstrIndex = targetIndex;
			    oprand->iType = OP_TYPE_INSTR_INDEX;
			}else{
			    printf("invalid label\n");
			    exit(-1);
			}
	    }
	}
}

void AssmblSourceFile()
{
	// 遍历源代码，找出所有的函数、标签。
	FindFunctionOrLabel();
	// 重置词法分析器
    RestLexer();
	// 处理指令。
	AssembleInstruction();
	// 把指令中的函数名、标签名换成它们对应的指令索引。
	ReplaceLabelWithInstrStreamIndex();
	gScriptHeader->instrNum = instrStreamIndex;
}

void SaveVariable()
{
	// 第三次遍历
	printf("the third loop start\n");
	RestLexer();

	int token;
	int address = 0;
	int size = 0;
	int length = 0;
	char *variableName;
	char *variableValue;
	char *variableDataTypeName;
	int	variableDataTypeToken;
	while(True){
		length = 0;
		token = GetNextTokenExceptNewLine();

		if(token == TYPE_TOKEN_TEXT)	break;
		if(token == TYPE_TOKEN_DATA){
			int nextToken = GetLookAheadToken();
			if(nextToken == TYPE_TOKEN_GLOBL || nextToken == TYPE_TOKEN_ALIGN){
				printf("no string\n");
				continue;
			}
parse_string:
			token = GetNextTokenExceptNewLine();		// .str0
			variableName = GetCurrentTokenLexeme();
			token = GetNextTokenExceptNewLine();		// :
			token = GetNextTokenExceptNewLine();	   //  .string
			variableDataTypeName = GetCurrentTokenLexeme();		// 获取数据类型的字符串形式
			variableDataTypeToken = token;						// 获取数据类型的token
			// 获取字符串的值
			if(token == TYPE_TOKEN_KEYWORD_STRING){
				token = GetNextTokenExceptNewLine();        // "
				token = GetNextTokenExceptNewLine();        // 字符串的值
				variableValue = GetCurrentTokenLexeme();
				token = GetNextTokenExceptNewLine();        // "
				length = strlen(variableValue);

				int n = fwrite(variableValue, length, 1, fp);
				size += n * length;
			}else{
				token = GetNextTokenExceptNewLine();	   
				variableValue = GetCurrentTokenLexeme();
				length = GetDataTypeSize(variableDataTypeToken);

				// TODO 我知道这里是相同的冗余代码。我不确定这两种数据类型的处理方式是不是一样，
				// 暂时先这样写。等以后能确认二者的处理方式完全相同，再修改这段代码。
				// 这种半成品，如果被某个自作聪明的人看到，一定会以为他看到了很愚蠢的错误。
				// if(token == TYPE_TOKEN_BYTE){
				if(variableDataTypeToken == TYPE_TOKEN_BYTE){
					char value = atoi(variableValue);	
					int n = fwrite(&value, length, 1, fp);
					size += n * length;
				// }else if(token == TYPE_TOKEN_LONG){
				}else if(variableDataTypeToken == TYPE_TOKEN_LONG){
					int value = atoi(variableValue);
					int n = fwrite(&value, length, 1, fp);
					size += n * length;
				}
			}

			AddVariable(variableName, address);
		//	address += length;

			printf("variableName = %s, variableValue = %s, dataType = %s, address = %d\n", variableName, variableValue, variableDataTypeName, address);

			address += length;

			token = GetLookAheadToken();
			if(token == TYPE_TOKEN_INDENT)	goto parse_string;

		}else if(token == TYPE_TOKEN_GLOBL){
			// 解析全局变量。
			token = GetNextTokenExceptNewLine();		// num
			token = GetNextTokenExceptNewLine();		// num
			variableName = GetCurrentTokenLexeme();
			token = GetNextTokenExceptNewLine();		// :
			token = GetNextTokenExceptNewLine();		// .long
			variableDataTypeName = GetCurrentTokenLexeme();		// 获取数据类型的字符串形式
			variableDataTypeToken = token;						// 获取数据类型的token
			token = GetNextTokenExceptNewLine();		// 5
			variableValue = GetCurrentTokenLexeme();
			// 变量值的token
			int variableValueToken = token;

			length = GetDataTypeSize(variableDataTypeToken);
//			address += length;
//
			AddVariable(variableName, address);

			printf("variableName = %s, variableValue = %s, dataType = %s, address = %d\n", variableName, variableValue, variableDataTypeName, address);

			address += length;

			if(variableValueToken == TYPE_TOKEN_INDENT){
				// 把变量名换算成内存地址
				// 如果找不到这个变量，怎么处理？先报错。
				Variable var = GetVariable(variableValue);
				if(var == NULL){
					printf("undefined variable:%s\n", variableValue);
					exit(-1);
				}
				int n = fwrite(&(var->address), sizeof(int), 1, fp);
				size += n * sizeof(n);
			}else{
				// if(token == TYPE_TOKEN_BYTE){
				if(variableDataTypeToken == TYPE_TOKEN_BYTE){
					char value = atoi(variableValue);	
					int n = fwrite(&value, length, 1, fp);
					size += n * length;
				// }else if(token == TYPE_TOKEN_LONG){
				}else if(variableDataTypeToken == TYPE_TOKEN_LONG){
					int value = atoi(variableValue);
					int n = fwrite(&value, length, 1, fp);
					size += n * length;
				}
			}

			while(1){
				token = GetLookAheadToken();
				if(!(token == TYPE_TOKEN_LONG || token == TYPE_TOKEN_BYTE))	break;
				token = GetNextTokenExceptNewLine();		// .long
				variableDataTypeName = GetCurrentTokenLexeme();		// 获取数据类型的字符串形式
				variableDataTypeToken = token;						// 获取数据类型的token

				length = GetDataTypeSize(variableDataTypeToken);
//				address += length;

				token = GetNextTokenExceptNewLine();		// 5
				variableName = GetCurrentTokenLexeme();
				variableValueToken = token;
				printf("variableValue1 = %s, dataType = %s\n", variableValue, variableDataTypeName);

				if(variableValueToken == TYPE_TOKEN_INDENT){
					// 把变量名换算成内存地址
					// 如果找不到这个变量，怎么处理？先报错。
					Variable var = GetVariable(variableName);
					if(var == NULL){
						printf("undefined variable:%s\n", variableName);
						exit(-1);
					}
					int n = fwrite(&(var->address), sizeof(int), 1, fp);
					size += n * sizeof(int);
					length = sizeof(int);
				}else{
				//	if(token == TYPE_TOKEN_BYTE){
					if(variableDataTypeToken == TYPE_TOKEN_BYTE){
						char value = atoi(variableValue);	
						int n = fwrite(&value, length, 1, fp);
						size += n * length;
					// }else if(token == TYPE_TOKEN_LONG){
					}else if(variableDataTypeToken == TYPE_TOKEN_LONG){
						int value = atoi(variableValue);
						int n = fwrite(&value, length, 1, fp);
						size += n * length;
					}
				}

				address += length;
			}
		}
	}

	printf("size1 = %d\n", size);
	// 存储另一个变量表
	int variableNum = VariableTable->num;
	fwrite(&variableNum, sizeof(int), 1, fp);
	ListNode variableNode = VariableTable->pHead;
	Variable variable;
	int nameLength;
	for(int i = 0; i < variableNum; i++){
		variable = (Variable)(variableNode->pData);
		// 写入变量名的长度
		nameLength = strlen(variable->name);
		fwrite(&nameLength, sizeof(int), 1, fp);
		// 写入变量名
		fwrite(variable->name, nameLength, 1, fp);
		// 写入内存地址
		fwrite(&variable->address, sizeof(int), 1, fp);
	
		variableNode = variableNode->next;
	} 

	// 重写数据大小到文件中
	rewind(fp);
	fwrite(&address, sizeof(int), 1, fp);

	printf("variableNum = %d\n", variableNum);
	printf("address = %d\n", address);
}

void BuildXE()
{
	// FILE *fp = fopen("test.cxe", "rb");
	fp = fopen("test.cxe", "wb");
	if(fp == NULL){
		printf("open test.cxe failure\n");
		exit(-1);
	}

	gScriptHeader->size = 0;
	fwrite(&(gScriptHeader->size), 4, 1, fp);
	// 写入文件ID
	// fwrite(&(gScriptHeader->ID), strlen(gScriptHeader->ID), 1, fp);
	fwrite(&(gScriptHeader->ID), 4, 1, fp);
	// 写入文件版本号
	fwrite(&(gScriptHeader->majorVersion), 4, 1, fp);
	fwrite(&(gScriptHeader->minorVerson), 4, 1, fp);
	// 这个文件是否包含main函数
	fwrite(&(gScriptHeader->hasMainFunction), 4, 1, fp);
	fwrite(&(gScriptHeader->mainFunctionIndex), 4, 1, fp);
	// 指令的数量
	fwrite(&(gScriptHeader->instrNum), 4, 1, fp);

	// 写入指令流
//	int instrNum = InstrStream;
	for(int i = 0; i < instrStreamIndex; i++){
		Instr instr = InstrStream[i];
		// 写入操作码
		fwrite(&instr.opcode, sizeof(int), 1, fp);
		// 写入操作数的数量
		fwrite(&instr.oprandNum, sizeof(int), 1, fp);
		// 写入操作数
		int oprandNum = instr.oprandNum;
		Op opList = instr.opList;

		/*******************换算堆栈start*****************************/
		if(instr.opcode == OPCODE_ADD || instr.opcode == OPCODE_SUBL){
			Op src = &opList[0];
			Op dst = &opList[1];
			if(dst->iType == OP_TYPE_REG && (dst->iReg == REG_ESP || dst->iReg == REG_EBP)){
				if(src->iType == OP_TYPE_INT){
					int srcValue = src->iIntLiteral;
					// 养成不使用硬编码的习惯。
					int stackOffset = srcValue / STACK_SCALE_VALUE;
					src->iIntLiteral = stackOffset;
				}
			}
		} 
		/*******************换算堆栈end*****************************/

		Op oprand;
		for(int i = 0; i < oprandNum; i++){
			oprand = &opList[i];
			switch(oprand->iType){
				case OP_TYPE_INT:
					fwrite(&oprand->iType, sizeof(int), 1, fp);
					fwrite(&oprand->iIntLiteral, sizeof(int), 1, fp);	
					break;
				case OP_TYPE_FLOAT:
					fwrite(&oprand->iType, sizeof(int), 1, fp);
					fwrite(&oprand->fFloatLiteral, sizeof(float), 1, fp);	
					break;
				case OP_TYPE_STRING_INDEX:
					fwrite(&oprand->iType, sizeof(int), 1, fp);
					fwrite(&oprand->iStringTableIndex, sizeof(int), 1, fp);	
					break;
				case OP_TYPE_ABS_STACK_INDEX:
				
					break;
				case OP_TYPE_REL_STACK_INDEX:
				
					break;
				case OP_TYPE_INSTR_INDEX:
					fwrite(&oprand->iType, sizeof(int), 1, fp);
					fwrite(&oprand->iInstrIndex, sizeof(int), 1, fp);	
					break;
				case OP_TYPE_FUNC_INDEX:
					fwrite(&oprand->iType, sizeof(int), 1, fp);
					fwrite(&oprand->iFuncIndex, sizeof(int), 1, fp);	
					break;
				case OP_TYPE_REG:
					fwrite(&oprand->iType, sizeof(int), 1, fp);
					fwrite(&oprand->iReg, sizeof(int), 1, fp);	
					break;
				case OP_TYPE_REG_MEM:
					{
						fwrite(&oprand->iType, sizeof(int), 1, fp);
						fwrite(&oprand->iReg, sizeof(int), 1, fp);	
						int stackOffset = oprand->iOffsetIndex;
						// -4(%eax)，不一定是堆栈索引，不能用下面的代码处理。
						// 但我不知道该怎么处理。搁置。
						if(oprand->iReg == REG_ESP || oprand->iReg == REG_EBP){
							stackOffset = stackOffset / STACK_SCALE_VALUE;
						}
						fwrite(&stackOffset, sizeof(int), 1, fp);
						break;			
					}
				case OP_TYPE_MEM:
					{
						fwrite(&oprand->iType, sizeof(int), 1, fp);
						// TODO 最好在解析操作数时，把variableNameLength写到操作数中，以免
						// 在后期造成错误。
						// 什么是错误？以为A是那样的，按照那样使用A，但A实际是这样的。
						// 这本不是难题。然而，隐藏在复杂的系统之中，就成为了耗时几个小时也
						// 难以修复的问题。
//						int variableNameLength = strlen(oprand->variableName);
//						fwrite(&variableNameLength, sizeof(int), 1, fp);
//						fwrite(&oprand->variableNameLength, sizeof(int), 1, fp);
//						fwrite(oprand->variableName, oprand->variableNameLength, 1, fp);
						char *tmp = "num2";
						int len = strlen(tmp);
						int length = oprand->iMemory.length;
						fwrite(&length, sizeof(int), 1, fp);
						fwrite(oprand->iMemory.name, length, 1, fp);
						break;
					}
				default:
					printf("write data failure\n");
					exit(-1);
					break;
			}
		}
	}

	// 写入字符串表
	int strNum = StringTable->num;
	fwrite(&strNum, sizeof(int), 1, fp);
	ListNode strNode = StringTable->pHead;
	String str;
	for(int i = 0; i < strNum; i++){
		str = (String)strNode->pData;
		int length = strlen(str->val);
		// 用4个字节存储字符串的长度。
		fwrite(&length, 4, 1, fp);
		// 写入字符串的内容。
		fwrite(str->val, length, 1, fp);
		strNode = strNode->next;
	}
	// 写入全局变量表
	int symNum = SymbolTable->num;
	// 写入全局变量的数量
	fwrite(&symNum, 4, 1, fp);
	ListNode globlVarNode = SymbolTable->pHead;
	Symbol sym;
	for(int i = 0; i < symNum; i++){
		sym = (Symbol)globlVarNode->pData;
		// 写入isString
		fwrite(&(sym->isString), 4, 1, fp);
		// 写入变量值
		fwrite(&(sym->val->val), 4, 1, fp);

		globlVarNode = globlVarNode->next;
	}
	// 写入函数表
	int funcNum = FunctionTable->num;
	// 写入函数的数量
	fwrite(&funcNum, 4, 1, fp);
	ListNode funcNode = FunctionTable->pHead;
	// 把局部变量从循环中放到循环外面，是不是能减少产生的局部变量？
	Function func;
	int funcNameLength;
	char *funcName;
	for(int i = 0; i < funcNum; i++){
		func = (Function)funcNode->pData;
		// char *funcName = func->name;
		funcName = func->name;
		funcNameLength = strlen(funcName);
		// 写入函数名的长度。没有长度，无法读取函数名。
		fwrite(&funcNameLength, 4, 1, fp);
		// 写入函数名
		fwrite(funcName, funcNameLength, 1, fp);
		// 写入函数索引
		fwrite(&(func->index), 4, 1, fp);
		// 写入函数的entryPoint
		fwrite(&(func->entryPoint), 4, 1, fp);
		funcNode = funcNode->next;
	}

	// 存储另一个变量表和变量数据
	SaveVariable();
}

void LoadFile(char *filename)
{
	FILE *fp = fopen(filename, "rb");
	if(!fp){
		printf("open %s failure\n", filename);
		exit(-1);
	}
	// 统计文件的行数。
	int lineNum = 0;
	while(!feof(fp)){
		if(fgetc(fp) == '\n')	lineNum++;
	}
	lexer->lineNum = lineNum;
	printf("lineNum = %d\n", lineNum);
	fclose(fp);
	// 把每行数据存储到一个数组的元素中。
	sourceCode = (char **)MALLOC(sizeof(char *) * lineNum);

	// 重置文件指针。
//	rewind(fp);
	
//	int i = 0;
//	while(!feof(fp)){
//		sourceCode[i] = (char *)MALLOC(sizeof(char) * MAX_SIZE);
//		fgets(sourceCode[i], MAX_SIZE, fp);
//		i++;
//	}	
	fp = fopen(filename, "r");

	for(int i = 0; i < lineNum; i++){
		sourceCode[i] = (char *)MALLOC(sizeof(char) * MAX_SIZE);
		fgets(sourceCode[i], MAX_SIZE, fp);
		TrimWhitespace(sourceCode[i]);
//		printf("%s",sourceCode[i]);
	}

	fclose(fp);
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

int FindDataEntryIndex(char *name)
{
	int	targetIndex = -1;
	int index = 0;

	while(1){
		if(dataEntryArray[index] == NULL)	break;
		if(strcmp(name, dataEntryArray[index]->name) == 0){
			targetIndex = index;
			break;
		}
		index++;
	}

	return targetIndex;
}

DataEntry FindDataEntry(char *name)
{
	int targetIndex = FindDataEntryIndex(name);
	if(targetIndex == -1){
		return NULL;
	}else{
		return dataEntryArray[targetIndex];
	}
}

int FindStrtabEntryIndex(char *name)
{
	int index = -1;

	for(int i = 0; i < 100; i++){
		StrtabEntry entry = strtabEntryArray[i];
		if(strcmp(entry->name, name) == 0){
			return i;
		}
	}

	return index;
}

StrtabEntry FindStrtabEntry(char *name)
{
	int index = FindStrtabEntryIndex(name);
	
	if(index == -1){
		return NULL;
	}else{
		return strtabEntryArray[index];
	}
}

SectionDataNode FindSectionDataNode(char *name, SectionDataNode head)
{
	SectionDataNode node = head->next;

	while(node != NULL){
		if(strcmp(node->name, name) == 0){
			return node;
		}
		node = node->next;
	}

	return node;
}

// todo 以后再优化。
SectionDataNode FindSymbolSectionDataNode(char *name, SectionDataNode head)
{
	SectionDataNode node = FindSectionDataNode(name, head);

	return node;
}

StrtabEntry FindEntryInStrtabEntryList(char *name)
{
	StrtabEntry node = strtabEntryList;

	while(node != NULL){
		if(strcmp(node->name, name) == 0){
			return node;
		}
		node = node->next;
	}

	return node;
}

int FindIndexInStrtabEntryList(char *name)
{
	StrtabEntry node = FindEntryInStrtabEntryList(name);

	if(node == NULL){
		return -1;
	}else{
		return node->index;
	}
}

int FindShstrtabEntry(char *name)
{
	int index = -1;

	for(int i = 0; i < SHSTRTAB_ENTRY_ARRAY_SIZE; i++){
		char *entry = shstrtabEntryArray[i];
		if(strcmp(entry, name) == 0){
			return i;
		}
	}

	return index;
}

GloblVariableNode FindGloblVariableNode(char *name)
{
	GloblVariableNode node = globlVariableList;

	while(1){
		if(node == NULL)	break;

		printf("name = %s, node->name = %s\n", name, node->name);
		if(strcmp(name, node->name) == 0){
			return node;
		}

		node = node->next;
	}

	return NULL;
}

void AddGloblVariableNode(char *name)
{
	GloblVariableNode node = FindGloblVariableNode(name);
	if(node != NULL){
		return;
	}
	node = (GloblVariableNode)MALLOC(sizeof(struct globlVariableNode));
	strcpy(node->name, name);
	if(globlVariableList == NULL){
		globlVariableList = node;
	}else{
		preGloblVariablenNode->next = node;
	}
	preGloblVariablenNode = node;
}

unsigned char isGloblVariable(char *name)
{
	GloblVariableNode node = FindGloblVariableNode(name);

	if(node == NULL){
		return 0;
	}else{
		return 1;
	}
}

char IsData(int token)
{
	int result;
	switch(token){
		case TYPE_TOKEN_DATA:
		case TYPE_TOKEN_TEXT:
		case TYPE_TOKEN_SECTION:
			result = 1;
			break;
	//	case TYPE_TOKEN_SECTION:
	//		{
	//			result = 1;
	//			GetNextToken();
	//			break;
	//		}
		default:
			result = 0;
	}

	return result;
}

#define MAX_LINE 213

void TestDataEntryArray()
{
	printf("\nstring in dataEntryArray start\n");

	int index = 0;
	while(1){
		DataEntry entry = dataEntryArray[index];
		if(entry == NULL)	break;
		printf("%d---%s\n", index, entry->name);
		index++;
	}

	printf("\nstring in dataEntryArray end\n");
}

void TestStrtabEntryArray()
{
	printf("\nstring in strtab start\n");

	int index = 0;
	while(1){
		StrtabEntry strtabEntry = strtabEntryArray[index];
		if(strtabEntry == NULL)	break;
		printf("%d---%s\n", index, strtabEntry->name);
		index++;
	}

	printf("\nstring in strtab end\n");
}

void TestStrtabEntryList()
{
	printf("\nstring in strtabList start\n");

	StrtabEntry node = strtabEntryList;
	int index = 0;

	while(1){
		if(node == NULL)	break;
		printf("%d--%s\n", index++, node->name);
		node = node->next;
	}

	printf("\nstring in strtabList end\n");
}

void AddStrtabEntry(DataEntry entry)
{
	StrtabEntry strtabEntry = (StrtabEntry)MALLOC(sizeof(struct strtabEntry));
	memset(strtabEntry->name,0,200);
	strcpy(strtabEntry->name, entry->name);
	strtabEntry->length = strlen(strtabEntry->name);
	strtabEntry->offset = strtabEntryOffset;
	strtabEntryOffset += strtabEntry->length;
	strtabEntryArray[strtabEntryArrayIndex++] = strtabEntry;
}

void AddStrtabEntryListNode(StrtabEntry node)
{
//	node->next = NULL;

	if(strtabEntryList == NULL){
		strtabEntryList = node;
	}else{
		preStrtabEntryNode->next = node;
	}
	preStrtabEntryNode = node;
}

void ParseData()
{
	printf("开始处理数据\n");

	int isEnd = 0;
	int token;
	char *name;
	int currentSection = SECTION_TEXT;

	globlVariableList = NULL;

	

	DataEntry entry = (DataEntry)MALLOC(sizeof(struct dataEntry));
	dataEntryArray[dataEntryArrayIndex++] = entry;

	while(1){
		int isGlobl = 0;
		int isData = 0;
		int nextToken;

		token = GetNextToken();
		printf("token = %d\n", token);
		if(token == TYPE_TOKEN_INVALID)	break; 
		char *name = GetCurrentTokenLexeme();
		printf("ParseData name = %s\n", name);

		// 就在这里获取.section
		if(IsData(token)){
			if(token == TYPE_TOKEN_DATA){
	//			entry->section = SECTION_DATA; 
				currentSection = SECTION_DATA;
			}else if(token == TYPE_TOKEN_SECTION){
				nextToken = GetLookAheadToken();
				if(nextToken == TYPE_TOKEN_RODATA){
	//				entry->section = SECTION_RODATA;
					currentSection = SECTION_RODATA;
				}else if(nextToken == TYPE_TOKEN_DATA){
	//				entry->section = SECTION_DATA; 
					currentSection = SECTION_DATA;
				}else if(nextToken == TYPE_TOKEN_TEXT){
					// TODO 是否要在这里记录.text？
				}
				GetNextToken();
			
			}else{
				// TODO 执行到这里，就出错了。
			} 
		}

		// 就在这里处理数据。
		// .size   ch1, 1
		else if(token == TYPE_TOKEN_SIZE){
			// 跳过ch1
			GetNextToken();
			// 跳过,
			GetNextToken();
			token = GetNextToken();
			char *name = GetCurrentTokenLexeme();
			entry->size = atoi(name);
		}

		// .type   ch1, @object
		else if(token == TYPE_TOKEN_TYPE){
			// 跳过ch1
			GetNextToken();
			// 跳过,
			GetNextToken();
			token = GetNextToken();
			int symbolType = -1;
			if(token == TYPE_TOKEN_OBJECT){
				symbolType = SYMBOL_TYPE_OBJECT;
			}else if(token == TYPE_TOKEN_FUNCTION){
				symbolType = SYMBOL_TYPE_FUNC;
			}

			entry->symbolType = symbolType; 
		}

		// .byte   65
		// else if(token == TYPE_TOKEN_BYTE || token == TYPE_TOKEN_LONG || token == TYPE_TOKEN_STRING){
		else if(token == TYPE_TOKEN_BYTE || token == TYPE_TOKEN_LONG || token == TYPE_TOKEN_KEYWORD_STRING){
			// 数组、浮点数的值都是多个十进制数据。
			// 因为只是标志位，取值只需0和1即可，所以我用char。总觉得不符合惯例。
			char isFirstValue = 0;
			DataEntryValueNode head = (DataEntryValueNode)MALLOC(sizeof(struct dataEntryValueNode));
			DataEntryValueNode node = NULL;
			DataEntryValueNode preNode = NULL;
			do{
				if(token == TYPE_TOKEN_BYTE){
					entry->dataType = DATA_TYPE_BYTE;
				}else if(token == TYPE_TOKEN_LONG){
					entry->dataType = DATA_TYPE_LONG;
				}else if(token == TYPE_TOKEN_KEYWORD_STRING){
					entry->dataType = DATA_TYPE_STRING;
				}

				if(node == NULL){
					node = (DataEntryValueNode)MALLOC(sizeof(struct dataEntryValueNode)); 
					head->next = node;
				//	preNode = node;
				}

				preNode = node;
				// "how are you?"
				if(token == TYPE_TOKEN_KEYWORD_STRING){
					// 跳过"
					GetNextToken();
					GetNextToken();
//					token = GetNextToken();
					char *name = GetCurrentTokenLexeme();
					node->val.strVal = name;
					// 跳过"
					GetNextToken();
				}else{
					token = GetNextToken();
					if(token == TYPE_TOKEN_INDENT){
						// 这里就是识别.rel.data的地方。
						// 默认为不需要重定位的数据。如果已经标识了此entry的重定位属性，就无需再标识。
						if(entry->isRel == 0){
							entry->isRel = 1;
						}
						char *name = GetCurrentTokenLexeme();
						node->type = STR;
						node->val.strVal = name;
						// 就在这里把name换算成偏移量。
						// 不行。在这里无法实现。执行CalculateDataEntryOffset后，才知道偏移量。
						// 那么，把name存储起来，在BuildELF中根据name查询entry，然后读取offset。
					}else{
						char *name = GetCurrentTokenLexeme();
						node->type = NUM;
						node->val.numVal = atoi(name);
					}
				}

				node = (DataEntryValueNode)MALLOC(sizeof(struct dataEntryValueNode));
				preNode->next = node;

			// while后面有没有分号？看了资料，有。若被人知道我连这都忘记了，会被认为基础很差。呵呵。
			// 谁没有提笔忘字的时候？
				int nextToken = GetLookAheadToken();
				if(nextToken == TYPE_TOKEN_BYTE || nextToken == TYPE_TOKEN_LONG || nextToken == TYPE_TOKEN_STRING){
					token = GetNextToken();
				}
			}while(token == TYPE_TOKEN_BYTE || token == TYPE_TOKEN_LONG || token == TYPE_TOKEN_STRING);

			preNode->next = NULL;

			entry->valPtr = head->next;
			entry->section = currentSection;

			// 处理完数据的值，这就意味着处理完一条数据。
			entry = (DataEntry)MALLOC(sizeof(struct dataEntry));
			dataEntryArray[dataEntryArrayIndex++] = entry;
		}

		// 获取符号的名称，例如ch1:
		else if(token ==  TYPE_TOKEN_INDENT){
			char ch = GetLookAheadChar();
			if(ch == ':'){
				char *name2 = GetCurrentTokenLexeme();
				printf("name2 = %s\n", name2);
				memset(entry->name, 0, 200);
				char *name = GetCurrentTokenLexeme();
				// strcpy(entry->name, name); 
				GetNextToken();

				// 检查变量名的首字符是不是点号。
				if(entry->symbolType == SYMBOL_TYPE_OBJECT){
					printf("name3 = %s\n", name);
					strcpy(entry->name, name); 
					AddStrtabEntry(entry);
					// strcpy(entry->name, name); 
				//	entry = (DataEntry)MALLOC(sizeof(struct dataEntry));
				//	dataEntryArray[dataEntryArrayIndex++] = entry;
					// strcpy(entry->name, name); 
				// TODO 不知道两种情况有没有差异，先这样做。
				}else if(entry->symbolType == SYMBOL_TYPE_FUNC){
					printf("name4 = %s\n", name);
					int targetIndex = FindDataEntryIndex(name);
					strcpy(entry->name, name); 
					if(targetIndex == -1){
						AddStrtabEntry(entry);
						// TODO 最好把下面的代码放到遇到ret指令的时候。
						entry = (DataEntry)MALLOC(sizeof(struct dataEntry));
						dataEntryArray[dataEntryArrayIndex++] = entry;
					}else{
					//	AddStrtabEntry(entry);
						// TODO 这里需要写更多代码，因为还要处理函数中的指令。
						DataEntry targetEntry = dataEntryArray[targetIndex];
						int dataEntrySize = sizeof(struct dataEntry);
						memcpy(targetEntry, entry, dataEntrySize);
						// 本来，当前entry要用来存储已经存在的函数。但是这个函数重复了，所以，
						// 用这个entry存储新的数据，
						// 更新那个已经存在的函数在dataEntryArray中对应的元素。
						memset(entry, 0, dataEntrySize); 
					}
				}else{
					printf("name5 = %s\n", name);
					// ch1:
					// 在汇编代码中出现上面这样的字符串，只有三种情况：变量名，函数名，函数中的标签。
					// 不是前面两种情况，只能是第三种情况（函数中的标签）。像这种情况，直接跳过。
					strcpy(entry->name, name); 
//					entry = (DataEntry)MALLOC(sizeof(struct dataEntry));
//					dataEntryArray[dataEntryArrayIndex++] = entry;
				}
			}
		}else if(token == TYPE_TOKEN_LOCAL){
			// 解析 .local  num66.2504 这种数据。
			// .comm   num66.2504,4,4
			// DataEntry bssDataEntry = (DataEntry)MALLOC(sizeof(struct dataEntry));
			DataEntry bssDataEntry = entry;

			bssDataEntry->section = SECTION_BSS;
			bssDataEntry->symbolType = SYMBOL_TYPE_OBJECT;
			bssDataEntry->bind = LOCAL;

			GetNextToken();
			memset(bssDataEntry->name,0,200);
			strcpy(bssDataEntry->name, GetCurrentTokenLexeme());
			// 跳过 .comm
			GetNextToken();
			// 跳过 num66.2504
			GetNextToken();
			// 跳过逗号
			GetNextToken();
			// 解析 length
			GetNextToken();
			char *lengthStr = GetCurrentTokenLexeme();
			int length = atoi(lengthStr);
			bssDataEntry->size = length;
			// 跳过逗号
			GetNextToken();
			// 解析 alignment
			GetNextToken();
			char *alignmentStr = GetCurrentTokenLexeme();
			int alignment = atoi(alignmentStr);
			bssDataEntry->align = alignment;

			AddStrtabEntry(entry);

			// dataEntryArray[dataEntryArrayIndex++] = bssDataEntry;

			entry = (DataEntry)MALLOC(sizeof(struct dataEntry));
			dataEntryArray[dataEntryArrayIndex++] = entry;
//			bssDataEntryArray[bssDataEntryArrayIndex++] = bssDataEntry;
		}else if(token == TYPE_TOKEN_COMM){
			// 处理 .comm   ch4,1,1
			entry->section = SECTION_COM;
			entry->symbolType = SYMBOL_TYPE_OBJECT;
			// 跳过 .ch4
			GetNextToken();
			memset(entry->name,0,200);
			strcpy(entry->name, GetCurrentTokenLexeme());
			// 跳过逗号
			GetNextToken();
			// 解析 length
			GetNextToken();
			char *lengthStr = GetCurrentTokenLexeme();
			int length = atoi(lengthStr);
			entry->size = length;
			// 跳过逗号
			GetNextToken();
			// 解析 alignment
			GetNextToken();
			char *alignmentStr = GetCurrentTokenLexeme();
			int alignment = atoi(alignmentStr);
			entry->align = alignment;

			AddStrtabEntry(entry);
			// 收集Bind为GLOBAL的字符串。
			AddGloblVariableNode(entry->name);

			entry = (DataEntry)MALLOC(sizeof(struct dataEntry));
			dataEntryArray[dataEntryArrayIndex++] = entry;
		}else if(token == TYPE_TOKEN_CALL){
			// 处理 call sum
			// 跳过call
			GetNextToken();
			// 获取call，存储到name。
			char *name = GetCurrentTokenLexeme();
			if(FindDataEntryIndex(name) != -1){
			//	memset(entry, 0, sizeof(struct dataEntry));
				continue;
			}

			// 收集Bind为GLOBAL的字符串。
			AddGloblVariableNode(name);
		
			entry->section = SECTION_TEXT;
			entry->symbolType = SYMBOL_TYPE_NOTYPE;
			entry->bind = GLOBAL;
		
			memset(entry->name,0,200);
			strcpy(entry->name, name);
		
			AddStrtabEntry(entry);
		
			entry = (DataEntry)MALLOC(sizeof(struct dataEntry));
			dataEntryArray[dataEntryArrayIndex++] = entry;
		}else if(token == TYPE_TOKEN_GLOBL){
			// 处理.globl  str
			// 跳过 str
			GetNextToken();
			char *name = GetCurrentTokenLexeme();
			// 收集Bind为GLOBAL的字符串。
			AddGloblVariableNode(name);
		}
	}

	printf("处理数据结束\n");

	// 最后一个元素是空的。
	printf("dataEntryArrayIndex = %d\n", dataEntryArrayIndex);
	// todo 我想清理最后一个空元素，琢磨了一会儿，没找到正确的方法。搁置。
//	dataEntryArrayIndex > 1 ? dataEntryArrayIndex = dataEntryArrayIndex - 1 : NULL;:
//	dataEntryArray[dataEntryArrayIndex] = NULL;
}

void CalculateDataEntryOffset()
{
	int index = 0;
	while(1){
		DataEntry entry = dataEntryArray[index++];
		if(entry == NULL)	break;
		// 修正Bind
		unsigned char isGlobl = isGloblVariable(entry->name);
		if(isGlobl == 1){
			entry->bind = GLOBAL;
		}else{
			entry->bind = LOCAL;
		}

		if(entry->section == SECTION_DATA){
			entry->offset = dataEntryOffset;
			dataEntryOffset += entry->size;
		}else if(entry->section == SECTION_RODATA){
			// TODO 这样做正确吗？
			if(entry->size == 0){
				// todo 如果不是这种情况，应该怎么处理？
				if(entry->dataType == DATA_TYPE_STRING){
					entry->size = strlen(entry->valPtr->val.strVal);
				}
			}
			entry->offset = rodataEntryOffset;
			rodataEntryOffset += entry->size;
		}else{
			printf("%d in calculateDataEntryOffset", __LINE__);
		}
	}
}

void CalculateStrtabEntryOffset()
{
	StrtabEntry node = strtabEntryList;
	int offset = 0;
	int index = 0;

	while(node != NULL){
		node->offset = offset;
		node->index = index;

		offset += strlen(node->name);
		index++;
		node = node->next;
	}
}

void ReSortStrtab()
{
	preStrtabEntryNode = NULL;
	strtabEntryList = NULL;
	// 遍历`.strtab`，找到`Bind`为`local`的字符串，放入链表`.strtabList`。
	// 建立单链表的操作，还是有点不熟，想了一会儿才写出来。
	for(int i = 0; i < strtabEntryArrayIndex; i++){
		// StrtabEntry node = strtabEntryArray[i];
		StrtabEntry entry = strtabEntryArray[i];
		char *name = entry->name;
		unsigned char isGlobl = isGloblVariable(name);
		// printf("name = %s, isGlobl = %d\n", name, isGlobl);
		if(isGlobl == 1)	continue;
		int size = sizeof(struct strtabEntry);
		StrtabEntry node = (StrtabEntry)MALLOC(size);
		memcpy(node, entry, size);
		AddStrtabEntryListNode(node);
	}
	
	// 遍历`.strtab`，找到`Bind`为`globl`而且`Type`是`OBJECT`的字符串，放入链表`.strtabList`。
	for(int i = 0; i < strtabEntryArrayIndex; i++){
		// continue;
		StrtabEntry entry = strtabEntryArray[i];
		int size = sizeof(struct strtabEntry);
		StrtabEntry node = (StrtabEntry)MALLOC(size);
		memcpy(node, entry, size);
		char *name = entry->name;
		DataEntry dataEntryNode = FindDataEntry(name);
		if(dataEntryNode == NULL || dataEntryNode->bind == LOCAL){
			continue;
		}

		if(dataEntryNode->symbolType == SYMBOL_TYPE_OBJECT){
			AddStrtabEntryListNode(node);
		}
	}

	// 遍历`.strtab`，找到`Bind`为`globl`而且`Type`是`FUNC`的字符串，放入链表`.strtabList`。
	for(int i = 0; i < strtabEntryArrayIndex; i++){
		// StrtabEntry node = strtabEntryArray[i];
		StrtabEntry entry = strtabEntryArray[i];
		char *name = entry->name;
		int size = sizeof(struct strtabEntry);
		StrtabEntry node = (StrtabEntry)MALLOC(size);
		memcpy(node, entry, size);
		DataEntry dataEntryNode = FindDataEntry(name);
		if(dataEntryNode == NULL || dataEntryNode->bind == LOCAL){
			continue;
		}

		if(dataEntryNode->symbolType == SYMBOL_TYPE_FUNC){
			AddStrtabEntryListNode(node);
		}
	}

	// 遍历`.strtab`，找到`Bind`为`globl`而且`Type`是`NOTYPE`的字符串，放入链表`.strtabList`。
	for(int i = 0; i < strtabEntryArrayIndex; i++){
		// StrtabEntry node = strtabEntryArray[i];
		StrtabEntry entry = strtabEntryArray[i];
		char *name = entry->name;
		int size = sizeof(struct strtabEntry);
		StrtabEntry node = (StrtabEntry)MALLOC(size);
		memcpy(node, entry, size);
		DataEntry dataEntryNode = FindDataEntry(name);
		if(dataEntryNode == NULL || dataEntryNode->bind == LOCAL){
			continue;
		}

		if(dataEntryNode->symbolType == SYMBOL_TYPE_NOTYPE){
			AddStrtabEntryListNode(node);
		}
	}
	
}

void BuildELF()
{
	// ELF文件头
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)MALLOC(sizeof(Elf32_Ehdr));
	ehdr->e_ident[0] = 'E';
	ehdr->e_ident[1] = 'L';
	ehdr->e_ident[2] = 'F';

	ehdr->e_ident[3] = 1;
	ehdr->e_ident[4] = 1;
	ehdr->e_ident[5] = 1;

	for(int i = 6; i < 16; i++){
		ehdr->e_ident[i] = 0;
	}

	ehdr->e_type =	ET_REL; 	
	ehdr->e_machine = EM_386;
	ehdr->e_version = 1;
	ehdr->e_entry = 0;
	ehdr->e_phoff = 0;
	// TODO 需计算。
	ehdr->e_shoff = 0;
	// TODO 不知道设置成什么值。
	ehdr->e_flags = 0;
	// 是一个固定值。
	ehdr->e_ehsize = 52;
	ehdr->e_phentsize = 0;
	ehdr->e_phnum = 0;
	ehdr->e_shentsize = 40;
	// TODO 段表条目的数量，待定。
	ehdr->e_shnum = 13;
	// TODO .shstrtab在段表中的索引。
	ehdr->e_shstrndx = 0;

	// 段的内容
	// TODO .text和.rel.text太麻烦了，先不处理。
	// .text
	// .rel.text
	// .data
	// .rel.data
	// .rodata
	// .symtab
	// .strtab
	// .shstrtab
	
	// .text TODO
	SectionDataNode relTextDataHead = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
	SectionDataNode dataDataHead = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
	SectionDataNode relDataDataHead = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
	SectionDataNode rodataDataHead = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
	SectionDataNode symtabDataHead = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
	SectionDataNode strtabDataHead = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
	SectionDataNode shstrtabDataHead = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));

	// 初始化节点。
	SectionDataNode relTextDataNode , dataDataNode , relDataDataNode , rodataDataNode , \
        symtabDataNode , strtabDataNode , shstrtabDataNode;

    SectionDataNode preRelTextDataNode , preDataDataNode , preRelDataDataNode , preRodataDataNode , \
        preSymtabDataNode , preStrtabDataNode , preShstrtabDataNode;

	relTextDataNode = dataDataNode = relDataDataNode = rodataDataNode = \
		symtabDataNode = strtabDataNode = shstrtabDataNode = NULL;	

	preRelTextDataNode = preDataDataNode = preRelDataDataNode = preRodataDataNode = \
		preSymtabDataNode = preStrtabDataNode = preShstrtabDataNode = NULL;	

	// 遍历dataEntryArray
	for(int i = 0; i < 100; i++){
		DataEntry entry = dataEntryArray[i];
		// 这段代码是我运行程序出现错误后加上的。凭空琢磨这段代码的必要性，不容易想出原因。
		if(entry == NULL){
			break;
		}

		if(entry->section == SECTION_TEXT){

		}else if(entry->section == SECTION_DATA){
			// TODO 这这部分代码中，还应该获取.rel.data的数据。但我现在不知道怎么做。
			if(dataDataNode == NULL){
				dataDataNode = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
				dataDataHead->next = dataDataNode;
			}

			preDataDataNode = dataDataNode;
			// 真可恶！我发现，在这里要进行一个遍历。
			DataEntryValueNode valPtr = entry->valPtr;
			while(valPtr != NULL){
				// TODO 处理string似乎是多此一举。在.data中不存在字符串。
				int val;
				if(valPtr->type == STR){
					dataDataNode->val.strVal;
					char *str = valPtr->val.strVal;
					// 根据str查找对应的entry，然后读取offset。
					int entryIndex = FindDataEntryIndex(str);
					if(entryIndex == -1){
						// todo 出现这种情况，该怎么办？
						// 先终止程序。
						printf("%s, error in %s, %d\n",str,  __FILE__, __LINE__);
						exit(-2);
					}
					DataEntry entry = dataEntryArray[entryIndex];
				}else{
					// dataDataNode->val.numVal = valPtr->val.numVal;
					val = valPtr->val.numVal;
				}

				dataDataNode->val.numVal = val;

				dataDataNode = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
				preDataDataNode->next = dataDataNode;
				// 在这里，心算不明白链表的一些细节。
				valPtr = valPtr->next;
			}


		}else if(entry->section == SECTION_RODATA){
			if(rodataDataNode == NULL){
				rodataDataNode = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
				rodataDataHead->next = rodataDataNode;
			}

			preRodataDataNode = rodataDataNode;
			// 真可恶！我发现，在这里要进行一个遍历。
			DataEntryValueNode valPtr = entry->valPtr;
			if(entry->dataType == DATA_TYPE_STRING){
				rodataDataNode->val.strVal = valPtr->val.strVal;
			}else{
				rodataDataNode->val.numVal = valPtr->val.numVal;
			}

			rodataDataNode = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
			preRodataDataNode->next = rodataDataNode;

		}else{
			printf("error section %d\n", __LINE__);
		}

	}
	
	// 段表
	StrtabEntry strtabEntryNode = strtabEntryList;
	int symtabDataNodeIndex = 0;

	// 在.symtab中加入一些常量节点。
	char *nodeNameArray[6] = {"UND", "ch.c", ".text", ".data", ".bss", ".rodata"};
	int nodeIndexArray[6] = {SECTION_NDX_UND,SECTION_NDX_ABS,1,3,5,6};
	int nodeSymbolTypeArray[6] = {
// todo 找机会去掉这些自定义常量，因为我已经定义过了。
//		SYMBOL_TYPE_NOTYPE, SYMBOL_TYPE_FILE, SYMBOL_TYPE_SECTION,
//		SYMBOL_TYPE_SECTION,SYMBOL_TYPE_SECTION,SYMBOL_TYPE_SECTION
		STT_NOTYPE, STT_FILE, STT_SECTION,
		STT_SECTION, STT_SECTION, STT_SECTION
	};
	for(int i = 0; i < 6; i++){
		char *name = nodeNameArray[i];
		Elf32_Sym *sym = (Elf32_Sym *)MALLOC(sizeof(Elf32_Sym));

		int symbolType = nodeSymbolTypeArray[i] ;
		int bind = (int)LOCAL;

		sym->st_name = 0;
		sym->st_value = 0;
		sym->st_size = 0;
		sym->st_info = (bind << 4) | (symbolType & 0xf);
		// 不必理会st_other。
		sym->st_other = 0;
		sym->st_shndx = nodeIndexArray[i];

		int sectionDataNodeSize = sizeof(struct sectionDataNode);
		if(symtabDataNode == NULL){
			symtabDataNode = (SectionDataNode)MALLOC(sectionDataNodeSize);
			strcpy(symtabDataNode->name, name);
			symtabDataNode->index = symtabDataNodeIndex++;
			symtabDataNode->val.Elf32_Sym_Val = sym;
			symtabDataHead->next = symtabDataNode;
		}else{
			strcpy(symtabDataNode->name, name);
			symtabDataNode->val.Elf32_Sym_Val = sym;
			preSymtabDataNode->next = symtabDataNode;
		}
		preSymtabDataNode = symtabDataNode;
		symtabDataNode = (SectionDataNode)MALLOC(sectionDataNodeSize);
		symtabDataNode->index = symtabDataNodeIndex++;
	}


	while(1){
		if(strtabEntryNode == NULL){
			break;
		}
		char *name = strtabEntryNode->name;
		DataEntry entry = FindDataEntry(name);
		// todo 需要设置sym的成员的值。
		Elf32_Sym *sym = (Elf32_Sym *)MALLOC(sizeof(Elf32_Sym));
		
		Elf32_Word nameIndex = (Elf32_Word)FindIndexInStrtabEntryList(name);	
		Elf32_Addr offset = (Elf32_Addr)entry->offset;
		Elf32_Addr size = (Elf32_Word)entry->size;
		int symbolType = entry->symbolType;
		int bind = entry->bind;

		sym->st_name = nameIndex;
		sym->st_value = offset;
		sym->st_size = size;
		sym->st_info = (bind << 4) | (symbolType & 0xf);
		// 不必理会st_other。
		sym->st_other = 0;
		sym->st_shndx = entry->section;

		int sectionDataNodeSize = sizeof(struct sectionDataNode);
		if(symtabDataNode == NULL){
			symtabDataNode = (SectionDataNode)MALLOC(sectionDataNodeSize);
			strcpy(symtabDataNode->name, name);
			symtabDataNode->index = symtabDataNodeIndex++;
			symtabDataNode->val.Elf32_Sym_Val = sym;
			symtabDataHead->next = symtabDataNode;
	//		preSymtabDataNode = symtabDataNode;
		}else{
			// symtabDataNode = (SectionDataNode)MALLOC(sectionDataNodeSize);
			strcpy(symtabDataNode->name, name);
			symtabDataNode->val.Elf32_Sym_Val = sym;
			preSymtabDataNode->next = symtabDataNode;
	//		preSymtabDataNode = symtabDataNode;
		}
		preSymtabDataNode = symtabDataNode;
		symtabDataNode = (SectionDataNode)MALLOC(sectionDataNodeSize);
		symtabDataNode->index = symtabDataNodeIndex++;
		
		
		strtabEntryNode = strtabEntryNode->next;
	}

	// .rel.data
	strtabEntryNode = strtabEntryList;
	int relDataDataNodeIndex = 0;
	while(strtabEntryNode != NULL){
		char *name = strtabEntryNode->name;
		DataEntry entry = FindDataEntry(name);

		int sectionDataNodeSize = sizeof(struct sectionDataNode);

		if(entry->isRel == 1){
			Elf32_Rel *relEntry = (Elf32_Rel *)MALLOC(sizeof(Elf32_Rel));
			Elf32_Addr offset = (Elf32_Addr)entry->offset;
			int relocationType = R_386_32;
			// SectionDataNode dataNode = FindSymbolSectionDataNode(name, symtabDataHead);
			SectionDataNode dataNode = FindSymbolSectionDataNode(".rodata", symtabDataHead);
			if(dataNode == NULL){
				printf("error in .rel.data.name = %s, line = %d", name, __LINE__);
				exit(-1);
			}
			int symbolIndex = dataNode->index;
			Elf32_Word info = (Elf32_Word)((symbolIndex << 8) | relocationType);
			relEntry->r_offset = offset;
			relEntry->r_info = info;

			if(relDataDataNode == NULL){
				relDataDataNode = (SectionDataNode)MALLOC(sectionDataNodeSize);
				relDataDataHead = relDataDataNode;
				// 暂时不需要name。
				// strcpy(relDataDataNode->name, name);
				relDataDataNode->index = relDataDataNodeIndex++;
				relDataDataNode->val.Elf32_Rel_Val = relEntry;
			}else{
				relDataDataNode = (SectionDataNode)MALLOC(sectionDataNodeSize);
				// strcpy(relDataDataNode->name, name);
				relDataDataNode->index = relDataDataNodeIndex++;
				relDataDataNode->val.Elf32_Rel_Val = relEntry;
				preRelDataDataNode->next = relDataDataNode;
			}

			preRelDataDataNode = relDataDataNode;
		}

		strtabEntryNode = strtabEntryNode->next;
	}
	

	printf("BuildELF is over\n");
}

int main(int argc, char *argv[])
{
	CurrentHeap = &ProgramHeap;

	char *filename = argv[1];

	printf("parse file %s\n", filename);

	lexer = (Lexer)MALLOC(sizeof(struct _lexer));
	
	Init();

	LoadFile(filename);

	while(True){
		int nextToken = GetLookAheadToken();
		if(IsData(nextToken)){
			ParseData();
		}else{
			GetNextToken();
			char *name = GetCurrentTokenLexeme();
			printf("0 ParseData name = %s\n", name);
		}

		if(lexer->currentLine == lexer->lineNum) break;
	}

	//ReSortStrtab();

	CalculateDataEntryOffset();
	ReSortStrtab();
	CalculateStrtabEntryOffset();

	TestDataEntryArray();
	TestStrtabEntryArray();
	TestStrtabEntryList();

	// 生成可重定位文件。
	BuildELF();

	printf("Game over\n");
	
	return 0;
}
