#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
typedef struct _instNode{
	char str[100];
	int caseNum;
	int valNum;
	int val[10];
	struct _instNode* link;
}instNode;
instNode* newInstNode(); // 빈 노드를 반환 
void instSetInit(); // 명령어 종류를 읽어들여, 명령어 리스트를 만든다 
void loadOneInstruction(FILE*, instNode**); // 재귀적으로 명령어 리스트 생성 
int getInstruction(); // bash에서 명령어를 읽어들인다 
void parser(char[], int*, char[100][100]); // 문자열을 파싱
int classifyInstruction(int , char[100][100]); // 명령어를 분류 
void storeInstruction(char[]);// 규칙에 맞는 명령어 저장
void help(); // 도움말 출력 
