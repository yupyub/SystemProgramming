#ifndef MY1641
#define MY1641
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>

//////////////////////////////////
#define MAX(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })  
#define MIN(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })  
/////////////////////////////////
#define INPUT_ERROR (0)
#define INPUT_EXIT (2)
#define INPUT_NORMAL (1)
//
#define MEMORY_ERROR_VALUE_EXCEED (3)
#define MEMORY_ERROR_ADDRESS_EXCEED (4) 
#define MEMORY_ERROR_ADDRESS_START_END (5)
//



typedef struct _instructionNode{
	char str[100];
	int caseNum;
	struct _instructionNode* link;
}instructionNode;
typedef struct _historyNode{
	int count;
	char str[100];
	struct _historyNode* link;
}historyNode;

void instructionSetInit(); // 명령어 종류를 읽어들여, 명령어 리스트를 만든다 
void loadOneInstruction(FILE*, instructionNode**); // 재귀적으로 명령어 리스트 생성 
void parser(char str[], int* argv, char argc[100][100]); // 문자열을 파싱
int classifyInput(int argv, char argc[100][100]); // 명령어를 분류 
void storeHistory(char str[]);// 규칙에 맞는 명령어 저장
void printError(int errorCase); // 에러 종류별 에러구문 출력 (추가구현)
int inappropriateInput(int argv, char argc[100][100]); // 적합하지 않은 명령어 처리 (함수 포인터 0)
int help(int argv, char argc[100][100]); // 도움말 출력 (함수 포인터 1)
int printDirectory(int argv, char argc[100][100]); // 현재위치 디랙토리 파일 출력 (함수 포인터 2)
int quitProgram(int argv, char argc[100][100]); // 프로그램 종료 (함수 포인터 3)
int printHistory(int argv, char argc[100][100]); // 명령어 기록 출력 (함수 포인터 4)
void recurPrintOneHistory(historyNode* history); // 재귀적으로 하니씩 출력

////////
// functions in MemoryInstructions.c 
int isHex(char str[]); // 16진수인지 확인하기 위해 각각의 char을 검사한다
void printMemoryOneLine(int memoryIdx,int s,int e); // 메모리의 내용물을 1줄 출력
int dumpMemory(int argv, char argc[100][100]); // 메모리의 내용을 출력
int editMemory(int argv, char argc[100][100]); // 메모리의 address 번지의 값을 value로 변경
int fillMemory(int argv, char argc[100][100]); // start 번지부터 end 번지까지의 값을 value로 변경
int resetMemory(int argv, char argc[100][100]); // 메모리 전체를 전부 0으로 변경
////////



#endif
