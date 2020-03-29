#include "20161641.h"
instructionNode* instructionSet = NULL;
historyNode* inputHistory = NULL;
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

}
