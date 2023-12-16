#include "instr.h"

OFFSET_TYPE GetOffsetType(int offset)
{
//	if(0 <= offset && offset <= 256){
//		return EIGHT;
//	}
//
//	if(257 <= offset && offset <= 65535){
//		return SIXTEEN;
//	}

	if(0 <= offset && offset <= 127){
		return EIGHT;
	}

	if(128 <= offset && offset <= 32768){
		return SIXTEEN;
	}

	return THIRTY_TWO;
}

OprandType GetOprandType()
{
	StartPeekToken();	

	unsigned char leftParenthesis = 0;
	unsigned char rightParenthesis = 0;
	unsigned char commaCount = 0;

	unsigned char isFirstInstr = 1;
	
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
			|| token == TYPE_TOKEN_STAR			\
			|| token == TYPE_TOKEN_CLOSE_PARENTHESES)){

			break;
		}

		if(leftParenthesis == 0 && token == TYPE_TOKEN_COMMA){
			break;
		}

		if(token == TYPE_TOKEN_INDENT){
			// 如果是指令，需要断开。
			// TODO 最好的方法是在GetNextToken识别token是不是指令，把指令和普通的indent区分开。
			// 我不愿意现在花精力做这件事，因此像这样打补丁。
			char *name = GetCurrentTokenLexeme();
			InstructionSet instrCode = FindInstrCode(name);
			// if(isFirstInstr != 1 && instrCode != I_INSTR_INVALID){
			if(instrCode != I_INSTR_INVALID){
				break;
			}

			if(strcmp(name, "st") == 0){
				token = GetNextToken();			// (
				if(token == TYPE_TOKEN_OPEN_PARENTHESES){
					token = GetNextToken();		// 0
					if(token == IMM){
						token = GetNextToken();	// )
						if(token == TYPE_TOKEN_CLOSE_PARENTHESES){
							// TODO 如果不能满足所有条件，怎么办？
							return T_ST;
						}
					}
				}
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
		}else if(preToken == TYPE_TOKEN_INDENT){
			type = IDENT;
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
	char hasStar = 0;
	char ch = GetLookAheadChar();
	if(ch == '*'){
		hasStar = 1;
		// 跳过星号*。
		// 例如，jmpl    *512(,%eax, 8) 。
		// TODO 不知道会不会连512一起跳过了。
		GetNextToken();
	}

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
	// 一个SIB实例，(%ebx,%eax,2)。
	SIB sib = (SIB)MALLOC(sizeof(struct sib));
	GetNextToken();		// 获取(
	// SIB实例，*512(,%eax, 8)。
	char *baseRegName = "%ebp";
	if(hasStar == 1){
		// SIB实例，*512(,%eax, 8)。
		// char *baseRegName = "%ebp";
	}else{
		GetNextToken();		// 获取%ebx
		baseRegName = GetCurrentTokenLexeme();
	}
	
	GetNextToken();		// 获取,
	GetNextToken();		// 获取%eax
	char *indexRegName = GetCurrentTokenLexeme();
	GetNextToken();		// 获取,
	GetNextToken();		// 获取2
	char *scaleName = GetCurrentTokenLexeme();
	GetNextToken();		// 获取)

	sib->base = FindRegIndex(++baseRegName)->index;
	sib->index = FindRegIndex(++indexRegName)->index;
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
		// TODO 这里应该判断regInfo是不是NULL，我懒得马上写。
		RegInfo regInfo = FindRegIndex(name);
		opr->value.regBaseMem.reg = regInfo->index;
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
		opr->value.reg = FindRegIndex(name);
		opr->type = REG;
	}else if(type == T_ST){
		// st(0)
		GetNextToken();				//	跳过st
		GetNextToken();				//	跳过(
		GetNextToken();				//	获取 0
		char *name = GetCurrentTokenLexeme();
		GetNextToken();				//	跳过)
		opr->type = T_ST;
		opr->value.stIndex = StrToNumber(name);
	}else{
		// 跳过Mov num, %eax中的num。
		GetNextToken();
		opr->value.immIndent = GetCurrentTokenLexeme();
		opr->type = IDENT;
	}

	// 下面的代码生成调试用的数据。
	// TODO 为immStr等分配空间时设置的33等值需优化：1. 不应该用硬编码；2. 这些值可能不正确。
	if(type == IMM){
		int imm = opr->value.immediate;
		char *immStr = (char *)MALLOC(33);
		sprintf(immStr, "%d", imm); 
		opr->oprandStr = immStr;
	}else if(type == T_SIB){
		char *fmt = "%d(%s, %s, %d)";
		char *str = (char *)MALLOC(20);
		SIB sib = &(opr->value.sib);
		
		sprintf(str, fmt, sib->offset, sib->base, sib->index, sib->scale); 
		opr->oprandStr = str;
	}else if(type == REG_BASE_MEM){
		// char *fmt = "%d(%s)";
		char *fmt = "%d(%d)";
		char *str = (char *)MALLOC(20);
		RegInfo reg = opr->value.reg;
		MemoryAddress regBaseMem = &(opr->value.regBaseMem);
		
		// TODO 有时间时把寄存器编号换成寄存器名称。
		sprintf(str, fmt, regBaseMem->offset, regBaseMem->reg);
		opr->oprandStr = str;
	}else if(type == IMM_BASE_MEM){
		char *fmt = "%d";
		char *str = (char *)MALLOC(32);
		int imm = opr->value.immediate;
		
		sprintf(str, fmt, imm);
		opr->oprandStr = str;
	}else if(type == REG){
		char *fmt = "%s";
		char *str = (char *)MALLOC(20);
		RegInfo reg = opr->value.reg;
		
		sprintf(str, fmt, reg->name);
		opr->oprandStr = str;
	}else if(type == T_ST){
		char *fmt = "ST(%d)";
		char *str = (char *)MALLOC(20);
		int stIndex = opr->value.stIndex;
		
		sprintf(str, fmt, stIndex);
		opr->oprandStr = str;
	}else{
		opr->oprandStr = opr->value.immIndent;
	}

	return opr;
}

Instruction GenerateSimpleInstr(int prefix, Opcode opcode, ModRM modRM,\
	 SIB sib, OffsetInfo offsetInfo, NumericData immediate)
{
	int size = sizeof(struct instruction);
	Instruction instr = (Instruction)MALLOC(size);
	instr->prefix = prefix;
	instr->opcode = opcode;
	instr->modRM = modRM;
	instr->sib = sib;
	// instr.c:274:18: error: expected expression before '{' token
	// instr->offset = {EMPTY, 0};
	instr->offset.type = EMPTY;
	instr->offset.value = 0;

	instr->relTextEntry = NULL;
	instr->immediate = immediate;

	offsetInInstr += prefix != 0 ? 1 : 0;

	offsetInInstr += opcode.primaryOpcode != -1 ? 1 : 0;
	offsetInInstr += opcode.secondaryOpcode != -1 ? 1 : 0;

	offsetInInstr += modRM != NULL ? 1 : 0;
	offsetInInstr += sib != NULL ? 1 : 0;

	// TODO 这里，就是搜集指令中的重定位的地方。
	int offset = 0;
	RelTextEntry entry = NULL;
	if(offsetInfo != NULL){
		if(offsetInfo->name != NULL){
			entry = (RelTextEntry)MALLOC(sizeof(struct relTextEntry));
			// TODO 这个值是否正确？是正确的。它是在指令中的偏移量。
			// 在重定位信息中，需要这个值。
			entry->offset = offsetInInstr;
			entry->name = offsetInfo->name;

			// 先把.rel.text中的重定位类型都设置成R_386_32，在外面再把call等的重定位修正为
			// R_386_PC32。
			entry->relType = R_386_32;

			// instr->relTextEntry = entry;
		}else{
			instr->offset.value = offsetInfo->offset;
		}
	}

	instr->relTextEntry = entry;

	// r/m的寻址模式是32位直接寻址。
	if(modRM != NULL && modRM->mod == 0b00 && modRM->rm == 0b101){
		offsetInInstr += 4;
	}

//	// TODO 这里，就是搜集指令中的重定位的地方。
//	int offset = 0;
//	if(offsetInfo != NULL){
//		RelTextEntry entry = NULL;
//		if(offsetInfo->name != NULL){
//			entry = (RelTextEntry)MALLOC(sizeof(struct relTextEntry));
//			entry->offset = offsetInInstr;
//			entry->name = offsetInfo->name;
//
//			instr->relTextEntry = entry;
//		}else{
//			instr->offset = offsetInfo->offset;
//		}
//	}

	if(offset != 0 && modRM != NULL){
		if(modRM->mod == 0b01){
			offsetInInstr += 1;
		}else if(modRM->mod == 0b10){
			offsetInInstr += 4;
		}else{
			// TODO 不需要做任何处理。
		}
	}

	if(modRM == NULL){
		if(immediate.type != EMPTY){
			int len = 0;
			if(immediate.type == EIGHT){
				len = 1;
			}else if(immediate.type == SIXTEEN){
				len = 2;
			}else{
				len = 4;
			}
			offsetInInstr += len;
		}
	}

	if(immediate.type != EMPTY){
		OFFSET_TYPE immediateType = immediate.type;
		int size = 0;
		if(immediateType == EIGHT){
			size = 1;
		}else if(immediateType == SIXTEEN){
			size = 2;
		}else if (immediateType == THIRTY_TWO){
			size = 4;
		}else{
			// TODO 并不存在这种情况。
		}

		offsetInInstr += size;
	}

	return instr;
}

