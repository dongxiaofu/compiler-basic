//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <ctype.h>

#include "common.h"
#include "elf.h"
#include "assembler.h"
#include "instr.h"

SectionDataNode symLinkList = NULL;;
SectionDataNode preSym = NULL;;

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
	instrHead = NULL;
	// TODO 下面的代码都是写虚拟机时使用的代码，可能已经无用了。找机会删除。
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

int myStrlen(char *name)
{
	int len = strlen(name);
	return len;
	int newLen = 0;
	for(int i = 0; i < len; i++){
		char currentChar = name[i];
		// 处理汉字。一个汉字用三个字节表示。
		if(currentChar < 0){
			i += 2;
		}
		newLen++;
	}

	return newLen;
}

int FindShStrTabOffset(char *name)
{
	// 这是我用正则表达式生成的。
//	char *arr[7] = {".rel.text",".rel.data",".bss",".rodata",".symtab",".strtab",".shstrtab"};
	
	unsigned int length = 0;
	for(int i = 0; i < shstrtabArr_SIZE; i++){
	//	length += strlen(arr[0]) + 1;
		length += strlen(shstrtabArr[i]) + 1;
	}
	// char *shStrTabStr = ".rel.text.rel.data.bss.rodata.symtab.strtab.shstrtab";
//	char *shStrTabStr = ".rel.text.\000rel.data\000.bss\000.rodata\000.symtab\000.strtab\000.shstrtab\000";
	int nameLength = strlen(name);

	char *str = shStrTabStr;
	for(int i = 0; i < length; i++){
		if(strncmp(str, name, nameLength) == 0){
			return i;
		}
		str++;
	}
	
	return -1;
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

Elf32_Sym *FindSymbol(char *name)
{
	

}

SegmentInfo FindSegmentInfoNode(char *name)
{ 
	// 声明segmentInfoNode时，初始值时NULL。
	SegmentInfo node = segmentInfoNode->next;

	while(node != NULL){
		if(strcmp(name, node->name) == 0){
			return node;
		}

		node = node->next;
	}

	return node;
}

GloblVariableNode FindGloblVariableNode(char *name)
{
	GloblVariableNode node = globlVariableList;

	while(1){
		if(node == NULL)	break;

//		printf("name = %s, node->name = %s\n", name, node->name);
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
			}else if(token == TYPE_TOKEN_TEXT){
				// TODO 是否要在这里记录.text？
				// 可以在这里记载。我之所以有过滤，是因为ch.s的开头就是.text。
				// 无需担心。除了local变量外，遇到其他段名，例如.data，会覆盖.text。
				// 那么，被误认为.text的local变量怎么办？我打算遇到local就把段名修正为.bss。
				currentSection = SECTION_TEXT;
				StartPeekToken();
				token = GetNextToken();
				if(token == TYPE_TOKEN_LOCAL){
					currentSection = SECTION_BSS;
				}
				EndPeekToken();
			}else if(token == TYPE_TOKEN_SECTION){
				nextToken = GetLookAheadToken();
				if(nextToken == TYPE_TOKEN_RODATA){
	//				entry->section = SECTION_RODATA;
					currentSection = SECTION_RODATA;
				}else if(nextToken == TYPE_TOKEN_DATA){
	//				entry->section = SECTION_DATA; 
					currentSection = SECTION_DATA;
				}else if(nextToken == TYPE_TOKEN_TEXT){
					// TODO 我并不确定是否会出现这种情况。已经把这段代码移动到前面了。
					// TODO 是否要在这里记录.text？
					// 可以在这里记载。我之所以有过滤，是因为ch.s的开头就是.text。
					// 无需担心。除了local变量外，遇到其他段名，例如.data，会覆盖.text。
					// 那么，被误认为.text的local变量怎么办？我打算遇到local就把段名修正为.bss。
					currentSection = SECTION_TEXT;
					StartPeekToken();
					token = GetNextToken();
					if(token == TYPE_TOKEN_LOCAL){
						currentSection = SECTION_BSS;
					}
					EndPeekToken();
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
				OFFSET_TYPE dataTypeSize = EMPTY;
				if(token == TYPE_TOKEN_BYTE){
					entry->dataType = DATA_TYPE_BYTE;
					// todo 最好把这些数值换成常量。
					entry->dataTypeSize = 1;
					dataTypeSize = EIGHT;
				}else if(token == TYPE_TOKEN_LONG){
					entry->dataType = DATA_TYPE_LONG;
					entry->dataTypeSize = 4;
					dataTypeSize = THIRTY_TWO;
				}else if(token == TYPE_TOKEN_KEYWORD_STRING){
					entry->dataType = DATA_TYPE_STRING;
					// todo 可能有问题。
					entry->dataTypeSize = 4;
					dataTypeSize = THIRTY_TWO;
				}

				if(node == NULL){
					entry->dataEntryValueNodeNum++;
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
						node->val.numVal.type = dataTypeSize; 
						// 把2576980378转换为2147483647。
						// node->val.numVal.value = atoi(name);
						// node->val.numVal.value = atol(name);
						node->val.numVal.value = atoll(name);
					}
				}

				node = (DataEntryValueNode)MALLOC(sizeof(struct dataEntryValueNode));
				preNode->next = node;
				entry->dataEntryValueNodeNum++;

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
			entry->dataEntryValueNodeNum--;

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

				}else if(entry->symbolType == SYMBOL_TYPE_FUNC){
					printf("name4 = %s\n", name);
					int targetIndex = FindDataEntryIndex(name);
					strcpy(entry->name, name); 
					// TODO 要非常注意。如果把这行代码放在下面的if(targetIndex == -1)中，
					// 达不到预期目的。
					// 这行代码也许能够提升到上一层级。可能并不需要提升到上面。
					entry->section = currentSection;
					// 遇到main:，是一个函数的开始，也是另一个函数的结束，所以，我在几个月前写下来
					// 下面的注释：最好把下面的代码放到遇到ret指令的时候。
					if(targetIndex == -1){
						// 当一个函数名在已经收集到的函数库中不存在时，这个函数是一个新函数的开始，
						// 是旧函数的结束。
						AddStrtabEntry(entry);
						// TODO 最好把下面的代码放到遇到ret指令的时候。
						entry = (DataEntry)MALLOC(sizeof(struct dataEntry));
						dataEntryArray[dataEntryArrayIndex++] = entry;
					}else{
						// TODO 我不太理解这里为什么要这样写。
						// 当初，我为了处理call sum和sum:，确实处理过函数重复的问题。
						// 但这里的逻辑似乎不完全是为了处理这种情况。
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
		}else if(token == TYPE_TOKEN_ALIGN){
			// 处理 .align 4
			// 跳过 4
			GetNextToken();
			char *alignmentStr = GetCurrentTokenLexeme();
			int alignment = atoi(alignmentStr);
			entry->align = alignment;
		}
	}

	printf("处理数据结束\n");

	// 最后一个元素是空的。
	printf("dataEntryArrayIndex = %d\n", dataEntryArrayIndex);
	// todo 我想清理最后一个空元素，琢磨了一会儿，没找到正确的方法。搁置。
//	dataEntryArrayIndex > 1 ? dataEntryArrayIndex = dataEntryArrayIndex - 1 : NULL;:
//	dataEntryArray[dataEntryArrayIndex] = NULL;
}

// todo 想不到更好的函数名，只能用这个名字。
Instruction DealWithInstr(InstructionSet instrCode)
{
	// todo 先这样做吧。下面的分类，暂时没有作用。
	Instruction instr = parseInstructionFunctions[(int)instrCode](instrCode);
	// TODO 本想在GenerateSimpleInstr里面实现下面的功能，可在那个函数中，已经没有InstructionSet了。
	if(instrCode == I_CALL || (I_JE <= instrCode && instrCode <= I_JMP) ){
		if(instr->relTextEntry){
			instr->relTextEntry->relType = R_386_PC32;	
		}
	}

	return instr;

//	return parseInstructionFunctions[(int)instrCode](instrCode);
	// FPU指令
	if(I_FCHS <= instrCode && instrCode <= I_FMULL){

	}else{
		// FPU指令
		if(I_CDQ <= instrCode && instrCode <= I_RET){
			// 无操作数指令
			// parseInstructionFunctions[(int)instrCode](instrCode);

		}else if(I_MULL <= instrCode && instrCode <= I_CALL){
			// 单操作数指令


		}else{
			// 双操作数指令

		}
	}
}

void ParseInstr()
{
	printf("开始处理指令\n");
	int token;

	// TODO 要检查一下这种设置头结点的方法有没有用。
	if(instrHead == NULL){
		instrHead = (Instruction)MALLOC(sizeof(struct instruction));
	}

	// Instruction instrHead = (Instruction)MALLOC(sizeof(struct instruction));
	Instruction instrDataNode, preInstrDataNode;
	instrDataNode = preInstrDataNode = NULL;
	
	
	while(1){
		token = GetNextToken();
		printf("token = %d\n", token);
		if(token == TYPE_TOKEN_INVALID)	break; 

		// TODO 不能处理call sum，多次调试我才怀疑漏掉了token == TYPE_TOKEN_CALL。
		// 像这样用特殊条件判断，我很不满意。要想个方法处理好。
		if(token == TYPE_TOKEN_INDENT || token == TYPE_TOKEN_CALL){
			char *name = GetCurrentTokenLexeme();
			InstructionSet instrCode = FindInstrCode(name);
			if(instrCode == I_INSTR_INVALID){
				printf("%s is not a valid instr\n", name);
			}else{
				printf("%s is a valid instr\n", name);
				// 创建这个链表，我不能立刻心算出思路，用口算刻意梳理思路才想出了方案。
				// 创建这个链表和其他链表可能稍微有点不同。节点是函数的返回值，而不是临时新建的。
				instrDataNode = DealWithInstr(instrCode);
				if(instrDataNode == NULL){
					printf("instr should not be NULL\n");
					exit(instrCode);
				}
				
				instrDataNode->name = name;

				// if(preInstrDataNode == NULL){
				if(instrHead->next == NULL){
					instrHead->next = instrDataNode;
					preInstrDataNode = instrDataNode;
				}else{
					preInstrDataNode->next = instrDataNode;	
					preInstrDataNode = instrDataNode;
				}
			}
		}
	}

	printf("over\n");
}

int RoundUpNumDeprecated(int num)
{
	int newNum = num;
	
//	if(num % 4 == 0 || num % 8 == 0){
//		return newNum;
//	}

//	if(num % 4 != 0){
//		newNum = (num / 4 + 1) * 4;
//	}else if(num % 8 != 0){
//		newNum = (num / 8 + 1) * 8;
//	}

	if(num > 8 && num % 8 != 0){
		newNum = (num / 8 + 1) * 8;
	}else if(num % 4 != 0){
		newNum = (num / 4 + 1) * 4;
	}

	return newNum;
}

int RoundUpNum(int num, int alignment)
{
	if(alignment == 0)	return num;

	if(num % alignment != 0){
		return (((num / alignment) + 1) * alignment);
	}else{
		return num;
	}
}

void CalculateDataEntryOffset()
{
	SegmentInfo dataSegmentInfoNode = FindSegmentInfoNode(".data");
	if(dataSegmentInfoNode == NULL){
		dataSegmentInfoNode = (SegmentInfo)MALLOC(sizeof(struct segmentInfo));
		strcpy(dataSegmentInfoNode->name, ".data");
		preSegmentInfoNode->next = dataSegmentInfoNode;
		preSegmentInfoNode = dataSegmentInfoNode;
	}
	
	SegmentInfo rodataSegmentInfoNode = FindSegmentInfoNode(".rodata");
	if(rodataSegmentInfoNode == NULL){
		rodataSegmentInfoNode = (SegmentInfo)MALLOC(sizeof(struct segmentInfo));
		strcpy(rodataSegmentInfoNode->name, ".rodata");
		preSegmentInfoNode->next = rodataSegmentInfoNode;
		preSegmentInfoNode = rodataSegmentInfoNode;
	}
		

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
			dataEntryOffset = RoundUpNum(dataEntryOffset, entry->align);
			entry->offset = dataEntryOffset;
			dataEntryOffset += entry->size;
//			printf("name = %s, size = %d, dataEntryOffset = %d\n", \
				entry->name, entry->size, dataEntryOffset);
		}else if(entry->section == SECTION_RODATA){
			// TODO 这样做正确吗？
			if(entry->size == 0){
				// todo 如果不是这种情况，应该怎么处理？
				if(entry->dataType == DATA_TYPE_STRING){
					int len = myStrlen(entry->valPtr->val.strVal);
					entry->size = len + 1; 
					printf("Ro str = %s, len = %d\n", entry->valPtr->val.strVal, entry->size);
				}else{
					printf("Ro str = %d\n", entry->valPtr->val.numVal);
				}
			}
			rodataEntryOffset = RoundUpNum(rodataEntryOffset, entry->align);
			entry->offset = rodataEntryOffset;
			rodataEntryOffset += entry->size;
		}else{
			printf("%d in calculateDataEntryOffset\n", __LINE__);
		}
	}

	dataSegmentInfoNode->size = dataEntryOffset;
	rodataSegmentInfoNode->size = rodataEntryOffset;
}

