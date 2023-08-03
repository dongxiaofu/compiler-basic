struct mblock
{
    struct mblock *next;
    char *begin;
    char *avail;
    char *end;
};

typedef struct heap
{
    struct mblock *last;
    struct mblock head;
} *Heap;

#define HEAP(hp)    struct heap  hp = { &hp.head }
#define True 1
#define False 0
#define END_OF_FILE -1

// 一个程序最多有MAX_INSTR_NUM条指令。究竟有多少？这是由什么决定的？
#define MAX_INSTR_NUM	4096
#define MAX_SIZE 4096
#define STATE_NO_STRING 0
#define STATE_IN_STRING	1
#define STATE_END_STRING 2

#define STATE_IN_FUNCTION 1
#define STATE_OUT_FUNCTION 0

// token的种类。
#define TYPE_TOKEN_INVALID			-1
#define TYPE_TOKEN_INSTR			0
#define TYPE_TOKEN_INT				1
#define TYPE_TOKEN_FLOAT			2
#define TYPE_TOKEN_STRING			3
#define TYPE_TOKEN_FUNCTION			4
#define TYPE_TOKEN_LABEL			5

#define TYPE_TOKEN_QUOT				6	//"
#define TYPE_TOKEN_COMMA			7	//,
#define TYPE_TOKEN_COLON			8	//:
#define TYPE_TOKEN_OPEN_PARENTHESES		9	//(
#define TYPE_TOKEN_CLOSE_PARENTHESES		10	//)
#define TYPE_TOKEN_PERCENT_SIGN				11	// %

#define TYPE_TOKEN_LONG				12	// .long
#define TYPE_TOKEN_BYTE				13	// .byte
#define TYPE_TOKEN_DATA				14	// .data
#define TYPE_TOKEN_GLOBL			15	// .globl
#define TYPE_TOKEN_TEXT				16	// .text
#define TYPE_TOKEN_KEYWORD_STRING			17	// .string

#define TYPE_TOKEN_NEWLINE					18	// \n
#define TYPE_TOKEN_INDENT					19	// 例如，str0

#define TYPE_TOKEN_REGISTER					20	// 寄存器，例如，%ebp
#define TYPE_TOKEN_IMMEDIATE				21	// 立即数，例如，$5

#define TYPE_TOKEN_STAR						22	// *

#define TYPE_TOKEN_COMM						23	// .comm
#define TYPE_TOKEN_ALIGN					24	// .align
#define TYPE_TOKEN_OBJECT					25	// @object
#define TYPE_TOKEN_SECTION					26	// .section
#define TYPE_TOKEN_RODATA					27	// .rodata
#define TYPE_TOKEN_TYPE					28	// .type
#define TYPE_TOKEN_SIZE						29	// .size

#define DATA_TYPE_INVALID					-1
#define DATA_TYPE_INT						0
#define DATA_TYPE_ADDRESS					1

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

typedef struct _lexer
{
	int string_state;
	int function_state;
	int token_type;
	int currentLine;
	int currentFuncIndex;
	int lineNum;
	int index0, index1;
	char lexeme[MAX_SIZE];
} *Lexer;

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

typedef struct _Label
{
	char name[MAX_SIZE];
	int index;
	// 行标签所在函数的索引。
	int functionIndex;
	// 行标签指向的指令的索引。
	int targetIndex;
} *Label;

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

// 例如@object。还有哪些值？不清楚。
#define SYMBOL_TYPE_OBJECT	1

// 数据类型，例如byte
#define DATA_TYPE_BYTE		1
#define DATA_TYPE_LONG		2
#define DATA_TYPE_STRING	3

union DataEntryValue{
	char *strVal;
	int numVal;	
};

typedef struct dataEntry{
	int symbolType;
	int size;
	char name[200];
	int dataType;
	int section;
	union DataEntryValue val;
} *DataEntry;

DataEntry dataEntryArray[100];
// error: increment of read-only variable 'dataEntryArrayIndex'
// const int dataEntryArrayIndex = 0;
static int dataEntryArrayIndex = 0;

void *MALLOC(int size);

int GetDataTypeSize(int dataType);

void GetInstrByMnemonic(char *mnemonic, InstrLookup *instrPtr);
int AddInstr(char *mnemonic, int opcode, int oprandNum);
void SetInstrOprandType(int instrIndex, int operandIndex, OpType type);
void InitInstrTable();

void Init();

int IsWhitespace(char ch);
int IsCharDelimeter(char ch);
int IsCharIdent(char ch);
int IsCharNumeric(char ch);
int IsStringInt(char *str);
int IsStringFloat(char *str);
int IsStringIdent(char *str);
int IsRegister(char *str);
int IsImmediateOperand(char *str);
void TrimWhitespace(char *str);
int SkipToNewline();

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
int GetNextToken();
int GetNextTokenExceptNewLine();
char *GetCurrentTokenLexeme();

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
void LoadFile(char *filename);
// void *MALLOC(int size);
int HeapAllocate(Heap heap, int size);


// 新增
char IsData(int token);
void ParseData();
