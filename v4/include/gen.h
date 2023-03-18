#ifndef GEN_H
#define GEN_H

#include "lex.h"
#include "stmt.h"
#include "type2.h"
#include "ast.h"
#include "expr.h"

// TODO 中间代码中的opcode
// enum {ADDR,DEREF,ADD,MOV,CALL,JMP,JZ,JNZ,JE,JNE,JL,JLE,JG,JGE,DEC, INC, RET, NOP};
enum {
#define OPCODE(opcode,opcodeName, func)		opcode,
#include "opcode.h"
#undef OPCODE
};

// typedef struct irinst
// {
// 	struct irinst *prev;
// 	struct irinst *next;
// 	Type ty;
// 	int opcode;
// 	Symbol opds[3];
// } *IRInst;

// typedef struct *cfgedge CFGEdge;
// typedef struct cfgedge *CFGEdge;
// 
// typedef struct bblock
// {
// 	struct bblock *prev;
// 	struct bblock *next;
// 	IRInst irinst;
// 	Symbol sym;
// 	CFGEdge precursors;
// 	CFGEdge successors;	
// } *BBlock;

// TODO 很神奇的写法。cfgedge需用到bblock，而bblock又需用到cfgedge。应该把哪个放在前面呢？
// typedef struct cfgedge
struct cfgedge
{
	BBlock block;
	struct cfgedge *next;
// } *CFGEdge;
};

// static BBlock CurrentBBlock;

void AppendPrecursors(BBlock head, BBlock tail);
void AppendSuccessors(BBlock head, BBlock tail);
void DrawCFGEdge(BBlock head, BBlock tail);

void AppendIRInst(IRInst irinst);
void GenerateMov(Type ty, Symbol dst, Symbol src);
void GenerateIndirectMove(Type ty, Symbol dst, Symbol src);
void GenerateJmp(BBlock bb);
void GenerateBranch(Type ty, BBlock dest, int opcode, Symbol src1, Symbol src2);
void GenerateReturn(Type ty, Symbol dest);
void GenerateAssign(Type ty, int opcode, Symbol dst, Symbol src1, Symbol src2);
void GenerateFunctionCall(Type ty, Symbol recv, Symbol faddr, ArgBucket arg);

BBlock CreateBBlock();
void StartBBlock(BBlock bblock);


#endif