void CalculateStrtabEntryOffset()
{
	StrtabEntry node = strtabEntryList;
	int offset = 0;
	int index = 0;

	SegmentInfo strtabSegmentInfoNode = FindSegmentInfoNode(".strtab");
	if(strtabSegmentInfoNode == NULL){
		strtabSegmentInfoNode = (SegmentInfo)MALLOC(sizeof(struct segmentInfo));
		strcpy(strtabSegmentInfoNode->name, ".strtab");
		preSegmentInfoNode->next = strtabSegmentInfoNode;
		preSegmentInfoNode = strtabSegmentInfoNode;
	}

	while(node != NULL){
		node->offset = offset;
		node->index = index;

		int length = strlen(node->name) + 1;
		printf("Cal name = %s, len = %d\n", node->name, length);
		offset += length;
		index++;

		// 字符串的结束符是一个空字符，占用一个字节。
		strtabSegmentInfoNode->size += length;

		node = node->next;
	}

	printf("strtabSegmentInfoNode->size = %d\n", strtabSegmentInfoNode->size);
}

// 把strtab中的字符串拼接成一个字符串。
void StrcatStrtab()
{
	unsigned int strtabStringSize = sizeof(struct strtabString);
	strtabString = (StrtabString)MALLOC(strtabStringSize);

	// 这个函数的难点是连接字符串。
	// unsigned int size = 0;
	unsigned int size = 1;
	StrtabEntry entry = strtabEntryList;
	while(entry != NULL){
		char *name = entry->name;
		size += strlen(name) + 1;

		entry = entry->next;
	}

	strtabString->length = size;

	char *str = (char *)MALLOC(size);
	strtabString->string = str;

	printf("StrcatStrtab start\n");
	size = 0;
	str++;
	entry = strtabEntryList;
	while(entry != NULL){
		char *name = entry->name;
		// 复制粘贴导致的恶果！
		// size += strlen(name) + 1;
		size = strlen(name) + 1;
		printf("name = %s, length = %d\n", name, size);
		memcpy(str, name, size);
		str += size;

		entry = entry->next;
	}
	printf("StrcatStrtab end\n");
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

		localVariableCount++;
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
	

	// 为什么在这个函数中调用它？因为它依赖ReSortStrtab处理后的strtabEntryList。
	// 我担心在使用ReSortStrtab前调用StrcatStrtab，所以在ReSortStrtab调用StrcatStrtab。
	// 我并不完全满意这种做法，但我暂时不知道其他更好的做法。
	StrcatStrtab();
}

Elf32_Ehdr *GenerateELFHeader()
{
	// ELF文件头
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)MALLOC(sizeof(Elf32_Ehdr));

	ehdr->e_ident[0] = 0x7F;
	ehdr->e_ident[1] = 0x45;
	ehdr->e_ident[2] = 0x4C;

	ehdr->e_ident[3] = 0x46;
	ehdr->e_ident[4] = 1;
	ehdr->e_ident[5] = 1;
	ehdr->e_ident[6] = 1;

	for(int i = 7; i < 16; i++){
		ehdr->e_ident[i] = 0;
	}

	ehdr->e_type =	ET_REL; 	
	ehdr->e_machine = EM_386;
	ehdr->e_version = 1;
	ehdr->e_entry = 0;
	ehdr->e_phoff = 0;
	// TODO 需计算。
	// 段表在文件中的偏移。
	ehdr->e_shoff = 0;
	// TODO 不知道设置成什么值。
	ehdr->e_flags = 0;
	// 是一个固定值。
	ehdr->e_ehsize = 52;
	ehdr->e_phentsize = 0;
	ehdr->e_phnum = 0;
	ehdr->e_shentsize = 40;
	// TODO 段表条目的数量，待定。
	// ehdr->e_shnum = 13;
	ehdr->e_shnum = SHSTRTAB_ENTRY_ARRAY_SIZE;
	// TODO .shstrtab在段表中的索引。
	// ehdr->e_shstrndx = 0;
	ehdr->e_shstrndx = 8;

	return ehdr;
}

