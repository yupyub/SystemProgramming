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
int endLoc = 0;
void initAssemble(){ // lstNode 배열과 Symbol 리스트를 초기화 한다
	symbolSetPrev = symbolSet; // 이후free 추가
	symbolSet = NULL;
	for(int i = 0;i<lstArrSize;i++){
		lstArr[i].locCount = 0;
		lstArr[i].str[0] = 0;
		lstArr[i].objCode = 0;
	}
	lstArrSize = baseIdx = baseLine = mffMax = endLoc = 0;
	startFlag = endFlag = -1;
}
int assembleFile(int argv, char argc[100][100]){ // 입력받은 파일의 object file과 listing file을 만든다
	if(argv != 2)
		return INPUT_ERROR;
	initAssemble();
	FILE *fp = fopen(argc[1],"r");
	if(fp == NULL)
		return FILE_DOESNT_EXIST;
	int err = makeLocationCount(fp); // Location Count 생성
	fclose(fp);
	if(err != 0){
		symbolSet = symbolSetPrev;
		return err;
	}
	err = makeObjCode(); // Object Code 생성
	if(err != 0){
		symbolSet = symbolSetPrev;
		return err;
	}
	char fileName1[100],fileName2[100];
	int i = 0;
	while(argc[1][i] != '.' && argc[1][i] != '\0'){
		fileName1[i] = fileName2[i] = argc[1][i];
		i++;
	}
	fileName1[i] = fileName2[i] = '\0';
	strcat(fileName1,".lst");
	strcat(fileName2,".obj");
	fp = fopen(fileName1,"w");
	makeListingFile(fp); // .lst file 생성
	fclose(fp);
	fp = fopen(fileName2,"w");
	makeObjectFile(fp); // .obj file 생성
	fclose(fp);
	printf("%c[1;32m",27);	// assemble을 성공한 경우 초록색으로 출력 (추가구현)
	printf("Successfully");
	printf("%c[0m",27);
	printf(" %s.\n",argc[1]);
	return INPUT_NORMAL;
}
int makeLocationCount(FILE *fp){ // location count를 할당하고, symbol table을 만든다
	char str[100];
	int argc = 0;
	char argv[100][100];
	int locCount = 0,locTemp = 0;
	int symFlag = 0;
	while(fgets(str,100,fp)!=NULL){ // asm 파일의 코드를 한줄씩 읽어들임
		if(strlen(str)>0)
			str[strlen(str)-1] = '\0';
		strcpy(lstArr[lstArrSize].str,str);
		if(str[0] != ' ' && str[0] != '\t') symFlag = 1;
		else symFlag = 0;
		parser(str,&argc,argv,", \f\n\r\t\v"); //parsing
		if(argc == 0){
			lstArr[lstArrSize].locCount = -1;
			lstArr[lstArrSize].objCode = -1;
			lstArrSize++;
			continue;
		}
		if(argv[0][0] == '.'){ // Comment
			lstArr[lstArrSize].locCount = -1;
			lstArr[lstArrSize].objCode = -1;
			lstArrSize++;
			continue;
		}
		if(strcmp("START",argv[symFlag]) == 0){ // Start location setting
			startFlag = lstArrSize;
			strcpy(programName,argv[0]);
			locCount = strtol(argv[symFlag+1],NULL,16);
			locTemp = locCount;
			lstArr[lstArrSize].locCount = locCount;
			lstArr[lstArrSize].objCode = -1;
			lstArrSize++;
			continue;
		}
		if(startFlag == -1){ // if START doesn't exist
			printf("LINE : (%d) :",lstArrSize*5+5);
			return ASSEM_START_OPCODE_DOESNT_EXIST; 
		}
		if(strcmp("END",argv[symFlag]) == 0){ // end the file reading
			lstArr[lstArrSize].locCount = -1;
			lstArr[lstArrSize].objCode = -1;
			lstArrSize++;
			endFlag = lstArrSize;
			break;
		}
		if(strcmp("BASE",argv[symFlag]) == 0){ // set BASE address
			lstArr[lstArrSize].locCount = -1;
			lstArr[lstArrSize].objCode = -1;
			baseLine = lstArrSize;
			baseIdx = -1;
			lstArrSize++;
			strcpy(baseName,argv[symFlag+1]);
			continue;
		}
		locTemp = retLocCount(argc,argv,symFlag);
		if(locTemp == -1){ 				// OPCODE ERROR CASE
			printf("LINE : (%d) :",lstArrSize*5+5);
			return ASSEM_OPCODE_ERROR;
		}
		lstArr[lstArrSize].locCount = locCount;
		lstArr[lstArrSize].objCode = 0;	
		if(symFlag&&!storeSymbol(argv[0],locCount,lstArrSize,&symbolSet,&symbolSet)){ // Duplicated symbol
			printf("LINE : (%d) :",lstArrSize*5+5);
			return ASSEM_SYMBOL_DUPLICATION_ERROR;
		}
		lstArrSize++;
		locCount += locTemp;
	}
	endLoc = locCount;
	if(endFlag == -1){ // if END doesn't exist
		printf("LINE : (%d) :",lstArrSize*5);
		return ASSEM_END_OPCODE_DOESNT_EXIST; 
	}
	if(baseIdx == -1){ // base operend name error
		baseIdx = recurFindSymbol(baseName,symbolSet);
		if(baseIdx == -1){  // base operend name error
			printf("LINE : (%d) :",baseLine*5+5);
			return ASSEM_BASE_NAME_ERROR;
		}
	}
	else{ // base doesn't decleared warning
		printf("WARNING : BASE DOESN'T DECLEARED\n");
		baseIdx = -1;
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
			return MAX(1,(strlen(argv[symFlag+1])-3)/2);
		else return 0;
	}
	int plusFlag = 0;
	char str[100];
	if(argv[symFlag][0] == '+'){ // EXTENDED MODE
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
	if((*sNow) != NULL){ // For string sorting
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
	if(strcmp(str,"") == 0)
		return 0;
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
		return -1;
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
			if(str[0] != ' ' && str[0] != '\t') symFlag = 1; // check the symbol
			else symFlag = 0;
			parser(str,&argc,argv,", \f\n\r\t\v");
			for(int i = argc;i<100;i++) // Initialization
				argv[i][0] = 0;
			if(strcmp("WORD",argv[symFlag]) == 0){
				lstArr[i].objCode = (long long)atoi(argv[symFlag+1]);
				lstArr[i].objStr[0] = '6';
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
				else{ // wrong byte operand
					printf("LINE : (%d) :",i*5+5);
					return ASSEM_BYTE_WRONG_OPERAND;
				}
			}
			else{
				if(argv[symFlag][0] == '+'){ // extended mode
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
				if(opTemp->val[e] == 1){ // format 1
					lstArr[i].objCode = opTemp->opcode;
					lstArr[i].objStr[0] = '2';
				}
				else if(opTemp->val[e] == 2){ // format 2
					op2*=4;
					lstArr[i].objCode=op1;
					lstArr[i].objCode*=16;
					lstArr[i].objCode+=op2;
					lstArr[i].objCode*=16;
					x = retRegister(argv[symFlag+1]);
					if(x == -1){ // wrong register name
						printf("LINE : (%d) :",i*5+5);
						return ASSEM_WRONG_REGISTER;
					}
					lstArr[i].objCode+=x;
					lstArr[i].objCode*=16;
					x = retRegister(argv[symFlag+2]);
					if(x == -1){ // wrong register name
						printf("LINE : (%d) :",i*5+5);
						return ASSEM_WRONG_REGISTER;
					}
					lstArr[i].objCode+=x;
					lstArr[i].objStr[0] = '4';
				}
				else if(opTemp->val[e] == 3 || opTemp->val[e] == 4){ // format 3 or 4
					if(argc>1+symFlag){
						if(argv[symFlag+1][0] == '#'){ // immidiate addressing
							n = 0, ii = 1;
							strcpy(tmp,argv[symFlag+1]+1); 
						}
						else if(argv[symFlag+1][0] == '@'){ // indirect addressing
							n = 1, ii = 0;
							strcpy(tmp,argv[symFlag+1]+1);
						}
						else{ // simple addressing
							n = 1, ii = 1;
							strcpy(tmp,argv[symFlag+1]);
						}
						if(argc == 3+symFlag)
							x = 1;
						addrIdx = recurFindSymbol(tmp,symbolSet);
						if(addrIdx == -1){
							disp = atoi(tmp);
							if(disp == 0 && tmp[0] != '0'){ // symbol doesn't exist
								printf("LINE : (%d) :",i*5+5);
								return ASSEM_SYMBOL_DOESNT_EXIST;
							}
							if(disp>4095 && e == 0){ // number out of range (format 3)
								printf("LINE : (%d) :",i*5+5);
								return ASSEM_NUMBER_OUT_OF_RANGE;
							}
							if(disp>=(1<<20) && e == 1){ // number out of range (format 4)
								printf("LINE : (%d) :",i*5+5);
								return ASSEM_NUMBER_OUT_OF_RANGE;
							}
						}
						else if(e == 0){
							int pcCounter = lstArr[addrIdx].locCount-(lstArr[i].locCount+3);
							if(-2048<=pcCounter && pcCounter<=2047){ // pc relative
								p = 1;
								if(pcCounter<0){ // 2's complement
									pcCounter += 4096;
									disp = pcCounter;
								}
								else
									disp = pcCounter;
							}
							else{ // base relative
								b = 1;
								if(baseIdx!=-1)
									disp = lstArr[addrIdx].locCount-lstArr[baseIdx].locCount;
								else 
									disp = -1;
								if(0>disp || disp >4095){ // address out of range
									printf("LINE : (%d) :",i*5+5);
									return ASSEM_ADDRESS_OUT_OF_RANGE;
								}
							}

						}
					}
					else{
						n = 1, ii = 1;
					}
					if(e)
						b = 0,p = 0;
					// calculate object code
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
	int lenMax = 60; // lenMax < 256
	int idx = startFlag;
	fprintf(fp,"H%-6s%06X%06X\n",programName,lstArr[startFlag].locCount,endLoc);	
	while(idx<endFlag){
		idx = writeOneLine(fp,idx,lenMax);
	}
	for(int i = 0;i<mffMax;i++){
		fprintf(fp,"M%06X05\n",modFormFour[i]);
	}
	fprintf(fp,"E%06X",lstArr[startFlag].locCount);
}
int writeOneLine(FILE* fp,int i,int lenMax){ // 1줄씩 적어준다, 다음 시작 index를 return
	char str[300] = {0,};
	char tmp[10];
	if(lstArr[i].locCount == -1) // 공백, 주석, BASE 선언부
		return i+1;
	if(lstArr[i].objCode == -1) //변수
		return i+1;
	fprintf(fp,"T%06X",lstArr[i].locCount);
	for(;i<endFlag;i++){
		if(lstArr[i].locCount == -1) // 공백, 주석, BASE 선언부
			continue;
		if(lstArr[i].objCode == -1) //변수
			break;
		else if(lstArr[i].objCode == -2) //string 으로 저장
			strcpy(tmp,lstArr[i].objStr);
		else{
			if(lstArr[i].objStr[0] == '6')
				sprintf(tmp,"%06llX",lstArr[i].objCode);
			else if(lstArr[i].objStr[0] == '8')
				sprintf(tmp,"%08llX",lstArr[i].objCode);
			else if(lstArr[i].objStr[0] == '4')
				sprintf(tmp,"%04llX",lstArr[i].objCode);
			else if(lstArr[i].objStr[0] == '2')
				sprintf(tmp,"%02llX",lstArr[i].objCode);
			else
				sprintf(tmp,"%llX",lstArr[i].objCode);
		}
		if(strlen(str)+strlen(tmp)>lenMax){ // exceed linemax
			i--;
			break;
		}
		strcat(str,tmp);
	}
	fprintf(fp,"%02X%s\n",(int)strlen(str)/2,str);
	return i+1;
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
