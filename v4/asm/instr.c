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


return NULL;
}

Instruction ParseFaddsInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseFaddlInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseFsubsInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseFsublInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseFdivsInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseFdivlInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseFnstswInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseFldsInstr(InstructionSet instrCode)
{


return NULL;
}

Instruction ParseFldlInstr(InstructionSet instrCode)
{


return NULL;
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


return NULL;
}

Instruction ParseFmullInstr(InstructionSet instrCode)
{


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