MemoryInfo GetMemoryInfo(Oprand opr)
{
	MemoryInfo mem = (MemoryInfo)MALLOC(sizeof(struct memoryInfo));

	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	int mod = 0;
	int rm = 0;

	OprandType type = opr->type;

	// 在SIB、寄存器基址中，如果偏移量是0，在指令中就无需包含偏移。
	if(type == T_SIB){
		sib = &(opr->value.sib);
		int offset = sib->offset;
		if(offset == 0){
			mod = 0b00;
		}else{
			offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
			offsetInfo->type = THIRTY_TWO;
			offsetInfo->offset = offset;
			OFFSET_TYPE offsetType = GetOffsetType(offset);
			if(offsetType == EIGHT){
				mod = 0b01;
			}else{
				mod = 0b10;
			}
		}
		rm = 0b100;
	}else if(type == REG_BASE_MEM){
		struct memoryAddress regBaseMem = opr->value.regBaseMem;
		int offset = regBaseMem.offset; 
		if(offset == 0){
			mod = 0b00;
		}else{
			offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
			offsetInfo->type = THIRTY_TWO;
			offsetInfo->offset = offset;
			OFFSET_TYPE offsetType = GetOffsetType(offset);
			if(offsetType == EIGHT){
				mod = 0b01;
			}else{
				mod = 0b10;
			}
		}

		rm = regBaseMem.reg;
	}else if(type == IMM_BASE_MEM || type == IDENT){
		mod = 0b00;
		rm = 0b101;

		offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
		offsetInfo->type = THIRTY_TWO;

		if(type == IMM_BASE_MEM){
			offsetInfo->offset = opr->value.immBaseMem;
		}

		if(type == IDENT){
			offsetInfo->name = opr->value.immIndent;
		}
	}

	mem->sib = sib;
	mem->mod = mod;
	mem->rm = rm;
	mem->offsetInfo = offsetInfo;

	return mem;
}

Instruction ParseFchsInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xD9, 0xE0};

	NumericData immediate = {EMPTY, 0};

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, NULL, NULL, 0, immediate);
}

Instruction ParseFldzInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xD9, 0xEE};
	NumericData immediate = {EMPTY, 0};

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, NULL, NULL, 0, immediate);
}

Instruction ParseFucomppInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xDA, 0xE9};
	NumericData immediate = {EMPTY, 0};

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, NULL, NULL, 0, immediate);
}

Instruction ParseFld1Instr(InstructionSet instrCode)
{
	Opcode opcode = {0xD9, 0xE8};
	NumericData immediate = {EMPTY, 0};

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, NULL, NULL, 0, immediate);
}

Instruction ParseFaddsInstr(InstructionSet instrCode)
{
	// d8 05 45 23 01 00    	fadds  0x12345
	
	Opcode opcode = {0xD8, -1};
	NumericData immediate = {EMPTY, 0};

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
	OffsetInfo offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = offset;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
}

Instruction ParseFaddlInstr(InstructionSet instrCode)
{
	// dc 05 45 23 01 00    	fadds  0x12345
	
	Opcode opcode = {0xDC, -1};
	NumericData immediate = {EMPTY, 0};

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
	OffsetInfo offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = offset;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
}

Instruction ParseFsubsInstr(InstructionSet instrCode)
{
	// d8 25 45 23 01 00    	fsubs  0x12345

	Opcode opcode = {0xD8, -1};
	NumericData immediate = {EMPTY, 0};

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
	OffsetInfo offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = offset;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
}

Instruction ParseFsublInstr(InstructionSet instrCode)
{
	// dc 25 45 23 01 00    	fsubl  0x12345
	
	Opcode opcode = {0xDC, -1};
	NumericData immediate = {EMPTY, 0};

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
	OffsetInfo offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = offset;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
}

Instruction ParseFdivsInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xD8, -1};
	NumericData immediate = {EMPTY, 0};

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
	OffsetInfo offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = offset;	

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
}

Instruction ParseFdivlInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xDC, -1};
	NumericData immediate = {EMPTY, 0};

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
	OffsetInfo offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = offset;	

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
}

Instruction ParseFnstswInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xDF, 0xE0};
	NumericData immediate = {EMPTY, 0};

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, NULL, NULL, NULL, immediate);
}

Instruction ParseFldsInstr(InstructionSet instrCode)
{
	// 0:	d9 05 45 23 01 00    	flds   0x12345
	NumericData immediate = {EMPTY, 0};

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
	OffsetInfo offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = offset;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
}

// todo 这个函数和ParseFldsInstr太相似了。找机会优化它。
Instruction ParseFldlInstr(InstructionSet instrCode)
{
	// dd 05 45 23 01 00    	fldl   0x12345
	NumericData immediate = {EMPTY, 0};

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
	OffsetInfo offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = offset;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
}

Instruction ParseFildlInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {0xDB, -1};
	// int offset = 0;
	ModRM modRM = NULL;
	SIB sib = NULL;
	NumericData immediate = {EMPTY, 0};

	modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->regOrOpcode = 0;

	Oprand oprand = ParseOprand();
	MemoryInfo mem = GetMemoryInfo(oprand);
	modRM->mod = mem->mod;
	modRM->rm = mem->rm;
	// offset = mem->offset;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, 	modRM, sib, mem->offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = oprand;

	return instr;
}

Instruction ParseFildqInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {0xDF, -1};
	// int offset = 0;
	ModRM modRM = NULL;
	SIB sib = NULL;
	NumericData immediate = {EMPTY, 0};

	modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->regOrOpcode = 5;

	Oprand oprand = ParseOprand();
	MemoryInfo mem = GetMemoryInfo(oprand);
	modRM->mod = mem->mod;
	modRM->rm = mem->rm;
	// offset = mem->offset;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, 	modRM, sib, mem->offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = oprand;

	return instr;
}

Instruction GenerateFstpEtcInstr(InstructionSet instrCode, int primaryOpcode)
{
	int prefix = 0;
	Opcode opcode = {primaryOpcode, -1};
	// int offset = 0;
	ModRM modRM = NULL;
	SIB sib = NULL;
	NumericData immediate = {EMPTY, 0};

	modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->regOrOpcode = 3;

	Oprand oprand = ParseOprand();
	MemoryInfo mem = GetMemoryInfo(oprand);
	modRM->mod = mem->mod;
	modRM->rm = mem->rm;
	// offset = mem->offset;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, 	modRM, sib, mem->offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = oprand;

	return instr;
}

