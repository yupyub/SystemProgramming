#include "20161641.h"
lstNode lstArr[1000];
int modFormFour[100];
int mffMax = 0;
int lstArrSize = 0, baseIdx = 0, baseLine = 0;
char baseName[50];
char programName[50];
symbolNode* symbolSetPrev = NULL;
symbolNode* symbolSet = NULL;
int startFlag = -1,endFlag = -1;
void initAssemble(){ // lstNode 배열과 Symbol 리스트를 초기화 한다
	symbolSetPrev = symbolSet; // 이후free 추가
	symbolSet = NULL;
	for(int i = 0;i<lstArrSize;i++){
		lstArr[i].locCount = 0;
		lstArr[i].str[0] = 0;
		lstArr[i].objCode = 0;
	}
	lstArrSize = baseIdx = baseLine = mffMax = 0;
	startFlag = endFlag = -1;
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
	if(err != 0){
		symbolSet = symbolSetPrev;
		return err;
	}
	err = makeObjCode();
	if(err != 0){
		symbolSet = symbolSetPrev;
		return err;
	}
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
	makeListingFile(fp);
	fclose(fp);
	///////////////////////////////////////
	fp = fopen(fileName2,"w");
	if(fp == NULL)
		return FILE_DOESNT_EXIST;
	makeObjectFile(fp);
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
		if(strlen(str)>0)
			str[strlen(str)-1] = '\0';
		strcpy(lstArr[lstArrSize].str,str);
		if(str[0] != ' ' && str[0] != '\t') symFlag = 1;
		else symFlag = 0;
		parser(str,&argc,argv,", \f\n\r\t\v");
		if(argc == 0){
			lstArr[lstArrSize].locCount = -1;
			lstArr[lstArrSize].objCode = -1;
			lstArrSize++;
			continue;
		}
		if(argv[0][0] == '.'){
			lstArr[lstArrSize].locCount = -1;
			lstArr[lstArrSize].objCode = -1;
			lstArrSize++;
			continue;
		}
		if(strcmp("START",argv[symFlag]) == 0){
			startFlag = lstArrSize;
			strcpy(programName,argv[0]);
			locCount = strtol(argv[symFlag+1],NULL,16);
			locTemp = locCount;
			lstArr[lstArrSize].locCount = locCount;
			lstArr[lstArrSize].objCode = -1;
			lstArrSize++;
			continue;
		}
		if(startFlag == -1){
			printf("LINE : (%d) :",lstArrSize);
			return ASSEM_START_OPCODE_DOESNT_EXIST; 
		}
		if(strcmp("END",argv[symFlag]) == 0){
			lstArr[lstArrSize].locCount = -1;
			lstArr[lstArrSize].objCode = -1;
			lstArrSize++;
			endFlag = lstArrSize;
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
			printf("[%s],",argv[0]);
			printf("LINE : (%d) :",lstArrSize);
			return ASSEM_SYMBOL_DUPLICATION_ERROR;
		}
		lstArrSize++;
		locCount += locTemp;
	}
	if(endFlag == -1){
		printf("LINE : (%d) :",lstArrSize-1);
		return ASSEM_END_OPCODE_DOESNT_EXIST; 
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
int retRegister(char str[]){ // string에 해당하는 register return
	if(strcmp(str,"A") == 0)
		return 0;
	else if(strcmp(str,"B") == 0)
		return 3;
	else if(strcmp(str,"X") == 0)
		return 1;
	else if(strcmp(str,"S") == 0)
		return 4;
	else if(strcmp(str,"L") == 0)
		return 2;
	else if(strcmp(str,"T") == 0)
		return 5;
	else if(strcmp(str,"PC") == 0)
		return 8;
	else if(strcmp(str,"F") == 0)
		return 6;
	else if(strcmp(str,"SW") == 0)
		return 9;
	else
		return 0;
}
int makeObjCode(){ // ObjCode를 만든다
	int argc = 0,symFlag = 0;
	char argv[100][100];
	char str[100],tmp[100];
	int op1,op2;
	int n,ii,x,b,p,e;
	int disp,addr;
	int addrIdx;
	for(int i = 0;i<lstArrSize;i++){
		op1 = op2 = n = ii = x = b = p = e = disp = addr = 0;
		if(lstArr[i].objCode != -1){
			strcpy(str,lstArr[i].str);
			if(str[0] != ' ' && str[0] != '\t') symFlag = 1;
			else symFlag = 0;
			parser(str,&argc,argv,", \f\n\r\t\v");
			for(int i = argc;i<100;i++)
				argv[i][0] = 0;
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
				if(opTemp->val[e] == 1){
					lstArr[i].objCode = opTemp->opcode;
					lstArr[i].objStr[0] = '2';
				}
				else if(opTemp->val[e] == 2){
					op2*=4;
					lstArr[i].objCode=op1;
					lstArr[i].objCode*=16;
					lstArr[i].objCode+=op2;
					lstArr[i].objCode*=16;
					lstArr[i].objCode+=retRegister(argv[symFlag+1]);
					lstArr[i].objCode*=16;
					lstArr[i].objCode+=retRegister(argv[symFlag+2]);
					lstArr[i].objStr[0] = '4';
				}
				else if(opTemp->val[e] == 3 || opTemp->val[e] == 4){
					if(argc>1+symFlag){
						if(argv[symFlag+1][0] == '#'){
							n = 0, ii = 1;
							strcpy(tmp,argv[symFlag+1]+1);
						}
						else if(argv[symFlag+1][0] == '@'){
							n = 1, ii = 0;
							strcpy(tmp,argv[symFlag+1]+1);
						}
						else{
							n = 1, ii = 1;
							strcpy(tmp,argv[symFlag+1]);
						}
						if(argc == 3+symFlag)
							x = 1;
						addrIdx = recurFindSymbol(tmp,symbolSet);
						if(addrIdx == -1){
							disp = atoi(tmp); // 숫자가 아닌경우 error check 해줘야 함
						}
						else{
							int pcCounter = lstArr[addrIdx].locCount-(lstArr[i].locCount+3);
							if(-2048<=pcCounter && pcCounter<=2047){
								p = 1;
								if(pcCounter<0){ // 2의 보수를 취해준다
									pcCounter += 4096;
									disp = pcCounter;
								}
								else
									disp = pcCounter;
							}
							else{
								b = 1;
								disp = lstArr[addrIdx].locCount-lstArr[baseIdx].locCount;
								if(0>disp || disp >4095)  // ERROR
									disp = 0;
							}

						}
					}
					else{
						n = 1, ii = 1;
					}
					if(e)
						b = 0,p = 0;
					lstArr[i].objCode=op1;
					lstArr[i].objCode*=4;
					lstArr[i].objCode+=op2;
					lstArr[i].objCode*=2;
					lstArr[i].objCode+=n;
					lstArr[i].objCode*=2;
					lstArr[i].objCode+=ii;
					lstArr[i].objCode*=2;
					lstArr[i].objCode+=x;
					lstArr[i].objCode*=2;
					lstArr[i].objCode+=b;
					lstArr[i].objCode*=2;
					lstArr[i].objCode+=p;
					lstArr[i].objCode*=2;
					lstArr[i].objCode+=e;
					if(e == 0){
						lstArr[i].objCode*=4096;
						lstArr[i].objCode+=disp;	
						lstArr[i].objStr[0] = '6';
					}
					else{
						if(addrIdx != -1){
							disp = lstArr[addrIdx].locCount;
							modFormFour[mffMax++] = lstArr[i].locCount+1;		
						}
						lstArr[i].objCode*=1048576;
						lstArr[i].objCode+=disp;	
						lstArr[i].objStr[0] = '8';
					}
				}
			}
		}
	}
	return 0;
}
void makeListingFile(FILE *fp){ // listing file을 만든다
	for(int i = 0;i<lstArrSize;i++){
		fprintf(fp,"%-7d ",i*5+5);
		if(lstArr[i].locCount < 0)
			fprintf(fp,"    ");
		else
			fprintf(fp,"%04X",lstArr[i].locCount);
		fprintf(fp,"  %s",lstArr[i].str);
		if(lstArr[i].objCode == -1)
			fprintf(fp,"\n");
		else{
			for(int j = (int)strlen(lstArr[i].str);j<30;j++)
				fprintf(fp," ");	
			if(lstArr[i].objCode != -2){
				if(lstArr[i].objStr[0] == '6')
					fprintf(fp,"%06llX\n",lstArr[i].objCode);
				else if(lstArr[i].objStr[0] == '8')
					fprintf(fp,"%08llX\n",lstArr[i].objCode);
				else if(lstArr[i].objStr[0] == '4')
					fprintf(fp,"%04llX\n",lstArr[i].objCode);
				else if(lstArr[i].objStr[0] == '2')
					fprintf(fp,"%02llX\n",lstArr[i].objCode);
				else
					fprintf(fp,"%llX\n",lstArr[i].objCode);
			}
			else
				fprintf(fp,"%s\n",lstArr[i].objStr);
		}
	}
}
void makeObjectFile(FILE *fp){ // object file을 만든다
	int 
	fprintf(fp,"H%-6s",programName,lstArr[startFlag].);
	for(int i = startFlag+1;i<endFlag;i++){

	}


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
