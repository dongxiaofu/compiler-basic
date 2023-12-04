#include "instr.h"

OprandType GetOprandType()
{
	StartPeekToken();	

	unsigned char leftParenthesis = 0;
	unsigned char rightParenthesis = 0;
	unsigned char commaCount = 0;
	
	int token, preToken;
	preToken = -1;

	while(1){
		token = GetNextToken();
		// 在所有的token中都没有换行符。原因是什么？我不知道。一时半会儿我理解不了去年写的GetNextToken。
//		if(token == TYPE_TOKEN_NEWLINE){
//			break;
//		}
//		我认为SIB只能有逗号、寄存器、括号、数字组成。
//		我修改了这个函数。我认为操作数的构成要素只能是逗号、寄存器、括号、标识符、立即数、数字。
//		TODO token == TYPE_TOKEN_INDENT 可能有问题。我有没有把指令从标识符中分离出来？
		if(!(token == TYPE_TOKEN_REGISTER || token == TYPE_TOKEN_INT \
			|| token == TYPE_TOKEN_OPEN_PARENTHESES	\
			|| token == TYPE_TOKEN_COMMA			\
			|| token == TYPE_TOKEN_INT			\
			|| token == TYPE_TOKEN_IMMEDIATE			\
			|| token == TYPE_TOKEN_INDENT			\
			|| token == TYPE_TOKEN_CLOSE_PARENTHESES)){

			break;
		}

		if(token == TYPE_TOKEN_INDENT){
			// 如果是指令，需要断开。
			// TODO 最好的方法是在GetNextToken识别token是不是指令，把指令和普通的indent区分开。
			// 我不愿意现在花精力做这件事，因此像这样打补丁。
			char *name = GetCurrentTokenLexeme();
			InstructionSet instrCode = FindInstrCode(name);
			if(instrCode != I_INSTR_INVALID){
				break;
			}
		}

		if(token == TYPE_TOKEN_OPEN_PARENTHESES){
			leftParenthesis = 1;
		}
		
		if(token == TYPE_TOKEN_CLOSE_PARENTHESES){
			rightParenthesis = 1;
			break;
		}

		if(token == TYPE_TOKEN_COMMA){
			commaCount++;
		}

		preToken = token;
	}

	OprandType type;
	if(leftParenthesis & rightParenthesis){
		type = REG_BASE_MEM;	
		if(commaCount == 2){
			type = T_SIB;
		}
	}else{
		// 合法的操作数，如果不包含小括号，必定只有一个标识符。
		// 这个if..else if语句群可以改成多个if语句。
		if(preToken == TYPE_TOKEN_INT){
			type = IMM_BASE_MEM;
		}else if(preToken == TYPE_TOKEN_IMMEDIATE){
			type = IMM;
		}else if(preToken == TYPE_TOKEN_REGISTER){
			type = REG;
		}	
	}

	EndPeekToken();

	if(preToken == -1){
		printf("there is a invalid oprand in line %d in file %s\n", __LINE__, __FILE__);
		exit(-1);
	}

	return type;
}

SIB ParseSIB()
{
	StartPeekToken();
	int token = GetNextToken();
	int offset = -1;
	char *name;
	if(token == TYPE_TOKEN_INT){
		name = GetCurrentTokenLexeme();
		offset = StrToNumber(name);
	}
	EndPeekToken();

	// offset不是-1，表示第一个token是-4(%ebp)中的-4。应该跳过它。
	if(offset != -1){
		GetNextToken();
	}
	// 要处理小括号。
	// 一个SIB实例，(%ebx,%eax,2)
	SIB sib = (SIB)MALLOC(sizeof(struct sib));
	GetNextToken();		// 获取(
	GetNextToken();		// 获取%ebx
	char *baseRegName = GetCurrentTokenLexeme();
	GetNextToken();		// 获取,
	GetNextToken();		// 获取%eax
	char *indexRegName = GetCurrentTokenLexeme();
	GetNextToken();		// 获取,
	GetNextToken();		// 获取2
	char *scaleName = GetCurrentTokenLexeme();
	GetNextToken();		// 获取)

	sib->base = FindRegIndex(++baseRegName);
	sib->index = FindRegIndex(++indexRegName);
	sib->scale = StrToNumber(scaleName);
	sib->offset = offset;
	
	return sib;
}

