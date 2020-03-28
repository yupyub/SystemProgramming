#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
typedef struct _instNode{
	char str[100];
	int caseNum;
	int valNum;
	int val[10];
	struct _instNode* link;
}instNode;
instNode* newInstNode(); // 빈 노드를 반환 
void instSetInit(); // 명령어 종류를 읽어들여, 명령어 리스트를 만든다 
int getInstruction(); // 명령어를 읽어들인다 
instNode* classifyInstruction(char[]); // 명령어를 분류 
void storeInstruction(instNode*); // 규칙에 맞는 명령어 저장
void help(); // 도움말 출력 
