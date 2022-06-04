#ifndef GEN_H
#define GEN_H

#include "lex.h"
#include "stmt.h"
#include "type2.h"
#include "expr.h"

typedef struct irinst
{
	struct irinst *prev;
	struct irinst *next;
	Type ty;
	int opcode;
	Symbol opds[3];
} *IRInst;

// typedef struct *cfgedge CFGEdge;
typedef struct cfgedge *CFGEdge;

typedef struct bblock
{
	struct bblock *prev;
	struct bblock *next;
	IRInst irinst;
	CFGEdge precursors;
	CFGEdge successors;	
} *BBlock;

// TODO 很神奇的写法。cfgedge需用到bblock，而bblock又需用到cfgedge。应该把哪个放在前面呢？
// typedef struct cfgedge
struct cfgedge
{
	BBlock block;
	struct cfgedge *next;
// } *CFGEdge;
};

#endif
