#include "20161641.h"
extern int progaddr;
int A, X, L, PC, B, S, T;
int runSeq[1000]; // 프로그램의 실행 순서대로 locCount를 저장
int visitSeq[1000]; // bp로 지정된 locCount를 Check
int breakPoint(int argc, char argv[100][100]){ // break point를 설정한다
    if(argc > 2)
        return INPUT_ERROR;
    if(argc == 1)
        printBp();
    else if(strcmp(argv[1],"clear") == 0){
        // BreakPoint 초기화

        // ?????

        //////////////////////
        printf("\t[");
	    printf("%c[1;32m",27);	// ok 초록색으로 출력
	    printf("ok");
	    printf("%c[0m",27);
        printf("] clear all breakpoints\n");
    }
    else{
        int tmp = strtol(argv[1],NULL,16);
        // BreakPoint 저장
        printf("tmp : %d\n",tmp);
        // ?????

        /////////////////////
        printf("\t[");
	    printf("%c[1;32m",27);	// ok 초록색으로 출력
	    printf("ok");
	    printf("%c[0m",27);
        printf("] creat breakpoint %s\n",argv[1]);
    }
    return INPUT_NORMAL;
}
void printBp(){ // 저장된 BreakPoint를 출력한다


}
void initRegister(){ // Initialize Registers
    PC = progaddr; // 프로그램의 시작주소로 초기화
    ////// L = programLength; // 프로그램의 길이로 초기화
    L = 0; // loader 만든 후 위의 것으로 변경 필요 !!!!!!!!!!!!!!!!!!!!
    //////
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
    // bp를 만나면 정지

    return INPUT_NORMAL;
}