Instruction ParseFstpsInstr(InstructionSet instrCode)
{
	// GenerateFstpEtcInstr(InstructionSet instrCode, int primaryOpcode)
	return GenerateFstpEtcInstr(instrCode, 0xD9);
}

Instruction ParseFstplInstr(InstructionSet instrCode)
{
	// GenerateFstpEtcInstr(InstructionSet instrCode, int primaryOpcode)
	return GenerateFstpEtcInstr(instrCode, 0xDD);
}

Instruction ParseFstpInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {0xDD, 0xD8};
	int offset = 0;
	ModRM modRM = NULL;
	SIB sib = NULL;
	NumericData immediate = {EMPTY, 0};

	// dd d9                	fstp   %st(1)
	Oprand oprand = ParseOprand();
	opcode.secondaryOpcode += oprand->value.stIndex;

	OffsetInfo offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = offset;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, 	modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = oprand;

	return instr;
}

Instruction ParseFldcwInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {0xD9, -1};
	// int offset = 0;
	ModRM modRM = NULL;
	SIB sib = NULL;
	NumericData immediate = {EMPTY, 0};

	modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->regOrOpcode = 5;

	Oprand oprand = ParseOprand();
	MemoryInfo mem = GetMemoryInfo(oprand);
	modRM->mod = mem->mod;
	modRM->rm = mem->rm;
	// offset = mem->offset;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, 	modRM, sib, mem->offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = oprand;

	return instr;
}

Instruction GenerateFistpInstr(InstructionSet instrCode, int primaryOpcode, int regOrOpcode)
{
	int prefix = 0;
	Opcode opcode = {primaryOpcode, -1};
	// int offset = 0;
	ModRM modRM = NULL;
	SIB sib = NULL;
	NumericData immediate = {EMPTY, 0};

	modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->regOrOpcode = regOrOpcode;

	Oprand oprand = ParseOprand();
	MemoryInfo mem = GetMemoryInfo(oprand);
	modRM->mod = mem->mod;
	modRM->rm = mem->rm;
	// offset = mem->offset;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, 	modRM, sib, mem->offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = oprand;

	return instr;
}

Instruction ParseFistplInstr(InstructionSet instrCode)
{
	// GenerateFistpInstr(InstructionSet instrCode, int primaryOpcode, int regOrOpcode)
	return GenerateFistpInstr(instrCode, 0xDB, 3);
}

Instruction ParseFistpllInstr(InstructionSet instrCode)
{
	// GenerateFistpInstr(InstructionSet instrCode, int primaryOpcode, int regOrOpcode)
	return GenerateFistpInstr(instrCode, 0xDF, 7);
}

Instruction ParseFstsInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {0xD9, -1};
	// int offset = 0;
	ModRM modRM = NULL;
	SIB sib = NULL;
	NumericData immediate = {EMPTY, 0};

	modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->regOrOpcode = 2;

	Oprand oprand = ParseOprand();
	MemoryInfo mem = GetMemoryInfo(oprand);
	modRM->mod = mem->mod;
	modRM->rm = mem->rm;
	// offset = mem->offset;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr =  GenerateSimpleInstr(prefix, opcode, 	modRM, sib, mem->offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = oprand;

	return instr;
}

Instruction ParseFstlInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {0xDD, -1};
	// int offset = 0;
	ModRM modRM = NULL;
	SIB sib = NULL;
	NumericData immediate = {EMPTY, 0};

	modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->regOrOpcode = 2;

	Oprand oprand = ParseOprand();
	MemoryInfo mem = GetMemoryInfo(oprand);
	modRM->mod = mem->mod;
	modRM->rm = mem->rm;
	// offset = mem->offset;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, 	modRM, sib, mem->offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = oprand;

	return instr;
}

Instruction ParseFmulsInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xD8, -1};
	NumericData immediate = {EMPTY, 0};

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

	OffsetInfo offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = offset;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
}

Instruction ParseFmullInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xDC, -1};
	NumericData immediate = {EMPTY, 0};

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
	OffsetInfo offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = offset;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
}

Instruction ParseCdqInstr(InstructionSet instrCode)
{
	Opcode opcode = {0x99, -1};
	NumericData immediate = {EMPTY, 0};

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, NULL, NULL, NULL, immediate);
}

Instruction ParseRetInstr(InstructionSet instrCode)
{
	GetNextToken();
	// unsigned char opcode = 0xC3;
	Opcode opcode = {0xC3, -1};
	NumericData immediate = {EMPTY, 0};

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, NULL, NULL, NULL, immediate);
}

Instruction ParseMullInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	opcode.primaryOpcode = 0xF7;

	modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->regOrOpcode = 4;

	Oprand opr = ParseOprand();
	OprandType type = opr->type;

	if(type == REG){
		RegInfo reg = opr->value.reg;
		char regSize = reg->size;

		if(regSize == 32){
			modRM->rm = reg->index;
		}else{
			// TODO 不会出现这种情况。
		}
	}else{
		MemoryInfo mem = GetMemoryInfo(opr);

		modRM->mod = mem->mod;
		modRM->rm = mem->rm;
		offsetInfo = mem->offsetInfo;
		sib = mem->sib;
	}

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = opr;

	return instr;
}

Instruction ParseIdivlInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	opcode.primaryOpcode = 0xF7;

	modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->regOrOpcode = 7;

	Oprand opr = ParseOprand();
	OprandType type = opr->type;

	if(type == REG){
		RegInfo reg = opr->value.reg;
		char regSize = reg->size;

		if(regSize == 32){
			modRM->rm = reg->index;
		}else{
			// TODO 不会出现这种情况。
		}
	}else{
		MemoryInfo mem = GetMemoryInfo(opr);

		modRM->mod = mem->mod;
		modRM->rm = mem->rm;
		offsetInfo = mem->offsetInfo;
		sib = mem->sib;
	}

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = opr;

	return instr;
}

Instruction ParseDivlInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	opcode.primaryOpcode = 0xF7;

	modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->regOrOpcode = 6;

	Oprand opr = ParseOprand();
	OprandType type = opr->type;

	if(type == REG){
		RegInfo reg = opr->value.reg;
		char regSize = reg->size;

		if(regSize == 32){
			modRM->rm = reg->index;
		}else{
			// TODO 不会出现这种情况。
		}
	}else{
		MemoryInfo mem = GetMemoryInfo(opr);

		modRM->mod = mem->mod;
		modRM->rm = mem->rm;
		offsetInfo = mem->offsetInfo;
		sib = mem->sib;
	}

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = opr;

	return instr;
}

Instruction ParseNeglInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	opcode.primaryOpcode = 0xF7;

	modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->regOrOpcode = 3;

	Oprand opr = ParseOprand();
	OprandType type = opr->type;

	if(type == REG){
		RegInfo reg = opr->value.reg;
		char regSize = reg->size;

		if(regSize == 32){
			modRM->rm = reg->index;
		}else{
			// TODO 不会出现这种情况。
		}
	}else{
		MemoryInfo mem = GetMemoryInfo(opr);

		modRM->mod = mem->mod;
		modRM->rm = mem->rm;
		offsetInfo = mem->offsetInfo;
		sib = mem->sib;
	}

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr =  GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = opr;

	return instr;
}

Instruction ParseNotlInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	opcode.primaryOpcode = 0xF7;

	modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->regOrOpcode = 2;

	Oprand opr = ParseOprand();
	OprandType type = opr->type;

	if(type == REG){
		RegInfo reg = opr->value.reg;
		char regSize = reg->size;

		if(regSize == 32){
			modRM->rm = reg->index;
		}else{
			// TODO 不会出现这种情况。
		}
	}else{
		MemoryInfo mem = GetMemoryInfo(opr);

		modRM->mod = mem->mod;
		modRM->rm = mem->rm;
		offsetInfo = mem->offsetInfo;
		sib = mem->sib;
	}

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr =  GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = opr;

	return instr;
}

