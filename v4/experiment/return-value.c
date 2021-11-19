#include <stdio.h>

typedef struct node{
	int a;
	struct node *next;
} *Node;

typedef struct person{
	int age;
	int height;
} *Person;

int test();
Node test2();
Person test5();
char *test3();
int *test4();

int main(int argc, char *argv[]){

	int b = test();
	printf("b = %d\n", b);

	Node node2 = test2();
	printf("node.a = %d\n", node2->a);

	Person p1 = test5();
	printf("p1.age = %d, p2.height = %d\n", p1->age, p1->height);

	char *str = test3();
	printf("str = %s\n", str);

	int *c = test4();
	printf("*c = %d\n", *c);

	return 0;
}

int test(){
	int a = 5;
	return a;
}

Node test2(){
	Node node1;
	struct node node2 = {3,NULL};
	// struct node node2 = {3};
	node1 = &node2;
	return node1;
}

Person test5(){
	Person p1;
	struct person p2 = {83};
	// struct node node2 = {3};
	p1 = &p2;
	return p1;
}

char *test3(){
	char str[20] = "hello,world";
	
	return str;
}

int *test4(){
	int a = 78;
	int *b = &a;
	return b;
}

