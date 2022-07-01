#include "symbol.h"
#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "exprchk.h"
#include "tranexpr.h"
#include "dumpir.h"
#include "output.h"

// #define DST irinst->opds[0]
// #define SRC1 irinst->opds[1]
// #define SRC2 irinst->opds[2]
// #define OP irinst->opcode

char *OPCODENAMES[] = {
#define OPCODE(opcode, opcodeName, func) opcodeName,
#include "opcode.h"
#undef OPCODE
};

// 存储中间码的文件的句柄。
FILE *IRFile;

void DumpIR(IRInst irinst)
{
	fprintf(IRFile, "%s", "\t");

	switch(OP){
		case JMP:
			{
				fprintf(IRFile, "%s %s\n", "goto",((BBlock)DST)->sym->name);
				break;
			}
		case RET:
			{
				fprintf(IRFile, "%s %s\n", "return", DST->name);
				break;
			}
		case MOV:
			{
				fprintf(IRFile, "%s %s, %s\n", "MOV", DST->name, SRC1->name);	
				break;
			}
		case CALL:
			{
				fprintf(IRFile, "call %s\n", SRC1->name);
				break;
			}
		case JZ:
			{
				fprintf(IRFile, "if(!%s) goto %s\n", SRC1->name, ((BBlock)DST)->sym->name);
				break;
			}
		case JNZ:
			{
				fprintf(IRFile, "if(%s) goto %s\n", SRC1->name, ((BBlock)DST)->sym->name);
				break;
			}
		case JE:
		case JNE:
		case JL:
		case JLE:
		case JG:
		case JGE:
			{
				char *src1Name = SRC1->name;
				char *opcode = OPCODENAMES[OP];
				char *src2Name = SRC2->name;
				char *dstName = ((BBlock)DST)->sym->name;
				fprintf(IRFile, "if(%s %s %s) goto %s\n", src1Name, opcode, src2Name, dstName);
				break;
			}
		case ADD:
		case SUB:
			{
				fprintf(IRFile, "%s: %s %s %s\n", DST->name, SRC1->name, OPCODENAMES[OP], SRC2->name);
				break;
			}
		default:
			{


				break;
			}

	}
//		if(irinst->opcode == JMP){
//			BBlock target = (BBlock)irinst->opds[0];
//			printf("%s %s\n", "JMP", target->sym->name);
//		}else if(irinst->opcode == RET){
//			if(irinst->opds[0]->kind == SK_CONSTANT){
//				printf("%s %d\n", "RET", irinst->opds[0]->val.i[0]);
//			}else{
//				printf("%s %s\n", "RET", irinst->opds[0]->name);
//			}
//		}else{
//		printf("dst name = %s,", irinst->opds[0]->name);
//		if(irinst->opds[1] != NULL){
//			if(irinst->opds[1]->kind == SK_CONSTANT){
//				printf("src val = %d,", irinst->opds[1]->val.i[0]);
//			}else{
//				printf("src name = %s,", irinst->opds[1]->name);
//			}
//		}
//
//		if(irinst->opds[2] != NULL){
//			if(irinst->opds[2]->kind == SK_CONSTANT){
//				printf("src2 val = %d,", irinst->opds[2]->val.i[0]);
//			}else{
//				printf("src2 name = %s,", irinst->opds[2]->name);
//			}
//		}
//		}

		printf("\n");
}

void DumpBBlock(BBlock bblock)
{
	fprintf(IRFile, "%s:\n", bblock->sym->name);
	// 一个基本块中的中间码组成一个双向链表。我写过一次遍历双向链表的代码。
	// 此刻，再写一次。下一次，我就直接复用之前的代码。
	IRInst head = &bblock->irinst;
	IRInst irinst = head->next;
	// 遍历双向链表，终止条件是：当前结点是头结点。
	while(irinst != head){
		DumpIR(irinst);
		irinst = irinst->next;	
	}
}

void DumpFunction(FunctionSymbol fsym)
{
	BBlock entryBB = fsym->entryBB;
	BBlock exitBB = fsym->exitBB;

	fprintf(IRFile, "function %s\n", fsym->name);

	// 所有的基本块组成一个双向链表。但是，函数的基本块有出口基本块，
	// 所以，遍历函数的基本块，只需要遍历到函数的出口基本块。
	BBlock bb = entryBB;
	while(bb != exitBB){
		DumpBBlock(bb);
		bb = bb->next;
	}

	DumpBBlock(exitBB);
	
	fprintf(IRFile, "\tret\n");

	fprintf(IRFile, "\n\n");
}

void DumpTranslateUnit(AstTranslationUnit transUnit)
{
	AstNode p = transUnit->decls;
	VariableSymbol sym;

	IRFile = fopen("ir.txt", "w+");

	while(p){
		if(p->kind == NK_Function){
			AstFunction func = (AstFunction)p;
			DumpFunction(func->fsym);
		}
		p = p->next;
	}
}