Instruction ParseJeInstr(InstructionSet instrCode)
{
	int prefix = 0x0F;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};


	opcode.primaryOpcode = 0x84;

	offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = 0xFFFFFFFC;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
}

Instruction ParseJnpInstr(InstructionSet instrCode)
{
	int prefix = 0x0F;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};


	opcode.primaryOpcode = 0x8B;

	offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = 0xFFFFFFFC;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
}

Instruction ParseJneInstr(InstructionSet instrCode)
{
	int prefix = 0x0F;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	opcode.primaryOpcode = 0x85;

	offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = 0xFFFFFFFC;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
}

Instruction ParseJpInstr(InstructionSet instrCode)
{
	int prefix = 0x0F;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	opcode.primaryOpcode = 0x8A;

	offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = 0xFFFFFFFC;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
}

Instruction ParseJgInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};


	opcode.primaryOpcode = 0x7F;

	offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = 0xFFFFFFFC;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
}

Instruction ParseJaInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};


	opcode.primaryOpcode = 0x77;

	offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = 0xFFFFFFFC;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
}

Instruction ParseJlInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};


	opcode.primaryOpcode = 0x7C;

	offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = 0xFFFFFFFC;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
}

Instruction ParseJbInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = 0;
	NumericData immediate = {EMPTY, 0};


	opcode.primaryOpcode = 0x72;

	offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = 0xFFFFFFFC;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
}

Instruction ParseJgeInstr(InstructionSet instrCode)
{
	int prefix = 0x0F;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};


	opcode.primaryOpcode = 0x8D;
	offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = 0xFFFFFFFC;
	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
}

Instruction ParseJaeInstr(InstructionSet instrCode)
{
	int prefix = 0x0F;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};


	opcode.primaryOpcode = 0x83;
	offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = 0xFFFFFFFC;
	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
}

Instruction ParseJleInstr(InstructionSet instrCode)
{
	int prefix = 0x0F;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};


	opcode.primaryOpcode = 0x8E;
	Oprand oprand = ParseOprand();
	offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = 0xFFFFFFFC;
	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
}

Instruction ParseJbeInstr(InstructionSet instrCode)
{
	int prefix = 0x0F;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};


	opcode.primaryOpcode = 0x86;

	Oprand oprand = ParseOprand();
	MemoryInfo mem = GetMemoryInfo(oprand);
	offsetInfo = mem->offsetInfo;
	// TODO 最好做一下错误检测。
	offsetInfo->offset = 0xFFFFFFFC;
	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
}

Instruction ParseJmpInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	int offset = 0;
	NumericData immediate = {EMPTY, 0};

	Oprand opr = ParseOprand();
	OprandType type = opr->type;

	if(type == T_SIB){
		// 0:	ff 24 c5 00 02 00 00 	jmp    *0x200(,%eax,8)
		// c5-->11-000-101-->scal=3,index=000,base=0b101=5(是ebp的编号)
		// 24-->00-100-100-->mod = 00, reg/opcode = 100, rm = 100
		opcode.primaryOpcode = 0xFF;

		modRM = (ModRM)MALLOC(sizeof(struct modRM));

		offset = sib->offset;
		OFFSET_TYPE offsetType = GetOffsetType(offset);
		if(offset == 0){
			modRM->mod = 0b00;
		}else{
			if(offsetType == EIGHT){
				modRM->mod = 0b01;
			}else if(offsetType == SIXTEEN){
				// TODO 为什么引导SIB没有16位偏移？
			}else if(offsetType == THIRTY_TWO){
				modRM->mod = 0b10;
			}
		}
		
		modRM->regOrOpcode = 0b100;
		modRM->rm = 0b100;

		sib = ParseSIB();
	}else if(type == IDENT){
		// TODO 把所有的标识符一律识别为相对重定位。
		// 机器码：e9 fc ff ff ff       	jmp    13 <main+0x13>
		// TODO 没有理会操作数是8位还是32位。统一当作32位处理。
		opcode.primaryOpcode = 0xE9;
		MemoryInfo mem = GetMemoryInfo(opr);
		OffsetInfo offsetInfo = mem->offsetInfo;
	// TODO 最好做一下错误检测。
		offsetInfo->offset = 0xFFFFFFFC;
	}

	OffsetInfo offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = offset;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = opr;

	return instr;
}

Instruction ParsePushlInstr(InstructionSet instrCode)
{
	int prefix = -1;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	Oprand opr = ParseOprand();
	OprandType type = opr->type;

	if(type == REG){
		RegInfo reg = opr->value.reg;
		opcode.primaryOpcode = 0x50 + reg->index;
	}else{
		opcode.primaryOpcode = 0xFF;

		modRM = (ModRM)MALLOC(sizeof(struct modRM));
		modRM->regOrOpcode = 6;

		MemoryInfo mem = GetMemoryInfo(opr);

		modRM->mod = mem->mod;
		modRM->rm = mem->rm;
		sib = mem->sib;

		offsetInfo = mem->offsetInfo;
	}

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = opr;

	return instr;
}

Instruction ParsePoplInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	int offset = 0;
	NumericData immediate = {EMPTY, 0};

	opcode.primaryOpcode = 0x58;

	// 只有一个操作数。按照指令模板，这个操作数只能是寄存器。
	// TODO 如果操作数不是寄存器怎么办？在本项目，一律不处理这种意外。
	// TODO 以后有时间，可以增加对意外的处理。
	Oprand opr = ParseOprand();
	RegInfo reg = opr->value.reg;

	modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->regOrOpcode = reg->index;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, NULL, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = opr;

	return instr;
}

Instruction ParseIncbInstr(InstructionSet instrCode)
{
	Oprand opr = ParseOprand();
	NumericData immediate = {EMPTY, 0};

	OprandType type = opr->type;

	Opcode opcode = {-1, -1};
	int prefix = -1;
	OffsetInfo offsetInfo = NULL;
	SIB sib = NULL;
	ModRM modRM = (ModRM)MALLOC(sizeof(struct modRM));

/**
 * 一些机器码。
0:  fe c0                   inc    %al
2:   66 43                   inc    %bx
4:   43                      inc    %ebx
fe c3                	inc    %bl
 *
 * c0--->11000000--->11-000-000
 * c3--->11000011--->11-000-011
 */

	opcode.primaryOpcode = 0xFE;

	if(type == REG){
		RegInfo reg = opr->value.reg;
		char regSize = reg->size;
		if(regSize == 8){
			modRM->mod = 3;
			modRM->rm = reg->index;
		}else{
			// TODO 不会出现这种情况。
		}
	}else{
		MemoryInfo mem = GetMemoryInfo(opr);

		modRM->mod = mem->mod;
		modRM->rm = mem->rm;
		// offset = mem->offset;
		sib = mem->sib;

		offsetInfo = mem->offsetInfo;
	}

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = opr;

	return instr;
}

Instruction ParseIncwInstr(InstructionSet instrCode)
{
	int prefix = -1;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	Oprand opr = ParseOprand();
	OprandType type = opr->type;
	// 16位指令的前缀是0x66。
	prefix = 0x66;
	if(type == REG){
		RegInfo reg = opr->value.reg;
		char regSize = reg->size;

		if(regSize == 16){
			opcode.primaryOpcode = 0x40 + reg->index;
			modRM = NULL;
		}else{
			// TODO 不会出现这种情况。
		}
	}else{
		modRM = (ModRM)MALLOC(sizeof(struct modRM));

		MemoryInfo mem = GetMemoryInfo(opr);

		modRM->mod = mem->mod;
		modRM->rm = mem->rm;
		offsetInfo = mem->offsetInfo;
		sib = mem->sib;
	}

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr =  GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = opr;

	return instr;
}