SectionData GetSectionData()
{
	SectionData sectionData = (SectionData)MALLOC(sizeof(struct sectionData));

// .text TODO
	SectionDataNode relTextDataHead = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
	SectionDataNode dataDataHead = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
	SectionDataNode relDataDataHead = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
	SectionDataNode rodataDataHead = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
	SectionDataNode symtabDataHead = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
	SectionDataNode strtabDataHead = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
	SectionDataNode shstrtabDataHead = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));

	SectionDataNode sectionHeaderDataHead = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));

	// 这段代码是我在Sublime Text3中使用正则表达式生成的。也许比复制粘贴快不了多少，但有趣一些，不枯燥。
	sectionData->relText = relTextDataHead;
	sectionData->data = dataDataHead;
	sectionData->relData = relDataDataHead;
	sectionData->rodata = rodataDataHead;
	sectionData->symtab = symtabDataHead;
	sectionData->strtab = strtabDataHead;
	sectionData->shstrtab = shstrtabDataHead;

	sectionData->sectionHeader = sectionHeaderDataHead;

	// 初始化节点。
	SectionDataNode relTextDataNode , dataDataNode , relDataDataNode , rodataDataNode , \
        symtabDataNode , strtabDataNode , shstrtabDataNode;

    SectionDataNode preRelTextDataNode , preDataDataNode , preRelDataDataNode , preRodataDataNode , \
        preSymtabDataNode , preStrtabDataNode , preShstrtabDataNode;

	relTextDataNode = dataDataNode = relDataDataNode = rodataDataNode = \
		symtabDataNode = strtabDataNode = shstrtabDataNode = NULL;	

	preRelTextDataNode = preDataDataNode = preRelDataDataNode = preRodataDataNode = \
		preSymtabDataNode = preStrtabDataNode = preShstrtabDataNode = NULL;	

	int textSize = 0;
	int relTextEntryNum = 0;
	int dataSize = 0;
	int relDataEntryNum = 0;
	int rodataSize = 0;
	int symtabSize = 0;
	int strtabSize = 0;
	int shstrtabSize = 0;

	// 遍历dataEntryArray
	for(int i = 0; i < 100; i++){
		DataEntry entry = dataEntryArray[i];
		// 这段代码是我运行程序出现错误后加上的。凭空琢磨这段代码的必要性，不容易想出原因。
		if(entry == NULL){
			break;
		}

		int sectionType = entry->section;
		char isValidSection = sectionType == SECTION_TEXT \
			|| sectionType == SECTION_DATA \
			|| sectionType == SECTION_RODATA;

		if(isValidSection == 0){
			printf("error section %d\n", __LINE__);
		}

		if(sectionType == SECTION_TEXT){
			continue;
		}

		if(sectionType == SECTION_DATA){
			// TODO 这这部分代码中，还应该获取.rel.data的数据。但我现在不知道怎么做。
			if(dataDataNode == NULL){
				dataDataNode = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
				dataDataHead->next = dataDataNode;

				dataDataNode->isLast = 1;
			}else{
				dataDataNode->isLast = 0;
			}

			preDataDataNode = dataDataNode;
			// 真可恶！我发现，在这里要进行一个遍历。
			DataEntryValueNode valPtr = entry->valPtr;
			int dataTypeSize = entry->dataTypeSize;
			int offset = entry->offset;
			char *variableName = entry->name;
			int elementIndex = -1;

			// `r_info`占用4个字节，高24位是`.symtab`的条目的索引，低8位是重定位类型。
			// Elf32_Word r_info = (RODATA_SYMTAB_INDEX << 8) | R_386_32;
			Elf32_Word r_info = (5 << 8) | R_386_32;

			while(valPtr != NULL){
				NumericData val = {0, EMPTY};
				if(valPtr->type == STR){
					// 在这里搜集.rel.data。
					// 需要.data中的偏移量。该偏移量存储到.rel.data的r_offset。
					// 还需要.rodata中的偏移量。把这个偏移量存储到dataDataNode->val.numVal。
					Elf32_Rel *relEntry = (Elf32_Rel *)MALLOC(sizeof(Elf32_Rel));
					// 这句，我想了会才写出来。
					// 为什么是4？因为我认为.data中的数据是以4字节为单位存储。例如，浮点数，数组。
					// 不对。如果是字符型数组，就不是以4字节为单位。
					// 应该以数据类型的大小为单位。
					// relEntry->r_offset = entry->offset + 4*(++elementIndex);
					relEntry->r_offset = offset + dataTypeSize * (++elementIndex);
					relEntry->r_info = r_info;
					// 把.rel.data存储在哪里？
					if(relDataDataNode == NULL){
						relDataDataNode = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
						// relDataDataNode->isLast = 1;
						relDataDataHead->next = relDataDataNode;
					}else{
						// relDataDataNode->isLast = 0;
						relDataDataNode = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
						preRelDataDataNode->next = relDataDataNode;
						// relDataDataNode->isLast = 1;
					}

					relDataDataNode->val.Elf32_Rel_Val = relEntry;
					preRelDataDataNode = relDataDataNode;

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
					val.value = entry->offset;
					val.type = THIRTY_TWO;
				}else{
					val = valPtr->val.numVal;
				}

				memcpy(dataDataNode->name, variableName, strlen(variableName));  
				dataDataNode->val.numVal = val;

				dataDataNode->isLast = 0;

				dataDataNode = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
				dataDataNode->isLast = 1;
				preDataDataNode->next = dataDataNode;
				preDataDataNode = dataDataNode;
				// 在这里，心算不明白链表的一些细节。
				valPtr = valPtr->next;
			}

			continue;
		}

		if(sectionType == SECTION_RODATA){
			if(rodataDataNode == NULL){
				rodataDataNode = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
				rodataDataNode->isLast = 1;
				rodataDataHead->next = rodataDataNode;
			}

			preRodataDataNode = rodataDataNode;
			// 真可恶！我发现，在这里要进行一个遍历。
			DataEntryValueNode valPtr = entry->valPtr;
			if(entry->dataType == DATA_TYPE_STRING){
				rodataDataNode->val.strVal = valPtr->val.strVal;
				rodataDataNode->valType = STR;
			}else{
				// 出乎我的意料。有一次，我遇到struct不能赋值给struct的代码。
				// 这行代码能正常运行。
				rodataDataNode->val.numVal = valPtr->val.numVal;
				rodataDataNode->valType = NUM;
			}

			rodataDataNode->isLast = 0;
			rodataDataNode = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
			rodataDataNode->isLast = 1;
			preRodataDataNode->next = rodataDataNode;

			continue;
		}

	}

	return sectionData;
}

Elf32_Word GetStName(char *name)
{
	// strtabString 是一个全局变量。隐患极大。最好复制一份数据，不要直接使用它。
	char *str = strtabString->string;
	unsigned int length = strtabString->length;
	// TODO GetSubStrIndex的返回值可能是-1。我应该在此处理异常。
	int index = GetSubStrIndex(name, str, length);
	if(index == -1){
		// TODO
		printf("an exception in %d in %s\n", __LINE__, __FILE__);
	}
	
	Elf32_Word stName = (Elf32_Word)index; 
	
	return stName;
}

