#ifndef INSTR_H
#define INSTR_H

#include "common.h"

OFFSET_TYPE GetOffsetType(int offset);
OprandType GetOprandType();
SIB ParseSIB();
Oprand ParseOprand();
Instruction GenerateSimpleInstr(int prefix, Opcode opcode, ModRM modRM,\
	SIB sib, int offset, int immediate);
Instruction ParseFchsInstr(InstructionSet instrCode);
Instruction ParseFldzInstr(InstructionSet instrCode);
Instruction ParseFucomppInstr(InstructionSet instrCode);
Instruction ParseFld1Instr(InstructionSet instrCode);
Instruction ParseFaddsInstr(InstructionSet instrCode);
Instruction ParseFaddlInstr(InstructionSet instrCode);
Instruction ParseFsubsInstr(InstructionSet instrCode);
Instruction ParseFsublInstr(InstructionSet instrCode);
Instruction ParseFdivsInstr(InstructionSet instrCode);
Instruction ParseFdivlInstr(InstructionSet instrCode);
Instruction ParseFnstswInstr(InstructionSet instrCode);
Instruction ParseFldsInstr(InstructionSet instrCode);
Instruction ParseFldlInstr(InstructionSet instrCode);
Instruction ParseFildlInstr(InstructionSet instrCode);
Instruction ParseFildqInstr(InstructionSet instrCode);
Instruction ParseFstpsInstr(InstructionSet instrCode);
Instruction ParseFstplInstr(InstructionSet instrCode);
Instruction ParseFldcwInstr(InstructionSet instrCode);
Instruction ParseFistplInstr(InstructionSet instrCode);
Instruction ParseFistpllInstr(InstructionSet instrCode);
Instruction ParseFstsInstr(InstructionSet instrCode);
Instruction ParseFstlInstr(InstructionSet instrCode);
Instruction ParseFstpInstr(InstructionSet instrCode);
Instruction ParseFmulsInstr(InstructionSet instrCode);
Instruction ParseFmullInstr(InstructionSet instrCode);
Instruction ParseCdqInstr(InstructionSet instrCode);
Instruction ParseRetInstr(InstructionSet instrCode);
Instruction ParseMullInstr(InstructionSet instrCode);
Instruction ParseIdivlInstr(InstructionSet instrCode);
Instruction ParseDivlInstr(InstructionSet instrCode);
Instruction ParseNeglInstr(InstructionSet instrCode);
Instruction ParseNotlInstr(InstructionSet instrCode);
Instruction ParseJeInstr(InstructionSet instrCode);
Instruction ParseJnpInstr(InstructionSet instrCode);
Instruction ParseJneInstr(InstructionSet instrCode);
Instruction ParseJpInstr(InstructionSet instrCode);
Instruction ParseJgInstr(InstructionSet instrCode);
Instruction ParseJaInstr(InstructionSet instrCode);
Instruction ParseJlInstr(InstructionSet instrCode);
Instruction ParseJbInstr(InstructionSet instrCode);
Instruction ParseJgeInstr(InstructionSet instrCode);
Instruction ParseJaeInstr(InstructionSet instrCode);
Instruction ParseJleInstr(InstructionSet instrCode);
Instruction ParseJbeInstr(InstructionSet instrCode);
Instruction ParseJmpInstr(InstructionSet instrCode);
Instruction ParsePushlInstr(InstructionSet instrCode);
Instruction ParsePoplInstr(InstructionSet instrCode);
Instruction ParseIncbInstr(InstructionSet instrCode);
Instruction ParseIncwInstr(InstructionSet instrCode);
Instruction ParseInclInstr(InstructionSet instrCode);
Instruction ParseDecbInstr(InstructionSet instrCode);
Instruction ParseDecwInstr(InstructionSet instrCode);
Instruction ParseDeclInstr(InstructionSet instrCode);
Instruction ParseRepInstr(InstructionSet instrCode);
Instruction ParseCallInstr(InstructionSet instrCode);
Instruction ParseOrlInstr(InstructionSet instrCode);
Instruction ParseXorlInstr(InstructionSet instrCode);
Instruction ParseAndlInstr(InstructionSet instrCode);
Instruction ParseShllInstr(InstructionSet instrCode);
Instruction ParseSHLDInstr(InstructionSet instrCode);
Instruction ParseSarlInstr(InstructionSet instrCode);
Instruction ParseShrlInstr(InstructionSet instrCode);
Instruction ParseAddlInstr(InstructionSet instrCode);
Instruction ParseSublInstr(InstructionSet instrCode);
Instruction ParseImullInstr(InstructionSet instrCode);
Instruction ParseMovlInstr(InstructionSet instrCode);
Instruction ParseMovbInstr(InstructionSet instrCode);
Instruction ParseMovwInstr(InstructionSet instrCode);
Instruction ParseCmplInstr(InstructionSet instrCode);
Instruction ParseTestInstr(InstructionSet instrCode);
Instruction ParseMovsblInstr(InstructionSet instrCode);
Instruction ParseMovswlInstr(InstructionSet instrCode);
Instruction ParseMovzblInstr(InstructionSet instrCode);
Instruction ParseMovzwlInstr(InstructionSet instrCode);
Instruction ParseMovzxInstr(InstructionSet instrCode);
Instruction ParseLealInstr(InstructionSet instrCode);


static Instruction (*parseInstructionFunctions[INSTRUCTION_SETS_SIZE])(InstructionSet instrCode) = {
	#define GENERAGE_FUNCTION(name)		Parse##name##Instr,
	#include "function_name.txt"
	#undef GENERAGE_FUNCTION
};





#endif
