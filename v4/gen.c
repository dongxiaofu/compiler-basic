#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "symbol.h"
#include "gen.h"

void AppendIRInst(IRInst irinst)
{
	// TODO the code is error
//	IRInst lastIrinst = CurrentBBlock->irinst;
//	lastIrinst->next = irinst;
//	irinst->prev = lastIrinst;
//	CurrentBBlock->irinst->prev->next = irinst;
//	irinst->prev = CurrentBBlock->irinst->prev->next;
//	irinst->next = CurrentBBlock->irinst;
//	CurrentBBlock->irinst->prev = irinst;

	CurrentBBlock->irinst.prev->next = irinst;
	irinst->prev = CurrentBBlock->irinst.prev->next;
	irinst->next = &CurrentBBlock->irinst;
	CurrentBBlock->irinst.prev = irinst;
}

void GenerateMov(Type ty, Symbol dst, Symbol src)
{
	IRInst irinst = (IRInst)MALLOC(sizeof(struct irinst));
	irinst->ty = dst->ty;
	irinst->opcode = MOV;
	irinst->opds[0] = dst;
	irinst->opds[1] = src;
	AppendIRInst(irinst);
}

void GenerateJmp(BBlock bb)
{
	DrawCFGEdge(CurrentBBlock, bb);

	IRInst irinst = (IRInst)MALLOC(sizeof(struct irinst));
	irinst->ty = T(VOID);
	irinst->opcode = JMP;
	irinst->opds[0] = bb;
	irinst->opds[1] = irinst->opds[2] = NULL;
	AppendIRInst(irinst);
}

void GenerateBranch(Type ty, BBlock dest, int opcode, Symbol src1, Symbol src2)
{
	DrawCFGEdge(CurrentBBlock, dest);

	IRInst irinst = (IRInst)MALLOC(sizeof(struct irinst));
	irinst->ty = ty;
	irinst->opcode = opcode;
	irinst->opds[0] = (Symbol)dest;
	irinst->opds[1] = src1;
	irinst->opds[2] = src2;
	AppendIRInst(irinst);
}

void GenerateFunctionCall(Type ty, Symbol recv, Symbol faddr, ArgBucket arg)
{
	// 生成一条中间码，函数调用的中间代码。
	IRInst irinst = (IRInst)MALLOC(sizeof(struct irinst));
	// TODO 函数的返回值的数据类型是什么？这是我随意设置的。
	irinst->ty = T(INT);
	irinst->opcode = CALL;
	irinst->opds[0] = recv;
	irinst->opds[1] = faddr;
	irinst->opds[2] = arg;
	AppendIRInst(irinst);	
}

void GenerateAssign(Type ty, int opcode, Symbol dst, Symbol src1, Symbol src2)
{
	IRInst irinst = (IRInst)MALLOC(sizeof(struct irinst));
	irinst->ty = ty;
	irinst->opcode = opcode;
	irinst->opds[0] = dst;
	irinst->opds[1] = src1;
	irinst->opds[2] = src2;
	AppendIRInst(irinst);
}

void AppendPrecursors(BBlock head, BBlock tail)
{
	CFGEdge headCfgedge = (CFGEdge)MALLOC(sizeof(struct cfgedge));
	headCfgedge->block = head;

	CFGEdge tailCfgedge = (CFGEdge)MALLOC(sizeof(struct cfgedge));
	tailCfgedge->block = tail;

	CFGEdge precursors = tail->precursors;
	headCfgedge->next = precursors;
	tail->precursors = headCfgedge;
}

void AppendSuccessors(BBlock tail, BBlock head)
{
	CFGEdge headCfgedge = (CFGEdge)MALLOC(sizeof(struct cfgedge));
	headCfgedge->block = head;

	CFGEdge tailCfgedge = (CFGEdge)MALLOC(sizeof(struct cfgedge));
	tailCfgedge->block = tail;

	CFGEdge successors = head->successors;
	tailCfgedge->next = successors;
	head->successors = tailCfgedge;
}

void DrawCFGEdge(BBlock head, BBlock tail)
{
	AppendPrecursors(head, tail);
	AppendSuccessors(tail, head);
}

BBlock CreateBBlock()
{
	BBlock bblock = (BBlock)MALLOC(sizeof(struct bblock));
	bblock->irinst.opcode = NOP;	
	bblock->irinst.prev = bblock->irinst.next = &bblock->irinst;
	
	return bblock;
}

void StartBBlock(BBlock bblock)
{
	CurrentBBlock->next = bblock;
	bblock->prev = CurrentBBlock;

	IRInst lastIrinst = CurrentBBlock->irinst.prev;
	if(lastIrinst->opcode != JMP){
		DrawCFGEdge(CurrentBBlock, bblock);
	}

	CurrentBBlock = bblock;
}