Instruction ParseInclInstr(InstructionSet instrCode)
{
	// 和INCW的处理机制的差异只有一点：没有前缀。
	int prefix = -1;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	Oprand opr = ParseOprand();
	OprandType type = opr->type;

	if(type == REG){
		RegInfo reg = opr->value.reg;
		char regSize = reg->size;

		if(regSize == 32){
			opcode.primaryOpcode = 0x40 + reg->index;
			modRM = NULL;
		}else{
			// TODO 不会出现这种情况。
		}
	}else{
		modRM = (ModRM)MALLOC(sizeof(struct modRM));
		
		MemoryInfo mem = GetMemoryInfo(opr);

		modRM->mod = mem->mod;
		modRM->rm = mem->rm;
		offsetInfo = mem->offsetInfo;
		sib = mem->sib;
	}

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = opr;

	return instr;
}

Instruction ParseDecbInstr(InstructionSet instrCode)
{
	Oprand opr = ParseOprand();	
	NumericData immediate = {EMPTY, 0};

	OprandType type = opr->type;

	Opcode opcode = {-1, -1};
	int prefix = -1;
	OffsetInfo offsetInfo = NULL;
	SIB sib = NULL;
	ModRM modRM = (ModRM)MALLOC(sizeof(struct modRM));;

	opcode.primaryOpcode = 0xFE;

	if(type == REG){
		RegInfo reg = opr->value.reg;
		char regSize = reg->size;
		if(regSize == 8){
			modRM->mod = 3;
			modRM->regOrOpcode = 1;
			modRM->rm = reg->index;
		}else{
			// TODO 不会出现这种情况。
		}
	}else{
		MemoryInfo mem = GetMemoryInfo(opr);

		modRM->mod = mem->mod;
		modRM->rm = mem->rm;

//		OffsetInfo offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
		offsetInfo = mem->offsetInfo;

		sib = mem->sib;
	}

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = opr;

	return instr;
}

Instruction ParseDecwInstr(InstructionSet instrCode)
{
	int prefix = -1;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	Oprand opr = ParseOprand();
	OprandType type = opr->type;

	// 16位指令的前缀是0x66。
	prefix = 0x66;
	if(type == REG){
		RegInfo reg = opr->value.reg;
		char regSize = reg->size;

		if(regSize == 16){
			opcode.primaryOpcode = 0x48 + reg->index;
			modRM = NULL;
		}else{
			// TODO 不会出现这种情况。
		}
	}else{
		modRM = (ModRM)MALLOC(sizeof(struct modRM));
		modRM->regOrOpcode = 1;
		
		MemoryInfo mem = GetMemoryInfo(opr);

		modRM->mod = mem->mod;
		modRM->rm = mem->rm;

		offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
		offsetInfo = mem->offsetInfo;

		sib = mem->sib;
	}

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = opr;

	return instr;
}

Instruction ParseDeclInstr(InstructionSet instrCode)
{
	// 和DECW的处理机制的差异只有一点：没有前缀。
	int prefix = -1;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	Oprand opr = ParseOprand();
	OprandType type = opr->type;

	if(type == REG){
		RegInfo reg = opr->value.reg;
		char regSize = reg->size;

		if(regSize == 32){
			opcode.primaryOpcode = 0x48 + reg->index;
			modRM = NULL;
		}else{
			// TODO 不会出现这种情况。
		}
	}else{
		modRM = (ModRM)MALLOC(sizeof(struct modRM));
		modRM->regOrOpcode = 1;
		
		MemoryInfo mem = GetMemoryInfo(opr);

		modRM->mod = mem->mod;
		modRM->rm = mem->rm;

		offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
		offsetInfo = mem->offsetInfo;

		sib = mem->sib;
	}

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = opr;

	return instr;
}

Instruction ParseRepInstr(InstructionSet instrCode)
{
	// 由于模板限制，这条指令是 rep movsb。
	Opcode opcode = {0xF3, 0xA4};
	NumericData immediate = {EMPTY, 0};

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, NULL, NULL, NULL, immediate);
}

Instruction ParseCallInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xE8, -1};
	NumericData immediate = {EMPTY, 0};

// 我没有想到在后面需要依靠call sum中的sum来识别重定位类型。
//	OffsetInfo offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
//	offsetInfo->offset = 0xFFFFFFFC;

	Oprand oprand = ParseOprand();
	MemoryInfo mem = GetMemoryInfo(oprand);
	OffsetInfo offsetInfo = mem->offsetInfo;
	// TODO 最好做一下错误检测。
	offsetInfo->offset = 0xFFFFFFFC;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	return GenerateSimpleInstr(0, opcode, NULL, NULL, offsetInfo, immediate);
}

// 处理xorl、andl、orl。
Instruction ParseLogicalInstr(InstructionSet instrCode, LogicalInstrOpcodes opcodeParam, \
		char regOrOpcode)
{
	int prefix = 0;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	int dstEax = opcodeParam.dstEax;
	int srcImm32 = opcodeParam.srcImm32;
	int srcImm8 = opcodeParam.srcImm8;
	int srcReg = opcodeParam.srcReg;
	int dstReg = opcodeParam.dstReg;

    Oprand src = ParseOprand();
    // 跳过逗号。
    GetNextToken();
    Oprand dst = ParseOprand();

    OprandType srcType = src->type;
    OprandType dstType = dst->type;

    modRM = (ModRM)MALLOC(sizeof(struct modRM));

    // 35 id           XOR EAX, imm32
    if(srcType == IMM && dstType == REG){
        immediate.value = src->value.immediate;
        OFFSET_TYPE immType = GetOffsetType(immediate.value);
        RegInfo reg = dst->value.reg;
        char *regName = reg->name;

        if(immType == THIRTY_TWO && strcmp("eax", regName) == 0){
			immediate.type = THIRTY_TWO;
            opcode.primaryOpcode = dstEax;
            //GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
			Instruction instr = GenerateSimpleInstr(0, opcode, NULL, NULL, NULL, immediate);

			int operandIndex = 0;
			instr->oprands[operandIndex++] = src;
			instr->oprands[operandIndex++] = dst;

			return instr;
        }
    }

    if(srcType == IMM){
    	// 立即数。
    	modRM->regOrOpcode = regOrOpcode;
    	immediate.value = src->value.immediate;
        OFFSET_TYPE immType = GetOffsetType(immediate.value);
        if(immType == EIGHT){
        	opcode.primaryOpcode = srcImm8;
			immediate.type = EIGHT;
        }else if(immType == THIRTY_TWO){
        	opcode.primaryOpcode = srcImm32;
			immediate.type = THIRTY_TWO;
        }else{
			// TODO 根据机器码的规则，立即数不是8位就是32位，把16位的立即数也当成32位。
			// 需要进一步验证。
			immediate.type = THIRTY_TWO;
		}

        // 处理dst
        if(dstType == REG){
        	RegInfo reg = dst->value.reg;
        	modRM->mod = 0b11;
        	modRM->rm = reg->index;
        }else{
        	MemoryInfo mem = GetMemoryInfo(dst);
        	modRM->mod = mem->mod;
        	modRM->rm = mem->rm;
        	offsetInfo = mem->offsetInfo;
        	sib = mem->sib;
        }
    }else if(srcType == REG){
    	// 寄存器。
    	// 指令与机器码：31  /r  XOR r/m32,r32。
    	opcode.primaryOpcode = srcReg;
    	RegInfo reg = src->value.reg;
    	modRM->regOrOpcode = reg->index;
    	// TODO 思路不顺畅。
    	// 处理完src，接着处理dst啊。dst的类型是r/m32。和上面是一致，可以复用代码。
    	// 处理dst
        if(dstType == REG){
        	RegInfo reg = dst->value.reg;
        	modRM->mod = 0b11;
        	modRM->rm = reg->index;
        }else{
        	MemoryInfo mem = GetMemoryInfo(dst);
        	modRM->mod = mem->mod;
        	modRM->rm = mem->rm;
			offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
        	offsetInfo = mem->offsetInfo;
        	sib = mem->sib;
        }
    }else{
    	// 内存地址。
    	// 机器码与指令：33  /r  XOR r32,r/m32。
    	opcode.primaryOpcode = dstReg;
    	if(dstType == REG){
    		RegInfo dstReg = dst->value.reg;
    		modRM->regOrOpcode = dstReg->index;
    		// 处理src。虽然上面的指令中显示内存地址是r/m32，但实际上它只能是m32，因为src是寄存器
    		// 的情况在其他分支中处理了。
    		MemoryInfo mem = GetMemoryInfo(src);
        	modRM->mod = mem->mod;
        	modRM->rm = mem->rm;
        	offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
        	offsetInfo = mem->offsetInfo;
        	sib = mem->sib;
    	}
    }

    //GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = src;
	instr->oprands[operandIndex++] = dst;

	return instr;
}

