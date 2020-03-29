#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
typedef struct _instructionNode{
	char str[100];
	int caseNum;
	int valNum;
	struct _instructionNode* link;
}instructionNode;
typedef struct _historyNode{
	char str[100];
	struct _historyNode* link;
}historyNode;
void instructionSetInit(); // 명령어 종류를 읽어들여, 명령어 리스트를 만든다 
void loadOneInstruction(FILE*, instructionNode**); // 재귀적으로 명령어 리스트 생성 
void functionPointerInit();
int getInput(); // bash에서 명령어를 읽어들인다 
void parser(char[], int*, char[100][100]); // 문자열을 파싱
int classifyInput(int , char[100][100]); // 명령어를 분류 
void storeHistory(char[]);// 규칙에 맞는 명령어 저장
void printError(int); // 에러 종류별 에러구문 출력 (추가구현)
int inappropriateInput(int, char [100][100]); // 적합하지 않은 명령어 처리 (함수 포인터 0)
int help(int, char[100][100]); // 도움말 출력 (함수 포인터 1)
int printDirectory(int, char[100][100]); // 현재위치 디랙토리 파일 출력 (함수 포인터 2)
int quitProgram(int, char[100][100]); // 프로그램 종료 (함수 포인터 3)
int printHistory(int, char[100][100]); // 명령어 기록 출력 (함수 포인터 4)
