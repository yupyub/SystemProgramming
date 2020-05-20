#include "20161641.h"
int programAddress;
int progaddr(int argc, char argv[100][100]){ // loader 또는 run 명령어를 수행할 때 시작하는 주소를 지정한다
    if(argc != 2)
        return INPUT_ERROR;
    programAddress = strtol(argv[1],NULL,16); 
    return INPUT_NORMAL;
}
/*
int Pass1(FILE *fp, int fileNumber){ // Pass1 수행, ESTAB 생성


    return INPUT_NORMAL;
}
*/
int loader(int argc, char argv[100][100]){ // .obj 파일을 읽어서 linking/loading작업을 수행후, memory에 저장한다
    if(argc<2)
        return INPUT_ERROR;
    

    return INPUT_NORMAL;
}
