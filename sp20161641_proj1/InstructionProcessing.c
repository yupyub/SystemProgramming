#include "20161641.h"
instructionNode* instructionSet = NULL;
historyNode* historySet = NULL;
int (*functionPointer[20])(int,char[100][100])={\
	inappropriateInput,\
		help,\
		printDirectory,\
		quitProgram,\
		printHistory};

void instructionSetInit(){ // 명령어 종류를 읽어들여, 명령어 리스트를 만든다 
	FILE *fp = fopen("operations.txt","r");
	loadOneInstruction(fp,&instructionSet);
	fclose(fp);
}
void loadOneInstruction(FILE *fp,instructionNode **instSet){ // 재귀적으로 명령어 리스트 생성
	char str[100];
	if(fscanf(fp,"%s",str)==EOF)
		return;
	*instSet=(instructionNode*)malloc(sizeof(instructionNode));
	strcpy((*instSet)->str,str);
	fscanf(fp,"%d",&(*instSet)->caseNum);
	fscanf(fp,"%d",&(*instSet)->valNum);
	(*instSet)->link=NULL;
	loadOneInstruction(fp,&(*instSet)->link);
}
void parser(char str[], int* argc, char argv[100][100]){
	char sep[] = ", \n";
	char* token;
	*argc = 0;
	token = strtok(str,sep);
	while(token != NULL){
		strcpy(argv[(*argc)++],token);
		token = strtok(NULL, sep);
	}
}
int classifyInput(int argv, char argc[100][100]){ // 명령어를 분류 
	if(argv == 0)
		return 0;
	for(instructionNode *tempNode=instructionSet;tempNode != NULL;tempNode = tempNode->link){
		if(strcmp(tempNode->str,*argc))
			continue;
		return tempNode->caseNum;
	}
	return 0;
}
void storeHistory(char str[]){ // 규칙에 맞는 명령어 저장
	historyNode* newNode =(historyNode*)malloc(sizeof(historyNode));
	str[strlen(str)-1] = '\0';
	strcpy(newNode->str,str);
	newNode->count = historySet == NULL ? 1 : historySet->count+1;
	newNode->link = historySet;
	historySet=newNode;
}

int printHistory(int argc, char argv[100][100]){ // 명령어 기록 출력 (함수 포인터 4)
	recurPrintOneHistory(historySet);
	printf("%-7d %s\n",historySet==NULL ? 1 : historySet->count+1,argv[0]); // 자신을 호출한 hist 명령어 출력해야 함: 
	return 1;
}
void recurPrintOneHistory(historyNode *history){ // 재귀적으로 하나씩 출력
	if(history==NULL)
		return;
	recurPrintOneHistory(history->link);
	printf("%-7d %s\n",history->count,history->str);
}