Instruction ParseOrlInstr(InstructionSet instrCode)
{
	LogicalInstrOpcodes opcodeParam = {0x0D, 0x81, 0x83, 0x09, 0x0B};
	int regOrOpcode = 1;
	return ParseLogicalInstr(instrCode, opcodeParam, regOrOpcode);
}

Instruction ParseXorlInstr(InstructionSet instrCode)
{
    /*****************************
    * 
    *   35 id           XOR EAX, imm32
    *   81  /6  id  XOR r/m32,imm32
    *   83  /6  ib  XOR r/m32,imm8
    *   31  /r  XOR r/m32,r32
    *   33  /r  XOR r32,r/m32
    * 
    * 
    * ************************/

    LogicalInstrOpcodes opcodeParam = {0x35, 0x81, 0x83, 0x31, 0x33};
	int regOrOpcode = 6;
	return ParseLogicalInstr(instrCode, opcodeParam, regOrOpcode);
}

Instruction ParseAndlInstr(InstructionSet instrCode)
{
	LogicalInstrOpcodes opcodeParam = {0x25, 0x81, 0x83, 0x21, 0x23};
	int regOrOpcode = 4;
	return ParseLogicalInstr(instrCode, opcodeParam, regOrOpcode);
}

Instruction GenerateShiftInstr(InstructionSet instrCode, int regOrOpcode)
{
	int prefix = 0;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	Oprand src = ParseOprand();
	// 跳过逗号。
	GetNextToken();
	Oprand dst = ParseOprand();

	OprandType srcType = src->type;
	OprandType dstType = dst->type;

	modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->regOrOpcode = regOrOpcode;

	if(srcType == IMM){
		opcode.primaryOpcode = 0xC1;

		immediate.value = src->value.immediate;
		immediate.type = EIGHT;
	}else if(srcType == REG){
		opcode.primaryOpcode = 0xD3;
	}else{
		// TODO 不会出现这种状况。按本项目的习惯，对这种意外一律不处理。
	}

	if(dstType == REG){
		modRM->mod = 0b11;
		RegInfo dstReg = dst->value.reg;
		modRM->rm = dstReg->index;
	}else{
		MemoryInfo mem = GetMemoryInfo(dst);
		modRM->mod = mem->mod;
		modRM->rm = mem->rm;

		offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
		offsetInfo = mem->offsetInfo;
		sib = mem->sib;
	}

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr =  GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = src;
	instr->oprands[operandIndex++] = dst;

	return instr;
}

Instruction ParseShllInstr(InstructionSet instrCode)
{
	// GenerateShiftInstr(InstructionSet instrCode, int regOrOpcode)
	return GenerateShiftInstr(instrCode, 4);
}


Instruction ParseSarlInstr(InstructionSet instrCode)
{
	// GenerateShiftInstr(InstructionSet instrCode, int regOrOpcode)
	return GenerateShiftInstr(instrCode, 7);
}

Instruction ParseShrlInstr(InstructionSet instrCode)
{
	// GenerateShiftInstr(InstructionSet instrCode, int regOrOpcode)
	return GenerateShiftInstr(instrCode, 5);
}

Instruction ParseImullInstr(InstructionSet instrCode)
{
	int prefix = -1;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	// 这一次，先读取两个操作数。
	Oprand opr1 = ParseOprand();
	Oprand opr2 = NULL;
	char ch = GetLookAheadChar();
	if(ch == ','){
		// 跳过逗号
		GetNextToken();
		opr2 = ParseOprand();
	}

	modRM = (ModRM)MALLOC(sizeof(struct modRM));

	// 只有一个操作数。
	if(opr2 == NULL){
		OprandType type = opr1->type;

		opcode.primaryOpcode = 0xF7;

		modRM->regOrOpcode = 5;

		if(type == REG){
			RegInfo reg = opr2->value.reg;
			char regSize = reg->size;

			if(regSize == 32){
				modRM->mod = 0b11;
				modRM->rm = reg->index;
			}else{
			// TODO 不会出现这种情况。
			}
		}else{
			MemoryInfo mem = GetMemoryInfo(opr2);

			modRM->mod = mem->mod;
			modRM->rm = mem->rm;
			offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
			offsetInfo = mem->offsetInfo;
			sib = mem->sib;
		}
	}else{
		opcode.primaryOpcode = 0x0F;
		opcode.secondaryOpcode = 0xAF;

		// 有两个操作数。src = opr1, dst = opr2。
		OprandType type1 = opr1->type;
		OprandType type2 = opr2->type;

		// TODO 我不确定 type1 == type1 == REG 是否等价于 type1 == REG && type2== REG。
		if(type1 == type2 == REG){
			// 机器码：0f af d8             	imul   %eax,%ebx
			// ebx存储在reg/opcode中。依据是我琢磨出来的那条规则。
			// 什么规则？指令的哪个操作数必须是寄存器，那个操作数就存储在reg/opcode中。
			// d8-->11-011-000--->mod=0b11,reg/opcode=0b011,rm=0b000。

			// 机器码：	0f af cb             	imul   %ebx,%ecx
			// cb-->11-001-011-->mod=0b11,reg/opcode=0b001,rm=0b011。
			modRM->mod = 0b11;
			RegInfo reg1 = opr1->value.reg;
			RegInfo reg2 = opr2->value.reg;
			modRM->regOrOpcode = reg2->index;
			modRM->rm = reg1->index;
		}else{
			// TODO 是否需要判断 src == IMM && dst == reg？
			// 不判断。本项目对这种意外情况一律不处理。

			// 操作数组合`src = IMM, dst = reg`，需要转换。
			// 机器码：69 db 00 04 00 00    	imul   $0x400,%ebx,%ebx。
			// db-->11-011-011--->mod=0b11,reg=3,rm=0b011-->rm是寄存器的编号。
			// 第二个操作数是dst。
			modRM->mod = 0b11;
			RegInfo reg2 = opr2->value.reg;
			modRM->regOrOpcode = reg2->index;
			modRM->rm = reg2->index;
			// 第一个操作数是立即数。
			immediate.value = opr1->value.immediate;
			immediate.type = GetOffsetType(immediate.value) != EIGHT ? THIRTY_TWO : EIGHT;
		}
	}

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr =   GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = opr1;
	instr->oprands[operandIndex++] = opr2;

	return instr;
}

// 这是一个中途新建的函数。在写代码前，要考虑到这种细节，恐怕比较烦。
char IsMem(OprandType type)
{
	// TODO 我当然知道可以用一个`||`来实现通用的效果，
	// 可我嫌那样写出来的代码只有一行，太长了。
	char isMem = 0;

	switch(type){
	case IMM_BASE_MEM:
		isMem = 1;
		break;
	case REG_BASE_MEM:
		isMem = 1;
		break;
	case T_SIB:
		isMem = 1;
		break;
	case IDENT:
		isMem = 1;
		break;
	default:
		isMem = 0;

	}

	return isMem;
}

