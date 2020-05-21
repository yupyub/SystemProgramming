#include "20161641.h"
int progaddr;
int execaddr;
estabNode estab[5][100];
int progLen[5];
int esMax[5];
int setProgaddr(int argc, char argv[100][100]){ // loader 또는 run 명령어를 수행할 때 시작하는 주소를 지정한다
    if(argc != 2)
        return INPUT_ERROR;
    progaddr = strtol(argv[1],NULL,16); 
    return INPUT_NORMAL;
}
void initEstab(){ // estab 초기화
    for(int i = 0;i<5;i++){
        for(int j = 0;j<100;j++){
            estab[i][j].addr = 0;
            estab[i][j].name[0] = '\0';
        }
        esMax[i] = 0;
    }
}
int storeEstab(int fn, char name[],int addr){ // estab 저장 및 주소 반환
    for(int i = 6;i>0;i--)
        if(name[i] == ' ' || name[i] == '\n')
            name[i] = '\0';
    for(int i = 0;i<esMax[fn];i++)
        if(strcmp(estab[fn][i].name,name) == 0) 
            return i; // already exist
    estab[fn][esMax[fn]].addr = addr;
    strcpy(estab[fn][esMax[fn]].name,name);
    esMax[fn]++;
    return -1;
}
int Pass1(FILE *fp[], int fileNumber){ // Pass1 수행, ESTAB 생성
    int csaddr = progaddr;
    int cslth,addr;
    int fpIdx = 0;
    char str[300];
    char addrStr[10];
    char name[10];
    while(fpIdx<fileNumber){
        fgets(str,300,fp[fpIdx]);
        strncpy(name, str+1,6);
        name[6] = '\0';
        cslth = strtol(str+13,NULL,16);
        if(storeEstab(fpIdx,name,csaddr) != -1)
            return DUPLICATED_EXTERNAL_SYMBOL;
        while(fgets(str,300,fp[fpIdx]) != NULL){
            if(str[0] != 'D')
                continue;
            for(int i = 1;i<strlen(str)-1;i+=12){ // fgets 에서 '\n' 까지 저장함
                strncpy(name,str+i,6);
                strncpy(addrStr,str+6+i,6);
                name[6] = addrStr[6] = '\0';
                addr=strtol(addrStr,NULL,16);
                if(storeEstab(fpIdx,name,csaddr+addr) != -1)
                    return DUPLICATED_EXTERNAL_SYMBOL;
            }
        }
        csaddr += cslth;
        progLen[fpIdx] = cslth;
        fpIdx++;
    }
    return INPUT_NORMAL;
}
int Pass2(FILE *fp[], int fileNumber){ // Pass2 수행, Linking Loading 수행
    int csaddr = progaddr;
    int fpidx = 0;
    char str[300];
    char temp[10];
    while(fpIdx<fileNumber){
        fgets(str,300,fp[fpIdx]);
        cslth = strtol(str+13,NULL,16);
        while(fgets(str,300,fp[fpidx]) != NULL){
            if(str[0] == 'R'){
                        
            }
            else if(str[0] == 'T'){


            }
            else if(str[0] == 'M'){

            }
        }
        if(str[0] == 'E'){
            if(str[1] != '\n'){
                strncpy(temp,str+1,6);
                temp[6] = '\0';
                execaddr = csaddr+strtol(temp,NULL,16);
            }
        }
        csaddr += cslth;
        fpIdx++;
    }
    return INPUT_NORMAL;
}
int loader(int argc, char argv[100][100]){ // .obj 파일을 읽어서 linking/loading작업을 수행후, memory에 저장한다
    if(argc<2)
        return INPUT_ERROR;
    initEstab();
    FILE* fp[5];
    for(int i = 1;i<argc;i++){ // open the .obj files
        fp[i-1] = fopen(argv[i],"r");
        if(fp[i-1] == NULL)
            return FILE_DOESNT_EXIST; 
    }
    int fileNumber = argc-1;
    int ret = Pass1(fp,fileNumber);
    if(ret != INPUT_NORMAL)
        return ret;
    ret = Pass2(fp,fileNumber);
    if(ret != INPUT_NORMAL)
        return ret;
    int totalLen = 0;
    printf("control symbol address length\n");
    printf("section name\n");
    printf("--------------------------------\n");
    for(int i = 0;i<fileNumber;i++){
        for(int j = 0;j<esMax[i];j++){
            if(j == 0)
                printf("%-7s%14.04X   %04X\n",estab[i][j].name,estab[i][j].addr,progLen[i]);
            else
                printf("%14s   %04X\n",estab[i][j].name,estab[i][j].addr);
        }
        totalLen += progLen[i];
    }
    printf("--------------------------------\n");
    printf("           total length %04X\n",totalLen);
    return INPUT_NORMAL;
}
