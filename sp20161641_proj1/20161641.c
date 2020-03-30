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
		int caseNum = functionPointer[classifyInput(argc,argv)](argc,argv);
		switch(caseNum){
			case INPUT_ERROR:
				inappropriateInput(argc,argv);
				break;
			case INPUT_EXIT:
				return 0;
			case INPUT_NORMAL:
				storeHistory(tmpHist);
				break;
			default : 
				printError(caseNum);
		}
	}
	return 1;
}
