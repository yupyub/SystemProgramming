#include "20161641.h"
lstNode lstArr[1000];
int lstArrSize = 0, baseIdx = 0, baseLine = 0;
char baseName[50];
symbolNode* symbolSet = NULL;
void initAssemble(){ // lstNode 배열과 Symbol 리스트를 초기화 한다
	symbolSet = NULL; // 이후free 추가
	for(int i = 0;i<lstArrSize;i++){
		lstArr[i].locCount = 0;
		lstArr[i].str[0] = 0;
		lstArr[i].objCode = 0;
	}
	lstArrSize = baseIdx = baseLine = 0;
}
int assembleFile(int argv, char argc[100][100]){ // 입력받은 파일의 object file과 listing file을 만든다
	if(argv != 2)
		return INPUT_ERROR;
	initAssemble();
	FILE *fp = fopen(argc[1],"r");
	if(fp == NULL)
		return FILE_DOESNT_EXIST;
	int err = makeLocationCount(fp);
	fclose(fp);
	if(err != 0)
		return err;
	/////////////////////////////////////////
	char fileName1[100],fileName2[100];
	int i = 0;
	while(argc[1][i] != '.'){
		fileName1[i] = fileName2[i] = argc[1][i];
		i++;
	}
	fileName1[i] = fileName2[i] = '\0';
	strcat(fileName1,".lst");
	strcat(fileName2,".obj");
	////////////////////////////////////////
	fp = fopen(fileName1,"w");
	if(fp == NULL)
		return FILE_DOESNT_EXIST;
	makeListing(fp);
	fclose(fp);
	///////////////////////////////////////
	fp = fopen(fileName2,"w");
	if(fp == NULL)
		return FILE_DOESNT_EXIST;
	makeObject(fp);
	fclose(fp);
	return INPUT_NORMAL;
}
int makeLocationCount(FILE *fp){ // location count를 할당하고, symbol table을 만든다
	char str[100];
	int argc = 0;
	char argv[100][100];
	int locCount = 0,locTemp = 0;
	int symFlag = 0;
	while(fgets(str,100,fp)!=NULL){
		str[strlen(str)-1] = '\0';
		strcpy(lstArr[lstArrSize].str,str);
		if(str[0] != ' ' && str[0] != '\t') symFlag = 1;
		else symFlag = 0;
		if(str[0] == '.'){ // Comment
			lstArr[lstArrSize].locCount = -1;
			lstArr[lstArrSize].objCode = -1;
			lstArrSize++;
			continue;
		}
		parser(str,&argc,argv,", \f\n\r\t\v");
		if(strcmp("START",argv[symFlag]) == 0){
			locCount = atoi(argv[symFlag+1]);
			locTemp = locCount;
			lstArr[lstArrSize].locCount = locCount;
			lstArr[lstArrSize].objCode = -1;
			lstArrSize++;
			continue;
		}
		if(strcmp("END",argv[symFlag]) == 0){
			lstArr[lstArrSize].locCount = -1;
			lstArr[lstArrSize].objCode = -1;
			lstArrSize++;
			break;
		}
		if(strcmp("BASE",argv[symFlag]) == 0){
			lstArr[lstArrSize].locCount = -1;
			lstArr[lstArrSize].objCode = -1;
			baseLine = lstArrSize;
			lstArrSize++;
			strcpy(baseName,argv[symFlag+1]);
			continue;
		}
		locTemp = retLocCount(argc,argv,symFlag);
		if(locTemp == -1){ 				// ERROR CASE
			printf("LINE : (%d) :",lstArrSize);
			return ASSEM_OPCODE_ERROR;
		}
		lstArr[lstArrSize].locCount = locCount;
		lstArr[lstArrSize].objCode = 0;	
		if(symFlag&&!storeSymbol(argv[0],locCount,lstArrSize,&symbolSet,&symbolSet)){
			printf("LINE : (%d) :",lstArrSize);
			return ASSEM_SYMBOL_DUPLICATION_ERROR;
		}
		lstArrSize++;
		locCount += locTemp;
	}
	baseIdx = recurFindSymbol(baseName,symbolSet);
	if(baseIdx == -1){
		baseIdx = 0;
		printf("LINE : (%d) :",baseLine);
		return ASSEM_BASE_NAME_ERROR;
	}
	return 0;
}
int retLocCount(int argc,char argv[100][100],int symFlag){ // 각 operation이 얼만큼의 크기를 갖는지 return한다
	if(strcmp("WORD",argv[symFlag]) == 0)
		return 3;
	else if(strcmp("RESW",argv[symFlag]) == 0)
		return 3*atoi(argv[symFlag+1]);
	else if(strcmp("RESB",argv[symFlag]) == 0)
		return atoi(argv[symFlag+1]);
	else if(strcmp("BYTE",argv[symFlag]) == 0){
		if(argv[symFlag+1][0] == 'C')
			return strlen(argv[symFlag+1])-3;		
		else if(argv[symFlag+1][0] == 'X')
			return 1;
		else return 0;
	}
	int plusFlag = 0;
	char str[100];
	if(argv[symFlag][0] == '+'){
		plusFlag = 1;
		strcpy(str,argv[symFlag]+1);
	}
	else
		strcpy(str,argv[symFlag]);
	opcodeNode* opTemp = retOpcode(str);
	if(opTemp == NULL)
		return -1;
	return opTemp->val[plusFlag];
}
int storeSymbol(char str[], int locCount, int arrIdx, symbolNode** sNow, symbolNode** sPrev){ // 재귀적으로 정렬을 유지하면서 symbol을 저장한다
	if((*sNow) != NULL){
		int cmp = strcmp((*sNow)->str,str);
		if(cmp == 0) return 0;
		else if(cmp < 0) {
			return storeSymbol(str,locCount,arrIdx,&((*sNow)->link),sNow);
		}
	}
	symbolNode* newNode =(symbolNode*)malloc(sizeof(symbolNode));
	strcpy(newNode->str,str);
	newNode->locCount = locCount;
	newNode->arrIdx = arrIdx;
	if((*sNow)== NULL && (*sPrev) == NULL){
		newNode->link = NULL;
		(*sPrev) = newNode;
	}
	else if((*sNow) == (*sPrev)){
		newNode->link = (*sNow);
		(*sPrev)=newNode;
	}
	else {
		newNode->link = (*sNow);
		(*sPrev)->link = newNode;
	}
	return 1;
}
void makeListing(FILE *fp){ // listing file을 만든다
	int argc = 0,symFlag = 0;
	char argv[100][100];
	char str[100],tmp[100];
	int op1,op2;
	int n,ii,x,b,p,e;
	int disp,addr;
	for(int i = 0;i<lstArrSize;i++){
		op1 = op2 = n = ii = x = b = p = e = disp = addr = 0;
		if(lstArr[i].objCode != -1){
			strcpy(str,lstArr[i].str);
			if(str[0] != ' ' && str[0] != '\t') symFlag = 1;
			else symFlag = 0;
			parser(str,&argc,argv,", \f\n\r\t\v");
			if(strcmp("WORD",argv[symFlag]) == 0){
				lstArr[i].objCode = (long long)atoi(argv[symFlag+1]);
			}
			else if(strcmp("RESW",argv[symFlag]) == 0){
				lstArr[i].objCode = -1;		
			}
			else if(strcmp("RESB",argv[symFlag]) == 0){
				lstArr[i].objCode = -1;		
			}
			else if(strcmp("BYTE",argv[symFlag]) == 0){
				int j = 2;
				if(argv[symFlag+1][0] == 'C'){
					while(argv[symFlag+1][j] != '\'' && argv[symFlag+1][j] != '\0'){
						disp *= 16*16;
						disp += argv[symFlag+1][j];
						j++;
					}
					lstArr[i].objCode = disp;		
				}
				else if(argv[symFlag+1][0] == 'X'){
					while(argv[symFlag+1][j] != '\'' && argv[symFlag+1][j] != '\0'){
						tmp[j-2] = argv[symFlag+1][j];
						j++;
					}
					tmp[j-2] = '\0';
					lstArr[i].objCode = -2;
					strcpy(lstArr[i].objStr,tmp);
				}
				// else // error case
			}
			else{
				if(argv[symFlag][0] == '+'){
					e = 1;
					strcpy(tmp,argv[symFlag]+1);
				}
				else
					strcpy(tmp,argv[symFlag]);
				opcodeNode* opTemp = retOpcode(tmp);
				op2 = opTemp->opcode;
				op1 = op2/16;
				op2 %= 16;
				op2 /= 4;
////////////////////////////////////////



////////////////////////////////////////
				




			}
		}
		///////////////////////////////////////////// 출력
		fprintf(fp,"%-7d ",i*5+5);
		if(lstArr[i].locCount == -1)
			fprintf(fp,"    ");
		else
			fprintf(fp,"%04X",lstArr[i].locCount);
		fprintf(fp,"  %s",lstArr[i].str);
		if(lstArr[i].objCode == -1)
			fprintf(fp,"\n");
		else{
			for(int j = (int)strlen(lstArr[i].str);j<30;j++)
				fprintf(fp," ");	
			if(lstArr[i].objCode != -2)
				fprintf(fp,"%llX\n",lstArr[i].objCode);
			else
				fprintf(fp,"%s\n",lstArr[i].objStr);
		}
	}
}

void makeObject(FILE *fp){ // object file을 만든다

}
int printSymbol(int argv, char argc[100][100]){ // symbol table을 출력한다
	if(argv != 1)
		return INPUT_ERROR;
	recurPrintSymbol(symbolSet);
	return INPUT_NORMAL;
}
int recurFindSymbol(char str[],symbolNode *node){ // 재귀적으로 symbol을 탐색
	if(node==NULL)
		return -1;
	if(strcmp(node->str,str) == 0) 
		return node->arrIdx;
	return recurFindSymbol(str,node->link);
}
void recurPrintSymbol(symbolNode *node){ // 재귀적으로 하나씩 출력
	if(node==NULL)
		return;
	printf("\t%-9s%04X\n",node->str,node->locCount);
	recurPrintSymbol(node->link);
}