Oprand ParseOprand()
{
	OprandType type = GetOprandType();

	Oprand opr = (Oprand)MALLOC(sizeof(struct oprand));	

	// IMM, T_SIB, REG_BASE_MEM, IMM_BASE_MEM, REG
	if(type == IMM){
		int token = GetNextToken();
		char *name = GetCurrentTokenLexeme();
		name++;
		opr->value.immediate = StrToNumber(name);
		opr->type = IMM;
	}else if(type == T_SIB){
		SIB sib = ParseSIB();
		opr->value.sib = *sib;
	}else if(type == REG_BASE_MEM){
		StartPeekToken();
		int token = GetNextToken();
		int offset = -1;
		char *name;
		if(token == TYPE_TOKEN_INT){
			name = GetCurrentTokenLexeme();
			offset = StrToNumber(name);
		}
		EndPeekToken();
		
		// offset不是-1，表示第一个token是-4(%ebp)中的-4。应该跳过它。
		if(offset != -1){
			GetNextToken();
		}

		// 跳过(。
		GetNextToken();
		// 只能是寄存器，否则就不是合法的汇编代码。本项目只负责处理正确的汇编代码。
		int currentToken = GetNextToken();
		if(currentToken != TYPE_TOKEN_REGISTER){
			printf("%s is not a register in line %d\n", GetCurrentTokenLexeme(), __LINE__);
			exit(__LINE__);
		}
		// 计算寄存器的编号。
		// 前面获取的name是寄存器，%eax。
		name = GetCurrentTokenLexeme();
		name++;
		int regIndex = FindRegIndex(name);
		opr->value.regBaseMem.reg = regIndex;
		opr->value.regBaseMem.offset = offset;
		
		opr->type = REG_BASE_MEM;

	}else if(type == IMM_BASE_MEM){
		int token = GetNextToken();
		char *name = GetCurrentTokenLexeme();
		opr->value.immBaseMem = StrToNumber(name);
		opr->type = IMM_BASE_MEM;
	}else if(type == REG){
		// 计算寄存器的编号。
		// 前面获取的name是寄存器，%eax。
		int token = GetNextToken();
		char *name = GetCurrentTokenLexeme();
		name++;
		int regIndex = FindRegIndex(name);
		opr->value.reg = regIndex;
		opr->type = REG;
	} 

	return opr;
}

Instruction ParseFchsInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xD9, 0xE0};

	int size = sizeof(struct instruction);
	Instruction instr = (Instruction)MALLOC(size);
	instr->prefix = -1;
	instr->opcode = opcode;
	instr->modRM = NULL;
	instr->sib = NULL;
	instr->offset = -1;
	instr->immediate = -1;

	return instr;
}

Instruction ParseFldzInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xD9, 0xEE};

	int size = sizeof(struct instruction);
	Instruction instr = (Instruction)MALLOC(size);
	instr->prefix = -1;
	instr->opcode = opcode;
	instr->modRM = NULL;
	instr->sib = NULL;
	instr->offset = -1;
	instr->immediate = -1;

	return instr;
}

Instruction ParseFucomppInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xDA, 0xE9};

	int size = sizeof(struct instruction);
	Instruction instr = (Instruction)MALLOC(size);
	instr->prefix = -1;
	instr->opcode = opcode;
	instr->modRM = NULL;
	instr->sib = NULL;
	instr->offset = -1;
	instr->immediate = -1;

	return instr;
}

Instruction ParseFld1Instr(InstructionSet instrCode)
{
	Opcode opcode = {0xD9, 0xE8};

	int size = sizeof(struct instruction);
	Instruction instr = (Instruction)MALLOC(size);
	instr->prefix = -1;
	instr->opcode = opcode;
	instr->modRM = NULL;
	instr->sib = NULL;
	instr->offset = -1;
	instr->immediate = -1;

	return instr;
}

