#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "common.h"

// ---- Operand Type Bitfield Flags ---------------------------------------------------

// The following constants are used as flags into an operand type bit field, hence
// their values being increasing powers of 2.

#define OP_FLAG_TYPE_INT        1           // Integer literal value
#define OP_FLAG_TYPE_FLOAT      2           // Floating-point literal value
#define OP_FLAG_TYPE_STRING     4           // Integer literal value
#define OP_FLAG_TYPE_MEM_REF    8           // Memory reference (variable or array
// index, both absolute and relative)
#define OP_FLAG_TYPE_LINE_LABEL 16          // Line label (used for jumps)
#define OP_FLAG_TYPE_FUNC_NAME  32          // Function table index (used for Call)
#define OP_FLAG_TYPE_REG        64         // Register

// ---- Assembled Instruction Stream ------------------------------------------------------

#define OP_TYPE_INT                 0           // Integer literal value
#define OP_TYPE_FLOAT               1           // Floating-point literal value
#define OP_TYPE_STRING_INDEX        2           // String literal value
#define OP_TYPE_ABS_STACK_INDEX     3           // Absolute array index
#define OP_TYPE_REL_STACK_INDEX     4           // Relative array index
#define OP_TYPE_INSTR_INDEX         5           // Instruction index
#define OP_TYPE_FUNC_INDEX          6           // Function index
#define OP_TYPE_REG                 7           // Register
#define OP_TYPE_REG_MEM             8           // 处理像-4(%ebp)这样的内存操作数。
#define OP_TYPE_LABEL_INDEX			9
#define OP_TYPE_MEM             	10           // 处理像num这样的内存操作数。

// 操作码
#define OPCODE_ADD                  1
#define OPCODE_SUBL					OPCODE_ADD + 1
#define OPCODE_PUSHL                OPCODE_SUBL + 1
#define OPCODE_POPL                 OPCODE_PUSHL + 1
#define OPCODE_MOVL                 OPCODE_POPL + 1
#define OPCODE_JMP                  OPCODE_MOVL + 1
#define OPCODE_CALL                 OPCODE_JMP + 1
#define OPCODE_RET                  OPCODE_CALL + 1

#define REG_INVALID					-1
// 寄存器
// 8个通用寄存器：EAX、EBX、ECX、EDX、ESI、EDI、ESP、EBP
#define REG_EAX						0
#define REG_EBX						1
#define REG_ECX						2
#define REG_EDX						3
#define REG_ESI						4
#define REG_EDI						5
#define REG_ESP						6
#define REG_EBP						7

#define INVALID_INSTR_STREAM_INDEX	-1
#define NO_FUNCTION -1

// tokenType
#define TOKEN_INVALID			1
#define TOKEN_FUNC				2
#define TOKEN_LABEL				3
#define TOKEN_INSTR				4
#define TOKEN_IDENT				5

// 段表中的条目的索引。
typedef enum {
	TEXT = 1, DATA, RO_DATA, SYM_TAB, STR_TAB, REL_TEXT, REL_DATA, SH_STR_TAB 
} SECTION_HEADER_ENTRY_INDEX;

typedef struct _OpMemory
{
	int length;
	char *name;                     // 变量名称
} OpMemory;

typedef struct _Op                              // An assembled operand
{
  int iType;                                  // Type
  union                                       // The value
  {
      int iIntLiteral;                        // Integer literal
      float fFloatLiteral;                    // Float literal
      int iStringTableIndex;                  // String table index
      int iStackIndex;                        // Stack index
      int iInstrIndex;                        // Instruction index
	  int iLabelIndex;						  // Label index
      int iFuncIndex;                         // Function index
      int iReg;                               // Register code
//	  int variableNameLength;
//	  char *variableName;					  // 变量名称
	  OpMemory iMemory;						  // 变量名称
  };
  int iOffsetIndex;                           // Index of the offset
} *Op;

typedef int OpType;

