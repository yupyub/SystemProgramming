#include "20161641.h"
extern int execaddr;
extern int progaddr;
extern int totalLen;
extern unsigned char memory[65536][16];
int reg[20]; // Registers
typedef enum {A, X, L, B, S, T, F, FF, PC, SW}regNum;
// 0:A, 1:X, 2:L, 3:B, 4:S, 5:T, 6:F, 7:F2, 8:PC, 9:SW
int NewLoad = 0; // 프로그램이 새롭게 Load되었는지 알려주는 Flag
int bp[1000];
int bpMax = 0;
int breakPoint(int argc, char argv[100][100]){ // break point를 설정한다
    if(argc > 2)
        return INPUT_ERROR;
    if(argc == 1)
        printBp();
    else if(strcmp(argv[1],"clear") == 0){ // bp 초기화 
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
void store(int address,int value,int byte){ // 메모리에 byte 길이의 값을 저장한다
    char temp[10], shortStr[3];
    if(value<0)
        value -= 0xFF000000;
    sprintf(temp,"%06X",value);
    for(int i = 0;i<byte;i++){
        strncpy(shortStr,temp+i*2,2);
        shortStr[2] = '\0';
        memory[(address+i)/16][(address+i)%16] = strtol(shortStr,NULL,16);
    }
}
int returnValue(int addr,int byte){ // 메모리에 저장된 byte 만큼의 값을 반환한다
    int temp = memory[addr/16][addr%16];
    for(int i = 1;i<byte;i++){
        temp *= 16*16;
        temp += memory[(addr+i)/16][(addr+i)%16];
    }
    return temp;
}
void runOneInstruction(){ // PC기준 1개의 명령어 실행
    // copy.obj 파일의 명령어만 구현되어있음 (ERROR 처리 미구현)
    int opcode = memory[reg[PC]/16][reg[PC]%16];
    int format = 0;
    switch(opcode/0x10){ //format별로 나누어주기
        case 0x9:
        case 0xA:
        case 0xB:
        format = 2;
        break;
        case 0xC:
        case 0xF:
        format = 1;
        break;
        default:
        format = 3;
        if(memory[(reg[PC]+1)/16][(reg[PC]+1)%16]&(1<<4))
            format = 4;
    }
    if(format == 1 || opcode == 0){ //format1 미구현, 빈 memory인 경우 예외처리
        reg[PC]++;
        return;
    }
    else if(format == 2){
        int reg1 = memory[(reg[PC]+1)/16][(reg[PC]+1)%16];
        int reg2 = reg1%16;
        reg1 /= 16;
        switch(opcode){
            case 0xB8: // TIXR
            reg[X]++;
            if(reg[X] == reg[reg1])
                reg[SW] = '=';
            else if (reg[X]<reg[reg1])
                reg[SW] = '<';
            else
                reg[SW] = '>';
            break;
            case 0xA0: // COMPR
            if(reg[reg1] == reg[reg2])
                reg[SW] = '=';
            else if (reg[reg1]<reg[reg2])
                reg[SW] = '<';
            else
                reg[SW] = '>';
            break;
            case 0xB4: // CLEAR
            reg[reg1] = 0;
            break;
        }
        reg[PC]+=2;
    }
    else{ // format 3 and 4
        int ni = opcode%4;
        int addr = memory[(reg[PC]+1)/16][(reg[PC]+1)%16];
        int xbpe = addr/16;
        addr %= 16;
        addr *= 16*16;
        addr += memory[(reg[PC]+2)/16][(reg[PC]+2)%16];
        if(format == 3 && (xbpe & (1<<1)) && addr>(1<<11)) // 2의 보수
            addr -= (1<<12);
        reg[PC] += 3;
        if(format == 4){
            addr *=16*16;
            addr += memory[reg[PC]/16][reg[PC]%16];
            reg[PC]++;
        }
        if(xbpe & (1<<3))
            addr += reg[X];
        if(xbpe & (1<<2))
            addr += reg[B];
        if(xbpe & (1<<1))
            addr += reg[PC];
        if(ni == 2)
            addr = returnValue(addr,3);
        switch((opcode/4)*4){
            case 0x14: // STL
            store(addr,reg[L],3);
            break;
            case 0x68: // LDB
            if(ni == 1)
                reg[B] = addr;
            else
                reg[B] = returnValue(addr,3);
            break;
            case 0x48: // +JSUB
            reg[L] = reg[PC];
            reg[PC] = addr;
            break;
            case 0x00: // LDA
            if(ni == 1)
                reg[A] = addr;
            else
                reg[A] = returnValue(addr,3);
            break;
            case 0x28: // COMP
            if(ni != 1)
                addr = returnValue(addr,3);
            if(reg[A] == addr)
                reg[SW] = '=';
            else if (reg[A]<addr)
                reg[SW] = '<';
            else
                reg[SW] = '>';
            break;
            case 0x30: // JEQ
            if(reg[SW] == '=')
                reg[PC] = addr;
            break;
            case 0x3C: // J
            reg[PC] = addr;
            break;
            case 0x0C: // STA
            store(addr,reg[A],3);
            break;
            case 0x74: // LDT
            if(ni != 1)
                addr = returnValue(addr,3);
            reg[T] = addr;
            break;
            case 0xE0: // TD
            reg[SW] = '<';
            break;
            case 0xD8: // RD
            reg[A] = 0;
            break;
            case 0x54: // STCH
            store(addr,reg[A]%16,1);
            break;
            case 0x38: // JLT
            if(reg[SW] == '<')
                reg[PC] = addr;
            break;
            case 0x10: // STX
            store(addr,reg[X],3);
            break;
            case 0x4C: // RSUB
            reg[PC] = reg[L];
            break;
            case 0x50: // LDCH
            if(ni != 1)
                addr = returnValue(addr,3);
            reg[A] = (reg[A]&0xFFFFFF00) + (addr/0x10000);
            break;
            case 0xDC: // WD
            //
            break;
        }
    }
}
void printRegisters(){ // Print Registers
    printf("    A : %06X   X : %06X\n", reg[A], reg[X]);
    printf("    L : %06X  PC : %06X\n", reg[L], reg[PC]);
    printf("    B : %06X   S : %06X\n", reg[B], reg[S]);
    printf("    T : %06X\n", reg[T]);
}
int runProgram(int argc, char argv[100][100]){ // memory에 load된 프로그램을 실행한다
    if(argc > 1)
        return INPUT_ERROR;
    if(NewLoad){ // 새롭게 loading 한 경우만 초기화 수행
        NewLoad = 0;
        for(int i = 0;i<10;i++)
            reg[i] = 0;
        reg[PC] = execaddr; // 프로그램의 시작주소로 초기화
        reg[L] = totalLen; // 프로그램의 길이로 초기화
    }
    while(reg[PC]<progaddr+totalLen){ // while문으로 runOneInstruction() 실행하면서
        runOneInstruction();
        for(int i = 0;i<bpMax;i++){
            if(reg[PC] == bp[i]){ // bp를 만나면 함수 종료
                printRegisters();
                printf("        Stop at checkpoint[%X]\n",bp[i]);
                return INPUT_NORMAL;
            }
        }
    }
    printRegisters();
    printf("        End Program\n");
    return INPUT_NORMAL;
}