Instruction ParseFaddsInstr(InstructionSet instrCode)
{
	// d8 05 45 23 01 00    	fadds  0x12345
	
	Opcode opcode = {0xD8, -1};

	ModRM modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->mod = 0;
	modRM->regOrOpcode = 0;
	// vim没有高亮0b101，但这种写法是正确的。
	// 为什么是101？有空时写上注释。
	modRM->rm = 0b101;

	// 只有一个操作数需要处理。
	int token = GetNextToken();
	// 这个操作数是一个内存地址，应该存储到偏移字段。
	// 这个操作数是不是合法的字符串数值，不是本函数甚至本程序的职责，不理会。
	char *name = GetCurrentTokenLexeme();
	int offset = StrToNumber(name);

	int size = sizeof(struct instruction);
	Instruction instr = (Instruction)MALLOC(size);
	instr->prefix = -1;
	instr->opcode = opcode;
	instr->modRM = modRM;
	instr->sib = NULL;
	instr->offset = offset;
	instr->immediate = -1;

	return instr;
}

Instruction ParseFaddlInstr(InstructionSet instrCode)
{
	// dc 05 45 23 01 00    	fadds  0x12345
	
	Opcode opcode = {0xDC, -1};

	ModRM modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->mod = 0;
	modRM->regOrOpcode = 0;
	// vim没有高亮0b101，但这种写法是正确的。
	// 为什么是101？有空时写上注释。
	modRM->rm = 0b101;

	// 只有一个操作数需要处理。
	int token = GetNextToken();
	// 这个操作数是一个内存地址，应该存储到偏移字段。
	// 这个操作数是不是合法的字符串数值，不是本函数甚至本程序的职责，不理会。
	char *name = GetCurrentTokenLexeme();
	int offset = StrToNumber(name);

	int size = sizeof(struct instruction);
	Instruction instr = (Instruction)MALLOC(size);
	instr->prefix = -1;
	instr->opcode = opcode;
	instr->modRM = modRM;
	instr->sib = NULL;
	instr->offset = offset;
	instr->immediate = -1;

	return instr;
}

Instruction ParseFsubsInstr(InstructionSet instrCode)
{
	// d8 25 45 23 01 00    	fsubs  0x12345

	Opcode opcode = {0xD8, -1};

	ModRM modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->mod = 0;
	modRM->regOrOpcode = 4;
	// vim没有高亮0b101，但这种写法是正确的。
	// 为什么是101？有空时写上注释。
	modRM->rm = 0b101;

	// 只有一个操作数需要处理。
	int token = GetNextToken();
	// 这个操作数是一个内存地址，应该存储到偏移字段。
	// 这个操作数是不是合法的字符串数值，不是本函数甚至本程序的职责，不理会。
	char *name = GetCurrentTokenLexeme();
	int offset = StrToNumber(name);

	int size = sizeof(struct instruction);
	Instruction instr = (Instruction)MALLOC(size);
	instr->prefix = -1;
	instr->opcode = opcode;
	instr->modRM = modRM;
	instr->sib = NULL;
	instr->offset = offset;
	instr->immediate = -1;

	return instr;
}

Instruction ParseFsublInstr(InstructionSet instrCode)
{
	// dc 25 45 23 01 00    	fsubl  0x12345
	
	Opcode opcode = {0xDC, -1};

	ModRM modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->mod = 0;
	modRM->regOrOpcode = 4;
	// vim没有高亮0b101，但这种写法是正确的。
	// 为什么是101？有空时写上注释。
	modRM->rm = 0b101;

	// 只有一个操作数需要处理。
	int token = GetNextToken();
	// 这个操作数是一个内存地址，应该存储到偏移字段。
	// 这个操作数是不是合法的字符串数值，不是本函数甚至本程序的职责，不理会。
	char *name = GetCurrentTokenLexeme();
	int offset = StrToNumber(name);

	int size = sizeof(struct instruction);
	Instruction instr = (Instruction)MALLOC(size);
	instr->prefix = -1;
	instr->opcode = opcode;
	instr->modRM = modRM;
	instr->sib = NULL;
	instr->offset = offset;
	instr->immediate = -1;

	return instr;
}

