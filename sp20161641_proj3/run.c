#include "20161641.h"
int breakPoint(int argc, char argv[100][100]){ // break point를 설정한다
    if(argc > 2)
        return INPUT_ERROR;

    printf("BreakPoint\n");

    return INPUT_NORMAL;
}
int runProgram(int argc, char argv[100][100]){ // memory에 load된 프로그램을 실행한다
    if(argc > 1)
        return INPUT_ERROR;

    printf("RunProgram\n");

    return INPUT_NORMAL;
}