typedef struct _InstrLookup
{
	char mnemonic[MAX_SIZE];
	int opcode;
	int oprandNum;
	OpType oprand[10];	
// }*Instr;
} InstrLookup;

typedef struct _Instr
{
	int opcode;
	int oprandNum;
	// Op *opList;
	Op opList;
// }*Instr;
} Instr;


typedef struct _String
{
	char name[MAX_SIZE];
	int index;
	char val[MAX_SIZE];
} *String;

typedef struct _Variable
{
	char name[MAX_SIZE];
	int index;
	int address;
} *Variable;

typedef struct _Function
{
	char name[MAX_SIZE];
	int index;
	// 函数的第一条指令在指令表中的索引。
	int entryPoint;
} *Function;


typedef union _SymbolVal
{
	unsigned int val;
	char address[MAX_SIZE];
} *SymbolVal;

typedef struct _Symbol
{
	char name[MAX_SIZE];
	int index;
	// 用size和value记录字符串、数组、浮点数据。
	int size;
	int *value;
	int dataType;
	int isString;
	SymbolVal val;
} *Symbol;

typedef struct _ListNode
{
	void *pData;
	struct _ListNode *next;
} *ListNode;

typedef struct _LinkedList
{
	ListNode pHead;
	ListNode pTail;
	int num;
} *LinkedList;

// 文件头
typedef struct _ScriptHeader
{
	// 数据大小。
	int size;
	char ID[MAX_SIZE];
	int majorVersion;
	int minorVerson;
	int hasMainFunction;
	int mainFunctionIndex;
	int instrNum;
} *ScriptHeader;

// 全局字符串表
LinkedList StringTable;
// 全局变量表
LinkedList SymbolTable;
// 行标签表
LinkedList LineLabelTable;
// 函数表
LinkedList FunctionTable;
// 变量表
LinkedList VariableTable;

// 指令表
InstrLookup InstrTable[MAX_INSTR_NUM];
// 一个程序中的所有指令存储在这个指令表中。
Instr InstrStream[MAX_INSTR_NUM];

// 文件头
ScriptHeader gScriptHeader;

#define STACK_SCALE_VALUE		4

// 新增
// section name
#define SECTION_TEXT	1
#define SECTION_DATA	2
#define SECTION_RODATA	3
#define SECTION_BSS		4
#define SECTION_COM		5
#define SECTION_ABS		6
#define SECTION_UND		7

// .symtab中的一些特殊Ndx
#define SECTION_NDX_UND 0x0000
#define SECTION_NDX_ABS 0xfff1
#define SECTION_NDX_COM 0xfff2


// 例如@object。还有哪些值？不清楚。
// todo 找机会修改成enum。我不想马上修改，因为需要修改很多地方。
#define SYMBOL_TYPE_NOTYPE	0
#define SYMBOL_TYPE_OBJECT	1
#define SYMBOL_TYPE_FUNC	2
#define SYMBOL_TYPE_SECTION	3
#define SYMBOL_TYPE_FILE	4

// 数据类型，例如byte
#define DATA_TYPE_BYTE		1
#define DATA_TYPE_LONG		2
#define DATA_TYPE_STRING	3

// .rodata在段表中的索引。
#define RODATA_SYMTAB_INDEX 2

union SectionDataVal{
   NumericData numVal;
   char *strVal;
   // 和系统的命名风格一致，所以，这两个变量名比较怪异。
   Elf32_Sym *Elf32_Sym_Val;
   Elf32_Rel *Elf32_Rel_Val;
   Elf32_Shdr *Elf32_Shdr_Val;
   // TODO 不知道怎么存储机器指令。
};

enum DataEntryValueType{
	STR = 1, NUM
};

typedef struct sectionDataNode{
   // 这是新增的。需要根据name来查找对应的元素。最开始，我哪会想到这些呢？
   char name[100];
   union SectionDataVal val;
   enum DataEntryValueType valType;
   int index;
   char isLast;
   struct sectionDataNode *next;
}* SectionDataNode;

