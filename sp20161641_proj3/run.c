#include "20161641.h"
extern int progaddr;
extern int totalLen;
int A, X, L, PC, B, S, T;
int bp[1000];
int bpMax = 0;
int breakPoint(int argc, char argv[100][100]){ // break point를 설정한다
    if(argc > 2)
        return INPUT_ERROR;
    if(argc == 1)
        printBp();
    else if(strcmp(argv[1],"clear") == 0){
        bpMax = 0;
        printf("\t[");
	    printf("%c[1;32m",27);	// ok 초록색으로 출력
	    printf("ok");
	    printf("%c[0m",27);
        printf("] clear all breakpoints\n");
    }
    else{
        if(isHex(argv[1]) == 0)
            return INPUT_ERROR;
        int tmp = strtol(argv[1],NULL,16);
        if(tmp<0 || tmp >totalLen)
            return BP_VALUE_OUT_OF_RANGE;
        bp[bpMax++] = tmp;
        printf("\t[");
	    printf("%c[1;32m",27);	// ok 초록색으로 출력
	    printf("ok");
	    printf("%c[0m",27);
        printf("] creat breakpoint %s\n",argv[1]);
    }
    return INPUT_NORMAL;
}
void printBp(){ // 저장된 BreakPoint를 출력한다
    printf("            breakpoint\n");
    printf("            ----------\n");
    for(int i = 0;i<bpMax;i++)
        printf("            %X\n",bp[i]);
}
void initRegister(){ // Initialize Registers
    PC = progaddr; // 프로그램의 시작주소로 초기화
    L = totalLen; // 프로그램의 길이로 초기화
    A = X = B = S = T = 0;
}
void runOneInstruction(){ // PC기준 1개의 명령어 실행


}
int runProgram(int argc, char argv[100][100]){ // memory에 load된 프로그램을 실행한다
    if(argc > 1)
        return INPUT_ERROR;
    initRegister(); // Initialize Register
    printf("RunProgram\n");
    // while문으로 runOneInstruction() 실행하면서
    // bp를 만나면 정지, 정지 위치 저장

    return INPUT_NORMAL;
}