Instruction ParseFdivsInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xD8, -1};

	ModRM modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->mod = 0;
	modRM->regOrOpcode = 6;
	// vim没有高亮0b101，但这种写法是正确的。
	// 为什么是101？有空时写上注释。
	modRM->rm = 0b101;

	// 只有一个操作数需要处理。
	int token = GetNextToken();
	// 这个操作数是一个内存地址，应该存储到偏移字段。
	// 这个操作数是不是合法的字符串数值，不是本函数甚至本程序的职责，不理会。
	char *name = GetCurrentTokenLexeme();
	int offset = StrToNumber(name);

	int size = sizeof(struct instruction);
	Instruction instr = (Instruction)MALLOC(size);
	instr->prefix = -1;
	instr->opcode = opcode;
	instr->modRM = modRM;
	instr->sib = NULL;
	instr->offset = offset;
	instr->immediate = -1;

	return instr;
}

Instruction ParseFdivlInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xDC, -1};

	ModRM modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->mod = 0;
	modRM->regOrOpcode = 6;
	// vim没有高亮0b101，但这种写法是正确的。
	// 为什么是101？有空时写上注释。
	modRM->rm = 0b101;

	// 只有一个操作数需要处理。
	int token = GetNextToken();
	// 这个操作数是一个内存地址，应该存储到偏移字段。
	// 这个操作数是不是合法的字符串数值，不是本函数甚至本程序的职责，不理会。
	char *name = GetCurrentTokenLexeme();
	int offset = StrToNumber(name);

	int size = sizeof(struct instruction);
	Instruction instr = (Instruction)MALLOC(size);
	instr->prefix = -1;
	instr->opcode = opcode;
	instr->modRM = modRM;
	instr->sib = NULL;
	instr->offset = offset;
	instr->immediate = -1;

	return instr;
}

Instruction ParseFnstswInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xDF, 0xE0};

	int size = sizeof(struct instruction);
	Instruction instr = (Instruction)MALLOC(size);
	instr->prefix = -1;
	instr->opcode = opcode;
	instr->modRM = NULL;
	instr->sib = NULL;
	instr->offset = -1;
	instr->immediate = -1;

	return instr;
}

Instruction ParseFldsInstr(InstructionSet instrCode)
{
	// 0:	d9 05 45 23 01 00    	flds   0x12345
	
	Opcode opcode = {0xD9, -1};
	ModRM modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->mod = 0;
	modRM->regOrOpcode = 0;
	// vim没有高亮0b101，但这种写法是正确的。
	modRM->rm = 0b101;

	// 只有一个操作数需要处理。
	int token = GetNextToken();
	// 这个操作数是一个内存地址，应该存储到偏移字段。
	// 这个操作数是不是合法的字符串数值，不是本函数甚至本程序的职责，不理会。
	char *name = GetCurrentTokenLexeme();
	int offset = StrToNumber(name);

	int size = sizeof(struct instruction);
	Instruction instr = (Instruction)MALLOC(size);
	instr->prefix = -1;
	instr->opcode = opcode;
	instr->modRM = modRM;
	instr->sib = NULL;
	instr->offset = offset;
	instr->immediate = -1;

	return instr;
}

// todo 这个函数和ParseFldsInstr太相似了。找机会优化它。
Instruction ParseFldlInstr(InstructionSet instrCode)
{
	// dd 05 45 23 01 00    	fldl   0x12345
	
	Opcode opcode = {0xDD, -1};
	ModRM modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->mod = 0;
	modRM->regOrOpcode = 0;
	// vim没有高亮0b101，但这种写法是正确的。
	// 为什么是101？有空时写上注释。
	modRM->rm = 0b101;

	// 只有一个操作数需要处理。
	int token = GetNextToken();
	// 这个操作数是一个内存地址，应该存储到偏移字段。
	// 这个操作数是不是合法的字符串数值，不是本函数甚至本程序的职责，不理会。
	char *name = GetCurrentTokenLexeme();
	int offset = StrToNumber(name);

	int size = sizeof(struct instruction);
	Instruction instr = (Instruction)MALLOC(size);
	instr->prefix = -1;
	instr->opcode = opcode;
	instr->modRM = modRM;
	instr->sib = NULL;
	instr->offset = offset;
	instr->immediate = -1;

	return instr;
}