union DataEntryValue{
	char *strVal;
	NumericData numVal;	
};

// TODO 这种结构设计得合理吗？可以使用。
typedef struct dataEntryValueNode{
	union DataEntryValue val;
	enum DataEntryValueType type;
	struct dataEntryValueNode *next;
} *DataEntryValueNode;

enum BindType{
	LOCAL = 0, GLOBAL, WEAK
};

// enum RelocationType{
//	R_386_32 = 0, R_386_PC32
// };

// todo 数值应该上多少？没有看资料，我随意设置的。
// 就两个数值，不设计成enum。enum似乎不能直接当成int等数据类型使用。
// 发现我已经在elf.h中定义了这两个变量。
// #define R_386_32 	0
// #define R_386_PC32	1

typedef struct dataEntry{
	int symbolType;
	// 所有数据的长度。
	int size;
	// 数据类型的大小。
	int dataTypeSize;
	char name[200];
	int dataType;
	int section;
	int isRel;		// 是否需要重定位
	int align;		// 对齐
	DataEntryValueNode valPtr;
	// valPtr链表中的节点的数量。
	int dataEntryValueNodeNum;
	// 在各自的section中的偏移量，例如，在.data中的偏移量，在.rodata中的偏移量。
	int offset;	
	// todo bind应该设计成其他数据类型吗？
	// 想把bind的值设计成enum，又嫌麻烦。直接用1和2多简单。
	enum BindType bind;		// LOCAL,GLOBAL
} *DataEntry;

typedef struct strtabEntry{
	char name[100];
	int offset;
	int length;
	int index;
	struct strtabEntry *next;
} *StrtabEntry;

// Bind为globl的变量。
typedef struct globlVariableNode{
	char name[100];
	struct globlVariableNode *next;
} *GloblVariableNode;

typedef struct segmentInfo{
	char name[100];
	int size;
	struct segmentInfo *next;
} *SegmentInfo;

// TODO 想不到更好的名字。这完全是重构时产生的一个结构体。
typedef struct sectionData{
	SectionDataNode relText;
	SectionDataNode data;
	SectionDataNode relData;
	SectionDataNode rodata;
	SectionDataNode symtab;
	SectionDataNode strtab;
	SectionDataNode shstrtab;
	SectionDataNode sectionHeader;
}*SectionData;

typedef struct sectionOffset{
  unsigned int text;
  unsigned int relText;
  unsigned int data;
  unsigned int relData;
  unsigned int rodata;
  unsigned int symtab;
  unsigned int strtab;
  unsigned int shstrtab;
  unsigned int sectionHeader;
}*SectionOffset;

// 存储Bind为globl的变量的单链表。
// todo 是否需要用static修饰？
// globlVariableList是链表的头结点，preGloblVariablenNode是在构造链表的过程中使用的变量。
// 才过了十天左右，我已经看不懂自己设计的两个变量了，所以，增加一些注释。
static GloblVariableNode globlVariableList;
GloblVariableNode preGloblVariablenNode;

DataEntry dataEntryArray[100];
// error: increment of read-only variable 'dataEntryArrayIndex'
// const int dataEntryArrayIndex = 0;
static int dataEntryArrayIndex = 0;

StrtabEntry strtabEntryArray[100];
static int strtabEntryArrayIndex = 0;
static int strtabEntryOffset = 0;

// LOCAL变量的数目。
// TODO 在GenerateSymtab中会看到一个数组。这个数组生成默认的六个LOCAL符号。
static unsigned int localVariableCount = 6;

static StrtabEntry strtabEntryList;
StrtabEntry preStrtabEntryNode;
// 把StrtabEntry中的name连接成字符串。
typedef struct strtabString{
	char *string;
	unsigned int length;
} *StrtabString;
static StrtabString strtabString;

static int dataEntryOffset = 0;
static int rodataEntryOffset = 0;

