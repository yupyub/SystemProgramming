#include "20161641.h"
instructionNode* instructionSet = NULL;
historyNode* historySet = NULL;
int (*functionPointer[20])(int,char[100][100]);
void instructionSetInit(){ // 명령어 종류를 읽어들여, 명령어 리스트를 만든다 
	FILE *fp = fopen("operations.txt","r");
	loadOneInstruction(fp,&instructionSet);
	fclose(fp);
	return;
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
void functionPointerInit(){
	functionPointer[0] = inappropriateInput;
	functionPointer[1] = help;
	functionPointer[2] = printDirectory;
	functionPointer[3] = quitProgram;
	functionPointer[4] = printHistory;
	return;
}
int getInput(){ // bash에서 명령어를 읽어들인다 
	char input[100]; // 명령어를 읽어들일 char 배열 
	int argc;
	char argv[100][100];
	char tmpHist[100];
	printf("sicsim> ");
	fgets(input,100,stdin);
	strcpy(tmpHist,input);
	parser(input,&argc,argv);
	int ret = functionPointer[classifyInput(argc,argv)](argc,argv);
	if(ret == 1)  // 1을 제외한 모든 리턴 값은 오류사항을 뜻함(0은 종료)
		storeHistory(tmpHist);
	return ret;
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
	return;
}
int classifyInput(int argv, char argc[100][100]){ // 명령어를 분류 
	instructionNode* tempNode = instructionSet;
	int caseNum = 0,valNum = 0;
	if(argv == 0)
		return 0;
	while(tempNode != NULL){
		if(strcmp(tempNode->str,*argc) == 0){
			caseNum = tempNode->caseNum;
			valNum = tempNode->valNum;
			break;
		}
		tempNode = tempNode->link;
	}
	if(valNum>=argv-1) // 적합한 명령어인 경우
		return caseNum;
	return 0;
}
void storeHistory(char str[]){ // 규칙에 맞는 명령어 저장
	historyNode* tempNode = historySet;
	historyNode* newNode =(historyNode*)malloc(sizeof(historyNode));
	int len = strlen(str);
	str[len-1] = '\0';
	newNode->count = 1;
	strcpy(newNode->str,str);
	newNode->link = NULL;
	if(tempNode != NULL){
		while(tempNode -> link != NULL){
			tempNode = tempNode->link;
		}
		newNode->count = tempNode->count + 1;
		tempNode->link = newNode;
	}
	else
		historySet = newNode;
	return;
}
int printHistory(int argc, char argv[100][100]){ // 명령어 기록 출력 (함수 포인터 4)
	historyNode* tempNode = historySet;
	int count = 0;
	while(tempNode != NULL){
		printf("%-7d %s\n",tempNode->count,tempNode->str);
		count = tempNode->count;
		tempNode = tempNode->link;
	}
	count++;
	printf("%-7d %s\n",count,argv[0]); // 자신을 호출한 hist 명령어 출력해야 함: 
	return 1;
}
