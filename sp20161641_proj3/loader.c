#include "20161641.h"
extern unsigned char memory[65536][16];
extern int NewLoad; // 프로그램이 새롭게 Load되었는지 알려주는 Flag
int progaddr;
int execaddr;
estabNode estab[5][100];
int progLen[5];
int totalLen;
int esMax[5]; // estab에 저장된 symbol의 개수
int setProgaddr(int argc, char argv[100][100]){ // loader 또는 run 명령어를 수행할 때 시작하는 주소를 지정한다
    if(argc != 2)
        return INPUT_ERROR;
    progaddr = strtol(argv[1],NULL,16);
    if(progaddr<0 || progaddr>0xFFFFF){
        progaddr = 0;
        return PROGADDR_OUT_OF_RANGE;
    }
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
void storeEstab(int fn, char name[],int addr){ // estab 저장
    for(int i = 6;i>0;i--)
        if(name[i] == ' ' || name[i] == '\n')
            name[i] = '\0';
    estab[fn][esMax[fn]].addr = addr;
    strcpy(estab[fn][esMax[fn]].name,name);
    esMax[fn]++;
}
int returnEstab(char name[], int fileNum){ // estab symbol에 해당하는 address 반환
    for(int i = 0;i<fileNum;i++)
        for(int j = 0;j<esMax[i];j++)
            if(strcmp(name,estab[i][j].name) == 0)
                return estab[i][j].addr;
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
        if(returnEstab(name,fileNumber) != -1){ // symbol duplicated
            printf("%s :: ",name);
            return DUPLICATED_EXTERNAL_SYMBOL;
        }
        storeEstab(fpIdx,name,csaddr);
        while(fgets(str,300,fp[fpIdx]) != NULL){
            if(str[0] != 'D')
                continue;
            for(int i = 1;i<strlen(str)-1;i+=12){ // fgets 에서 '\n' 까지 저장함
                strncpy(name,str+i,6);
                strncpy(addrStr,str+6+i,6);
                name[6] = addrStr[6] = '\0';
                addr=strtol(addrStr,NULL,16);
                if(returnEstab(name,fileNumber) != -1){ // symbol duplicted
                    printf("%s :: ",name);
                    return DUPLICATED_EXTERNAL_SYMBOL;
                }
                storeEstab(fpIdx,name,csaddr+addr);
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
    int cslth;
    int fpidx = 0;
    char str[300];
    char temp[10];
    char refDic[100][10] = {0,};
    while(fpidx<fileNumber){
        fgets(str,300,fp[fpidx]);
        cslth = strtol(str+13,NULL,16);
        while(fgets(str,300,fp[fpidx]) != NULL){
            if(str[0] == 'R'){ // R record
                int refIdx;
                strcpy(refDic[1],estab[fpidx][0].name); // 01은 프로그램 자체를 가리킴
                for(int i = 1;i<strlen(str)-1;i+=8){
                    strncpy(temp,str+i,2);
                    temp[2] = '\0';
                    refIdx = atoi(temp);
                    strncpy(temp,str+2+i,6);
                    temp[6] = '\0';
                    for(int j = 0;j<6;j++){
                        if(temp[j] == ' ' || temp[j] == '\n')
                            temp[j] = '\0';
                    }
                    strcpy(refDic[refIdx],temp);
                }
            }
            else if(str[0] == 'T'){ // T record
                char addrStr[7];
                char lenStr[3];
                strncpy(addrStr,str+1,6);
                strncpy(lenStr,str+7,2);
                addrStr[6] = lenStr[2] = '\0';
                int Taddr = strtol(addrStr,NULL,16) + csaddr;
                int Tlen = strtol(lenStr,NULL,16);
                for(int i = 0;i<Tlen;i++){
                    strncpy(temp,str+9+2*i,2);
                    temp[2] = '\0';
                    memory[Taddr/16][Taddr%16] = strtol(temp,NULL,16);
                    Taddr++;
                }
            }
            else if(str[0] == 'M'){ // M record
                // modify 하는 길이가 05, 06인지 구분할 필요 X
                char addrStr[7];
                strncpy(addrStr,str+1,6);
                addrStr[6] = '\0';
                int Maddr = strtol(addrStr,NULL,16) + csaddr;
                for(int i = 0;i<3;i++)
                    sprintf(temp+i*2,"%02X",memory[(Maddr+i)/16][(Maddr+i)%16]);
                temp[6] = '\0';
                int objValue;
                objValue = strtol(temp,NULL,16);
                if(!('0' <= temp[0] && temp[0] < '8')) // 음수인 경우 2의 보수
                    objValue = -(0xFFFFFF - objValue +1);
                strncpy(temp,str+10,2);
                temp[2] = '\0';
                int refIdx = atoi(temp);
                int refAddr = returnEstab(refDic[refIdx],fileNumber);
                if(refAddr == -1){ // 정의되지 않은 reference 값인 경우
                    printf("%s :: ",refDic[refIdx]);
                    return UNDEFINED_EXTERNAL_SYMBOL;
                }
                if(str[9] == '+')
                    objValue += refAddr;
                else if(str[9] == '-')
                    objValue -= refAddr;
                if(objValue < 0) // 음수인 경우 2의 보수
                    objValue -= 0xFF000000;
                sprintf(temp,"%06X",objValue);
                char shortStr[3];
                for(int i = 0;i<3;i++){
                    strncpy(shortStr,temp+i*2,2);
                    shortStr[2] = '\0';
                    memory[(Maddr+i)/16][(Maddr+i)%16] = strtol(shortStr,NULL,16);
                }
            }
        }
        if(str[0] == 'E'){ // E record
            if(str[1] != '\n'){ // execaddr이 있는 경우
                strncpy(temp,str+1,6);
                temp[6] = '\0';
                execaddr = csaddr+strtol(temp,NULL,16);
            }
        }
        csaddr += cslth;
        fpidx++;
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
    for(int i = 1;i<argc;i++){ // open the .obj files
        fclose(fp[i-1]);
        fp[i-1] = fopen(argv[i],"r");
        if(fp[i-1] == NULL)
            return FILE_DOESNT_EXIST; 
    }
    ret = Pass2(fp,fileNumber);
    if(ret != INPUT_NORMAL)
        return ret;
    totalLen = 0;
    printf("control symbol address length\n"); // 출력
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
    NewLoad = 1;
    return INPUT_NORMAL;
}
