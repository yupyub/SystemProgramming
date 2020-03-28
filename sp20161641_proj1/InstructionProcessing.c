#include "20161641.h"
char str[100]; // 명령어를 읽어들일 char 배열 
instNode* instSet = NULL;
instNode* instHistory = NULL;

instNode* newInstNode(){ // 빈 노드를 반환 
	instNode* newNode;
	newNode = (instNode*)malloc(sizeof(instNode));
	return newNode;
} 
void instSetInit(){ // 명령어 종류를 읽어들여, 명령어 리스트를 만든다 
	FILE *fp = fopen("operations.txt","r");
	instSet = newInstNode();
	fscanf(fp,"%s",instSet->str);
	fscanf(fp,"%d",&(instSet->caseNum));
	fscanf(fp,"%d",&(instSet->valNum));
	instSet->link = NULL;
	instNode* newNode = newInstNode();
	while(fscanf(fp,"%s",newNode->str) != EOF){
		fscanf(fp,"%d",&(newNode->caseNum));
		fscanf(fp,"%d",&(newNode->valNum));
		newNode->link = NULL;
		instNode* temp = instSet;
		while(temp->link != NULL)
			temp = temp->link;
		temp->link = newNode;
		newNode = newInstNode();
	}
	fclose(fp);
	return;
}
int getInstruction(){ // 명령어를 읽어들인다 
	scanf("%[^\n]%*c",str);
	instNode* tmpInst = classifyInstruction(str);

	/*
	////////////////////////////////////////
	printf("=====\n%s\n",tmpInst->str);
	printf("caseNum : %d\n",tmpInst->caseNum);
	printf("valNum : %d\n",tmpInst->valNum);
	printf("val : ");
	for(int i = 0;i<10;i++){
		printf("%d ",tmpInst->val[i]);
	}
	printf("\n");
	////////////////////////////////////////
	*/

	if(tmpInst->caseNum == -1)
		return 0;
	storeInstruction(tmpInst);
	switch(tmpInst->caseNum){
		case 2: 
			return 1;
	}
	return 0;
}
instNode* classifyInstruction(char str[]){ // 명령어를 분류 
	instNode* retNode = newInstNode();
	int idx = 0;
	int strLen = strlen(str);
	while(idx<strLen){
		if('a'<=str[idx] && str[idx]<='z')
			retNode->str[idx] = str[idx];
		else
			break;
		idx++;
	}
	//printf("Pass0\n");
	int opValNum = -1;
	instNode* tempNode = instSet;
	retNode->str[idx] = 0;
	while(1){
		if(strcmp(retNode->str,tempNode->str) == 0){
			opValNum = tempNode->valNum;
			retNode->caseNum = tempNode->caseNum;
			break;
		}
		if(tempNode -> link == NULL)
			break;
		tempNode = tempNode->link;
	}
	if(opValNum == -1){
		retNode->caseNum = -1;
		return retNode;
	}
	//printf("Pass1\n");
	int numCounter = 0;
	int tmpNum = -1;
	while(numCounter < opValNum && idx<strLen){
		tmpNum = -1;
		while(idx<strLen){
			if('0'<=str[idx] && str[idx]<='9'){
				if(tmpNum == -1)
					tmpNum = 0;
				tmpNum*=10;
				tmpNum += str[idx] - '0';
				idx++;
			}
			else if(str[idx] == ',' && tmpNum != -1){
				retNode->val[numCounter] = tmpNum;
				numCounter++;
				idx++;
				break;
			}
			else if(str[idx] == ' '){
				idx++;
			}
			else{
				opValNum = -1;
				idx++;
				break;
			}
		}
	}
	if(opValNum == -1){
		retNode->caseNum = -1;
		return retNode;
	}
	if(tmpNum != -1){
		retNode->val[numCounter] = tmpNum;
		numCounter++;
	}
	//printf("Pass2\n");
	while(idx< strLen){
		if(str[idx] != ' '){
			opValNum = -1;
			break;
		}
		idx++;
	}
	if(opValNum == -1)
		retNode->caseNum = -1;
	retNode->valNum = numCounter;
	return retNode;
}
void storeInstruction(instNode* inst){ // 규칙에 맞는 명령어 저장

}

