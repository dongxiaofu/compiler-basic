Instruction ParseFchsInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xD9, 0xE0};

	NumericData immediate = {EMPTY, 0};

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(0, opcode, NULL, NULL, 0, immediate);
return instr;

}

Instruction ParseFldzInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xD9, 0xEE};
	NumericData immediate = {EMPTY, 0};

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(0, opcode, NULL, NULL, 0, immediate);
return instr;

}

Instruction ParseFucomppInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xDA, 0xE9};
	NumericData immediate = {EMPTY, 0};

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(0, opcode, NULL, NULL, 0, immediate);
return instr;

}

Instruction ParseFld1Instr(InstructionSet instrCode)
{
	Opcode opcode = {0xD9, 0xE8};
	NumericData immediate = {EMPTY, 0};

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(0, opcode, NULL, NULL, 0, immediate);
return instr;

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
	Instruction instr = GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
return instr;

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
	Instruction instr = GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
return instr;

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
	Instruction instr = GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
return instr;

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
	Instruction instr = GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
return instr;

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
	Instruction instr = GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
return instr;

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
	Instruction instr = GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
return instr;

}

Instruction ParseFnstswInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xDF, 0xE0};
	NumericData immediate = {EMPTY, 0};

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(0, opcode, NULL, NULL, NULL, immediate);
return instr;

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
	Instruction instr = GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
return instr;

}

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
	Instruction instr = GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
return instr;

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
	instr->oprands[] = oprand;

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
	instr->oprands[] = oprand;

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
	instr->oprands[] = oprand;

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
	instr->oprands[] = oprand;

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
	Instruction instr = GenerateSimpleInstr(prefix, opcode, 	modRM, sib, mem->offsetInfo, immediate);
	instr->oprands[] = oprand;

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
	instr->oprands[] = oprand;

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
	Instruction instr = GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
return instr;

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
	Instruction instr = GenerateSimpleInstr(0, opcode, modRM, NULL, offsetInfo, immediate);
return instr;

}

Instruction ParseCdqInstr(InstructionSet instrCode)
{
	Opcode opcode = {0x99, -1};
	NumericData immediate = {EMPTY, 0};

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(0, opcode, NULL, NULL, NULL, immediate);
return instr;

}

Instruction ParseRetInstr(InstructionSet instrCode)
{
	GetNextToken();
	// unsigned char opcode = 0xC3;
	Opcode opcode = {0xC3, -1};
	NumericData immediate = {EMPTY, 0};

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(0, opcode, NULL, NULL, NULL, immediate);
return instr;

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
	instr->oprands[] = opr;

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
	instr->oprands[] = opr;

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
	instr->oprands[] = opr;

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
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
	instr->oprands[] = opr;

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
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
	instr->oprands[] = opr;

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
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
return instr;

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
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
return instr;

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
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
return instr;

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
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
return instr;

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
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
return instr;

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
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
return instr;

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
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
return instr;

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
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
return instr;

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
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
return instr;

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
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
return instr;

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
	offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = 0xFFFFFFFC;
	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
return instr;

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
	offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = 0xFFFFFFFC;
	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
return instr;

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
		offset = 0xFFFFFFFC;
	}

	OffsetInfo offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = offset;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
	instr->oprands[] = opr;

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
	instr->oprands[] = opr;

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
	instr->oprands[] = opr;

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
	instr->oprands[] = opr;

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
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
	instr->oprands[] = opr;

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
	instr->oprands[] = opr;

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
	instr->oprands[] = opr;

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
	instr->oprands[] = opr;

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
	instr->oprands[] = opr;

return instr;

}

Instruction ParseRepInstr(InstructionSet instrCode)
{
	// 由于模板限制，这条指令是 rep movsb。
	Opcode opcode = {0xF3, 0xA4};
	NumericData immediate = {EMPTY, 0};

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(0, opcode, NULL, NULL, NULL, immediate);
return instr;

}

Instruction ParseCallInstr(InstructionSet instrCode)
{
	Opcode opcode = {0xE8, -1};
	NumericData immediate = {EMPTY, 0};

	OffsetInfo offsetInfo = (OffsetInfo)MALLOC(sizeof(struct offsetInfo));
	offsetInfo->offset = 0xFFFFFFFC;

	//GenerateSimpleInstr(prefix, opcode, modRM, sib, offset, immediate)
	Instruction instr = GenerateSimpleInstr(0, opcode, NULL, NULL, offsetInfo, immediate);
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
	Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
	instr->oprands[] = opr1;
	instr->oprands[] = opr2;

	return instr;

}

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
		Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, offsetInfo, immediate);
		instr->oprands[] = src;
		instr->oprands[] = dst;

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
		Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, mem->offsetInfo, immediate);
		instr->oprands[] = src;
		instr->oprands[] = dst;

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
		Instruction instr = GenerateSimpleInstr(prefix, opcode, modRM, sib, NULL, immediate);

		instr->oprands[] = src;
		instr->oprands[] = dst;

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
		instr->oprands[] = src;

		instr->oprands[] = dst;

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
	instr->oprands[] = src;

	instr->oprands[] = dst;

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
	instr->oprands[] = opr1;

	instr->oprands[] = opr2;

	return instr;

}

