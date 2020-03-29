#include "20161641.h"
void printError(int errorCase){ // 에러 종류별 에러구문 출력 (추가구현)

}
int inappropriateInput(int argc,char argv[100][100]){ // 적합하지 않은 명령어 처리 (함수 포인터 0)
	printf(" >> ERROR!\n");
	return 2;
}
int help(int argc,char argv[100][100]){ // 도움말 출력 (함수 포인터 1) 
	printf("h[elp]\n");
	printf("d[ir]\n"); 
	printf("q[uit]\n"); 
	printf("hi[story]\n"); 
	printf("du[mp] [start, end]\n"); 
	printf("e[dit] address, value\n"); 
	printf("f[ill] start, end, value\n");
	printf("reset\n");
	printf("opcode mnemonic\n");
	printf("opcodelist\n");
	return 1;
}
int printDirectory(int argc, char argv[100][100]){ // 현재위치 디랙토리 파일 출력 (함수 포인터 2)
	printf("DIR\n");
	return 1;
}
int quitProgram(int argc, char argv[100][100]){ // 프로그램 종료 (함수 포인터 3)
	printf("Quit Program. Bye!\n"); // 지워야 함
	// 이후 선언된 메모리 Free등의 처리 추가구현
	return 0;  // 프로그램 종료를 위해 모든 함수 중 유일하게 0을 return 한다
}