// 使用Generate是为了和ParseMovbInstr这些函数区分开。
Instruction GenerateMovInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	Oprand src = ParseOprand();
	// 跳过逗号。
	GetNextToken();
	Oprand dst = ParseOprand();

	OprandType srcType = src->type;
	OprandType dstType = dst->type;

	modRM = (ModRM)MALLOC(sizeof(struct modRM));

	// MOV r/m32,r32
	if(srcType == REG && IsMem(dstType) == 1 ){
		RegInfo srcReg = src->value.reg;
		int regSize = srcReg->size;
		if(regSize == 8){
			opcode.primaryOpcode = 0x88;
		}else if(regSize == 16){
			prefix = 0x66;
			opcode.primaryOpcode = 0x89;
		}else if(regSize == 32){
			// 89 d9                	mov    %ebx,%ecx
			// d9-->11-011-001-->mod=11,reg/opcode=011,rm=001
			opcode.primaryOpcode = 0x89;
		}

		MemoryInfo mem = GetMemoryInfo(dst);
		sib = mem->sib;
		modRM->mod = mem->mod;
		modRM->rm = mem->rm;
		modRM->regOrOpcode = srcReg->index;

		offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
		offsetInfo = mem->offsetInfo;

		//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
		Instruction instr =    GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

		int operandIndex = 0;
		instr->oprands[operandIndex++] = src;
		instr->oprands[operandIndex++] = dst;

		return instr;
	}

	// TODO 和上面的代码极其相似，我不能立刻想出优化的方法，先赶进度吧。
	// MOV r32,r/m32
	if(dstType == REG && IsMem(srcType) == 1){
		RegInfo dstReg = dst->value.reg;
		int regSize = dstReg->size;
		if(regSize == 8){
			opcode.primaryOpcode = 0x8A;
		}else if(regSize == 16){
			prefix = 0x66;
			opcode.primaryOpcode = 0x8B;
		}else if(regSize == 32){
			opcode.primaryOpcode = 0x8B;
		}

		MemoryInfo mem = GetMemoryInfo(src);
		sib = mem->sib;
		modRM->mod = mem->mod;
		modRM->rm = mem->rm;
		modRM->regOrOpcode = dstReg->index;

		//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
		Instruction instr =   GenerateSimpleInstr(prefix, opcode, modRM, sib, mem->offsetInfo, immediate);

		int operandIndex = 0;
		instr->oprands[operandIndex++] = src;
		instr->oprands[operandIndex++] = dst;

		return instr;
	}

	// MOV r32,imm32
	// 非常好处理，虽然我还没有头绪。
	if(srcType == IMM && dstType == REG){
		immediate.value = src->value.immediate;
		RegInfo dstReg = dst->value.reg;
		int regSize = dstReg->size;
		if(regSize == 8){
			immediate.type = EIGHT;
			opcode.primaryOpcode = 0xB0 + dstReg->index;
		}else if(regSize == 16){
			immediate.type = SIXTEEN;
			prefix = 0x66;
			opcode.primaryOpcode = 0xB8 + dstReg->index;
		}else if(regSize == 32){
			immediate.type = THIRTY_TWO;
			opcode.primaryOpcode = 0xB8 + dstReg->index;
		}

		modRM = NULL;

		//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
		Instruction instr =  GenerateSimpleInstr(prefix, opcode, modRM, sib, NULL, immediate);

		int operandIndex = 0;
		instr->oprands[operandIndex++] = src;
		instr->oprands[operandIndex++] = dst;

		return instr;
	}

	// MOV r/m32,imm32
	if(srcType == IMM && IsMem(dstType) == 1){
		immediate.value = src->value.immediate;
		OFFSET_TYPE immediateType = GetOffsetType(immediate.value);

		if(immediateType == 8){
			immediate.type = EIGHT;
			opcode.primaryOpcode = 0xC6;
		}else if(immediateType == 16){
			immediate.type = SIXTEEN;
			prefix = 0x66;
			opcode.primaryOpcode = 0xC7;
		}else if(immediateType == 32){
			immediate.type = THIRTY_TWO;
			opcode.primaryOpcode = 0xC7;
		}

		MemoryInfo mem = GetMemoryInfo(dst);
		sib = mem->sib;
		modRM->mod = mem->mod;
		modRM->rm = mem->rm;

		//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
		Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, mem->offsetInfo, immediate);

		int operandIndex = 0;
		instr->oprands[operandIndex++] = src;
		instr->oprands[operandIndex++] = dst;

		return instr;
	}
}

Instruction ParseMovlInstr(InstructionSet instrCode)
{
	return GenerateMovInstr(instrCode);
}

Instruction ParseMovbInstr(InstructionSet instrCode)
{
	return GenerateMovInstr(instrCode);
}

Instruction ParseMovwInstr(InstructionSet instrCode)
{
	return GenerateMovInstr(instrCode);
}

Instruction GenerateCmplEtcInstr(InstructionSet instrCode, CmplEtcOpcodes cmplEtcOpcodes, \
	int regOrOpcode)
{
	int prefix = 0;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	// src是立即数，dst是寄存器。这是由CGC使用的指令的模板决定的。

	Oprand src = ParseOprand();
	// 跳过逗号。
	GetNextToken();
	Oprand dst = ParseOprand();

	OprandType srcType = src->type;
	OprandType dstType = dst->type;

	// CMP EAX, imm32------3D id
	// OFFSET_TYPE immediateType = GetOffsetType(immediate.value);
	if(srcType == IMM && dstType == REG){
		// 测试后，发现数据不对，才修改这里。
		// 错误是怎么产生的？实际情况和我认为的情况不一致。
		// 此处的srcSize用srcType是最合适的，但在前面已经有一个srcType，所以，只能用srcSize。
		OFFSET_TYPE srcSize = GetOffsetType(src->value.immediate);
		// if(srcSize == THIRTY_TWO){
		if(srcSize != EIGHT){
			RegInfo dstReg = dst->value.reg;
			char *regName = dstReg->name;
			if(strcmp(regName, "eax") == 0){
				opcode.primaryOpcode = cmplEtcOpcodes.dstEax;
				immediate.value = src->value.immediate;
				immediate.type = THIRTY_TWO;
				//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
				Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, NULL, immediate);

				int operandIndex = 0;
				instr->oprands[operandIndex++] = src;
				instr->oprands[operandIndex++] = dst;

				return instr;
			}
		}
	}

	// CMP r/m32,imm32------81 /7 id
	// CMP r/m32,imm8------83 /7 ib
	// 需要区分是哪条机器码。根据imm的长度区分。
	if(srcType == IMM){
		immediate.value = src->value.immediate;
		OFFSET_TYPE immediateType = GetOffsetType(immediate.value);
		if(immediateType == EIGHT){
			immediate.type = EIGHT;
			opcode.primaryOpcode = cmplEtcOpcodes.srcImm8;
		}else if(immediateType == THIRTY_TWO){
			immediate.type = THIRTY_TWO;
			opcode.primaryOpcode = cmplEtcOpcodes.srcImm32;
		}else{
			// TODO 不会出现这种情况。暂时不做错误检查。
			printf("immediateType = %d\n", immediateType);
			exit(-1);
		}

		ModRM modRM = (ModRM)MALLOC(sizeof(struct modRM));

		if(dstType == REG){
			modRM->mod = 0b11;
			RegInfo dstReg = dst->value.reg;
			modRM->rm = dstReg->index;
		}else{
			MemoryInfo mem = GetMemoryInfo(dst);
			sib = mem->sib;

			offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
			offsetInfo = mem->offsetInfo;

			modRM->mod = mem->mod;
			modRM->rm = mem->rm;
		}

		//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
		Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

		int operandIndex = 0;
		instr->oprands[operandIndex++] = src;
		instr->oprands[operandIndex++] = dst;

		return instr;
	}


	// CMP r/m32,r32------39 /r
	// 这种情况，实际上只能是 CMP m32,r32------39 /r
	if(srcType == REG){
		opcode.primaryOpcode = cmplEtcOpcodes.srcReg;

		MemoryInfo mem = GetMemoryInfo(dst);
		sib = mem->sib;
		offsetInfo = mem->offsetInfo;

		ModRM modRM = (ModRM)MALLOC(sizeof(struct modRM));
		modRM->mod = mem->mod;
		modRM->rm = mem->rm;

		RegInfo srcReg = src->value.reg;
		modRM->regOrOpcode = srcReg->index;

		//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
		Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

		int operandIndex = 0;
		instr->oprands[operandIndex++] = src;
		instr->oprands[operandIndex++] = dst;

		return instr;
	}

	// CMP r32,r/m32------3B /r
	// 这种情况，实际上只能是 CMP r32,m32------3B /r
	if(dstType == REG){
		opcode.primaryOpcode = cmplEtcOpcodes.dstReg;

		MemoryInfo mem = GetMemoryInfo(src);
		sib = mem->sib;
		offsetInfo = mem->offsetInfo;

		ModRM modRM = (ModRM)MALLOC(sizeof(struct modRM));
		modRM->mod = mem->mod;
		modRM->rm = mem->rm;

		RegInfo dstReg = dst->value.reg;
		modRM->regOrOpcode = dstReg->index;

		//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
		Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

		int operandIndex = 0;
		instr->oprands[operandIndex++] = src;
		instr->oprands[operandIndex++] = dst;

		return instr;
	}
}

