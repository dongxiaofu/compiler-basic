#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char *str = "@how are you?@fine,thank you.@And you?@I'm fine too@";
	int len = strlen(str);

	int isEnd = 0;
	int index = 0;

	while(index < len){

		while(1){
			// if(index >= len) break;
	
			if(str[index] == '@' && isEnd == 1){
				index++;
				break;	
			}

			if(str[index] == '@'){
				isEnd = isEnd == 0 ? 1 : 0;
				index++;
			}
			
			printf("%c", str[index]);
			index++;
		}
		
		printf("\n");
	}


	return 0;
}