#define SHSTRTAB_ENTRY_ARRAY_SIZE	9
char *shstrtabEntryArray[SHSTRTAB_ENTRY_ARRAY_SIZE] = {"\000", ".text" ,".data" ,".rodata" ,".symtab" ,".strtab" ,".rel.text" ,".rel.data" ,".shstrtab"};


//char *shstrtabEntryArray[SHSTRTAB_ENTRY_ARRAY_SIZE] = {"\000", ".text", ".strtab", ".shstrtab" }; 

#define BSS_DATA_ENTRY_ARRAY_SIZE 100
DataEntry bssDataEntryArray[BSS_DATA_ENTRY_ARRAY_SIZE];
static int bssDataEntryArrayIndex = 0;

// TODO 调试用。
// 我不知道static是不是必需的。
// 这是临时的。
#define shstrtabArr_SIZE 8
static char *shstrtabArr[shstrtabArr_SIZE] = {"\000", ".rel.text",".rel.data",".bss",".rodata",".symtab",".strtab",".shstrtab"};
static char *shStrTabStr = "\000.rel.text\000.rel.data\000.bss\000.rodata\000.symtab\000.strtab\000.shstrtab\000";
//
//#define shstrtabArr_SIZE 3
//static char *shstrtabArr[shstrtabArr_SIZE] = {"\000", ".text",".shstrtab"};
//static char *shStrTabStr = "\000.text\000.shstrtab\000";

//#define shstrtabArr_SIZE 4
//static char *shstrtabArr[shstrtabArr_SIZE] = {"\000", ".text", "strtab", ".shstrtab"};
//static char *shStrTabStr = "\000.text\000.strtab\000.shstrtab\000";
// 链表的第一个节点。
// static SegmentInfo segmentInfoNode = NULL;
static SegmentInfo segmentInfoNode;
static SegmentInfo preSegmentInfoNode;

static SectionDataNode preRelTextDataNode;
static Instruction instrHead;
// TODO 可能无用。
// static SectionDataNode relTextDataNode;

// enum FPUInstructionSet{
// 	#include "../include/fpu_instruction_set.txt"
// };
// 
// enum InstructionSet{
// 	#include "../include/instruction_set.txt"
// };

// enum InstructionSet{
// 	#include "../include/fpu_instruction_set.txt"
// 链表的第一个节点。
// static SegmentInfo segmentInfoNode = NULL;
static SegmentInfo segmentInfoNode;
static SegmentInfo preSegmentInfoNode;

static SectionDataNode preRelTextDataNode;
static Instruction instrHead;
// TODO 可能无用。
// static SectionDataNode relTextDataNode;

// enum FPUInstructionSet{
// 	#include "../include/fpu_instruction_set.txt"
// };
// 
// enum InstructionSet{
// 	#include "../include/instruction_set.txt"
// };

// enum InstructionSet{
// 	#include "../include/fpu_instruction_set.txt"
// 	#include "../include/instruction_set.txt"
// };
//
// typedef enum{
// //	#define ARRAY_ELEMENT(code, name)	code,
// 	#define INSTR_ELEMENT(code, name)   code,
// 	// #include "../include/all_instruction_set.txt"
// 	#include "all_instruction_set.txt"
// 	#undef INSTR_ELEMENT
// //	#undef ARRAY_ELEMENT
// }InstructionSet;
// 
// #define INSTRUCTION_SETS_SIZE 77
// // char instructionSets[76][10] = {
// // char instructionSets[INSTRUCTION_SETS_SIZE][8] = {
// static char instructionSets[INSTRUCTION_SETS_SIZE][8] = {
// 	// #define ARRAY_ELEMENT(str)	"##str##"
// 	// #define ARRAY_ELEMENT(str)	str,
// 	// #define ARRAY_ELEMENT(code, name)	name,
// 	#define INSTR_ELEMENT(code, name)   name,
// 	// #include "../include/all_instruction_set.txt"
// 	#include "all_instruction_set.txt"
// 	#undef INSTR_ELEMENT
// //	#undef ARRAY_ELEMENT
// };

