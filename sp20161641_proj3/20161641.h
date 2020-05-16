#ifndef MY20161641
#define MY20161641
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
////////
#define MAX(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })  
#define MIN(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })  
////////
// 입력 관련 에러
#define ERROR_CHECKED (-1)
#define INPUT_ERROR (0)
#define INPUT_NORMAL (1)
#define INPUT_EXIT (2)
////////
// 메모리 명령어 관련 에러
#define MEMORY_ERROR_VALUE_EXCEED (3)
#define MEMORY_ERROR_ADDRESS_EXCEED (4) 
#define MEMORY_ERROR_ADDRESS_START_END (5)
////////
// opcode 관련 에러 
#define OPCODE_DOESNT_EXIST (6)
////////
// type 관련 에러
#define FILE_DOESNT_EXIST (7)
#define THIS_IS_DIRECTORY_ERROR (13)
////////
////////
// assembler 관련 에러
#define ASSEM_OPCODE_ERROR (8)
#define ASSEM_SYMBOL_DUPLICATION_ERROR (9)
#define ASSEM_BASE_NAME_ERROR (10)
#define ASSEM_END_OPCODE_DOESNT_EXIST (11)
#define ASSEM_START_OPCODE_DOESNT_EXIST (12)
#define ASSEM_WRONG_REGISTER (14)
#define ASSEM_SYMBOL_DOESNT_EXIST (15)
#define ASSEM_ADDRESS_OUT_OF_RANGE (16)
#define ASSEM_NUMBER_OUT_OF_RANGE (17)
#define ASSEM_BYTE_WRONG_OPERAND (18)
////////
typedef struct _instructionNode{
	char str[100];
	int caseNum;
	struct _instructionNode* link;
}instructionNode; // instruction을 저장하는 linked list의 노드
typedef struct _historyNode{
	int count;
	char str[100];
	struct _historyNode* link;
}historyNode; // history를 저장하는 linked list의 노드
typedef struct _opcodeNode{
	int opcode;
	char str[100];
	int val[2];
	struct _opcodeNode* link;
}opcodeNode; // opcode를 저장하는 linked list의 노드
typedef struct _lstNode{
	int locCount;
	char str[50];
	long long objCode;
	char objStr[10];
}lstNode; // asm 코드의 정보를 저장하는 노드
typedef struct _symbolNode{
	int locCount;
	int arrIdx;
	char str[50];
	struct _symbolNode* link;
}symbolNode; // symbol을 저장하는 노드
////////
// functions in assembleFunc.c
void initAssemble(); // lstNode 배열과 Symbol 리스트를 초기화 한다
int assembleFile(int argv, char argc[100][100]); // 입력받은 파일의 object file과 listing file을 만든다
int makeLocationCount(FILE *fp); // location count를 할당하고, symbol table을 만든다
int makeObjCode(); // ObjCode를 만든다
int retLocCount(int argc,char argv[100][100],int symFlag); // 각 operation이 얼만큼의 크기를 갖는지 return한다
int storeSymbol(char str[], int locCount, int arrIdx, symbolNode** sNow, symbolNode** sPrev); // 재귀적으로 정렬을 유지하면서 symbol을 저장한다
void makeListingFile(FILE *fp); // listing file을 만든다
int retRegister(char str[]); // string에 해당하는 register return
void makeObjectFile(FILE *fp); // object file을 만든다
int writeOneLine(FILE* fp,int i,int lenMax); // 1줄씩 적어준다, 다음 시작 index를 return
int printSymbol(int argv, char argc[100][100]); // symbol table을 출력한다
int recurFindSymbol(char str[],symbolNode *node); // 재귀적으로 symbol을 탐색
void recurPrintSymbol(symbolNode *node); // 재귀적으로 하나씩 출력
////////
////////
// functions in InstructionProcessing.c
void instructionSetInit(); // 명령어 종류를 읽어들여, 명령어 리스트를 만든다 
void loadOneInstruction(FILE *fp,instructionNode **instSet); // 재귀적으로 명령어 리스트 생성
void parser(char str[], int* argv, char argc[100][100], char sep[]); // 문자열을 파싱
int classifyInput(int argv, char argc[100][100]); // 명령어를 분류 
void storeHistory(char str[]);// 규칙에 맞는 명령어 저장
int printHistory(int argv, char argc[100][100]); // 명령어 기록 출력 (함수 포인터 4)
void recurPrintOneHistory(historyNode* history); // 재귀적으로 하니씩 출력
////////
////////
// functions in functions.c
void printError(int errorCase); // 에러 종류별 에러구문 출력
int inappropriateInput(int argv, char argc[100][100]); // 적합하지 않은 명령어 처리 (함수 포인터 0)
int typeFile(int argc,char argv[100][100]); // 입력받은 파일의 내용을 출력 (함수 포인터 11) 
int help(int argv, char argc[100][100]); // 도움말 출력 (함수 포인터 1)
int printDirectory(int argv, char argc[100][100]); // 현재위치 디랙토리 파일 출력 (함수 포인터 2)
int quitProgram(int argv, char argc[100][100]); // 프로그램 종료 (함수 포인터 3)
////////
////////
// functions in MemoryInstructions.c 
int isHex(char str[]); // 16진수인지 확인하기 위해 각각의 char을 검사한다
void printMemoryOneLine(int memoryIdx,int s,int e); // 메모리의 내용물을 1줄 출력
int dumpMemory(int argv, char argc[100][100]); // 메모리의 내용을 출력
int editMemory(int argv, char argc[100][100]); // 메모리의 address 번지의 값을 value로 변경
int fillMemory(int argv, char argc[100][100]); // start 번지부터 end 번지까지의 값을 value로 변경
int resetMemory(int argv, char argc[100][100]); // 메모리 전체를 전부 0으로 변경
////////
////////
// functions in OpcodeTable.c
int returnHash(char str[]); // string에 해당하는 hash값 반환
void makeOpcodeTable(); // opcode hash table을 만든다
opcodeNode* retOpcode(char str[100]); // 명령어에 해당하는 opcode 구조체를 반환
opcodeNode* recurFindOpcodeNode(opcodeNode *node,char str[]); // 재귀적으로 opcode를 찾는다
int opcodeMnemonic(int argv, char argc[100][100]); // 명령어에 해당하는 opcode를 출력
int recurFindOpcode(opcodeNode *node, char str[]); // 재귀적으로 opcode를 찾는다
int opcodeList(int argv, char argc[100][100]); // opcode hash table을 출력
void recurPrintOneHashTable(opcodeNode *node); // 재귀적으로 하나씩 출력
////////

#endif
