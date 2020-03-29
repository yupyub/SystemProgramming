#include "20161641.h"
instNode* instSet = NULL;
instNode* instHistory = NULL;

instNode* newInstNode(){ // 빈 노드를 반환 
	instNode* newNode;
	newNode = (instNode*)malloc(sizeof(instNode));
	return newNode;
} 
void instSetInit(){ // 명령어 종류를 읽어들여, 명령어 리스트를 만든다 
	FILE *fp = fopen("operations.txt","r");
	loadOneInstruction(fp,&instSet);
	fclose(fp);
	return;
}
void loadOneInstruction(FILE *fp,instNode **instSet){ // 재귀적으로 명령어 리스트 생성
	char str[100];
	if(fscanf(fp,"%s",str)==EOF)
		return;
	*instSet=newInstNode();
	strcpy((*instSet)->str,str);
	fscanf(fp,"%d",&(*instSet)->caseNum);
	fscanf(fp,"%d",&(*instSet)->valNum);
	(*instSet)->link=NULL;
	loadOneInstruction(fp,&(*instSet)->link);
}

int getInstruction(){ // bash에서 명령어를 읽어들인다 
	char input[100]; // 명령어를 읽어들일 char 배열 
	int argc;
	char argv[100][100];
	printf("sicsim> ");
	fgets(input,100,stdin);
	//scanf("%[^\n]%*c",input);
	parser(input,&argc,argv);
	printf("argc: %d\n", argc);
	for(int i = 0;i<argc+3;i++){
		printf(">> %s\n",argv[i]);
	}
	/*
	 = classifyInstruction(argc,argv);


	if(tmpInst->caseNum == -1)
		return 0;
	//storeInstruction(tmpInst);
	switch(tmpInst->caseNum){
		case 2: 
			return 1;
	}
	*/
	return 0;
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
int classifyInstruction(int argv, char argc[100][100]){ // 명령어를 분류 
	return 0;
}
void storeInstruction(char str[]){ // 규칙에 맞는 명령어 저장

}