Instruction ParseFildlInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseFildqInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseFstpsInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseFstplInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseFldcwInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseFistplInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseFistpllInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseFstsInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseFstlInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseFstpInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseFmulsInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xD8, -1};

	ModRM modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->mod = 0;
	modRM->regOrOpcode = 6;
	// vim没有高亮0b101，但这种写法是正确的。
	// 为什么是101？有空时写上注释。
	modRM->rm = 0b101;

	// 只有一个操作数需要处理。
	int token = GetNextToken();
	// 这个操作数是一个内存地址，应该存储到偏移字段。
	// 这个操作数是不是合法的字符串数值，不是本函数甚至本程序的职责，不理会。
	char *name = GetCurrentTokenLexeme();
	int offset = StrToNumber(name);

	int size = sizeof(struct instruction);
	Instruction instr = (Instruction)MALLOC(size);
	instr->prefix = -1;
	instr->opcode = opcode;
	instr->modRM = modRM;
	instr->sib = NULL;
	instr->offset = offset;
	instr->immediate = -1;

	return instr;
}

Instruction ParseFmullInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xDC, -1};

	ModRM modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->mod = 0;
	modRM->regOrOpcode = 1;
	// vim没有高亮0b101，但这种写法是正确的。
	// 为什么是101？有空时写上注释。
	modRM->rm = 0b101;

	// 只有一个操作数需要处理。
	int token = GetNextToken();
	// 这个操作数是一个内存地址，应该存储到偏移字段。
	// 这个操作数是不是合法的字符串数值，不是本函数甚至本程序的职责，不理会。
	char *name = GetCurrentTokenLexeme();
	int offset = StrToNumber(name);

	int size = sizeof(struct instruction);
	Instruction instr = (Instruction)MALLOC(size);
	instr->prefix = -1;
	instr->opcode = opcode;
	instr->modRM = modRM;
	instr->sib = NULL;
	instr->offset = offset;
	instr->immediate = -1;

	return instr;


return NULL;
}

Instruction ParseCdqInstr(InstructionSet instrCode)
{
	Opcode opcode = {0x99, -1};

	int size = sizeof(struct instruction);
	Instruction instr = (Instruction)MALLOC(size);
	instr->prefix = -1;
	instr->opcode = opcode;
	instr->modRM = NULL;
	instr->sib = NULL;
	instr->offset = -1;
	instr->immediate = -1;

	return instr;
}

Instruction ParseRetInstr(InstructionSet instrCode)
{
	GetNextToken();
	// unsigned char opcode = 0xC3;
	Opcode opcode = {0xC3, -1};

	int size = sizeof(struct instruction);
	Instruction instr = (Instruction)MALLOC(size);
	instr->prefix = -1;
	instr->opcode = opcode;
	instr->modRM = NULL;
	instr->sib = NULL;
	instr->offset = -1;
	instr->immediate = -1;

	return instr;
}

Instruction ParseMullInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseIdivlInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseDivlInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseNeglInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseNotlInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseJeInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseJnpInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseJneInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseJpInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseJgInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseJaInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseJlInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseJbInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseJgeInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseJaeInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseJleInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseJbeInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseJmpInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParsePushlInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParsePoplInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseIncbInstr(InstructionSet instrCode)
{
	Oprand opr = ParseOprand();

return NULL;
}

Instruction ParseIncwInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseInclInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseDecbInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseDecwInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseDeclInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseRepInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseCallInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseOrlInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseXorlInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseAndlInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseShllInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseSHLDInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseSarlInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseShrlInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseAddlInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseSublInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseImullInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseMovlInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseMovbInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseMovwInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseCmplInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseTestInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseMovsblInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseMovswlInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseMovzblInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseMovzwlInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseMovzxInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseLealInstr(InstructionSet instrCode)
{


return NULL;
}