Instruction ParseCmplInstr(InstructionSet instrCode)
{
	CmplEtcOpcodes cmplEtcOpcodes = {0x3D, 0x83, 0x81, 0x39, 0x3B};
	// GenerateCmplEtcInstr(InstructionSet instrCode, CmplEtcOpcodes cmplEtcOpcodes)
	return GenerateCmplEtcInstr(instrCode, cmplEtcOpcodes, 7);
}


Instruction ParseAddlInstr(InstructionSet instrCode)
{
	CmplEtcOpcodes cmplEtcOpcodes = {0x05, 0x83, 0x81, 0x01, 0x03};
	// GenerateCmplEtcInstr(InstructionSet instrCode, CmplEtcOpcodes cmplEtcOpcodes)
	return GenerateCmplEtcInstr(instrCode, cmplEtcOpcodes, 0);
}

Instruction ParseSublInstr(InstructionSet instrCode)
{
	CmplEtcOpcodes cmplEtcOpcodes = {0x2D, 0x81, 0x83, 0x29, 0x2B};
	// GenerateCmplEtcInstr(InstructionSet instrCode, CmplEtcOpcodes cmplEtcOpcodes)
	return GenerateCmplEtcInstr(instrCode, cmplEtcOpcodes, 5);
}

Instruction ParseTestInstr(InstructionSet instrCode)
{
	int prefix = 0;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	// src是立即数，dst是寄存器。这是由CGC使用的指令的模板决定的。

	Oprand src = ParseOprand();
	// 跳过逗号。
	GetNextToken();
	Oprand dst = ParseOprand();

	OprandType srcType = src->type;
	OprandType dstType = dst->type;

	immediate.value = src->value.immediate;
	immediate.type = THIRTY_TWO;

	RegInfo dstReg = dst->value.reg;

	// 0f be cb             	movsbl %bl,%ecx
	// cb-->11-001-011-->mod=11,reg/opcode=001,rm=011
	// f6 c4 44             	test   $0x44,%ah
	// c4-->11-000-100-->mod=11,reg/opcode=000,rm=010

	modRM = (ModRM)MALLOC(sizeof(struct modRM));
	modRM->mod = 0b11;
	// mod->regOrOpcode = dstReg->index;
	modRM->regOrOpcode = 0;		// 这是由这个指令的机器码规定的。
	// 由于这个指令的机器码规定reg/opcode是0，dst只能存储在rm。
	modRM->rm = dstReg->index;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = src;
	instr->oprands[operandIndex++] = dst;

	return instr;
}


Instruction ParseMovzxInstr(InstructionSet instrCode, char signType, OFFSET_TYPE srcSize)
{
	int prefix = 0x0F;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	Oprand src = ParseOprand();
	// 跳过逗号。
	GetNextToken();
	Oprand dst = ParseOprand();

	OprandType srcType = src->type;
	OprandType dstType = dst->type;

	modRM = (ModRM)MALLOC(sizeof(struct modRM));

	// MOVZX r32,r/m8

	// 有符号。
	if(signType == 1){
		if(srcSize == EIGHT){
			opcode.primaryOpcode = 0xBE;
		}else if(srcSize == SIXTEEN){
			opcode.primaryOpcode = 0xBF;
		}else{
			// TODO 不会出现其他情况。
		}
	}else if(signType == 0){
		// 无符号。
		if(srcSize == EIGHT){
			opcode.primaryOpcode = 0xB6;
		}else if(srcSize == SIXTEEN){
			opcode.primaryOpcode = 0xB7;
		}else{
			// TODO 不会出现其他情况。
		}
	}

	RegInfo dstReg = dst->value.reg;
	modRM->regOrOpcode = dstReg->index;

	if(srcType == REG){
		modRM->mod = 0b11;
		RegInfo srcReg = src->value.reg;
		modRM->rm = srcReg->index;
	}else{
		MemoryInfo mem = GetMemoryInfo(src);
		sib = mem->sib;
		modRM->mod = mem->mod;
		modRM->rm = mem->rm;

		offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
		offsetInfo = mem->offsetInfo;
	}
	
	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = src;
	instr->oprands[operandIndex++] = dst;

	return instr;
}

Instruction ParseMovsblInstr(InstructionSet instrCode)
{
	// ParseMovzxInstr(InstructionSet instrCode, char signType, OFFSET_TYPE srcSize)
	return ParseMovzxInstr(instrCode, 1, EIGHT);
}

Instruction ParseMovswlInstr(InstructionSet instrCode)
{
	// ParseMovzxInstr(InstructionSet instrCode, char signType, OFFSET_TYPE srcSize)
	return ParseMovzxInstr(instrCode, 1, SIXTEEN);
}

Instruction ParseMovzblInstr(InstructionSet instrCode)
{
	// ParseMovzxInstr(InstructionSet instrCode, char signType, OFFSET_TYPE srcSize)
	return ParseMovzxInstr(instrCode, 0, EIGHT);
}

Instruction ParseMovzwlInstr(InstructionSet instrCode)
{
	// ParseMovzxInstr(InstructionSet instrCode, char signType, OFFSET_TYPE srcSize)
	return ParseMovzxInstr(instrCode, 0, SIXTEEN);
}

Instruction ParseLealInstr(InstructionSet instrCode)
{
	int prefix = -1;
	Opcode opcode = {-1, -1};
	ModRM modRM = NULL;
	SIB sib = NULL;
	OffsetInfo offsetInfo = NULL;
	NumericData immediate = {EMPTY, 0};

	// Leal的两条模板是：leal %0, %%edi 和 leal %0, %1。

	modRM = (ModRM)MALLOC(sizeof(struct modRM));

	// 处理第一个操作数。src。根据src确定rm和mod的值。
	Oprand opr1 = ParseOprand();
	// 这个操作数是内存地址。内存地址的种类：-4(%ebp),SIB,0x1234。
	OprandType type1 = opr1->type;

	opcode.primaryOpcode = 0x8D;

	MemoryInfo mem = GetMemoryInfo(opr1);
	modRM->mod = mem->mod;
	modRM->rm = mem->rm;

	offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo = mem->offsetInfo;

	sib = mem->sib;

	// 跳过操作数中间的逗号。
	GetNextToken();
	// 处理第二个操作数。dest。根据dst确定reg/opcode的值。
	// dst只能是寄存器。在本项目中不处理dst不是寄存器的情况。
	Oprand opr2 = ParseOprand();
	RegInfo reg = opr2->value.reg;
	modRM->regOrOpcode = reg->index;


	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);

	int operandIndex = 0;
	instr->oprands[operandIndex++] = opr1;
	instr->oprands[operandIndex++] = opr2;

	return instr;
}