// 究竟是用返回值呢还是用参数？都能达到目的。
void GenerateSymtab(SectionDataNode symtabDataHead)
{
//	symtabLinkList = symtabDataHead->head;
	// 初始化节点。
	SectionDataNode symtabDataNode,preSymtabDataNode;
	preSymtabDataNode = symtabDataNode = NULL;


	int textSize = 0;
	int relTextEntryNum = 0;
	int dataSize = 0;
	int relDataEntryNum = 0;
	int rodataSize = 0;
	int symtabSize = 0;
	int strtabSize = 0;
	int shstrtabSize = 0;

	// TODO 上面的变量是我复制过来的。懒得马上区分哪些还在被使用。有空再优化。

	int symtabDataNodeIndex = 0;

		// TODO 为什么要这样做？我非常怀疑这样做是不是对的。
		// 因为GCC也是这样做的。
		// 在.symtab中加入一些常量节点。
	char *nodeNameArray[6] = {"UND", "ch.c", ".text", ".data", ".bss", ".rodata"};
	int nodeIndexArray[6] = {SECTION_NDX_UND,SECTION_NDX_ABS,TEXT,DATA,5,RO_DATA};
	int nodeSymbolTypeArray[6] = {
// todo 找机会去掉这些自定义常量，因为我已经定义过了。
//		SYMBOL_TYPE_NOTYPE, SYMBOL_TYPE_FILE, SYMBOL_TYPE_SECTION,
//		SYMBOL_TYPE_SECTION,SYMBOL_TYPE_SECTION,SYMBOL_TYPE_SECTION
		STT_NOTYPE, STT_FILE, STT_SECTION,
		STT_SECTION, STT_SECTION, STT_SECTION
	};

	SegmentInfo shStrtabSegmentInfoNode = FindSegmentInfoNode(".shstrtab");
	if(shStrtabSegmentInfoNode == NULL){
		shStrtabSegmentInfoNode = (SegmentInfo)MALLOC(sizeof(struct segmentInfo));
		strcpy(shStrtabSegmentInfoNode->name, ".shstrtab");
		preSegmentInfoNode->next = shStrtabSegmentInfoNode; 
		preSegmentInfoNode = shStrtabSegmentInfoNode; 
	}

	for(int i = 0; i < 6; i++){
		char *name = nodeNameArray[i];
		int nameLen = 0;
		if(i == 0){
			nameLen = 1;
		}else{
			nameLen = strlen(name) + 1;
		}	
		shStrtabSegmentInfoNode->size += nameLen;	


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
			symtabDataNode->val.Elf32_Sym_Val = sym;
			symtabDataHead->next = symtabDataNode;

			symLinkList = symtabDataNode;	
			preSym = symtabDataNode;
		}else{
			strcpy(symtabDataNode->name, name);
			symtabDataNode->val.Elf32_Sym_Val = sym;
			preSymtabDataNode->next = symtabDataNode;

			preSym->next = symtabDataNode;
			preSym = symtabDataNode;
		}

//		symtabDataNode->val.Elf32_Sym_Val = sym;
		symtabDataNode->index = symtabDataNodeIndex++;
		symtabDataNode->isLast = 0;
		preSymtabDataNode = symtabDataNode;
		symtabDataNode = (SectionDataNode)MALLOC(sectionDataNodeSize);
		symtabDataNode->isLast = 1;
	}

	StrtabEntry strtabEntryNode = strtabEntryList;
	while(1){
		if(strtabEntryNode == NULL){
			break;
		}
		char *name = strtabEntryNode->name;
		DataEntry entry = FindDataEntry(name);
		// todo 需要设置sym的成员的值。
		Elf32_Sym *sym = (Elf32_Sym *)MALLOC(sizeof(Elf32_Sym));
		
		Elf32_Word nameIndex = GetStName(name);
		printf("nameIndex = %d, name = %s\n", nameIndex, name);
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
		printf("name = %s, st_shndx = %d\n", name, entry->section);

		int sectionDataNodeSize = sizeof(struct sectionDataNode);
		if(symtabDataNode == NULL){
			symtabDataNode = (SectionDataNode)MALLOC(sectionDataNodeSize);
			strcpy(symtabDataNode->name, name);
			printf("name2 = %s, st_shndx = %d\n", symtabDataNode->name, entry->section);
			symtabDataNode->isLast = 1;
			symtabDataNode->val.Elf32_Sym_Val = sym;
			symtabDataHead->next = symtabDataNode;
		}else{
			strcpy(symtabDataNode->name, name);
			printf("name3 = %s, st_shndx = %d\n", symtabDataNode->name, entry->section);
			symtabDataNode->val.Elf32_Sym_Val = sym;
			preSymtabDataNode->next = symtabDataNode;
		}
		
//		symtabDataNode->val.Elf32_Sym_Val = sym;
		symtabDataNode->index = symtabDataNodeIndex++;
		preSymtabDataNode = symtabDataNode;
		symtabDataNode->isLast = 0;
		symtabDataNode = (SectionDataNode)MALLOC(sectionDataNodeSize);
		symtabDataNode->isLast = 1;
		
		
		strtabEntryNode = strtabEntryNode->next;
	}

	printf("over\n");
}

