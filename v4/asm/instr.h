#ifndef INSTR_H
#define INSTR_H

//#include "assembler.h"

static void (*parseInstructionFunctions[INSTRUCTION_SETS_SIZE])(InstructionSet *instrCode) = {
 	#define GENERAGE_FUNCTION(name)		Parse##name##Instr,
 	#include "function_name.txt"
 	#undef GENERAGE_FUNCTION
};

void ParseFchsInstr(InstructionSet *instrCode);
void ParseFldzInstr(InstructionSet *instrCode);
void ParseFucomppInstr(InstructionSet *instrCode);
void ParseFld1Instr(InstructionSet *instrCode);
void ParseFaddsInstr(InstructionSet *instrCode);
void ParseFaddlInstr(InstructionSet *instrCode);
void ParseFsubsInstr(InstructionSet *instrCode);
void ParseFsublInstr(InstructionSet *instrCode);
void ParseFdivsInstr(InstructionSet *instrCode);
void ParseFdivlInstr(InstructionSet *instrCode);
void ParseFnstswInstr(InstructionSet *instrCode);
void ParseFldsInstr(InstructionSet *instrCode);
void ParseFldlInstr(InstructionSet *instrCode);
void ParseFildlInstr(InstructionSet *instrCode);
void ParseFildqInstr(InstructionSet *instrCode);
void ParseFstpsInstr(InstructionSet *instrCode);
void ParseFstplInstr(InstructionSet *instrCode);
void ParseFldcwInstr(InstructionSet *instrCode);
void ParseFistplInstr(InstructionSet *instrCode);
void ParseFistpllInstr(InstructionSet *instrCode);
void ParseFstsInstr(InstructionSet *instrCode);
void ParseFstlInstr(InstructionSet *instrCode);
void ParseFstpInstr(InstructionSet *instrCode);
void ParseFmulsInstr(InstructionSet *instrCode);
void ParseFmullInstr(InstructionSet *instrCode);
void ParseCdqInstr(InstructionSet *instrCode);
void ParseRetInstr(InstructionSet *instrCode);
void ParseMullInstr(InstructionSet *instrCode);
void ParseIdivlInstr(InstructionSet *instrCode);
void ParseDivlInstr(InstructionSet *instrCode);
void ParseNeglInstr(InstructionSet *instrCode);
void ParseNotlInstr(InstructionSet *instrCode);
void ParseJeInstr(InstructionSet *instrCode);
void ParseJnpInstr(InstructionSet *instrCode);
void ParseJneInstr(InstructionSet *instrCode);
void ParseJpInstr(InstructionSet *instrCode);
void ParseJgInstr(InstructionSet *instrCode);
void ParseJaInstr(InstructionSet *instrCode);
void ParseJlInstr(InstructionSet *instrCode);
void ParseJbInstr(InstructionSet *instrCode);
void ParseJgeInstr(InstructionSet *instrCode);
void ParseJaeInstr(InstructionSet *instrCode);
void ParseJleInstr(InstructionSet *instrCode);
void ParseJbeInstr(InstructionSet *instrCode);
void ParseJmpInstr(InstructionSet *instrCode);
void ParsePushlInstr(InstructionSet *instrCode);
void ParsePoplInstr(InstructionSet *instrCode);
void ParseIncbInstr(InstructionSet *instrCode);
void ParseIncwInstr(InstructionSet *instrCode);
void ParseInclInstr(InstructionSet *instrCode);
void ParseDecbInstr(InstructionSet *instrCode);
void ParseDecwInstr(InstructionSet *instrCode);
void ParseDeclInstr(InstructionSet *instrCode);
void ParseRepInstr(InstructionSet *instrCode);
void ParseCallInstr(InstructionSet *instrCode);
void ParseOrlInstr(InstructionSet *instrCode);
void ParseXorlInstr(InstructionSet *instrCode);
void ParseAndlInstr(InstructionSet *instrCode);
void ParseShllInstr(InstructionSet *instrCode);
void ParseSHLDInstr(InstructionSet *instrCode);
void ParseSarlInstr(InstructionSet *instrCode);
void ParseShrlInstr(InstructionSet *instrCode);
void ParseAddlInstr(InstructionSet *instrCode);
void ParseSublInstr(InstructionSet *instrCode);
void ParseImullInstr(InstructionSet *instrCode);
void ParseMovlInstr(InstructionSet *instrCode);
void ParseMovbInstr(InstructionSet *instrCode);
void ParseMovwInstr(InstructionSet *instrCode);
void ParseCmplInstr(InstructionSet *instrCode);
void ParseTestInstr(InstructionSet *instrCode);
void ParseMovsblInstr(InstructionSet *instrCode);
void ParseMovswlInstr(InstructionSet *instrCode);
void ParseMovzblInstr(InstructionSet *instrCode);
void ParseMovzwlInstr(InstructionSet *instrCode);
void ParseMovzxInstr(InstructionSet *instrCode);
void ParseLealInstr(InstructionSet *instrCode);


// static void (*parseInstructionFunctions[INSTRUCTION_SETS_SIZE])(InstructionSet *instrCode) = {
// 	#define GENERAGE_FUNCTION(name)		Parse##name##Instr,
// 	#include "function_name.txt"
// 	#undef GENERAGE_FUNCTION
// };

#endif
