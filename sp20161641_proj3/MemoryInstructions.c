#include "20161641.h"
#define INDEX_MAX 1048575 // 65536*16-1
#define VALUE_MIN 32 // 0x20
#define VALUE_MAX 126 // 0x7E
unsigned char memory[65536][16] = {0,};
int isHex(char str[]){ // 16진수인지 확인하기 위해 각각의 char을 검사한다
	for(int i = 0;i<strlen(str);i++){
		if('0'<=str[i] && str[i]<='9')
			continue;
		if('A'<=str[i] && str[i]<='F')
			continue;
		if('a'<=str[i] && str[i]<='f')
			continue;
		return 0; // 16진수의 값이 아닌 경우
	}
	return 1; // 16진수가 맞는 경우
}
void printMemoryOneLine(int memoryIdx,int s,int e){ // 메모리의 내용물을 1줄 출력
	printf("%05X ",memoryIdx*16);
	for(int i = 0;i<s;i++)
		printf("   ");
	for(int i = s;i<=e;i++)
		printf("%02X ",memory[memoryIdx][i]);
	for(int i = e+1;i<16;i++)
		printf("   ");
	printf("; ");
	for(int i = 0;i<s;i++)
		printf(".");
	for(int i = s;i<=e;i++)
		printf("%c",(VALUE_MIN<=memory[memoryIdx][i]&&memory[memoryIdx][i]<=VALUE_MAX) ? \
		memory[memoryIdx][i] : '.');
	for(int i = e+1;i<16;i++)
		printf(".");
	printf("\n");
}
int dumpMemory(int argv, char argc[100][100]){ // 메모리의 내용을 출력
	static int prevIndex = -1; // 마지막 실행주소를 static으로 저장
	if(argv>3)
		return INPUT_ERROR;
	for(int i = 1;i<argv;i++)
		if(!isHex(argc[i]))
			return INPUT_ERROR;
	int index[2];
	index[0] = prevIndex == INDEX_MAX ? 0 : prevIndex+1; // start index
	index[1] = MIN(index[0]+159,INDEX_MAX); // end index
	for(int i = 1;i<argv;i++)
		index[i-1] = strtol(argc[i],NULL,16);
	if(argv == 2)
		index[1] = MIN(index[0]+159,INDEX_MAX);
	if(index[0]>index[1])
		return MEMORY_ERROR_ADDRESS_START_END;
	if(index[1]>INDEX_MAX)
		return MEMORY_ERROR_ADDRESS_EXCEED;
	// index 예외처리 후. index에 따라 출력
	if(index[0]/16 == index[1]/16)
		printMemoryOneLine(index[0]/16,index[0]%16,index[1]%16);
	else{
		printMemoryOneLine(index[0]/16,index[0]%16,15);
		for(int i = index[0]/16+1;i<index[1]/16;i++){
			printMemoryOneLine(i,0,15);
		}
		printMemoryOneLine(index[1]/16,0,index[1]%16);
	}
	prevIndex = index[1];
	return INPUT_NORMAL;
}
int editMemory(int argv, char argc[100][100]){ // 메모리의 address 번지의 값을 value로 변경
	if(argv!=3)
		return INPUT_ERROR;
	for(int i = 1;i<argv;i++)
		if(!isHex(argc[i]))
			return INPUT_ERROR;
	int address = strtol(argc[1],NULL,16);
	int value = strtol(argc[2],NULL,16);
	if(address>INDEX_MAX || address<0)
		return MEMORY_ERROR_ADDRESS_EXCEED;
	if(value>255|| value < 0)
		return MEMORY_ERROR_VALUE_EXCEED;
	// address, value 예외처리 후, memory의 address위치에 value 할당
	memory[address/16][address%16] = value;
	return INPUT_NORMAL;
}
int fillMemory(int argv, char argc[100][100]){ // start 번지부터 end 번지까지의 값을 value로 변경
	if(argv!=4)
		return INPUT_ERROR;
	for(int i = 1;i<argv;i++)
		if(!isHex(argc[i]))
			return INPUT_ERROR;
	int index[2];
	int value;
	for(int i = 1;i<3;i++)
		index[i-1] = strtol(argc[i],NULL,16);
	value = strtol(argc[3],NULL,16);
	if(index[0]>index[1])
		return MEMORY_ERROR_ADDRESS_START_END;
	if(index[1]>INDEX_MAX || index[0]<0)
		return MEMORY_ERROR_ADDRESS_EXCEED;
	if(value>255|| value < 0)
		return MEMORY_ERROR_VALUE_EXCEED;
	// address, value 예외처리 후, memory의 address위치에 value 할당
	for(int i = index[0];i<=index[1];i++){
		memory[i/16][i%16] = value;
	}
	return INPUT_NORMAL;
}
int resetMemory(int argv, char argc[100][100]){ // 메모리 전체를 전부 0으로 변경
	if(argv!=1)
		return INPUT_ERROR;
	for(int i = 0;i<65536;i++)
		for(int j = 0;j<16;j++)
			memory[i][j] = 0;
	return 	INPUT_NORMAL;
}