// static void (*parseInstructionFunctions[INSTRUCTION_SETS_SIZE])(InstructionSet *instrCode) = {
// 	// #define GENERAGE_FUNCTION(name)		void Parse##name##Instr(InstructionSet *instrCode),
// 	#define GENERAGE_FUNCTION(name)		Parse##name##Instr,
// 	#include "function_name.txt"
// 	#undef GENERAGE_FUNCTION
// };

int GetDataTypeSize(int dataType);

void GetInstrByMnemonic(char *mnemonic, InstrLookup *instrPtr);
int AddInstr(char *mnemonic, int opcode, int oprandNum);
void SetInstrOprandType(int instrIndex, int operandIndex, OpType type);
void InitInstrTable();

void Init();

char GetLookAheadChar();
int GetLookAheadToken();

int AddNode(LinkedList table, void *pData);
int AddString(char *name, char *val);
int AddVariable(char *name, int address);
String GetString(char *name);
Variable GetVariable(char *name);

Label GetLabel(char *name);
Label GetLabelByIndex(int index);
int AddLabel(char *name, int functionIndex, int targetIndex);


int AddSymbol(char *name, int isString, int dataType, SymbolVal val);
Symbol GetSymbol(char *name);
Function GetFunction(char *name);
Function GetFunctionByIndex(int index);
int AddFunction(char *name);

int GetImmediateVal(char *immediateOprand);
int GetRegIndex(char *regName);

void RestLexer();
void FindFunctionOrLabel();

int CheckTokenType(int token);

void DealWithOprandImmediate(InstrLookup instrLookup, Op opList, char *oprandLexeme, int opIndex);
void DealWithOprandRegister(InstrLookup instrLookup, Op opList, char *oprandLexeme, int opIndex);
void DealWithOprandIndent(InstrLookup instrLookup, Op opList, char *oprandLexeme, int opIndex);
void DealWithOprandInt(char *oprandLexeme, Op opList, int opIndex);
void DealWithOprand(InstrLookup instrLookup);
void DealWithInstruction();

void ReplaceLabelWithInstrStreamIndex();
void AssembleInstruction();

void AssmblSourceFile();

void SaveVariable();
void BuildXE();
int RoundUpNum(int num, int alignment);
void CalculateDataEntryOffset();
void LoadFile(char *filename);


// 新增
// todo 自定义统计字符串的长度的函数。我想不到更好的名字。
int myStrlen(char *name);
int FindShStrTabOffset(char *name);
int FindDataEntryIndex(char *name);
DataEntry FindDataEntry(char *name);
int FindStrtabEntryIndex(char *name);
StrtabEntry FindStrtabEntry(char *name);
SectionDataNode FindSectionDataNode(char *name, SectionDataNode head);
SectionDataNode FindSymbolSectionDataNode(char *name, SectionDataNode head);
StrtabEntry FindEntryInStrtabEntryList(char *name);
int FindIndexInStrtabEntryList(char *name);
int FindShstrtabEntry(char *name);
SegmentInfo FindSegmentInfoNode(char *name);
GloblVariableNode FindGloblVariableNode(char *name);
void AddGloblVariableNode(char *name);
unsigned char isGloblVariable(char *name);
char IsData(int token);
void AddStrtabEntry(DataEntry entry);
void AddStrtabEntryListNode(StrtabEntry node);
void ParseData();
Instruction DealWithInstr(InstructionSet instrCode);
void ParseInstr();
void CalculateStrtabEntryOffset();
void ReSortStrtab();
Elf32_Ehdr *GenerateELFHeader();
SectionData GetSectionData();
void GenerateSymtab(SectionDataNode symtabDataNode);
void GenerateRelData(SectionDataNode relDataDataHead, SectionDataNode symtabDataHead);
void GenerateSectionHeaders(SectionDataNode sectionHeaderDataHead, SectionOffset sectionOffset);
void WriteELF(Elf32_Ehdr *ehdr, SectionOffset sectionOffset, SectionData sectionData);
void BuildELF();


#endif
