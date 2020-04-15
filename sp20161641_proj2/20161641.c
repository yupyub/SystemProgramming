#include "20161641.h"
extern int (*functionPointer[20])(int,char[100][100]); // InstructionProcessing.c 에 선언됨
int main(){
	char input[100]; // 명령어를 읽어들일 char 배열 
	int argc; // 파싱 후 분할된 인자들의 수
	char argv[100][100]; // 파싱 후 분할되어 저장되는 인자들 저장
	char tmpHist[100]; // 파싱 과정에서 input배열이 변경됨으로 history 저장을 위해 기존 명령어 저장
	instructionSetInit(); // instruction 들을 읽어들여 저장
	makeOpcodeTable(); // opcode hash table을 만든다
	while(1){
		printf("sicsim> ");
		fgets(input,100,stdin);
		input[strlen(input)-1] = '\0'; // fgets는 개행까지 읽어들임으로 개행을 '\0'으로 변경
		strcpy(tmpHist,input);
		parser(input,&argc,argv,", \t"); // 명령어 파싱
		int caseNum = functionPointer[classifyInput(argc,argv)](argc,argv);
		switch(caseNum){
			case INPUT_ERROR: // 입력이 잘못 들어온 경우
				inappropriateInput(argc,argv);
				break;
			case ERROR_CHECKED: // 에러 처리를 완료한 경우
				break;
			case INPUT_EXIT: // quit, q 명령어가 입력된 경우
				return 0; // 프로그램 종료 (정상적으로 종료 : retuen 0)
			case INPUT_NORMAL: // 적합한 명령어가 입력되어 정상적으로 처리가 완료된 경우
				storeHistory(tmpHist); // 명령어 저장
				break;
			default : // 다른 경우는 모두 에러가 생긴 경우
				printError(caseNum); //에러문 출력
		}
	}
	return 1; // 비정상적으로 종료함 (return 1)
}
