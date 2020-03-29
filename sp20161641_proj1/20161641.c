#include "20161641.h"
extern int (*functionPointer[20])(int,char[100][100]);
int main(){
	char input[100]; // 명령어를 읽어들일 char 배열 
	int argc;
	char argv[100][100];
	char tmpHist[100];
	instructionSetInit();
	while(1){
		printf("sicsim> ");
		fgets(input,100,stdin);
		strcpy(tmpHist,input);
		parser(input,&argc,argv);
		switch(functionPointer[classifyInput(argc,argv)](argc,argv)){
			case INPUT_ERROR:
				break;
			case INPUT_EXIT:
				return 0;
			case INPUT_NORMAL:
				storeHistory(tmpHist);
				break;
		}
	}
	return 1;
}
