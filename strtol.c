#include <stdio.h>
#include <stdlib.h>

int main(){
	char str[100];
	int num;
	while(1){
		scanf("%s",str);
		num = strtol(str,NULL,16);
		printf(">> %d\n",num);
	}
}
