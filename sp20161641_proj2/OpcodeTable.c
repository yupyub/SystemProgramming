#include "20161641.h"
#define TABLE_MAX (20)
opcodeNode* hashTable[TABLE_MAX];
int returnHash(char str[]){ // string에 해당하는 hash값 반환
	int hash = 401;
	for(int i = 0;str[i] != '\0';i++)
		hash = ((hash<<4)+(int)(str[i]))%TABLE_MAX;
	return hash%TABLE_MAX;
}
void makeOpcodeTable(){ // opcode hash table을 만든다
	FILE *fp = fopen("opcode.txt","r");
	int argc,hashNum;
	char str[100],argv[100][100];
	//fflush(stdout);
	while(fgets(str,99,fp)!=NULL){
		if(str[strlen(str)-1] == '\n')
			str[strlen(str)-1] = '\0';
		parser(str,&argc,argv," /\f\n\r\t\v");
		opcodeNode *newNode = (opcodeNode*)malloc(sizeof(opcodeNode));
		strcpy(newNode->str,argv[1]);
		newNode->opcode = strtol(argv[0],NULL,16);
		newNode->val[0] = atoi(argv[2]);
		newNode->val[1] = (argc == 4 ? atoi(argv[3]):0);
		hashNum = returnHash(newNode->str);
		newNode->link = hashTable[hashNum];
		hashTable[hashNum] = newNode;
		//fflush(stdout);
	}
	fclose(fp);
}
opcodeNode* retOpcode(char str[100]){ // 명령어에 해당하는 opcode 구조체를 반환
	opcodeNode* tmp = NULL;
	for(int i = 0;i<TABLE_MAX;i++){
		tmp = recurFindOpcodeNode(hashTable[i],str);
		if(tmp!=NULL)
			return tmp;
	}
	return NULL;
}
opcodeNode* recurFindOpcodeNode(opcodeNode *node,char str[]){ // 재귀적으로 opcode를 찾는다
	if(node == NULL)
		return NULL;
	if(strcmp(node->str,str) == 0)
		return node;
	return recurFindOpcodeNode(node->link,str);
}
int opcodeMnemonic(int argv, char argc[100][100]){ // 명령어에 해당하는 opcode를 출력
	if(argv != 2)
		return INPUT_ERROR;
	int tmp;
	for(int i = 0;i<TABLE_MAX;i++){
		tmp = recurFindOpcode(hashTable[i],argc[1]);
		if(tmp!=-1){
			printf("opcode is %X\n",tmp);
			return INPUT_NORMAL;
		}
	}
	return OPCODE_DOESNT_EXIST;
}
int recurFindOpcode(opcodeNode *node,char str[]){ // 재귀적으로 opcode를 찾는다
	if(node == NULL)
		return -1;
	if(strcmp(node->str,str) == 0)
		return node->opcode;
	return recurFindOpcode(node->link,str);
}
int opcodeList(int argv, char argc[100][100]){ // opcode hash table을 출력
	if(argv != 1)
		return INPUT_ERROR;
	for(int i = 0;i<TABLE_MAX;i++){
		printf("%d: ",i);
		recurPrintOneHashTable(hashTable[i]);
		printf("\n");
	}
	return INPUT_NORMAL;
}
void recurPrintOneHashTable(opcodeNode *node){ // 재귀적으로 하나씩 출력
	if(node==NULL)
		return;
	printf("[%s %X] ",node->str,node->opcode);
	if(node->link != NULL)
		printf("-> ");
	recurPrintOneHashTable(node->link);
}
