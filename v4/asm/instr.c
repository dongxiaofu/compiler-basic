#include "instr.h"

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
