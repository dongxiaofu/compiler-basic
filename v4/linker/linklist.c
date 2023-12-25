#include "linklist.h"

Node InitLinkList()
{
	Node node = (Node)MALLOC(sizeof(struct node));
	node->pre = node;
	node->next = node;

	return node;
}

// void AppendNode(Node list, char *str)
void AppendNode(Node list, Node node)
{
	if(list == NULL){
		printf("list can't be NULL in %d in %s\n", __LINE__, __FILE__);
		exit(-1);
	}

//	// 遍历链表。
//	Node currentNode = list;
//	Node preNode = NULL;
//	while(currentNode != NULL){
//		preNode = currentNode;
//		currentNode = currentNode->next;
//	}
//
//	preNode->next = node;

	Node currentNode = list;
	currentNode->pre->next = node;
	node->pre = currentNode->pre;
	node->next = currentNode;
	currentNode->pre = node;
}

Node FindNodeByStr(Node list, char *str)
{
	Node node = NULL;
	// 遍历链表。
	Node currentNode = list;
	Node preNode = NULL;
	// while(currentNode->pre != currentNode->next){
	while(currentNode != list){
		if(strcmp(currentNode->val.str, str) == 0){
			node = currentNode;
			break;
		}
		preNode = currentNode;
		currentNode = currentNode->next;
	}

	return node;
}

Node FindNodeByNum(Node list, int num)
{
	Node node = NULL;
	// 遍历链表。
	Node currentNode = list;
	Node preNode = NULL;
	// while(currentNode->pre != currentNode->next){
	while(currentNode != list){
		if(currentNode->val.num == num){
			node = currentNode;
			break;
		}
		preNode = currentNode;
		currentNode = currentNode->next;
	}

	return node;
}

Node MergeLinkList(Node list1, Node list2)
{
	if(list1 == NULL){
		return list2;
	}

	if(list2 == NULL){
		return list1;
	}

	list1->pre->next = list2->next;	
	list2->next->pre = list1->pre;
	list2->pre->next = list1;
	list1->pre = list2->pre;

	return list1;
}
