#include "20161641.h"
lstNode lstArr[1000];
int lstArrSize = 0;
SymbolNode SymbolSet = NULL;

void initAssemble(){ // lstNode 배열과 Symbol 리스트를 초기화 한다
	SymbolSet = NULL; // 이후free 추가
	for(int i = 0;i<lstArrSize;i++){
		lstArr[i].locCount = 0;
		lstArr[i].str[0] = 0;
		lstArr[i].objCode = 0;
	}
	lstArrSize = 0;
}
int assembleFile(int argv, char argc[100][100]){ // 입력받은 파일의 object file과 listing file을 만든다
	if(argv != 2)
		return INPUT_ERROR;
	initAssemble();
	FILE *fp = fopen(argc[1],"r");
	if(fp == NULL)
		return FILE_DOESNT_EXIST;
	int err = makeLocattionCount(fp);
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
	int symFlag = 0,commentFlag = 0;
	while(fgets(str,100,fp)!=NULL){
		if(str[0] != ' ' && str[0] != '\t') symFlag = 1;
		else symFlag = 0;
		if(str[0] == '.'){
			lstArr[lstArrSize].

		}
		parser(str,&argc,argv,", \t\n");
		if(strcmp("START",argv[symFlag]) == 0){
			locCount = atoi(argv[symFlag+1]);
			locTemp = locCount;
			//////
			continue;
		}
		if(strcmp("END",argv[symFlag]) == 0){
			locTemp = retLocCount(argv[0]);
		if(locTemp == -1






	return 0;
}
int retLocCount(char str[]){ // 각 줄이 얼만큼의 크기를 갖는지 return한다
// START : return -1
// END : return -2
// BYTE : 


}
void storeSymbol(char str[], int locCount){ // 재귀적으로 정렬을 유지하면서 symbol을 저장한다

}
void makeListing(FILE *fp){ // listing file을 만든다

}
void makeObject(FILE *fp){ // object file을 만든다

}
int printSymbol(int argv, char argc[100][100]){ // symbol table을 출력한다
	if(argv != 1)
		return INPUT_ERROR;
	


	return INPUT_NORMAL;
}
