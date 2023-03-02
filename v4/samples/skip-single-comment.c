#include <stdio.h>
#include <stdlib.h>

void skip_comment(char *str);


int main(int argc, char *argv[])
{
	char *str = "int main(int argc, char *argv[]) // test comment hello world";

	skip_comment(str);

	return 0;
}



void skip_comment(char *str)
{
	char ch;

	do{
		ch = *(str++);
		if(ch == '/'){
			printf("\t");
			// ch = (char)str++;
			ch = *(str++);
			if(ch != '/'){
				printf("error\n");
				exit(-1);
			}else{
				ch = *(str++);
			}

			// while(ch != '\n' || ch != '\0'){
			while(ch != '\n' && ch != '\0'){
				printf("%c", ch);
				
				ch = *(str++);
			}
			
			printf("\n");
		}

		printf("%c", ch);

	}while(ch != '\0');

	printf("\n");

}
