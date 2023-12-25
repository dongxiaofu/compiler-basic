#ifndef LINK_LIST_H
#define LINK_LIST_H

#include "common.h"

// typedef enum{
// 	NUM, STRING, SYMBOL_LINK
// } NODE_VALUE;
// 
// typedef union value{
// 	int num;
// 	char *str;
// 	SymbolLink symLink;
// } Value;

typedef struct node{
	Value val;
	NODE_VALUE type;	
	struct node *pre;
	struct node *next;
} *Node;

Node InitLinkList();

void AppendNode(Node list, Node node);
// TODO 链表的节点的值都不相同，查找才有意义。
// 当值是字符串、整型时，根据值可以查找节点。当节点的值是其他，例如结构体，我不知道怎么查找。
// 扩展？以后会遇到什么情况还是未知，我没法现在设计出能处理未知情况的代码。
// 先写完再说。
Node FindNodeByStr(Node list, char *str);
Node FindNodeByNum(Node list, int num);
Node MergeLinkList(Node list1, Node list2);

#endif