void GenerateRelText(SectionDataNode relTextDataHead)
{
	// TODO 我没有做错误检测，为了赶进度。
	Instruction instrDataNode = instrHead->next;
	SectionDataNode relDataDataNode, preRelDataDataNode;
	relDataDataNode = preRelDataDataNode = NULL;

	while(instrDataNode != NULL){
		RelTextEntry relTextEntry = instrDataNode->relTextEntry;
		// TODO 错误是怎么产生的？疏忽了我在instr.c中的设计：instrDataNode->relTextEntry可能是NULL。
		if(relTextEntry == NULL){
			instrDataNode = instrDataNode->next;
			continue;
		}
		// instrDataNode的唯一作用是从它里面获取relTextEntry。
		instrDataNode = instrDataNode->next;

		Elf32_Rel *rel = (Elf32_Rel *)MALLOC(sizeof(Elf32_Rel));
		rel->r_offset = relTextEntry->offset;	
		
		// TODO 
		SectionDataNode target = NULL;
		char *name = relTextEntry->name;
		SectionDataNode symtabDataNode = symLinkList;
		while(symtabDataNode != NULL){
			char *symName = symtabDataNode->name;
			if(strcmp(name, symName) == 0){
				target = symtabDataNode;
				break;
			}

			symtabDataNode = symtabDataNode->next;
		}

		if(target == NULL){
			// movl num, %eax
			// 如果num没有对应的符号，应该怎么设置它对应的rel.text？我还不知道。
			printf("todo in %d in %s\n", __LINE__, __FILE__);
			exit(-1);
		}else{
			// `r_info`占用4个字节，高24位是`.symtab`的条目的索引，低8位是重定位类型。
			int symbolIndex = target->index;
			rel->r_info = (symbolIndex << 8) | relTextEntry->relType;  
		}

		// 创建链表。
		SectionDataNode relDataDataNode = (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
		relDataDataNode->val.Elf32_Rel_Val = rel;
		if(relTextDataHead == NULL){
			relTextDataHead= (SectionDataNode)MALLOC(sizeof(struct sectionDataNode));
			relTextDataHead->next = relDataDataNode;
		}else{
			if(relTextDataHead->next == NULL){
				relTextDataHead->next = relDataDataNode;
			}else{
				preRelDataDataNode->next = relDataDataNode;
			}			
		} 

		preRelDataDataNode = relDataDataNode;
	}
}

void GenerateRelData(SectionDataNode relDataDataHead, SectionDataNode symtabDataHead)
{
	// 初始化节点。
    SectionDataNode relDataDataNode, preRelDataDataNode;

	relDataDataNode = NULL;	
	preRelDataDataNode = NULL;	

	int textSize = 0;
	int relTextEntryNum = 0;
	int dataSize = 0;
	int relDataEntryNum = 0;
	int rodataSize = 0;
	int symtabSize = 0;
	int strtabSize = 0;
	int shstrtabSize = 0;

	// TODO 上面的变量是我复制过来的。懒得马上区分哪些还在被使用。有空再优化。

	StrtabEntry strtabEntryNode = strtabEntryList;
	strtabEntryNode = strtabEntryList;
	int relDataDataNodeIndex = 0;
	while(strtabEntryNode != NULL){
		char *name = strtabEntryNode->name;
		DataEntry entry = FindDataEntry(name);

		int sectionDataNodeSize = sizeof(struct sectionDataNode);

		if(entry->isRel == 1){
			printf("rel.data.name = %s, line = %d\n", name, __LINE__);
			Elf32_Addr offset = (Elf32_Addr)entry->offset;
			int relocationType = R_386_32;
			// SectionDataNode dataNode = FindSymbolSectionDataNode(name, symtabDataHead);
			SectionDataNode dataNode = FindSymbolSectionDataNode(".rodata", symtabDataHead);
			if(dataNode == NULL){
				printf("error in .rel.data.name = %s, line = %d", name, __LINE__);
				exit(-1);
			}
//			int symbolIndex = dataNode->index;
//			char *nodeNameArray[6] = {"UND", "ch.c", ".text", ".data", ".bss", ".rodata"};
			int symbolIndex = RODATA_SYMTAB_INDEX; //RO_DATA;
//			Elf32_Word info = (Elf32_Word)((symbolIndex << 8) | relocationType);
			Elf32_Word info = 0;

			int dataEntryValueNodeNum = entry->dataEntryValueNodeNum;
			int dataEntryValueNodeSize = entry->dataTypeSize;
			for(int i = 0; i < dataEntryValueNodeNum; i++){

				Elf32_Rel *relEntry = (Elf32_Rel *)MALLOC(sizeof(Elf32_Rel));
				relEntry->r_offset = offset + dataEntryValueNodeSize * i;
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
		}
		strtabEntryNode = strtabEntryNode->next;
	}
}

Elf32_Off GetShOffset(char *name, SectionOffset sectionOffset)
{
	// .text
	// .data
	// .rodata
	// .symtab
	// .strtab
	// .rel.text
	// .rel.data
	// .shstrtab

	Elf32_Off sh_offset = 0x34;
	if(strcmp(name, ".text") == 0){

	}else if(strcmp(name, ".bss") == 0){
		// TODO 
		// sh_offset += sectionOffset->text;
	}else if(strcmp(name, ".data") == 0){
		sh_offset += sectionOffset->text;

	}else if(strcmp(name, ".rodata") == 0){
		sh_offset += sectionOffset->text;
		sh_offset += sectionOffset->data;

	}else if(strcmp(name, ".symtab") == 0){
		sh_offset += sectionOffset->text;
		sh_offset += sectionOffset->data;
		sh_offset += sectionOffset->rodata;

	}else if(strcmp(name, ".strtab") == 0){
		sh_offset += sectionOffset->text;
		sh_offset += sectionOffset->data;
		sh_offset += sectionOffset->rodata;
		sh_offset += sectionOffset->symtab;

	}else if(strcmp(name, ".rel.text") == 0){
		sh_offset += sectionOffset->text;
		sh_offset += sectionOffset->data;
		sh_offset += sectionOffset->rodata;
		sh_offset += sectionOffset->symtab;
		sh_offset += sectionOffset->strtab;

	}else if(strcmp(name, ".rel.data") == 0){
		sh_offset += sectionOffset->text;
		sh_offset += sectionOffset->data;
		sh_offset += sectionOffset->rodata;
		sh_offset += sectionOffset->symtab;
		sh_offset += sectionOffset->strtab; 
		sh_offset += sectionOffset->relText; 

	}else if(strcmp(name, ".shstrtab") == 0){
		sh_offset += sectionOffset->text;
		sh_offset += sectionOffset->data;
		sh_offset += sectionOffset->rodata;
		sh_offset += sectionOffset->symtab;
		sh_offset += sectionOffset->strtab; 
		sh_offset += sectionOffset->relText; 
		sh_offset += sectionOffset->relData; 

	}else{
		// TODO
	}

	return sh_offset;
}

// TODO 这个函数和GetShOffset非常相似，通过复制粘贴写的。
// 在这个函数中充斥大量硬编码，是不是需要修改？硬编码，有利于当前进度，不利于后续维护。
// sh_size，不符合本项目的命名规则，我只不过和业界的规则保持一致罢了。
Elf32_Word GetShSize(char *name, SectionOffset sectionOffset)
{
	// .text
	// .data
	// .rodata
	// .symtab
	// .strtab
	// .rel.text
	// .rel.data
	// .shstrtab

	Elf32_Off sh_size = 0;

	if(strcmp(name, ".text") == 0){

		sh_size = sectionOffset->text;
	}else if(strcmp(name, ".bss") == 0){
		// TODO 
		// sh_size = sectionOffset->text;
	}else if(strcmp(name, ".data") == 0){

		sh_size = sectionOffset->data;
	}else if(strcmp(name, ".rodata") == 0){
		sh_size = sectionOffset->rodata;

	}else if(strcmp(name, ".symtab") == 0){

		sh_size = sectionOffset->symtab;
	}else if(strcmp(name, ".strtab") == 0){

		sh_size = sectionOffset->strtab;
	}else if(strcmp(name, ".rel.text") == 0){

		sh_size = sectionOffset->relText; 
	}else if(strcmp(name, ".rel.data") == 0){

		sh_size = sectionOffset->relData; 
	}else if(strcmp(name, ".shstrtab") == 0){

		sh_size = sectionOffset->shstrtab;
	}else{
		// TODO
	}

	return sh_size;
}

void GenerateSectionHeaders(SectionDataNode sectionHeaderDataHead, SectionOffset sectionOffset)
{
	SectionDataNode preSectionHeaderData,sectionHeaderDataNode;
	preSectionHeaderData = sectionHeaderDataNode = NULL;

	for(int i = 0; i < SHSTRTAB_ENTRY_ARRAY_SIZE; i++){
		
		int sectionDataNodeSize = sizeof(struct sectionDataNode);

		// 创建链表。
		if(sectionHeaderDataHead == NULL){
			sectionHeaderDataHead = (SectionDataNode)MALLOC(sectionDataNodeSize);
			sectionHeaderDataNode = (SectionDataNode)MALLOC(sectionDataNodeSize);
			sectionHeaderDataHead->next = sectionHeaderDataNode;
		}else{
			sectionHeaderDataNode = (SectionDataNode)MALLOC(sectionDataNodeSize);
			if(sectionHeaderDataHead->next == NULL){
				sectionHeaderDataHead->next = sectionHeaderDataNode;
				preSectionHeaderData = sectionHeaderDataNode;
			}else{
				preSectionHeaderData->next = sectionHeaderDataNode;
			}
		}
		preSectionHeaderData = sectionHeaderDataNode;

		// 把填充好的Elf32_Shdr存储到当前sectionHeaderDataHead。这样做没有任何问题。
		Elf32_Shdr *shdr = (Elf32_Shdr *)MALLOC(sizeof(Elf32_Shdr));
		if(i == 0){
			sectionHeaderDataNode->val.Elf32_Shdr_Val = shdr;
			continue;
		}

		char *name = shstrtabEntryArray[i];
		// todo 当sh_name是-1时需要处理，我暂时懒得写这种代码。
		Elf32_Word sh_name=(Elf32_Word)FindShStrTabOffset(name);  
		Elf32_Word sh_type=(Elf32_Word)0;  
		Elf32_Word sh_flags=(Elf32_Word)0;    
		Elf32_Addr sh_addr=(Elf32_Word)0;  
		Elf32_Off 	  sh_offset = GetShOffset(name, sectionOffset);  
		Elf32_Word sh_size= GetShSize(name, sectionOffset);  
		Elf32_Word sh_link=(Elf32_Word)0;  
		Elf32_Word sh_info=(Elf32_Word)0;  
		Elf32_Word sh_addralign=(Elf32_Word)0;    
		Elf32_Word sh_entsize=(Elf32_Word)0;  

		// bss\data\rodata\shstrtab\strtab\symtab\text
		if(strcmp(name, ".bss") == 0){
		    sh_type=(Elf32_Word)SHT_NOBITS;
		    sh_flags=(Elf32_Word)SHF_ALLOC + SHF_WRITE;

			sh_addralign=(Elf32_Word)4;
		}else if(strcmp(name, ".data") == 0){
		    sh_type=(Elf32_Word)SHT_PROGBITS;
		    sh_flags=(Elf32_Word)SHF_ALLOC + SHF_WRITE;

			sh_addralign=(Elf32_Word)8;
		}else if(strcmp(name, ".rodata") == 0){
		    sh_type=(Elf32_Word)SHT_PROGBITS;
		    sh_flags=(Elf32_Word)SHF_ALLOC;

			sh_addralign=(Elf32_Word)1;
		}else if(strcmp(name, ".shstrtab") == 0){
		    sh_type=(Elf32_Word)SHT_STRTAB;
		    sh_flags=(Elf32_Word)0;      // none

			sh_addralign=(Elf32_Word)1;
		}else if(strcmp(name, ".strtab") == 0){
		    sh_type=(Elf32_Word)SHT_STRTAB;
		    // todo 不知道怎么处理。在《程序员的自我修养》3.4节有资料。
		    sh_flags=(Elf32_Word)0;

			sh_addralign=(Elf32_Word)1;
		}else if(strcmp(name, ".symtab") == 0){
		    sh_type=(Elf32_Word)SHT_SYMTAB;
		    // todo 不知道怎么处理。在《程序员的自我修养》3.4节有资料。
		    sh_flags=(Elf32_Word)0;

			sh_addralign=(Elf32_Word)4;
			sh_entsize=(Elf32_Word)sizeof(Elf32_Sym);  

			sh_link = (Elf32_Word)STR_TAB;
			// 花了很多很多时间才弄明白这个值是什么。它是LOCAL变量的数目。
			// sh_info = (Elf32_Word)SYM_TAB;
			sh_info = localVariableCount;
			
		}else if(strcmp(name, ".text") == 0){
		    sh_type=(Elf32_Word)SHT_PROGBITS;
		    sh_flags=(Elf32_Word)SHF_ALLOC + SHF_EXECINSTR;

			sh_addralign=(Elf32_Word)1;
		}else{
		    if(strcmp(name, ".rel.data") == 0 || strcmp(name, ".rel.text") == 0){
		        sh_type = SHT_REL;
				sh_addralign=(Elf32_Word)4;
				sh_entsize=(Elf32_Word)sizeof(Elf32_Rel);  
		    }
		    if(strcmp(name, ".rel.data") == 0){
				sh_link = (Elf32_Word)SYM_TAB;
				sh_info = (Elf32_Word)DATA;
			}

		    if(strcmp(name, ".rel.text") == 0){
				sh_link = (Elf32_Word)SYM_TAB;
				sh_info = (Elf32_Word)TEXT;
			}
		}
		
// TODO 我在前面根据段条目的名称设置了sh_link和sh_info。
// 那样做更好。按下面的做法，不能区分.rel.data和.rel.text，而它们的值是不一样的。
// 错误是怎么产生的？这里就有一个例子。
// 两三个月前，我写下了下面的代码，设置sh_link。几个月后，今天，我在前面写下了设置sh_link的代码。
// 新增代码时，我不记得我在几个月前在这里写了代码。执行代码时，旧代码覆盖了新代码设置的值。
// 我在新增代码时，没有考虑到我不记得的旧情况，错误就产生了。我若知道后面的代码，在新增代码时就会处
// 理好旧代码。又可以描述为这种情况：主观世界和客观世界不吻合，错误就产生了。
		// todo 还没有设置好值。我不知道怎么设置。在《程序员的自我修养》的3.4.2的表3-11有相关资料。
//		if(sh_type == SHT_REL){
//			sh_link=(Elf32_Word)0;
//			sh_info=(Elf32_Word)0;  
//		}else if(sh_type == SHT_STRTAB){
//			sh_link=(Elf32_Word)0;
//			sh_info=(Elf32_Word)0;  
//		}else{
//			sh_link=(Elf32_Word)SHN_UNDEF;
//			sh_info=(Elf32_Word)0;  
//		}

		// 赋值。我用正则表达式生成的。比较顺利。
		shdr->sh_name = sh_name;
		shdr->sh_type = sh_type;
		shdr->sh_flags = sh_flags;
		shdr->sh_addr = sh_addr;
		shdr->sh_offset = sh_offset;
		// shdr->sh_size = sh_size;
		shdr->sh_size = sh_size;
		shdr->sh_link = sh_link;
		shdr->sh_info = sh_info;
		shdr->sh_addralign = sh_addralign;
		shdr->sh_entsize = sh_entsize;

		sectionHeaderDataNode->val.Elf32_Shdr_Val = shdr;
	}
}

//	.data
unsigned int  WriteData(FILE *file, SectionDataNode dataDataHead)
{
	unsigned int len = 0;
	SectionDataNode dataDataNode = dataDataHead->next;

	while(dataDataNode != NULL && dataDataNode->isLast == 0){
		NumericData num = dataDataNode->val.numVal;
		int size = 0;
		switch(num.type){
			case EIGHT:
				size = 1;
				break;
			case SIXTEEN:
				size = 2;
				break;
			case THIRTY_TWO:
				size = 4;
				break;
			default:
				printf("name = %s, type = %d\n", dataDataNode->name, num.type);
				printf("error in %d in %s\n", __LINE__, __FILE__);
				exit(-1);
		}

		printf("name = %s, value = %lld\n", dataDataNode->name, num.value);

		int count = fwrite(&num.value, size, 1, file);
		len += count * size;

		dataDataNode = dataDataNode->next;
	}

	return len;
}
//	.rodata
unsigned int WriteRoData(FILE *file, SectionDataNode roDataDataHead)
{
	unsigned int len = 0;

	SectionDataNode rodataDataNode = roDataDataHead->next;

	while(rodataDataNode != NULL && rodataDataNode->isLast == 0){
		enum DataEntryValueType valType = rodataDataNode->valType;
		if(valType == STR){
			char *strVal = rodataDataNode->val.strVal; 
			int length = strlen(strVal) + 1;
			int count = fwrite(strVal, length, 1, file);
			len += count * length;
		}else{
			NumericData num = rodataDataNode->val.numVal;
			int size = 0;
			switch(num.type){
				case EIGHT:
					size = 1;
					break;
				case SIXTEEN:
					size = 2;
					break;
				case THIRTY_TWO:
					size = 4;
					break;
				default:
					printf("name = %s, type = %d\n", rodataDataNode->name, num.type);
					printf("error in %d in %s\n", __LINE__, __FILE__);
					exit(-1);
			}
			int count = fwrite(&num.value, size, 1, file);
			len += count * size;
		}
		rodataDataNode = rodataDataNode->next;	
	}

	return len;
}
//	.symtab
unsigned int WriteSymtab(FILE *file, SectionDataNode symtabDataHead)
{
	unsigned int len = 0;
	SectionDataNode symtabDataNode = symtabDataHead->next;

	printf("WriteSymtab start\n");
	while(symtabDataNode != NULL && symtabDataNode->isLast == 0){
		Elf32_Sym *symtabEntry = symtabDataNode->val.Elf32_Sym_Val;
		printf("st_name = %d\n", symtabEntry->st_name);
		unsigned int size = sizeof(Elf32_Sym);
		int count = fwrite(symtabEntry, size, 1, file);
		len += count * size;
		symtabDataNode = symtabDataNode->next;
	}
	printf("WriteSymtab end\n");

	return len;
}
//	.strtab
unsigned int WriteStrtab(FILE *file, SectionDataNode strtabDataHead)
{
	unsigned int len = 0;
//	StrtabEntry node = strtabEntryList;
//
//	while(node != NULL){
//		char *name = node->name;
//		unsigned int size = node->length + 1;
//		int count = fwrite(name, size, 1, file);	
//		len += count * size;
//		node = node->next;
//	}

	char *str = strtabString->string;
	unsigned int size = strtabString->length;	
	int count = fwrite(str, size, 1, file);	
	len = count * size;

	return len;
}
//	.rel.text
unsigned int WriteRelText(FILE *file, SectionDataNode relTextDataHead)
{
	unsigned int len = 0;
	SectionDataNode node = relTextDataHead->next;

	while(node != NULL && node->isLast == 0){
		char *name = node->name;
		Elf32_Rel *rel = node->val.Elf32_Rel_Val;
		unsigned int size = sizeof(Elf32_Rel);
		int count = fwrite(rel, size, 1, file);	
		len += count * size;
		node = node->next;
	}

	return len;
}
//	.rel.data
unsigned int WriteRelData(FILE *file, SectionDataNode relDataDataHead)
{
	unsigned int len = 0;
	SectionDataNode node = relDataDataHead->next;

	int c = 0;
	// while(node != NULL && node->isLast == 0){
	while(node != NULL){
		char *name = node->name;
		Elf32_Rel *rel = node->val.Elf32_Rel_Val;
		unsigned int size = sizeof(Elf32_Rel);
		int count = fwrite(rel, size, 1, file);	
		len += count * size;
		node = node->next;
		c++;
	}

	printf("c = %d\n", c);

	return len;
}
//	.shstrtab
unsigned int WriteShstrtab(FILE *file, SectionDataNode shstrtabDataHead)
{
//	char *arr[7] = {".rel.text",".rel.data",".bss",".rodata",".symtab",".strtab",".shstrtab"};
	unsigned int length = 0;
	for(int i = 0; i < shstrtabArr_SIZE; i++){
	//	length += strlen(arr[0]) + 1;
		length += strlen(shstrtabArr[i]) + 1;
	}
	// char *shStrTabStr = ".rel.text.rel.data.bss.rodata.symtab.strtab.shstrtab";
//	char *shStrTabStr = ".rel.text.\000rel.data\000.bss\000.rodata\000.symtab\000.strtab\000.shstrtab\000";

	unsigned int len = fwrite(shStrTabStr, length, 1, file);

	return len * length;

//	unsigned int len = 0;
//	// TODO 对于第一个元素, null，应该如何处理？
//	for(int i = 0; i < SHSTRTAB_ENTRY_ARRAY_SIZE; i++){
//		char *name = shstrtabEntryArray[i];
//		int size = strlen(name) + 1;
//
//		int count = fwrite(name, size, 1, file);
//		len += count * size;
//	}
//
//	return len;
}
// 段表
unsigned int WriteSectionHeaders(FILE *file, SectionDataNode sectionHeaderDataHead)
{
	unsigned int len = 0;
	// Elf32_Shdr *Elf32_Shdr_Val;
	SectionDataNode node = sectionHeaderDataHead->next;

	while(node != NULL && node->isLast == 0){
		Elf32_Shdr *shdr = node->val.Elf32_Shdr_Val;
		unsigned int size = sizeof(Elf32_Shdr);
		int count = fwrite(shdr, size, 1, file);
		len += count * size;

//		unsigned int arr[10] = {
//			5,0,0,0,0,
//			0,0,0,0,0
//		};
//		int count = fwrite(shdr, size, 1, file);
	
		node = node->next;
	}

	return len;
}

SectionOffset GetSectionOffset(SectionData sectionData)
{
unsigned int textOffset = 0;
unsigned int relTextOffset = 0;
unsigned int dataOffset = 0;
unsigned int relDataOffset = 0;
unsigned int rodataOffset = 0;
unsigned int symtabOffset = 0;
unsigned int strtabOffset = 0;
unsigned int shstrtabOffset = 0;
unsigned int sectionHeaderOffset = 0;




	SectionOffset sectionOffset = (SectionOffset)MALLOC(sizeof(struct sectionOffset));

	//	.data
	SectionDataNode dataDataHead = sectionData->data;
	//	.rodata
	SectionDataNode roDataDataHead = sectionData->rodata;
	//	.symtab
	SectionDataNode symtabDataHead = sectionData->symtab;
	//	.strtab
	SectionDataNode strtabDataHead = sectionData->strtab;
	//	.rel.text
	SectionDataNode relTextDataHead = sectionData->relText;
	//	.rel.data
	SectionDataNode relDataDataHead = sectionData->relData;
	//	.shstrtab
	SectionDataNode shstrtabDataHead = sectionData->shstrtab;
	//	 段表
	SectionDataNode sectionHeaderDataHead = sectionData->sectionHeader;

	// .text
//	unsigned int textOffset = 0x34;
	Instruction instrNode = instrHead->next;
	while(instrNode){
		if(instrNode->prefix != 0){
			textOffset += 1;
		}

	//  error: invalid initializer
	//	Opcode opcode = instrNode->opcode.primaryOpcode;

		unsigned char primaryOpcode = instrNode->opcode.primaryOpcode;
		textOffset += 1;

		char secondaryOpcode = instrNode->opcode.secondaryOpcode; 
		if(secondaryOpcode != -1){
			textOffset += 1;
		}

		if(instrNode->modRM != NULL){
			textOffset += 1;
		}

		if(instrNode->sib != NULL){
			textOffset += sizeof(struct sib) / 2;
		}

		// 偏移
		NumericData offset = instrNode->offset;
		OFFSET_TYPE type = offset.type;
		if(type != EMPTY){
			int value = offset.value;
			int size = 0;
			if(type == EIGHT){
				size = 1;
			}else if(type == SIXTEEN){
				size = 2;
			}else{
				size = 4;
			}
			textOffset += size;
		}
		// 立即数
		NumericData imm = instrNode->immediate;
		type = imm.type;
		if(type != EMPTY){
			int value = imm.value;
			int size = 0;
			if(type == EIGHT){
				size = 1;
			}else if(type == SIXTEEN){
				size = 2;
			}else{
				size = 4;
			}
			textOffset += size;
		}

		instrNode = instrNode->next;
	}

//	unsigned int relTextOffset = textOffset;
	SectionDataNode relTextNode = relTextDataHead->next;
	while(relTextNode != NULL && relTextNode->isLast == 0){
		Elf32_Rel *rel = relTextNode->val.Elf32_Rel_Val;
		unsigned int size = sizeof(Elf32_Rel);
		relTextOffset += size;
		relTextNode = relTextNode->next;
	}

//	unsigned int dataOffset = relTextOffset;
	SectionDataNode dataDataNode = dataDataHead->next;
	while(dataDataNode != NULL && dataDataNode->isLast == 0){
		NumericData num = dataDataNode->val.numVal;
		int size = 0;
		switch(num.type){
			case EIGHT:
				size = 1;
				break;
			case SIXTEEN:
				size = 2;
				break;
			case THIRTY_TWO:
				size = 4;
				break;
			default:
				printf("name = %s, type = %d\n", dataDataNode->name, num.type);
				printf("error in %d in %s\n", __LINE__, __FILE__);
				exit(-1);
		}

		dataOffset += size;

		dataDataNode = dataDataNode->next;
	}


//	unsigned int relDataOffset = dataOffset;
	SectionDataNode node = relDataDataHead->next;

	// while(node != NULL && node->isLast == 0){
	while(node != NULL){
		char *name = node->name;
		Elf32_Rel *rel = node->val.Elf32_Rel_Val;
		unsigned int size = sizeof(Elf32_Rel);
		relDataOffset += size;
		node = node->next;
	}


//	unsigned int rodataOffset = relDataOffset;
	SectionDataNode rodataDataNode = roDataDataHead->next;
	while(rodataDataNode != NULL && rodataDataNode->isLast == 0){
		enum DataEntryValueType valType = rodataDataNode->valType;
		if(valType == STR){
			char *strVal = rodataDataNode->val.strVal; 
			int len = strlen(strVal) + 1;
			rodataOffset += len;
		}else{
			NumericData num = rodataDataNode->val.numVal;
			int size = 0;
			switch(num.type){
				case EIGHT:
					size = 1;
					break;
				case SIXTEEN:
					size = 2;
					break;
				case THIRTY_TWO:
					size = 4;
					break;
				default:
					printf("name = %s, type = %d\n", rodataDataNode->name, num.type);
					printf("error in %d in %s\n", __LINE__, __FILE__);
					exit(-1);
			}
			rodataOffset += size;
		}
		rodataDataNode = rodataDataNode->next;	
	}


//	unsigned int symtabOffset = rodataOffset;
	SectionDataNode symtabDataNode = symtabDataHead->next;
	while(symtabDataNode != NULL && symtabDataNode->isLast == 0){
		Elf32_Sym *symtabEntry = symtabDataNode->val.Elf32_Sym_Val;
		unsigned int size = sizeof(Elf32_Sym);
		symtabOffset += size;
		symtabDataNode = symtabDataNode->next;
	}

//	unsigned int strtabOffset = symtabOffset;
	StrtabEntry strtabEntry = strtabEntryList;
	while(strtabEntry != NULL){
		char *name = strtabEntry->name;
		unsigned int size = strtabEntry->length + 1;
		strtabOffset += size;
		strtabEntry = strtabEntry->next;
	}
	// .strtab的第一个字符是\000。
	strtabOffset += 1;

//	unsigned int shstrtabOffset = strtabOffset;
//	for(int i = 0; i < SHSTRTAB_ENTRY_ARRAY_SIZE; i++){
//		char *name = shstrtabEntryArray[i];
//		int size = strlen(name) + 1;
//		shstrtabOffset += size;
//	}

//	char *arr[7] = {".rel.text",".rel.data",".bss",".rodata",".symtab",".strtab",".shstrtab"};
	unsigned int length = 0;
	for(int i = 0; i < shstrtabArr_SIZE; i++){
	//	length += strlen(arr[i]) + 1;
		length += strlen(shstrtabArr[i]) + 1;
	}
	// char *shStrTabStr = ".rel.text.rel.data.bss.rodata.symtab.strtab.shstrtab";
//	char *shStrTabStr = ".rel.text.\000rel.data\000.bss\000.rodata\000.symtab\000.strtab\000.shstrtab\000";
	shstrtabOffset += length;

//	unsigned int sectionHeaderOffset = shstrtabOffset;
	SectionDataNode sectionHeaderNode = sectionHeaderDataHead->next;

	int i = 0;
	while(sectionHeaderNode != NULL && sectionHeaderNode->isLast == 0){
		Elf32_Shdr *shdr = sectionHeaderNode->val.Elf32_Shdr_Val;
		unsigned int size = sizeof(Elf32_Shdr);
		sectionHeaderOffset += size;
		sectionHeaderNode = sectionHeaderNode->next;
		i++;
	}
	printf("i = %d\n", i);

	// 这是用正则表达式生成的代码。
	sectionOffset->text = textOffset;
	sectionOffset->relText = relTextOffset;
	sectionOffset->data = dataOffset;
	sectionOffset->relData = relDataOffset;
	sectionOffset->rodata = rodataOffset;
	sectionOffset->symtab = symtabOffset;
	sectionOffset->strtab = strtabOffset;
	sectionOffset->shstrtab = shstrtabOffset;
//	sectionOffset->sectionHeader = sectionHeaderOffset;

//	sectionOffset->text = textOffset;
//	sectionOffset->relText = textOffset;
//	sectionOffset->data = relTextOffset;
//	sectionOffset->relData = dataOffset;
//	sectionOffset->rodata = relDataOffset;
//	sectionOffset->symtab = rodataOffset;
//	sectionOffset->strtab = symtabOffset;
//	sectionOffset->shstrtab = strtabOffset;
//	sectionOffset->sectionHeader = shstrtabOffset;

	return sectionOffset;
}

void WriteELF(Elf32_Ehdr *ehdr, SectionOffset sectionOffset, SectionData sectionData)
{
	FILE *file;

	int len = 0;

	// SectionOffset sectionOffset = GetSectionOffset(sectionData);
	ehdr->e_shoff += 0x34;
	ehdr->e_shoff += sectionOffset->text;
	ehdr->e_shoff += sectionOffset->data;
	ehdr->e_shoff += sectionOffset->rodata;
	ehdr->e_shoff += sectionOffset->symtab;
	ehdr->e_shoff += sectionOffset->strtab;
	ehdr->e_shoff += sectionOffset->relText;
	ehdr->e_shoff += sectionOffset->relData;
	ehdr->e_shoff += sectionOffset->shstrtab;
	ehdr->e_shoff += sectionOffset->sectionHeader;


	file = fopen("my.o", "ab");
	if(file == NULL){
		printf("无法打开文件。\n");
		exit(-1);
	}

	// 开始生成ELF文件。
	// ELF文件头。
	unsigned int count = 0;
	count = fwrite(ehdr, sizeof(Elf32_Ehdr), 1, file);	
	// .text
	// preRelTextDataNode
	Instruction instrNode = instrHead->next;
//	instrNode = NULL;
	while(instrNode){
		if(instrNode->prefix != 0){
			count = fwrite(&(instrNode->prefix), 1, 1, file);
			len += count * 1;
		}

	//  error: invalid initializer
	//	Opcode opcode = instrNode->opcode.primaryOpcode;

		unsigned char primaryOpcode = instrNode->opcode.primaryOpcode;
		count = fwrite(&primaryOpcode, 1, 1, file);
		len += count * 1;

		char secondaryOpcode = instrNode->opcode.secondaryOpcode; 
		if(secondaryOpcode != -1){
			count = fwrite(&secondaryOpcode, 1, 1, file);
			len += count * 1;
		}

		if(instrNode->modRM != NULL){
			// count = fwrite(instrNode->modRM, sizeof(struct modRM), 1, file);
			count = fwrite(instrNode->modRM, 1, 1, file);
			len += count * 1;
		}

		if(instrNode->sib != NULL){
			// count = fwrite(instrNode->sib, sizeof(struct sib) / 2, 1, file);
			count = fwrite(instrNode->sib, 1, 1, file);
			len += count * 1;
		}

		// 偏移
		NumericData offset = instrNode->offset;
		OFFSET_TYPE type = offset.type;
		if(type != EMPTY){
			int value = offset.value;
			int size = 0;
			if(type == EIGHT){
				size = 1;
			}else if(type == SIXTEEN){
				size = 2;
			}else{
				size = 4;
			}
			count = fwrite(&value, size, 1, file);
			len += count * size;
		}
		// 立即数
		NumericData imm = instrNode->immediate;
		type = imm.type;
		if(type != EMPTY){
			int value = imm.value;
			int size = 0;
			if(type == EIGHT){
				size = 1;
			}else if(type == SIXTEEN){
				size = 2;
			}else{
				size = 4;
			}
			count = fwrite(&value, size, 1, file);
			len += count * size;
		}

		instrNode = instrNode->next;
	}

	printf("text size = %d\n", len);

//	unsigned int count = 0;

	// TODO 这是冗余的。我为了快点看效果才这样写。
//	SectionData sectionData = GetSectionData();
//	.data
	SectionDataNode dataDataHead = sectionData->data;
	count = WriteData(file, dataDataHead);
	printf("data size = %d\n", count);
//	.rodata
	SectionDataNode roDataDataHead = sectionData->rodata;
	count = WriteRoData(file, roDataDataHead);
	printf("rodata size = %d\n", count);
//	.symtab
	SectionDataNode symtabDataHead = sectionData->symtab;
	count = WriteSymtab(file, symtabDataHead);
	printf("symtab size = %d\n", count);
//	.strtab
	SectionDataNode strtabDataHead = sectionData->strtab;
	count = WriteStrtab(file, strtabDataHead);
	printf("strtab size = %d\n", count);
//	.rel.text
	SectionDataNode relTextDataHead = sectionData->relText;
	count = WriteRelText(file, relTextDataHead);
	printf("rel.text size = %d\n", count);
//	.rel.data
	SectionDataNode relDataDataHead = sectionData->relData;
	count = WriteRelData(file, relDataDataHead);
	printf("rel.data size = %d\n", count);
//	.shstrtab
	SectionDataNode shstrtabDataHead = sectionData->shstrtab;
	count = WriteShstrtab(file, shstrtabDataHead);
	printf("shstrtab size = %d\n", count);
//	 段表
	SectionDataNode sectionHeaderDataHead = sectionData->sectionHeader;
	count = WriteSectionHeaders(file, sectionHeaderDataHead);
	printf("sectionHeader size = %d\n", count);
}

void BuildELF()
{
	Elf32_Ehdr *ehdr = GenerateELFHeader();

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
	
	SectionData sectionData = GetSectionData();
//	SectionOffset sectionOffset = GetSectionOffset(sectionData);
	
	SectionDataNode symtabDataHead = sectionData->symtab;
	SectionDataNode relDataDataHead = sectionData->relData;
	SectionDataNode relTextDataHead = sectionData->relText;
	SectionDataNode sectionHeaderDataHead = sectionData->sectionHeader;

	// .symtab
	GenerateSymtab(symtabDataHead);
	// .rel.data
	GenerateRelData(relDataDataHead, symtabDataHead);
	// .rel.text
	GenerateRelText(relTextDataHead);
//	GetSectionOffset遍历链表。这些链表由上面的Generate创建。所以，必须先执行Generate。
	SectionOffset sectionOffset = GetSectionOffset(sectionData);
	// 段表
	GenerateSectionHeaders(sectionHeaderDataHead, sectionOffset);
	

	printf("BuildELF is over\n");

	// 把数据写入文件。这个文件就是生成的可重定位文件。
	WriteELF(ehdr, sectionOffset, sectionData);
}

int main(int argc, char *argv[])
{
//	int len = myStrlen("你好");
//	CurrentHeap = &ProgramHeap;

	segmentInfoNode = (SegmentInfo)MALLOC(sizeof(struct segmentInfo));
	preSegmentInfoNode = segmentInfoNode;

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


	// todo 我不知道怎么重置扫描器。像这样做碰碰运气。
	lexer = (Lexer)MALLOC(sizeof(struct _lexer));
	LoadFile(filename);

	while(True){
		int nextToken = GetLookAheadToken();
		if(!IsData(nextToken)){
			ParseInstr();
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
