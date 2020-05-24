#include "20161641.h"
void printError(int errorCase){ // 에러 종류별 에러구문 출력
	switch(errorCase){
		case MEMORY_ERROR_VALUE_EXCEED:
			printf("VALUE OUT OF RANGE\n");
			break;
		case MEMORY_ERROR_ADDRESS_EXCEED:
			printf("ADDRESS OUT OF RANGE\n");
			break;
		case MEMORY_ERROR_ADDRESS_START_END:
			printf("START ADDRESS IS BIGGER THEN END ADDRESS\n");
			break;
		case OPCODE_DOESNT_EXIST:
			printf("OPCODE DOESN'T EXIST\n");
			break;
		case FILE_DOESNT_EXIST:
			printf("FILE DOESN'T EXIST\n");
			break;
		case ASSEM_OPCODE_ERROR:
			printf("ASSEMBLER CODE : WRONG OPERATION\n");
			break;
		case ASSEM_SYMBOL_DUPLICATION_ERROR:
			printf("ASSEMBLER CODE : SYMBOL DUPLICATED\n");
			break;
		case ASSEM_BASE_NAME_ERROR:
			printf("ASSEMBLER CODE : WRONG BASE NAME\n");
			break;
		case ASSEM_END_OPCODE_DOESNT_EXIST:
			printf("ASSEMBLER CODE : END DOESN'T EXIST\n");
			break;
		case ASSEM_START_OPCODE_DOESNT_EXIST:
			printf("ASSEMBLER CODE : START DOESN'T EXIST\n");
			break;
		case THIS_IS_DIRECTORY_ERROR:
			printf("THIS IS DIRECTORY\n");
			break;
		case ASSEM_WRONG_REGISTER:
			printf("ASSEMBLER CODE : WRONG REGISTER NAME\n");
			break;
		case ASSEM_SYMBOL_DOESNT_EXIST:
			printf("ASSEMBLER CODE : SYMBOL DOESN'T EXIST\n");
			break;
		case ASSEM_ADDRESS_OUT_OF_RANGE:
			printf("ASSEMBLER CODE : ADDRESS OUT OF RANGE (Use format4)\n");
			break;
		case ASSEM_NUMBER_OUT_OF_RANGE:
			printf("ASSEMBLER CODE : NUMBER OUT OF RANGE\n");
			break;
		case ASSEM_BYTE_WRONG_OPERAND:
			printf("ASSEMBLER CODE : WRONG BYTE OPERAND\n");
			break;
		case DUPLICATED_EXTERNAL_SYMBOL:
			printf("DUPLICATED EXTERNAL SYMBOL IN OBJECT CODE\n");
			break;
		case UNDEFINED_EXTERNAL_SYMBOL:
			printf("UNDEFINED EXTERNAL SYMBOL IN OBJECT CODE\n");
			break;
		case BP_VALUE_OUT_OF_RANGE:
			printf("BP VALUE OUT OF RANGE\n");
			break;
		case PROGADDR_OUT_OF_RANGE:
			printf("PROGADDR OUT OF RANGE\n");
            break;
	}

}
int typeFile(int argc,char argv[100][100]){ // 입력받은 파일의 내용을 출력 (함수 포인터 11) 
	if(argc != 2)
		return INPUT_ERROR;
	DIR* dir = NULL;
	struct dirent* entry;
	struct stat buf;
	if((dir = opendir("./")) != NULL){
		while((entry = readdir(dir)) != NULL){
			lstat(entry->d_name,&buf);
			if(strcmp(entry->d_name,argv[1]) == 0){
				if(S_ISDIR(buf.st_mode)) // if it is diectory
					return THIS_IS_DIRECTORY_ERROR;
				else
					break;
			}
		}
		closedir(dir);
	}
	if(strcmp("./",argv[1]) == 0)
		return THIS_IS_DIRECTORY_ERROR;
	if(strcmp("../",argv[1]) == 0)
		return THIS_IS_DIRECTORY_ERROR;
	char str[1002];
	int notEnter = 0;
	FILE *fp = fopen(argv[1],"r");
	if(fp == NULL)
		return FILE_DOESNT_EXIST;
	while(fgets(str,1000,fp)!=NULL){ // read line by line
		printf("%s",str);
		if(strlen(str)>0 && str[strlen(str)-1] != '\n')
			notEnter = 1;
		else
			notEnter = 0;
	}
	fclose(fp);
	if(notEnter) // if file don't ends woth enter 
		printf("\n");
	return INPUT_NORMAL;
}
int inappropriateInput(int argc,char argv[100][100]){ // 적합하지 않은 명령어 처리 (함수 포인터 0)
	if(argc != 0) // 바로 엔터를 입력한 경우 제외
		printf("INAPPROPRIATE INPUT\n");
	return ERROR_CHECKED;
}
int help(int argc,char argv[100][100]){ // 도움말 출력 (함수 포인터 1) 
	if(argc != 1)
		return INPUT_ERROR;
	printf("h[elp]\n");
	printf("d[ir]\n"); 
	printf("q[uit]\n"); 
	printf("hi[story]\n"); 
	printf("du[mp] [start, end]\n"); 
	printf("e[dit] address, value\n"); 
	printf("f[ill] start, end, value\n");
	printf("reset\n");
	printf("opcode mnemonic\n");
	printf("opcodelist\n");
	printf("assemble filename\n");
	printf("type filename\n");
	printf("symbol\n");
    printf("progaddr [address]\n");
    printf("loader [object filename1] [object filename2] [...]\n");
    printf("bp\n");
    printf("bp clear\n");
    printf("bp [address]\n");
    printf("run\n");
	return INPUT_NORMAL;
}
int printDirectory(int argc, char argv[100][100]){ // 현재위치 디랙토리 파일 출력 (함수 포인터 2)
	if(argc != 1) 
		return INPUT_ERROR;
	DIR* dir = NULL;
	struct dirent* entry;
	struct stat buf;
	if((dir = opendir("./")) == NULL){
		printf("DIRECTORY OPEN ERROR\n");
		return INPUT_NORMAL;
	}
	int count = 0;
	char str[35];
	while((entry = readdir(dir)) != NULL){
		lstat(entry->d_name,&buf);
		if(strlen(entry->d_name)>=29){ // 이름이 매우 긴 경우 "..."으로 생략
			strncpy(str,entry->d_name,26);
			str[26] = str[27] = str[28] = '.';
			str[29] = '\0';
		}
		else{
			strncpy(str,entry->d_name,29);
			str[(int)strlen(entry->d_name)] = '\0';
		}
		if(S_ISDIR(buf.st_mode)){
			printf("%c[1;34m",27);	// 디랙토리인 경우 파란색으로 출력 (추가구현)
			printf("%*.*s",31,(int)strlen(entry->d_name)+1,strcat(str,"/"));
			printf("%c[0m",27);
		}
		else if(S_ISREG(buf.st_mode) && (S_IEXEC&buf.st_mode)){
			printf("%c[1;32m",27);	// 실행파일인 경우 초록색으로 출력 (추가구현)
			printf("%*.*s",31,(int)strlen(entry->d_name)+1,strcat(str,"*"));
			printf("%c[0m",27);
		}
		else
			printf("%*.*s",31,(int)strlen(entry->d_name),str);
		count++;
		if(count%3 == 0)
			printf("\n");
	}
	printf("\n");
	closedir(dir);
	return INPUT_NORMAL;
}
int quitProgram(int argc, char argv[100][100]){ // 프로그램 종료 (함수 포인터 3)
	if(argc != 1) 
		return INPUT_ERROR;
	return INPUT_EXIT;  // 프로그램 종료를 위해 INPUT_EXIT return
}
