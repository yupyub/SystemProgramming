#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "temp.h"
void SaveHistory() {
    Node *new_node;
    new_node = (Node*)malloc(sizeof(Node));
    strcpy(new_node->name_instruction, instruction);
    present->next = new_node;
    present = new_node;
    new_node->next = NULL;
}
void PrintHistory() {
    SaveHistory();
    Node *node;
    node = head_history->next;
    int i=1;
    while(node != NULL) {
        printf("%d\t%s\n",i,node->name_instruction);
        node = node->next;
        i++;
    }
}
void PrintListOfInstruction() {
    printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\nassemble filename\ntype filename\nsymbol\n");
    SaveHistory();
}
void PrintFileDirectory() {
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    struct stat buf;
    dir = opendir("."); //현재디렉토리를 연다.
    if(dir != NULL) {
        while((entry = readdir(dir)) != NULL) {
            lstat(entry->d_name,&buf);
            if(S_ISDIR(buf.st_mode))    //디렉토리인 경우
                printf("\t%s/",entry->d_name);
            else if(S_ISREG(buf.st_mode)) {
                //파일인 경우 실행파일일 경우에는 마지막에 *을 추가하여 붙인다.
                if(S_IEXEC & buf.st_mode)
                    printf("\t%s*",entry->d_name);
                else
                    printf("\t%s",entry->d_name);
            }
        }
        printf("\n");
    }
    SaveHistory();
    closedir(dir);
}
int CheckFactor() {
    char factor_str[MAX_INPUT_SIZE];    //instruction 인자의 시작위치부터 저장되는 문자열이다.
    char sub_str_instruction[MAX_INPUT_SIZE];   //instruction의 명령어 다음 위치부터 저장되는 문자열이다.
    int check = 0;  //하나의 인자가 저장이 완료되면 1씩 증가한다.
    int k=0;    //factor[check][k] : check번째 인자의 k번째 글자
    int num_comma = 0;  //','의 개수를 저장한다.
    int num_shold_stored_factor = 0;    //저장되어야할 인자의 수를 저장한다.
    int num_stored_factor = 0; //실제로 저장된 인자의 수를 구한다.
    int hex_flag = 0;   //"0x" 가 붙은 인자에 대하여 예외처리를 위한 변수이다.
    int is_factor_data = 0;     //is_factor_data == 0 이면 인자가 없는 경우이다.
    memset(factor,0,sizeof(factor));
    memset(factor_str,0,sizeof(factor_str));
    long size = strlen(instruction);
    //instruction[idx_command_end]부터 시작하는 부분 문자열 sub_str_instruction을 만든다.
    for(int i=idx_command_end; i<size; i++) {
        sub_str_instruction[i-idx_command_end] = instruction[i];
    }
    sub_str_instruction[size-idx_command_end] = '\0';
    //sub_str_instruction의 ','의 개수를 num_comma에 저장한다.
    for(int i=0; i<size; i++) {
        if(sub_str_instruction[i] == ',')
            num_comma++;
    }
    //sub_str_instruction을 탐색하여 ' ' 또는 '\t'이 아닌 값으로 시작하는 문자열을 만든다. factor_str이 그 문자열이다.
    for(int i=0 ; i<strlen(sub_str_instruction); i++) {
        if(sub_str_instruction[i] != ' ' && sub_str_instruction[i] != '\t') {
            for(int j = i; j < strlen(sub_str_instruction); j++) {
                factor_str[j-i] = sub_str_instruction[j];
            }
            break;
        }
    }
    //factor_str에 스페이스나 탭만 존재하면 인자가 없는 경우이다. dump의 경우 인자 없는 입력 또한 존재하므로 따로 처리해야한다. 인자가 없으므로 0을 리턴한다.
    num_shold_stored_factor = num_comma + 1;//','의 수 + 1 = 저장되어야 할 인자 수
    if(num_comma > 2)       //','의 최대 개수는 2개이다. 2개 초과시 return -1
        return -1;
    //factor_str을 탐색하여 인자가 존재하는 경우 is_factor_data에 1을 저장한다.
    for(int i=0; i<strlen(factor_str); i++) {
        if(!(factor_str[i] == ' ' || factor_str[i] == '\t'))
            is_factor_data = 1;
    }
    if(is_factor_data == 0) //is_factor_data
        return 0;
    unsigned long size_factor_str = strlen(factor_str);
    for(int i=0; i<size_factor_str; i++) {
        if(k>MAX_INPUT_SIZE-1) {
            printf("입력 받을 수 있는 최대 사이즈는 MAX_INPUT_SIZE 입니다\n");
            return -1;
        }
        //저장중지하고 인자 받았다고 체크한다. type or assemble 관련 명령어일 경우 파일명이 들어오므로 , 또한 입력을 받아줘야 하므로 is_type_or_assemble == 0 을 추가하였다.
        if(factor_str[i] == ',') {
            check++;
            k = 0;  //하나의 인자가 들어왔으므로 factor[check][k]를 시작하기 위해 k=0으로 초기화
        } else {
            //0x로 시작하는 인자가 들어오는 경우 0x를 무시한다. factor에 이미 값이 존재하는데 0x가 추가로 들어온 경우에는 유효하지 않은 경우이다.
            //ex) 0xff(O), 00xff(X), 100xff(X)
            //"0x" 다음에 ' '또는 '\t'이 오면 에러처리한다.
            if(hex_flag == 1 && (factor_str[i] == ' ' || factor_str[i] == '\t')) {
                return -1;
            }
            if(factor_str[i] == '0' && (factor_str[i+1] == 'x' || factor_str[i+1] == 'X') && i != size_factor_str-1 && factor[check][0] == '\0'){
                i++;
                hex_flag = 1;
                continue;
            }
            //인자에는 0~9, a~f, A~F만 들어갈 수 있다. ' '나 '\t'는 들어갈 수 없다.
            if(( ('0' <= factor_str[i] && factor_str[i] <= '9' ) || ('A' <= factor_str[i] && factor_str[i] <= 'F') || ('a' <= factor_str[i] && factor_str[i] <= 'f')) && factor_str[i]!= ' ' && factor_str[i] != '\t' && is_opcode != 1 && is_type_or_assemble != 1) {
                if((factor_str[i-1] == '\t' || factor_str[i-1] == ' ') && factor[check][0] != '\0'){
                    return -1;
                }
                factor[check][k] = factor_str[i];
                k++;
                hex_flag = 0;
            } else if((!(factor_str[i] == ' ' || factor_str[i] == '\t')) && is_opcode != 1 && is_type_or_assemble != 1) {
                return -1;
            } else if(factor_str[i] != ' ' && factor_str[i] != '\t' && is_opcode == 1 && is_type_or_assemble != 1){    //is_opcode == 1 인 경우에는 모든 문자를 입력받아야한다.
                if((factor_str[i-1] == '\t' || factor_str[i-1] == ' ') && factor[check][0] != '\0'){
                    return -1;
                }
                factor[check][k] = factor_str[i];
                k++;
            } else if(is_type_or_assemble == 1) {   //type or assemble에 관한 명령이므로 파일명이 입력된다 따라서 모든 문자를 입력받는다.
                factor[check][k] = factor_str[i];
                k++;
            }
        }
    }
    //실제로 저장된 인자의 수를 구한다.
    for(int i = 0; i < num_shold_stored_factor; i++) {
        if(factor[i][0] != '\0')
            num_stored_factor++;
    }
    if(num_stored_factor != num_shold_stored_factor)    //실제로 저장된 인자의 수와 저장되어야 할 수가 다를 경우 에러처리.
        return -1;
    return num_stored_factor;   //저장된 인자의 수 리턴
}
void Dump() {
    int num_of_factor = CheckFactor();  //인자의 유효성을 판단한다. 문제가 있는 경우 -1이 저장되고, 정상인 경우 인자의 갯수가 저장된다.
    long start,end; //시작 주소와 마지막 주소가 저장된다.
    char *error_ptr;    //strtol을 이용한 16진수 변환시 에러가 발생한 위치를 반환할 포인터 변수이다.
    /*메모리 내용 출력을 위한 시작, 끝값 설정*/
    if(num_of_factor == 0) {
        if(address >= MAX_MEMORY_SIZE-1)    //address가 MAX_MEMORY_SIZE-1인 경우에 dump를 했을 때 '0번지'부터 시작하기 위하여 address = -1로 초기화한다.
            address = -1;
        start = address+ 1;    //이전에 출력된 마지막 주소의 +1이 메모리 내용을 보여줄 시작 주소이다. dump를 처음 실행할 시 start = 0 이 된다.
        end = start + 159;      //총 160개의 출력을 위해 start + 159를 저장한다.
    } else if(num_of_factor == 1) {
        start = strtol(factor[0],&error_ptr,16);
        end = start + 159;
    } else if(num_of_factor == 2) {
        start = strtol(factor[0],&error_ptr,16);
        end = strtol(factor[1],&error_ptr,16);
    } else {
        printf("인자에 문제가 있습니다.\n");
        return;
    }
    if(start > MAX_MEMORY_SIZE-1 || start < 0) {
        printf("start의 값은 0 ~ MAX_MEMORY_SIZE-1 주소만 입력가능합니다.\n");
        return;
    }
    if(start > end) {
        printf("시작 인자가 끝 인자보다 큽니다.\n");
        return;
    }
    if(end > MAX_MEMORY_SIZE-1) {
        end = MAX_MEMORY_SIZE-1;
    }
    long line_start = start - start%16; //시작 라인 넘버가 저장됨
    long line_end = end - end%16; //마지막 라인 넘버가 저장됨
    for(long mem_no = line_start; mem_no <= line_end; mem_no+=16) {
        printf("%05lX ",mem_no);
        for(long j = mem_no; j < mem_no + 16; j++) {
            if(j < start || j > end) {
                printf("  ");
            } else {
                int val = virtualMemory[j];
                printf("%02X",val);
            }
            printf(" ");
        }
        printf("; ");
        for(long k = mem_no; k < mem_no + 16; k++) {
            if(20 <= virtualMemory[k] && virtualMemory[k] <= 0x7E) {
                printf("%c",virtualMemory[k]);
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
    address = end;  //출력된 마지막 주소를 address에 저장한다.
    SaveHistory();
}
void Edit() {
    int num_of_factor = CheckFactor();
    long address_edit, value;   //address_edit 주소의 메모리를 value로 바꾼다.
    char *error_ptr = NULL;     //strtol을 이용한 16진수 변환시 에러가 발생한 위치를 반환할 포인터 변수이다.
    //2개의 인자만 입력되어야 한다.
    if(num_of_factor != 2) {
        printf("인자 입력에 문제가 있습니다.\n");
        return;
    }
    address_edit = strtol(factor[0],&error_ptr,16);
    value = strtol(factor[1],&error_ptr,16);
    if(address_edit > MAX_MEMORY_SIZE-1 || address_edit < 0) {
        printf("0 ~ MAX_MEMORY_SIZE-1 사이의 주소만 수정할 수 있습니다.\n");
        return;
    }
    if(value > 0xFF) {
        printf("00~FF 사이의 값으로만 변경할 수 있습니다.\n");
        return;
    }
    virtualMemory[address_edit] = value;     //address_edit 번지의 값을 value로 변경한다.
    SaveHistory();
}
void Fill() {
    int num_of_factor = CheckFactor();
    long start,end,value;   //메모리의 start번지부터 end번지까지의 값을 value로 변경한다
    char *error_ptr = NULL; //strtol을 이용한 16진수 변환시 에러가 발생한 위치를 반환할 포인터 변수이다.
    if(num_of_factor != 3) {
        printf("인자 입력에 문제가 있습니다..\n");
        return;
    }
    start = strtol(factor[0], &error_ptr, 16);
    end = strtol(factor[1], &error_ptr, 16);
    value = strtol(factor[2], &error_ptr, 16);
    if(start > MAX_MEMORY_SIZE-1 || start < 0 || end > MAX_MEMORY_SIZE-1 || end < 0) {
        printf("start와 end의 값은 0 ~ MAX_MEMORY_SIZE-1 사이의 주소만 입력가능합니다.\n");
        return;
    }
    if(start > end) {
        printf("start가 end인자보다 큽니다.\n");
        return;
    }
    if(value > 0xFF) {
        printf("00~FF 사이의 값으로만 변경할 수 있습니다.\n");
        return;
    }
    //start번지부터 end번지의 값을 value로 변경한다.
    for(long i = start; i < end + 1; i++) {
        virtualMemory[i] = value;
    }
    SaveHistory();
}
void Reset(){
    memset(virtualMemory,0,sizeof(virtualMemory));  //메모리를 전부 0으로 초기화한다.
    SaveHistory();
}
int HashKey(char *input_mnemonic) {
    //mnemonic의 첫글자와 마지막글자를 20으로 나눈 나머지를 키값으로 설정한다.
    long size = strlen(input_mnemonic);
    int key = input_mnemonic[0] + input_mnemonic[size-1];
    return key%20;
}
void HashInput(int key, char *mnemonic, int value, char *format) {
    //새로운 노드에 opcode와 관련된 정보를 저장한 후 HashTable에 저장한다.
    HashNode *new_node =(HashNode*)malloc(sizeof(HashNode));
    new_node->opcode = value;
    strcpy(new_node->mnemonic,mnemonic);
    strcpy(new_node->format,format);
    if(HashTable[key] == NULL) {
        HashTable[key] = new_node;
        new_node->next = NULL;
    } else {
        new_node->next = HashTable[key];
        HashTable[key] = new_node;
    }
}
int FindHash(int find_key, char *target) {
    //HashTable[find_key]를 탐색하여 일치하는 mnemonic이 있는 경우 대응하는 opcode를 반환한다. 없는 경우 -1을 반환한다.
    HashNode *find_node = HashTable[find_key];
    while(find_node != NULL) {
        if(strcmp(find_node->mnemonic,target) == 0){
            target_hash = find_node;
            return find_node->opcode;
        }
        find_node = find_node->next;
    }
    //while문에서 찾지 못한 경우 -1 리턴
    return -1;
}
void Opcode() {
    int num_of_factor = CheckFactor();
    if(num_of_factor != 1) {
        printf("인자에 문제가 있습니다.\n");
        return;
    }
    int find_key = HashKey(factor[0]);
    int val_opcode = FindHash(find_key,factor[0]);
    if(val_opcode == -1) {
        printf("mnemonic을 소문자로 입력했거나 해당 mnemonic에 대한 opcode를 찾을 수 없습니다.\n");
        return;
    } else {
        printf("opcode is %02X\n",val_opcode);
    }
    SaveHistory();
}
void PrintOpcodeList() {
    for(int i = 0; i < 20 ; i++) {
        HashNode *node_for_print= HashTable[i];
        printf("%d :",i);
        if(HashTable[i] == NULL) {
            printf("\n");
            continue;
        }
        while(node_for_print != NULL) {
            printf(" [%s,%02X]",node_for_print->mnemonic,node_for_print->opcode);
            if(node_for_print->next != NULL) {
                printf(" ->");
            }
            node_for_print = node_for_print->next;
        }
        printf("\n");
    }
    SaveHistory();
}
void CheckCorrectInstruction(char* inst_call) {
    char real_command[12];  //인자를 제외한 순수 명령어만 저장하는 변수이다.
    int check_single = 0;   //instruction의 명령어가 인자가 없는 명령어일 경우 1을 저장한다.
    int check_plural = 0;   //instruction의 명령어가 인자가 있는 명령어일 경우 1을 저장한다.
    int idx_del_space = 0;  //명령어 앞에 ' ' 또는 '\t'가 있는 경우 해당 명령어의 시작 위치를 저장하는 index
    long size = strlen(instruction);    //instruction의 길이를 저장한다.
    memset(real_command, 0, sizeof(real_command));
    for(int i = 0; i < size; i++) {
        //처음으로 공백이 아닌 문자가 나오면 해당 인덱스를 저장
        if(!(instruction[i] == ' ' || instruction[i] == '\t')){
            idx_del_space = i;
            break;
        }
    }
    idx_command_start = idx_del_space;
    //idx_del_space부터 탐색하여 이후에 ' '나 '\t'이 있으면 그 전까지 real_command에 저장한다.
    int j = 0;  //명령어의 마지막 위치의 다음 위치를 저장하는 변수이다.
    for(int i = idx_del_space; i < size ; i++) {
        if(i-idx_del_space>11) {//real_command의 최대 사이즈를 초과한 입력이 들어오면 에러처리한다.
            strcpy(inst_call,"\0");
            return;
        }
        if(instruction[i] == ' ' || instruction[i] == '\t'){
            idx_command_end = i;
            break;
        }
        real_command[i-idx_del_space] = instruction[i];
        j = i;
        idx_command_end = i;
    }
    idx_command_end++;  //checkFacotr()에서 명령어의 마지막위치의 다음 위치를 접근하는데 쓰이므로 1 증가시킨다.
    j++;    //명령어의 마지막위치의 다음 위치를 가리켜야하므로 1 증가시킨다.
    //추출된 realcommand가 list_single_instruction에 존재하는지 확인한다. 인자가 없는 명령어일 경우, 입력된 command이외에 다른 문자가 있으면 입력오류이다.
    for(int i=0; i<12; i++) {
        if(strcmp(real_command,list_single_instruction[i]) == 0) {
            strcpy(inst_call,real_command);
            for(; j<size; j++) {
                if((instruction[j] == '\t') || (instruction[j] == ' ')) {
                    continue;
                } else {
                    strcpy(inst_call, "\0");
                    return;
                }
            }
            check_single = 1;
            break;
        }
    }
    //check_single == 1 : real_command가 list_single_instruction에 있음을 확인했으므로 return.
    if(check_single == 1)
        return;
    else {  //check_single == 0 : real_command가 list_single_instruction에 없으므로 list_plural_instruction에 존재하는지 확인한다.
        for(int j=0; j<12; j++) {
            if(strcmp(real_command, list_plural_instruction[j]) == 0) {    //유효한 명령어이다.
                strcpy(inst_call, real_command);
                check_plural = 1;
            }
        }
        if(check_plural == 0)
            strcpy(inst_call,"\0");
    }
}
void ImplementInstruction() {
    char inst_call[12];
    CheckCorrectInstruction(inst_call);
    if(strcmp(inst_call,"help") == 0 || strcmp(inst_call,"h") == 0) {
        PrintListOfInstruction();
    } else if(strcmp(inst_call,"d") == 0 || strcmp(inst_call,"dir") == 0) {
        PrintFileDirectory();
    } else if(strcmp(inst_call,"q") == 0 || strcmp(inst_call,"quit") == 0) {
        FreeMemory();
        exit(0);   //프로그램을 종료하기 전에 동적할당한 메모리를 해제한다.
    } else if(strcmp(inst_call,"hi") == 0 || strcmp(inst_call,"history") == 0) {
        PrintHistory();
    } else if(strcmp(inst_call,"reset") == 0) {
        Reset();
    } else if(strcmp(inst_call,"opcodelist") == 0) {
        PrintOpcodeList();
    } else if(strcmp(inst_call,"dump") == 0 || strcmp(inst_call,"du") == 0) {
        Dump();
    } else if(strcmp(inst_call,"edit") == 0 || strcmp(inst_call,"e") == 0) {
        Edit();
    } else if(strcmp(inst_call,"fill") == 0 || strcmp(inst_call,"f") == 0) {
        Fill();
    } else if(strcmp(inst_call,"opcode") == 0) {
        is_opcode = 1;
        Opcode();
        is_opcode = 0;
    } else if(strcmp(inst_call,"type") == 0) {
        is_type_or_assemble = 1;
        ReadAndPrintFile();
        is_type_or_assemble = 0;
    } else if(strcmp(inst_call,"assemble") == 0) {
        FreeSymbolTable(SymbolTable);
        is_type_or_assemble = 1;
        Assemble();
        is_type_or_assemble = 0;
    } else if(strcmp(inst_call,"symbol") == 0){
        PrintSymbolTable();
    } else if(strcmp(inst_call,"progaddr") == 0 ) {
        SetProgaddr();
    } else if(strcmp(inst_call,"loader") == 0) {
        LinkingLoader();
    } else if(strcmp(inst_call,"bp") == 0) {
        Debug();
    } else if(strcmp(inst_call,"run") == 0) {
        Run();
    } else {
        printf("입력에 오류가 있습니다.\n");
    }
    memset(inst_call,0,sizeof(inst_call));
}
ESTAB* SearchESTAB(char find_symbol[], int file_num) {
    ESTAB *search;
    for(int i = 0; i < 3; i++) {
        search = estab[i];
        while(search != NULL) {
            if(strcmp(find_symbol,search->symbol) == 0 || strcmp(find_symbol,search->csec_name) == 0 ) {
                return search;
            }
            search = search->next;
        }
    }
    return NULL;
}
int HashESTAB(char csname[], char symbol[], long indicated_address,int file_num) {
    ESTAB *search = estab[file_num];    //찾으려는 파일의 테이블 시작 노드를 저장
    ESTAB *input;   //새로 넣으려는 symbol을 저장하기 위한 노드이다.
    input = (ESTAB*)malloc(sizeof(ESTAB));
        //ESTAB에 csname이 이미 존재하는 경우
    if(strcmp(csname,estab[file_num]->csec_name) == 0 && strcmp(symbol,"\0") == 0) {
        return 1;
    } else if(strcmp(csname,estab[file_num]->csec_name) != 0 && strcmp(symbol,"\0") == 0) {
        strcpy(estab[file_num]->csec_name,csname);
        strcpy(estab[file_num]->symbol,"\0");
        estab[file_num]->address = csaddr;
        estab[file_num]->length = cslth;
        estab[file_num]->next = NULL;
    } else if(strcmp(symbol,"\0") != 0) {
        //symbol이 estab에 존재하는지 확인
        while(search->next != NULL) {
            if(strcmp(symbol,search->symbol) == 0) {
                return 1;
            }
            search = search->next;
        }
        search->next = input;
        strcpy(input->symbol,symbol);
        strcpy(input->csec_name,"\0");
        input->address = csaddr + indicated_address;
        input->next = NULL;
    }
    return 0;
}
long *ReturnRegisterAddress(char R) {
    //A 0 X 1 L 2 B 3 S 4 T 5 PC 8
    switch (R) {
        case '0':
            return &A;
        case '1':
            return &X;
        case '2':
            return &L;
        case '3':
            return &B;
        case '4':
            return &S;
        case '5':
            return &T;
        case '8':
            return &PC;
        default:
            break;
    }
    return NULL;
}
void GetXBPE(long half_byte_value, int *xbpe) {
    for(int i=0; i<4; i++) {
        xbpe[i] = half_byte_value%2;
        half_byte_value = half_byte_value/2;
    }
}
void ExecuteInstruction(long opcode, unsigned char *update_memory, long imm_value, long TA, int E) {
    int tmp = 0;
    long copy = 0;
    long target = 0;
    if(update_memory != NULL)
        target = *update_memory;
    A = A & 0x00FFFFFF;
    X = X & 0x00FFFFFF;
    L = L & 0x00FFFFFF;
    PC = PC & 0x00FFFFFF;
    B = B & 0x00FFFFFF;
    S = S & 0x00FFFFFF;
    T = T & 0x00FFFFFF;
    switch (opcode) {
        case 0x04:  //LDX
            if(update_memory == NULL) {
                X = imm_value;
            } else {
                X = *update_memory;
                for(int i=1; i<=2; i++) {
                    X = (X << 8) + *(update_memory+i);
                }
            }
            
            break;
        case 0x00:  //LDA
            if(update_memory == NULL) {
                A = imm_value;
            } else {
                A = *update_memory;
                for(int i=1; i<=2; i++) {
                    A = (A << 8) + *(update_memory+i);
                }
            }
            
            break;
        case 0x68:  //LDB
            if(update_memory == NULL) {
                B = imm_value;
            } else {
                B = *update_memory;
                for(int i=1; i<=2; i++)
                    B = (B << 8) + *(update_memory+i);
            }
            break;
        case 0x74: //LDT
            if(update_memory == NULL) {
                T = imm_value;
            } else {
                T = *update_memory;
                for(int i=1; i<=2; i++)
                    T = (T << 8) + *(update_memory+i);
            }
            break;
        case 0x50: //LDCH
            
            A = 0;
            if(update_memory == NULL) {
                A = imm_value;
            } else {
                A = *update_memory;
            }
            break;
        case 0x0C: //STA
            tmp = 0xFF0000;
            for(int i=2; i>=0; i--) {
                copy = A;
                copy = copy & tmp;
                copy = copy >> 8*i;
                *(update_memory+ 2-i) = copy;
                tmp = tmp >> 8;
            }
            break;
        case 0x10: //STX
            tmp = 0xFF0000;
            for(int i=2; i>=0; i--) {
                copy = X;
                copy = copy & tmp;
                copy = copy >> 8*i;
                *(update_memory+ 2-i) = copy;
                tmp = tmp >> 8;
            }
            break;
        case 0x14: //STL
            tmp = 0xFF0000;
            for(int i=2; i>=0; i--) {
                copy = L;
                copy = copy & tmp;
                copy = copy >> 8*i;
                *(update_memory+ 2-i) = copy;
                tmp = tmp >> 8;
            }
            break;
        case 0x54: //STCH
            *update_memory = A & 0x0000FF;
            break;
        case 0x3C: //J
            address_value = PC = TA;
            break;
        case 0x48: //JSUB
            L = PC;
            address_value = PC = TA;
            break;
        case 0x38: //JLT
            if(SW == '<')
                address_value = PC = TA;
            break;
        case 0x30: //JEQ
            if(SW == '=')
                address_value = PC = TA;
            break;
        case 0x4C: //RSUB
            address_value = PC = L;
            break;
        case 0x28: //COMP
            if(update_memory == NULL) {
                target = imm_value;
            } else {
                for(int i=1; i<3; i++){
                    target = (target << 8) + *(update_memory+i);
                }
            }
            if(A == target)
                SW = '=';
            else if(A < target)
                SW = '<';
            else if(A > target)
                SW = '>';
            break;
        case 0xE0: //TD
            SW = '<'; //ready
            break;
        case 0xD8: //RD
            A = 0;
            break;
        case 0xDC: //WD
                *update_memory =  A & 0x0000FF;
            break;
        default:
            break;
    }
}
void Run() {
    //progaddr 부터 메모리를 읽어온다.
    //i번재 run 수행시 bp[i]까지 진행
    //더이상 bp가 없는 경우 끝까지 수행
    char *errptr;   //strtol 사용시 err위치 저장
    long bp_value;  //bp 값 저장
    long end_address = 0;   //프로그램의 마지막 주소 저장
    char one_byte_str[3];   //첫번쨰 바이트 저장
    long one_byte_value;    //첫번쨰 바이트 저장
    char two_byte_str[3];   //두번째 바이트 저장
    long two_byte_value;    //두번째 바이트 저장
    char three_byte_str[3]; //세번째 바이트 저장
    long three_byte_value;  //세번째 바이트 저장
    char four_byte_str[3];  //네번째 바이트 저장
    long four_byte_value;   //네번쨰 바이트 저장
    char half_byte_str[2];  //하프 바이트 저장
    long half_byte_value;   //하프바이트 저장
    char R1,R2;             //첫번째, 두번째 레지스터 저장
    long *R1_value, *R2_value;  //첫번째, 두번째 레지스터 저장
    char TA_str[7]; //Target Address 저장
    long TA_value = 0;  //Target Address 저장
    unsigned char *update_memory = 0;   //가상메모리가 실제로 가리키는 주소를 저장
    int start_flag = 1; //처음 시작인지 확인하는 플래그
    long opcode = 0;    //opcode 저장
    int xbpe[4];    //xbpe 저장 x[3]: x x[2] : b x[1] : p x[0] : e
    int indirect_TA_value = 0;  //indirect일 경우 Target Address 저장
    char end_point[8];  //bp 걸린 부분 저장
    
    //bp가 지정되어 있지 않거나 지정한 bp를 다 확인한 경우 bp_value를 프로그램 끝 주소로 지정
    end_address = progaddr;
    for(int i=0; i<3; i++) {
        end_address += estab[i]->length;
    }
    bp_value = progaddr + end_address;
    if(end_flag == 0) {
        PC = address_value = progaddr;
        L =  progaddr + end_address;
    }
    //address_value부터 메모리를 읽어와 bp_value까지 탐색한다.
    while(address_value <= bp_value) {
        //탈출 조건
        if( (PC == progaddr + end_address ||  PC == 0 ) && start_flag == 0)
            break;
        address_value = PC;
        sprintf(one_byte_str,"%02hhX",virtualMemory[address_value]);
        one_byte_value = virtualMemory[address_value];
        start_flag = 0;
        if(one_byte_str[0] == 'C' || one_byte_str[0] == 'F') {
            //format1인 경우
            PC += 1;
            address_value++;
        } else if(one_byte_str[0] == '9'  || one_byte_str[0] == 'A' || one_byte_str[0] == 'B') {
            //format2인 경우 1 byte를 더 읽어 어떤 레지스터를 사용하는지 확인한다.
            //A 0 X 1 L 2 B 3 S 4 T 5 PC 8
            address_value++;
            PC += 2;
            sprintf(two_byte_str,"%02hhX",virtualMemory[address_value]);
            R1 = two_byte_str[0];
            R2 = two_byte_str[1];
            
            R1_value = ReturnRegisterAddress(R1);
            R2_value = ReturnRegisterAddress(R2);
            if(strcmp(one_byte_str,"A0") == 0)  {   //COMPR : 레지스터 수정 X 두 레지스터 비교해서 CC에 '<','>','='중 하나 넣어둔다.
                if(*R1_value == *R2_value) {
                    SW = '=';
                } else if(*R1_value > *R2_value) {
                    SW = '>';
                } else {
                    SW = '<';
                }
            } else if(strcmp(one_byte_str,"B4") == 0) { //CLEAR : R1레지스터 0으로 초기화
                *R1_value = 0;
            } else if(strcmp(one_byte_str,"B8") == 0) { //TIXR
                X++;
                if(X == *R1_value) {
                    SW = '=';
                } else if(X < *R1_value){
                    SW = '<';
                } else {
                    SW = '>';
                }
            } else if(strcmp(one_byte_str,"90") == 0) {
                
            } else if(strcmp(one_byte_str,"9C") == 0) {
                
            } else if(strcmp(one_byte_str,"98") == 0) {
                
            } else if(strcmp(one_byte_str,"AC") == 0) {
                
            } else if(strcmp(one_byte_str,"A4") == 0) {
                
            } else if(strcmp(one_byte_str,"94") == 0) { //SUBR

            } else if(strcmp(one_byte_str,"B0") == 0) { //SVC
                
            }
            address_value++;
        } else {
            //format 3 or 4인 경우
            //ni 분석 bb(xx) 12로 나눈 나머지가 xx
            long ni = one_byte_value & 0x3;
            opcode = one_byte_value - ni;
            address_value++;
            sprintf(two_byte_str,"%02hhX",virtualMemory[address_value]); //'xbpe''disp상위 4bit'
            two_byte_value = virtualMemory[address_value];
            half_byte_str[0] = two_byte_str[0];    //'xbpe'
            half_byte_str[1] = '\0';
            half_byte_value = strtol(half_byte_str,&errptr,16);
            TA_str[0] = two_byte_str[1];    //'disp상위 4bit'
            TA_str[1] = '\0';
            address_value++;
            sprintf(three_byte_str,"%02hhX",virtualMemory[address_value]);
            three_byte_value = virtualMemory[address_value];
            strcat(TA_str,three_byte_str);
            //TA_str에는 disp가 저장되어 있음.
            GetXBPE(half_byte_value, xbpe);
            if(xbpe[0] == 1)
                PC += 4;
            else
                PC += 3;
            if(xbpe[0] == 1) { // e = 1 -> format 4
                //format 4
                //TA_address + four_byte_str -> absoulte address
                address_value++;
                sprintf(four_byte_str,"%02hhX",virtualMemory[address_value]);
                four_byte_value = virtualMemory[address_value];
                strcat(TA_str, four_byte_str);
            }
            TA_value = strtol(TA_str, &errptr, 16);
            if(('8'  <=  TA_str[0] && TA_str[0] <= '9') || ('A' <= TA_str[0] && TA_str[0] <='F')) {
                //음수를 가져야 하는 경우
                TA_value =  -(0xFFF-TA_value+1);
            }
            //TA값 확정짓기
            if(xbpe[3] == 1) {  // x == 1
                TA_value += X;
                if(xbpe[2] == 1) {  // b == 1
                    TA_value += B;
                } else if( xbpe[1] == 1) {  // or p == 1
                    TA_value += PC;
                }
            } else {    // x == 0
                if(xbpe[2] == 1) {  // b == 1
                    TA_value += B;
                } else if( xbpe[1] == 1) {  // or p == 1
                    TA_value += PC;
                }
            }
            address_value++;
            if(ni == 0) {   //sic mode x(1)address(15)
                memset(TA_str, 0, sizeof(TA_str));
                if(xbpe[3] == 1) {
                    //bpe + 남은 halfbyte + three byte가 sic mode의 주소
                    two_byte_value = two_byte_value - 128;
                }
                TA_value = two_byte_value;
                TA_value = TA_value << 8;
                TA_value += three_byte_value;
                ExecuteInstruction(opcode, &virtualMemory[TA_value], -1, TA_value, xbpe[0]);
            } else if(ni == 1){ //immediate
                ExecuteInstruction(opcode, NULL, TA_value,TA_value,xbpe[0]);
            } else if(ni == 2) {    //indirect
                update_memory = &virtualMemory[virtualMemory[TA_value]];
                indirect_TA_value = 0;
                for(int i=0; i<3; i++) {
                    indirect_TA_value += virtualMemory[TA_value+i];
                    if(i!=2)
                        indirect_TA_value = indirect_TA_value <<8;
                }
                ExecuteInstruction(opcode, update_memory, -1, indirect_TA_value,xbpe[0]);
                
            } else if(ni == 3) {    //simple
                update_memory = &virtualMemory[TA_value];
                ExecuteInstruction(opcode, update_memory, -1, TA_value,xbpe[0]);
            }
            
        }
        for(int i = 0; bp[i][0] != '\0'; i++) {
            if(PC == strtol(bp[i],&errptr,16)) {
                strcpy(end_point,bp[i]);
                end_flag = 1;
                break;
            }
        }
        if(end_flag == 1) {
            end_flag = -1;
            break;
        }
    }
    if(end_address != 0) {
        run_bp_idx = 0;
        address_value = progaddr;
    }
    printf("A : %06lX  X : %06lX\n",A,X);
    printf("L : %06lX PC : %06lX\n",L,PC);
    printf("B : %06lX  S : %06lX\n",B,S);
    printf("T : %06lX\n",T);
    if(PC == end_address) {
        printf("            End Program\n");
        end_flag = 0;
        SaveHistory();
        return;
    } else {
        printf("            Stop at checkpoint[%s]\n",end_point);
        SaveHistory();
        return;
    }
}
void Debug() {
    char input_line[1000]; //입력을 복사한다.
    char *garbage;  //strtok에 사용되는 변수
    char *errptr;   //strtol사용시 err 위치 저장
    long max_valule = 0;    //bp 지정가능한 최대 길이 저장.
    for(int i=0; i<3; i++) {
        max_valule += estab[i]->length;
    }
    strcpy(input_line,instruction);
    if(strcmp(instruction,"bp") == 0) {
        printf("            breakpoint\n");
        printf("            ----------\n");
        for(int i=0; bp[i][0] != 0; i++) {
            printf("            %s\n",bp[i]);
        }
        return;
    }
    garbage = strtok(input_line," \t");
   
    while(garbage != NULL) {
        garbage = strtok(NULL," \t");
        if(garbage == NULL)
            break;
        if(strcmp(garbage,"clear") == 0) {
            memset(bp,0,sizeof(bp));
            bp_idx = 0;
            printf("            [ok] clear all breakpoints\n");
        } else {
            for(int i=0; i<strlen(garbage); i++) {
                if( ('0' <= garbage[i] && garbage[i]<= '9' ) || ('A' <= garbage[i] && garbage[i] <= 'F') || ('a' <= garbage[i] && garbage[i] <= 'f') ){
                } else {
                    printf("bp이후의 문자가 clear나 16진수 주소값이 아닙니다.\n");
                    return;
                }
            }
            if(max_valule < strtol(garbage,&errptr,16)) {
                printf("프로그램 라인을 초과한 bp 지정입니다.\n");
                return;
            }
            strcpy(bp[bp_idx],garbage);
            bp_idx++;
            printf("            [ok] create breakpoint %s\n",bp[bp_idx-1]);
        }
    }
    SaveHistory();
}
void LoaderPass2(FILE *fp[], int file_num) {
    csaddr = progaddr;
    execaddr = progaddr;
    char *errptr;   //strtol에 사용되는 포인터
    char input_record[1000]; //프로그램별로 한 라인씩 저장한다.
    char onebyte_str[3];    //objcode 1 byte씩(2글자) 짤라서 저장한다
    long onebyte_value = 0;    //onbyte_str을 메모리에 저장될 값으로 변환한다.
    char record_size_str[7];    //Text record 사이즈를 문자열로 저장한다.
    long record_size = 0;   //Text record 별 사이즈를 저장한다.
    char T_address_str[7];
    long T_address = 0; //Text record별 시작 주소를 저장한다.
    long mem_address = 0;   //실제 저장될 메모리의 주소
    char ref_num[3];    //R reocord의 reference number 저장.
    char r_symbol[7]; //R record의 symbol 저장
    char r_dictionary[100][7];  //r_dictionary[ref_num][] : ref_num에 대응하는 r_symbol이 저장되어 있음.
    char m_address_str[7];  //M record에 저장된 주소를 문자열로 저장
    long m_address; //m_address_str을 숫자로 바꿔 저장
    char objcode_str[10];   //objcode를 문자열로 그대로 받아온다
    long objcode_value; //objcode_str을 숫자로 바꿔 저장
    char e_address_str[7];  //E record에 주소가 기입되어 있는 경우 문자열로 그대로 저장
    long e_address_value;   //e_address_str을 숫자로 바꿔 저장
    for(int i=0; i<file_num; i++) {
        //Header record를 읽는다.
        memset(r_dictionary,0,sizeof(r_dictionary));
        fgets(input_record,sizeof(input_record),fp[i]);
        cslth = strtol(input_record+13,&errptr,16);
        while(fgets(input_record,sizeof(input_record),fp[i]) != NULL) {
            if(input_record[0] == 'T') {
                memset(T_address_str,0,sizeof(T_address_str));
                memset(record_size_str,0,sizeof(record_size_str));
                memset(onebyte_str,0,sizeof(onebyte_str));
                strncpy(T_address_str,input_record+1,6);
                T_address_str[6] = '\0';
                mem_address = T_address = strtol(T_address_str,&errptr,16);
                strncpy(record_size_str, input_record+7, 2);
                record_size_str[2] = '\0';
                record_size = strtol(record_size_str, &errptr, 16);
                mem_address += csaddr;
                for(int k = 0; k < record_size; k++) {
                    //1. 한 바이트씩 읽어서 memory에 올린다.
                    strncpy(onebyte_str, input_record+9+k*2, 2);
                    onebyte_str[2] = '\0';
                    onebyte_value = strtol(onebyte_str,&errptr,16);
                    virtualMemory[mem_address] = onebyte_value;
                    mem_address += 1;
                }
            } else if(input_record[0] == 'M') {
                //reference number에 대응하는 symbol의 값을 ESTAB에서 찾는다
                //M-record 주소 + csaddr + (0,1,2)를 하여 한 바이트씩 가져와서 다시 문자열로 합친다.
                //그 문자열을 16진수로 변환한다.
                //처음에 찾았던 값을 더하거나 뺀다
                memset(objcode_str,0,sizeof(objcode_str));
                memset(m_address_str,0,sizeof(m_address_str));
                strncpy(m_address_str,input_record+1,6);
                m_address_str[6] = '\0';
                m_address = strtol(m_address_str,&errptr,16) + csaddr;
                for(int k = 0; k < 3; k++) {
                    sprintf(onebyte_str,"%02X",virtualMemory[m_address+k]);
                    strcat(objcode_str,onebyte_str);
                }
                // 음수인 경우 실제로 음수값이 저장되도록 값을 변경 후 처리.
                if(('8' <= objcode_str[0] && objcode_str[0] <= '9') || ('A' <= objcode_str[0] && objcode_str[0] <= 'F')) {
                    objcode_value = strtol(objcode_str,&errptr,16);
                    objcode_value = 0xFFFFFF - objcode_value + 1;
                    objcode_value = -objcode_value;
                } else {
                    objcode_value = strtol(objcode_str,&errptr,16);
                }
                if(m_address == 0x24) {
                    printf("obj_value : ");
                    for(int i=0; i<3; i++) {
                        printf("%02X",virtualMemory[m_address+i]);
                    }
                    printf("\n");
                }
                strncpy(ref_num, input_record+10, 2);
                ref_num[2] = '\0';
                int key = atoi(ref_num);
                ESTAB* modi_estab = SearchESTAB(r_dictionary[key],file_num);
                if(modi_estab == NULL) {
                    printf("ESTAB에 존재하지 않는 symbol입니다.\n");
                    load_error_flag = 1;
                    return;
                }
                if(input_record[9] == '+') {
                    objcode_value += modi_estab->address;
                } else if(input_record[9] == '-') {
                    objcode_value -= modi_estab->address;
                }
                if(objcode_value < 0) {
                    objcode_value -= 0xFFFFFFFFFF000000;
                }
                sprintf(objcode_str,"%06lX",objcode_value);
                for(int k = 0; k<6; k+=2) {
                    strncpy(onebyte_str,objcode_str+k,2);
                    onebyte_str[2] = '\0';
                    onebyte_value = strtol(onebyte_str,&errptr,16);
                    virtualMemory[m_address] = onebyte_value;
                    m_address+=1;
                }
            } else if(input_record[0] == 'R') {
                strcpy(r_dictionary[1],estab[i]->csec_name); //reference number = 01 인 경우 control section name이 들어가야한다.
                for(int k = 0; k < strlen(input_record)-1; k+=8) {
                    strncpy(ref_num, input_record+1+k, 2);
                    ref_num[2] = '\0';
                    strncpy(r_symbol,input_record+3+k, 6);
                    r_symbol[6] = '\0';
                    //공백이 있는 경우 널값을 넣어준다.
                    for(int j = 0; j < 6; j++) {
                        if(r_symbol[j] == ' ' || r_symbol[j] == '\n')
                            r_symbol[j] = '\0';
                    }
                    int key = atoi(ref_num);
                    strcpy(r_dictionary[key],r_symbol);
                }
            }
        }
        if(input_record[0] == 'E') {
            if(input_record[1] != '\n') {
                strncpy(e_address_str,input_record+1,6);
                e_address_str[6] = '\0';
                e_address_value = strtol(e_address_str,&errptr,16);
                execaddr = csaddr + e_address_value;
            }
            csaddr += cslth;
        }
    }
}
void LoaderPass1(FILE *fp[], int file_num) {
    csaddr = progaddr;
    char *errptr;   //strtol에 사용되는 포인터
    char input_record[1000];    //프로그램별로 한 라인씩 저장한다.
    char csname[8];   //control section name을 저장
    char symbol[8];   //D record의 symbol 저장
    char address[8];   //D record에 지정되어있는 주소 저장.
    long hex_address;   //문자열로 저장되어있는 address를 hex_address로 16진수 변환하여 저장
    for(int i=0; i<file_num; i++) {
        //Header record를 읽는다.
        fgets(input_record,sizeof(input_record),fp[i]);
        cslth = strtol(input_record+13,&errptr,16);
        strncpy(csname,input_record+1,6);
        for(int j=0; j<6; j++) {
            if(csname[j] == ' ' || csname[j] == '\n')
                csname[j] = '\0';
        }
        if(HashESTAB(csname,"\0",0,i) == 1) {
            printf("ESTAB에 Control Section Name이 이미 존재합니다.\n");
            load_error_flag = 1;
            return;
        }//csname이 존재하는지 확인하고 이미 존재하는 경우 에러처리, 존재하지 않는 경우 ESTAB에 저장
        while(fgets(input_record,sizeof(input_record),fp[i]) != NULL) {
            if(input_record[0] == 'D') {
                for(int k = 0; k < strlen(input_record)/2; k += 12) {
                    strncpy(symbol,input_record+1+k,6);
                    for(int j=0; j<6; j++) {
                        if(symbol[j] == ' ' || symbol[j] == '\n')
                            symbol[j] = '\0';
                    }
                    strncpy(address,input_record+7+k,6);
                    address[6] = '\0';
                    hex_address = strtol(address,&errptr,16);
                    if(HashESTAB("\0", symbol, hex_address, i) == 1) {
                        printf("ESTAB에 Symbol이 이미 존재합니다.\n");
                        load_error_flag = 1;
                        return;
                    }
                    
                }
            }
        }
        csaddr += cslth;
    }
}
void LinkingLoader() {
    FILE *fp[3];
    char *garbage;  //strtok에 사용되는 문자열 임시저장변수
    char input_line[1000];  //첫 input복사
    char prog_name[3][100]; //각 file open을 위한 program name을 저장
    int file_num = 0;   //입력된 파일의 개수
    long total_length = 0;   //로드된 프로그램들의 길이의 총합
    address_value = 0;
    strcpy(input_line,instruction);
    garbage = strtok(input_line," \t");
    while(garbage != NULL) {
        garbage = strtok(NULL," \t");
        if(garbage == NULL)
            break;
        strcpy(prog_name[file_num],garbage);
        file_num++;
    }
    for(int i=0; i<file_num; i++) {
        fp[i] = fopen(prog_name[i], "r");
        if(fp[i] == NULL) {
            printf("파일명에 문제가 있습니다.\n");
            return;
        }
    }
    //ESTAB,REG 초기화 하고 Pass1 시작
    for(int i=0; i<3; i++) {
        estab[i]->address = 0;
        memset(estab[i]->csec_name,0,sizeof(estab[i]->csec_name));
        estab[i]->is_csec = 0;
        estab[i]->length = 0;
        estab[i]->next = NULL;
        memset(estab[i]->symbol,0,sizeof(estab[i]->symbol));
    }
    A = X = L = PC = B = S = T = 0;    //레지스터
    SW = '\0';
    LoaderPass1(fp,file_num);
    //파일포인터 위치를 처음으로 되돌린다.
    for(int i=0; i<file_num; i++) {
        rewind(fp[i]);
    }
    LoaderPass2(fp,file_num);
    //Loader의 실행이 성공적일 경우 loadmap 출력
    if(load_error_flag == 1) {
        load_error_flag = 0;
        return;
    }
    printf("control symbol address length\n");
    printf("section name\n");
    printf("--------------------------------\n");
    for(int i=0; i<file_num; i++) {
        ESTAB *print = estab[i];
        while(print != NULL) {
            if(strcmp(print->csec_name,"\0") != 0) {
                printf("%-7s%14.04lX   %04lX\n",print->csec_name,print->address,print->length);
                total_length += print->length;
            } else {
                printf("%14s   %04lX\n",print->symbol,print->address);
            }
            print = print->next;
        }
    }
    printf("--------------------------------\n");
    printf("           total length %04lX\n",total_length);
    SaveHistory();
}
void SetProgaddr() {
    char *errptr;
    int num_of_factor = CheckFactor();
    if(num_of_factor != 1) {
        //에러
        printf("인자에 문제가 있습니다\n");
        return;
    }
    progaddr = strtol(factor[0],&errptr,16);
    SaveHistory();
}
void InputLine() {
    int i = 0;
    while(1) {
        if(i > MAX_INPUT_SIZE) {
            printf("최대 입력 사이즈를 초과하였습니다. 다시 입력하십시오.\n");
            while(getchar() != '\n') {} //버퍼를 비워준다.
            memset(instruction,0,sizeof(instruction));
            i = 0;
            break;
        } else {
            scanf("%c",&instruction[i]);
        }
        if(instruction[i] == '\n') {
            instruction[i] = '\0';
            break;
        }
        i++;
    }
}
void FreeMemory() {
    Node *remove_node = NULL;
    HashNode *remove_hash_node = NULL;
    //history 구현에 사용된 메모리를 해제한다.
    if(head_history->next == NULL) {
        return;
    }
    while(head_history != NULL) {
        remove_node = head_history;
        head_history = head_history ->next;
        if(remove_node != NULL)
            free(remove_node);
        else {
            return;
        }
        remove_node = NULL;
    }
    //hashtable 구현에 사용된 메모리를 해제한다.
    for(int i = 0 ; i < 20; i++) {
        while(HashTable[i]->next != NULL) {
            remove_hash_node = HashTable[i];
            HashTable[i] = HashTable[i]->next;
            if(remove_hash_node != NULL)
                free(remove_hash_node);
            else
                return;
            remove_hash_node = NULL;
        }
    }
}
void ReadAndPrintFile() {
    /*
     파일이름(facotr[0]에 저장되어 있음)
     현재 디렉토리를 읽어온다.
     디렉토리를 탐색하여 입력받은 인자와 일치하는지 확인한다.
     존재하는 경우 -> 해당 파일을 읽고 출력한다.
     존재하지 않는 경우 -> 에러메시지 출력
     */
    int num_of_factor = CheckFactor();
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    FILE *fp = NULL;
    char line[1000];   //파일에 한 줄 입력 받기 위한 변수이다.
    if(num_of_factor == 1) {
        dir = opendir("."); //현재디렉토리를 연다.
        if(dir != NULL) {
            while((entry = readdir(dir)) != NULL) {
                if(strcmp(entry->d_name, factor[0]) == 0) {
                    fp = fopen(factor[0],"r");
                    if(fp == NULL) {
                        printf("%s를 불러오는데 문제가 있습니다.\n",factor[0]);
                        return;
                    }
                    while(fgets(line, sizeof(line), fp) != NULL){
                        fputs(line, stdout);
                    }
                    printf("\n");
                    SaveHistory();
                    return;
                }
            }
            closedir(dir);
            printf("현재 디렉토리에 해당 파일이 존재하지 않습니다\n");
            return;
        }
    } else {
        printf("파일명에 문제가 있습니다.\n");
        return;
    }
    
}
int FindSymbol(char *target) {
    for(int i = 25; i>=0; i--) {
        SymbolNode *find_node = SymbolTable[i];
        while(find_node != NULL) {
            if(strcmp(target,find_node->symbol) == 0) {
                return find_node->location;
            }
            find_node = find_node->next;
        }
    }
    return -1;   //찾지 못한 경우
}
void Assemble() {
    int num_of_factor = CheckFactor();  //인자의 개수를 저장한다. 인자에 문제가 있는 경우 -1이 저장.
    FILE *fp = NULL;    // .asm 파일에 대한 파일 포인터
    FILE *imfp = NULL;  // Pass1 과정의 intermediate file에 대한 파일포인터
    FILE *lstfp = NULL; // .lst 파일을 위한 파일포인터
    FILE *objfp = NULL; // .obj 파일을 위한 파일포인터
    char input[1000];    //한 줄을 입력받는 문자열이다
    char copy[1000];    //input이 복사된다.
    char component[4][100]; //component[0] : LABEL, componet[1] : OPCODE, componet[2] : OPERAND_1, component[3] : OPERAND_2
    char* strtok_Result = NULL; //strtok으로 구분한 문자를 저장할 변수
    char* error_ptr; //16진수 변환 중 에러가 발생할 때 반환할 포인터이다.
    long starting_address = 0;  //시작 주소를 저장하기 위한 변수
    long LOCCTR = 0;    //location counter를 저장하기 위한 변수
    int line_number = 5;    //line number를 저장하기 위한 변수
    int exist_flag = 0;     //SYMTAB에 label이 존재하는지 확인하기 위한 플래그이다.
    int program_size = 0;   //Pass1이 종료되면 프로그램 사이즈를 저장한다.
    char *mnemonic = NULL;  //OPTAB 탐색시 mnemonic을 저장하는 포인터이다.
    int increase = 0;   //increase 만큼 LOCCTR을 증가시킨다.
    char* lst_file_name;    // .lst 파일명 저장
    char* obj_file_name;    // .obj 파일명 저장
    char* extender;   //현재 들어온 파일의 확장자를 저장한다.
    int key = 0;    //HashTable의 key 저장
    int exist_opcode = 0;   //OPOCDE HashTable에 존재하는 경우 opcode값이, 없는 경우 -1이 저장된다.
    long operand_size = 0;  //OPERAND의 사이즈를 저장한다
    char space_printed[7];  //LOCCTR이 입력되지 않는 곳에 스페이스를 출력하기 위한 문자열.
    int is_END = 0; //END 유무를 체크한다.
    if(num_of_factor == 1) {
        //Pass 1
        fp = fopen(factor[0],"r");    //.asm 파일을 읽어온다.
        if(fp == NULL) {
            printf("해당 파일이 존재하지 않습니다.\n");
            return;
        }
        extender = (char*)malloc(sizeof(factor[0]));
        strcpy(extender, factor[0]);
        extender = strtok(extender,".");
        extender = strtok(NULL,".");    //확장자 저장
        //확장자가 asm파일이 아닌 경우 에러출력
        if(strcmp(extender,"asm") != 0) {
            printf(".asm 형식이 아닙니다.\n");
            return;
        }
        imfp = fopen("intermediate.txt", "wt"); //중간 파일 생성
        int i = 0;
        //차례대로 읽는다.
        while(fgets(input,sizeof(input),fp) != NULL) {
            if(input[strlen(input)-1] == '\n')
                input[strlen(input)-1] = '\0';
            strcpy(copy,input);
            memset(component,0,sizeof(component));
            //공백일 경우 intermediate file에 라인수만 입력해준다.
            if(input[0] == '\0') {
                fprintf(imfp,"%d\n",line_number);
                line_number += 5;
                continue;
            }
            if(input[0] == ' ' || input[0] == '\t') {
                strcpy(component[i], " ");
                i++;
            }
            
            strtok_Result = strtok(input," \t");
            if(strcmp(strtok_Result,".") == 0) {
                fprintf(imfp,"%-5d %s\n",line_number,copy);
                line_number += 5;
                i = 0;
                continue;
            }
            while(strtok_Result != NULL) {
                if(i>3) {
                    printf("%d : operand에 문제가 있습니다.\n",line_number);
                    return;
                }
                strcpy(component[i],strtok_Result);
                strtok_Result = strtok(NULL, " \t");
                if(strcmp(component[i],",") == 0) {
                    strcat(component[i-1],",");
                    strcpy(component[i],strtok_Result);
                    continue;
                }
                i++;
                
            }
            //주소가 FFFFF를 넘어가는 경우 에러처리
            if(LOCCTR > 0xFFFFF || LOCCTR < 0|| starting_address < 0) {
                printf("%d : 주소 범위를 초과하였습니다.(0~FFFFF)\n",line_number);
                return;
            }
            //주석 라인이 아닌 경우
            if(strcmp(component[0],".") != 0) {
                //LABEL이 존재한다면
                if(strcmp(component[0]," ") != 0) {
                    //symbole table에 이미 존재하는 경우 에러 출력 아닌 경우
                    if(strcmp(component[1],"START") != 0)
                        exist_flag = MakeSymbolTable(component[0], LOCCTR);
                    if(exist_flag == -1) {
                        printf("%d : %s 이 이미 SYMTAB에 존재합니다\n",line_number,component[0]);
                        return;
                    }
                }
                //Opcode Table에서 component[1](OPCODE) 탐색
                mnemonic = (char*)malloc(sizeof(component[1]));
                strcpy(mnemonic, component[1]);
                if(mnemonic[0] == '+') {    //format4인 경우 '+'를 제외한 문자열을 탐색한다.
                    mnemonic = strtok(mnemonic,"+");
                }
                key = HashKey(mnemonic);    //OPOCDE HashTable의 key값 저장
                exist_opcode = FindHash(key, mnemonic); //OPOCDE HashTable에 존재하는 경우 opcode값이, 없는 경우 -1이 저장된다.
                operand_size = strlen(component[2]);
                //Opcode Table에 존재하지 않는 경우 올바른 지시어인지 확인한다.
                if(exist_opcode == -1) {
                    if(strcmp(component[1],"START") == 0) {
                        //프로그램 중간에 START가 있을 경우 에러처리
                        if(line_number != 5) {
                            printf("%d : 프로그램 중간에 START가 올 수 없습니다.\n",line_number);
                            return;
                        }
                        for(int i = 0; i < strlen(component[2]); i++) {
                            if(('0' <= component[2][i] && component[2][i]<= '9') || ('A' <= component[2][i] && component[2][i] <= 'F')) {
                            }
                            else {
                                printf("%d : 시작 주소에는 16진수만 지정할 수 있습니다.\n",line_number);
                                return;
                            }
                        }
                        starting_address = strtol(component[2],&error_ptr,16);
                        LOCCTR = starting_address;
                        fprintf(imfp,"%-5d %04lX %-8s %-8s %-8s\n",line_number, LOCCTR, component[0], component[1], component[2]);
                        line_number += 5;
                        i = 0;
                        continue;
                    } else if(strcmp(component[1],"WORD") == 0) {
                        if(strcmp(component[3], "\0") != 0) {
                            printf("%d: WORD의 operand에 문제가 있습니다.\n",line_number);
                            return;
                        }
                        for(int i=0; i< operand_size; i++) {
                            if(!('0' <= component[2][i] && component[2][i] <='9')) {
                                printf("%d: WORD의 operand에 문제가 있습니다.\n",line_number);
                                return;
                            }
                        }
                        if(atoi(component[2]) > 0xFFFFFF || atoi(component[2]) < 0) {
                            printf("%d: WORD는 3byte 정수형 상수로서 (0~FFFFFF)를 초과할 수 없습니다.\n",line_number);
                            return;
                        }
                        if(strcmp(component[2],"\0") == 0 ) {
                            printf("%d : WORD의 OPERAND에 아무것도 입력되지 않았습니다.\n",line_number);
                            return;
                        }
                        increase = 3;
                    } else if(strcmp(component[1],"RESW") == 0) {
                        //component[2]에 10진수가 아닌 값이 있는 경우 에러.
                        for(int i=0; i< operand_size; i++) {
                            if(!('0' <= component[2][i] && component[2][i] <='9')) {
                                printf("%d: RESW의 operand에 문제가 있습니다.\n",line_number);
                                return;
                            }
                        }
                        if(strcmp(component[2],"\0") == 0 ) {
                            printf("%d : RESW의 OPERAND에 아무것도 입력되지 않았습니다.\n",line_number);
                            return;
                        }
                        if(strcmp(component[3], "\0") != 0) {
                            printf("%d: WORD의 operand에 문제가 있습니다.\n",line_number);
                            return;
                        }
                        increase = 3 * atoi(component[2]);
                    } else if(strcmp(component[1],"RESB") == 0) {
                        //component[2]에 10진수가 아닌 값이 있는 경우 에러.
                        for(int i=0; i< operand_size; i++) {
                            if(!('0' <= component[2][i] && component[2][i] <='9')) {
                                printf("%d: RESB의 operand에 문제가 있습니다.\n",line_number);
                                return;
                            }
                        }
                        if(strcmp(component[2],"\0") == 0 ) {
                            printf("%d : RESB의 OPERAND에 아무것도 입력되지 않았습니다.\n",line_number);
                            return;
                        }
                        if(strcmp(component[3], "\0") != 0) {
                            printf("%d: WORD의 operand에 문제가 있습니다.\n",line_number);
                            return;
                        }
                        increase = atoi(component[2]);
                    } else if(strcmp(component[1],"BYTE") == 0) {
                        //BYTE인 경우 입력된 문자열의 길이만큼 증가시켜야한다.
                        //C 로 시작하는 경우와 X로 시작하는 경우를 구분한다. 이 외에는 에러처리한다.
                        if(component[2][0] == 'C') {
                            //작은 따옴표 사이에 값이 들어가 있어야 올바른 입력이다.
                            if(component[2][1] == '\'' && component[2][operand_size-1] == '\'') {
                                //작은 따옴표를 제외한 길이만큼 증가시킨다.
                                if(operand_size-3 > 0x1E) {
                                    printf("%d : 최대 30글자까지 문자열 상수로 초기화 할 수 있습니다.\n",line_number);
                                    return;
                                }
                    
                                for(int k=2; k<operand_size-1; k++) {
                                    if(32 <= component[2][k] && component[2][k] <= 126)
                                        increase++;
                                    else {
                                        //입력할 수 없는 문자인 경우
                                        printf("%d: BYTE의 operand에 문제가 있습니다. 입력할 수 있는 아스키코드 범위 내의 값을 지켜야합니다.\n",line_number);
                                        return;
                                    }
                                }
                            } else {
                                //error
                                printf("%d: BYTE의 operand에 문제가 있습니다. ex)C'ABC'와 같은 형식을 지켜야합니다.\n",line_number);
                                return;
                            }
                        }
                        else if(component[2][0] == 'X') {
                            //작은 따옴표를 제외한 길이만큼 증가시킨다.
                            if(component[2][1] == '\'' && component[2][operand_size-1] == '\'') {
                                if(operand_size-3 > 0x1E) {
                                    printf("%d : 최대 30개의 바이트까지만 상수로 초기화 할 수 있습니다.\n",line_number);
                                    return;
                                }

                                for(int k=2; k<operand_size-1; k+=2) {
                                    if(('0' <= component[2][k] && component[2][k] <= '9') || ('A' <= component[2][k] && component[2][k] <= 'F') || ('a' <= component[2][k] && component[2][k] <= 'f')) {
                                        increase++;
                                    }
                                    else {
                                        //16진수의 표현이 아닌 경우 error
                                        printf("%d: BYTE의 operand에 문제가 있습니다. 16진수의형식을 지켜야합니다.\n",line_number);
                                        return;
                                    }
                                }
                            } else {
                                printf("%d: BYTE의 operand에 문제가 있습니다. ex)X'F1'와 같은 형식을 지켜야합니다.\n",line_number);
                                return;
                            }
                        } else {
                            printf("%d: BYTE의 operand에 문제가 있습니다. ex)C'ABC' 또는 X'ABC'와 같은 형식을 지켜야합니다.\n",line_number);
                            return;
                        }
                    } else if(strcmp(component[1],"END") == 0) {
                        //프로그램의 마지막인 경우 LOCCTR을 1 증가 시키고 시작주소와의 차이만큼을 프로그램 사이즈로 정한다. intermediate file에는 line number와 LOCCTR을 기록하지 않는다.
                        if(FindSymbol(component[2]) != starting_address) {
                            printf("%d : END의 OPERAND는 없거나 프로그램 내에서 처음 실행된 인스트럭션의 라벨만 올 수 있습니다\n",line_number);
                            return;
                        }
                        is_END++;
                        sprintf(space_printed, "%04lX",LOCCTR);
                        LOCCTR++;
                        program_size = (int)(LOCCTR - starting_address-1);
                        fprintf(imfp, "%-5d",line_number);
                        for(int i=0; i< strlen(space_printed)+11;i++) {
                            fprintf(imfp," ");
                        }
                        fprintf(imfp,"%-8s %s",component[1],component[2]);
                        break;
                    } else if(strcmp(component[1],"BASE") == 0) {
                        //BASE를 어떤 값으로 지정했다는 지시어
                        if(component[3][0] != 0) {
                            printf("%d : operand에 문제가 있습니다.\n",line_number);
                            return;
                        }
                        if(component[2][0]  == 0) {
                            printf("%d : operand에 BASE로 사용할 LABEL이 필요합니다.\n",line_number);
                            return;
                        }
                        if(strcmp(component[0]," ") != 0) {
                            printf("%d : BASE 가 있는 라인은 주소를 생성하지 않으므로 라벨을 가질 수 없습니다.\n",line_number);
                            return;
                        }
                        sprintf(space_printed, "%04lX",LOCCTR);
                        fprintf(imfp, "%-5d",line_number);
                        for(int i=0; i< strlen(space_printed)+2;i++) {
                            fprintf(imfp," ");
                        }
                        fprintf(imfp,"%-8s %s\n",component[1],component[2]);
                        line_number += 5;
                        i = 0;
                        continue;
                    } else {
                        printf("%d: 입력에 문제가 있습니다.\n",line_number);
                        return;
                    }
                }
                //Opcode Table에 존재하는 경우 해당 format size를 결정하여 LOCCTR을 증가한다.
                else {
                    //해당하는길이만큼 LOCCTR 증가
                    //opcode에 '+'가 붙어있는 경우만 format4
                    if(strcmp(target_hash->format,"1") == 0) {
                        //format 1이므로 1증가
                        increase++;
                    } else if(strcmp(target_hash->format,"2") == 0) {
                        //format 2이므로 2증가
                        increase += 2;
                    } else if(strcmp(target_hash->format,"3/4") == 0) {
                        if(component[1][0] == '+') {
                            //format 4이므로 4증가
                            increase += 4;
                        }
                        else {
                            //format 3이므로 3증가
                            increase += 3;
                        }
                    }
                }
                fprintf(imfp,"%-5d %04lX %-8s %-8s %s %s\n",line_number, LOCCTR, component[0], component[1], component[2], component[3]);
                
            }
            //intermediatae file에 line_number, LOCCTR, LABEL, OPCODE, OPERAND를 기록한다.
            line_number += 5;
            LOCCTR += increase;
            i = 0;
            increase = 0;
        }
    } else {
        printf("파일명에 문제가 있습니다.\n");
        return;
    }
    //END가 없는 경우 에러처리
    if(is_END != 1) {
        printf("asm파일에 END가 존재하지 않거나 중복선언되었습니다.\n");
        return;
    }
    fclose(imfp);
    free(strtok_Result);
    free(mnemonic);
    //Pass2
    //lst, obj 파일 생성
    lst_file_name = (char*)malloc(sizeof(factor[0]));
    obj_file_name = (char*)malloc(sizeof(factor[0]));
    strcpy(lst_file_name,factor[0]);
    lst_file_name = strtok(lst_file_name,".");
    strcat(lst_file_name,".lst");
    lstfp = fopen(lst_file_name,"wt");
    strcpy(obj_file_name,factor[0]);
    obj_file_name = strtok(obj_file_name,".");
    strcat(obj_file_name,".obj");
    objfp = fopen(obj_file_name, "wt");
    //read first input line (from intermediate file)
    imfp = fopen("intermediate.txt","r");
    ComponentInfo *compo = NULL;
    long im_line_size = 0; //intermediate.txt 한 라인의 길이를 저장하는 변수
    int E = 0;
    int objcode = 0;    //objcode를 계산하여 저장
    int max_size_record = 0x1E;
    int cur_size_record = 0;
    long PC = starting_address; //Program Counter이다.
    int label_locctr = 0;   //symbol table에서 찾은 값을 저장한다. symbol table에 없는 경우 -1이 저장된다.
    long BASE_locctr = 0;
    char buffer_objcode[100] = {0};
    char* buffer_modifi = (char*)calloc(0,sizeof(char)*10);
    char tmp_modifi[15] = {0};
    long length_locctr = 0;
    while(fgets(input,sizeof(input),imfp) != NULL) {
        //한 줄을 읽는다. 0,6,11,20,29 마다 새로운 인자들이 있음.
        im_line_size = strlen(input);
        compo = (ComponentInfo*)malloc(sizeof(ComponentInfo));
        memset(compo->line_number, 0, sizeof(compo->line_number));
        memset(compo->locctr, 0, sizeof(compo->locctr));
        memset(compo->label, 0, sizeof(compo->label));
        memset(compo->opcode, 0, sizeof(compo->opcode));
        memset(compo->operand_1, 0, sizeof(compo->operand_1));
        memset(compo->operand_2, 0, sizeof(compo->operand_2));
        
        for(int i=0; i< im_line_size; i++) {
            if(i == 0) {
                if(input[i] != '\n' && input[i] == ' ') {
                    strcpy(compo->line_number,"\0");
                    continue;
                }
                while(input[i] != '\n' &&input[i] != ' ') {
                    compo->line_number[i] = input[i];
                    i++;
                }
                compo->line_number[i] = '\0';
            } else if(i == 6){
                if(input[i] != '\n' && input[i] == ' ') {
                    strcpy(compo->locctr,"\0");
                    continue;
                }
                while(input[i] != '\n' && input[i] != ' ') {
                    compo->locctr[i-6] = input[i];
                    i++;
                }
                length_locctr = strlen(compo->locctr)+1;

                compo->locctr[i-6] = '\0';
            } else if(i == length_locctr + 6){   //label 처리
                if(input[i] != '\n' && input[i] == ' ') {
                    strcpy(compo->label,"\0");

                    continue;
                }
                while(input[i] != '\n' && input[i] != ' ') {
                    compo->label[i-length_locctr-6] = input[i];
                    i++;
                }
                compo->label[i-length_locctr-6] = '\0';
            } else if(i == length_locctr + 15) {  //opcode 처리
                if(input[i] != '\n' && input[i] == ' ') {
                    strcpy(compo->opcode,"\0");
                    continue;
                }
                while(input[i] != '\n' && input[i] != ' ') {
                    compo->opcode[i-length_locctr-15] = input[i];
                    i++;
                }
                compo->opcode[i-length_locctr-15] = '\0';
            } else if(i == length_locctr + 24) { //operand 처리
                if(input[i] == ' ') {
                    strcpy(compo->operand_1,"\0");
                    continue;
                }
                while(input[i] != '\n' && input[i] != ',' && i<im_line_size) {
                    if(input[i] == ' ')
                        break;
                    compo->operand_1[i-length_locctr-24] = input[i];
                    i++;
                }
                compo->operand_1[i-length_locctr-24] = '\0';
                while(i<im_line_size) {
                    if(input[i] == ' ' || input[i] == ',') {
                        i++;
                    }else {
                        break;
                    }
                }
                if(i<im_line_size){
                    int j = i;
                    while(input[i] != '\n' && input[i] != ' ' && i<im_line_size) {
                        compo->operand_2[i-j] = input[i];
                        i++;
                    }
                    compo->operand_2[i-j] = '\0';
                } else {
                    strcpy(compo->operand_2, "\0");
                }
            }
        }
        if(strcmp(compo->line_number, "5") == 0 && strcmp(compo->opcode,"START") != 0) {   //START가 없는 경우 현재 라인 그대로 입력하고 obj file에 임의로 지정한 이름을 넣는다.
            fprintf(objfp, "HNONAME%06lX%06X\n",starting_address,program_size);
        }
        if(strcmp(compo->opcode,"START") == 0 ) {
            //write listing line
            fprintf(lstfp, "%s",input);
            //write Header record to object program
            fprintf(objfp, "H%-6s%06lX%06X\n",compo->label,starting_address,program_size);
        } else if(strcmp(compo->opcode,"END") == 0) {
            //END의 OPERAND가 프로그램의 시작을 알리는 라벨이 아닌 경우 에러
            if(strcmp(compo->operand_1,"\0") != 0) {
                if(FindSymbol(compo->operand_1) != starting_address) {
                    printf("%d : END의 OPERAND는 없거나 프로그램 내에서 처음 실행된 인스트럭션의 라벨만 올 수 있습니다\n",line_number);
                    fclose(lstfp);
                    fclose(objfp);
                    fclose(fp);
                    remove(lst_file_name);
                    remove(obj_file_name);
                    return;
                }
            }
            fprintf(lstfp, "%s\t",input);
            fprintf(objfp,"%02X%s",cur_size_record,buffer_objcode);
            buffer_modifi[strlen(buffer_modifi)-1] = '\0';
            fprintf(objfp,"\n%s",buffer_modifi);
            free(buffer_modifi);
            fprintf(objfp, "\nE%06lX",starting_address);
            break;
        } else if(compo->locctr[0] == '.') {
            fprintf(lstfp, "%s",input);
            continue;
        } else {
            input[im_line_size-1] = '\0';
            fprintf(lstfp, "%s",input);
            //lst 파일에 objcode line 맞추기 위한 작업
            for(int i=0; i< 50 - im_line_size; i++) {
                fprintf(lstfp, " ");
            }
            
            mnemonic = (char*)malloc(sizeof(compo->opcode));
            strcpy(mnemonic, compo->opcode);
            //format 4인 경우이다.
            if(mnemonic[0] == '+') {
                mnemonic = strtok(mnemonic,"+");
                E = 1;
            }
            int key = HashKey(mnemonic);
            int exist_opcode = FindHash(key, mnemonic);
            long current_locctr = strtol(compo->locctr,&error_ptr,16);
            char buf_operand_1[100] = {0};
            char buf_non_optab[100] = {0};
            if(cur_size_record == 0 && strcmp(compo->opcode,"RESB") != 0 && strcmp(compo->opcode,"RESW") != 0 && strcmp(compo->label,"BASE") != 0 && compo->locctr[0] != '.') {
                fprintf(objfp, "T%06lX",current_locctr);
            } else if(strcmp(compo->label,"BASE") == 0) {
                //Obj code 생성안함
                BASE_locctr = FindSymbol(compo->opcode);
                if(BASE_locctr == -1) {
                    printf("%s : operand가 SYMTAB에 없습니다. \n",compo->line_number);
                    fclose(lstfp);
                    fclose(objfp);
                    fclose(fp);
                    remove(lst_file_name);
                    remove(obj_file_name);
                    return;
                }
            }
            if(exist_opcode == -1) {    //OPTAB에 없는 경우
                if(strcmp(compo->opcode,"BYTE") == 0) {
                    if(compo->operand_1[0] == 'C') {
                        for(int i=2; i < strlen(compo->operand_1)-1; i++) {
                            fprintf(lstfp,"%X",compo->operand_1[i]);
                            sprintf(buf_non_optab, "%X",compo->operand_1[i]);
                            strcat(buf_operand_1,buf_non_optab);
                            cur_size_record++;
                        }
                    } else if(compo->operand_1[0] == 'X') {
                        if((strlen(compo->operand_1)-3)%2 != 0) {
                            fprintf(lstfp,"0");
                            sprintf(buf_non_optab, "0");
                            strcat(buf_operand_1,buf_non_optab);
                        }
                        for(int i=2; i < strlen(compo->operand_1)-1; i++) {
                            fprintf(lstfp,"%c",compo->operand_1[i]);
                            sprintf(buf_non_optab, "%c",compo->operand_1[i]);
                            strcat(buf_operand_1,buf_non_optab);
                            if(i%2 == 0)
                                cur_size_record++;
                        }
                    }
                    if(cur_size_record > max_size_record) { //buffer의 사이즈, 현재까지 buffer에 입력된 objcode를 obj file에 기록하고 다음 Text record로 초기화한다.
                        fprintf(objfp, "%02lX%s\n",cur_size_record-(strlen(compo->operand_1)-3),buffer_objcode);
                        fprintf(objfp, "T%06lX",current_locctr);
                        memset(buffer_objcode,0,sizeof(buffer_objcode));
                        cur_size_record = (int)(strlen(compo->operand_1)-3);
                    }
                    strcat(buffer_objcode,buf_operand_1);
                }
                else if(strcmp(compo->opcode,"WORD") == 0) {
                    //operand의 10진수를 16진수로 바꿔줘야함.
                    fprintf(lstfp,"%06X",atoi(compo->operand_1));
                    sprintf(buf_operand_1,"%06X",atoi(compo->operand_1));
                    cur_size_record += 3;
                    if(cur_size_record > max_size_record) {
                        fprintf(objfp,"%02X%s\n",cur_size_record-3,buffer_objcode);
                        fprintf(objfp,"T%06lX",current_locctr);
                        memset(buffer_objcode, 0, sizeof(buffer_objcode));
                        cur_size_record = 3;
                    }
                    strcat(buffer_objcode,buf_operand_1);
                }
                else if(strcmp(compo->opcode,"RESB") == 0 || strcmp(compo->opcode,"RESW") == 0 ) {
                    //Obj code 생성안함
                    if(cur_size_record != 0) {
                        fprintf(objfp,"%02X%s\n",cur_size_record,buffer_objcode);
                        memset(buffer_objcode, 0, sizeof(buffer_objcode));
                        cur_size_record = 0;
                    }
                }
                else if(strcmp(compo->label,"BASE") != 0 && compo->locctr[0] == '.'){
                    printf("%s : 유효하지 않은 입력입니다. \n",compo->line_number);
                    fclose(lstfp);
                    fclose(objfp);
                    fclose(fp);
                    remove(lst_file_name);
                    remove(obj_file_name);
                    return;
                }
                fprintf(lstfp, "\n");
            } else {    //OPTAB에 있는 경우
                if(strcmp(compo->opcode,"RSUB") == 0) {
                    //RSUB의 경우 simple addressing이고 opcode값만 더해주면 된다.
                    objcode = target_hash->opcode + 3;
                    objcode = objcode << 16;
                    fprintf(lstfp, "%06X",objcode);
                    fprintf(lstfp, "\n");
                    sprintf(buf_operand_1, "%06X",objcode);
                    cur_size_record += 3;
                    if(cur_size_record > max_size_record) {
                        fprintf(objfp,"%02X%s\n",cur_size_record-3,buffer_objcode);
                        fprintf(objfp,"T%06lX",current_locctr);
                        memset(buffer_objcode, 0, sizeof(buffer_objcode));
                        cur_size_record = 3;
                    }
                    strcat(buffer_objcode,buf_operand_1);
                    objcode = 0;
                    continue;
                }
                if(strcmp(compo->operand_1,"\0") == 0) {
                    printf("%s : operand에 문제가 있습니다.\n",compo->line_number);
                    fclose(lstfp);
                    fclose(objfp);
                    fclose(fp);
                    remove(lst_file_name);
                    remove(obj_file_name);
                    return;
                }
                strtok_Result = (char*)malloc(sizeof(compo->operand_1));
                strcpy(strtok_Result,compo->operand_1);
                objcode = target_hash->opcode;
                if(compo->operand_1[0] == '#') {
                    //# -> immediate addressing n = 0, i = 1
                    strtok_Result = strtok(strtok_Result,"#");
                    objcode += 1;
                    if(strcmp(compo->operand_2,"\0") != 0) {
                        printf("%s : operand에 문제가 있습니다.\n",compo->line_number);
                        fclose(lstfp);
                        fclose(objfp);
                        fclose(fp);
                        remove(lst_file_name);
                        remove(obj_file_name);
                        return;
                    }
                } else if(compo->operand_1[0] == '@') {
                    //@ -> indirect addressing n = 1, i = 0
                    strtok_Result = strtok(strtok_Result,"@");
                    objcode += 2;
                } else {
                    //simple addressing n = 1, i = 1;
                    objcode += 3;
                }
                
                label_locctr = FindSymbol(strtok_Result);
                if(E == 1) {    //format 4
                    //3,4 형식의 2번째 Operand에는 "\0" 또는 X레지스터만 올 수 있다.
                    if(strcmp(compo->operand_2,"X") != 0 && strcmp(compo->operand_2,"\0") != 0) {
                        printf("%s : 유효하지 않은 입력입니다. 3/4형식의 2번째 operand는 X레지스터 외에 올 수 없습니다.\n",compo->line_number);
                        fclose(lstfp);
                        fclose(objfp);
                        fclose(fp);
                        remove(lst_file_name);
                        remove(obj_file_name);
                        return;
                    }
                    objcode = objcode << 4;
                    if(strcmp(compo->operand_2,"X") == 0) {
                        //x : 1
                        objcode += 8;
                    }
                    objcode += 1;   //xbpe(0001)
                    objcode = objcode << 20;
                    if(label_locctr != -1) {
                        objcode += label_locctr;
                    } else {
                        
                        if(compo->operand_1[0] == '#') {
                            objcode += atoi(strtok_Result);
                        }
                          else {
                              printf("%s : 유효하지 않은 입력입니다. SYMTAB에 없는 operand입니다.\n",compo->line_number);
                              fclose(lstfp);
                              fclose(objfp);
                              fclose(fp);
                              remove(lst_file_name);
                              remove(obj_file_name);
                              return;
                        }
                    }
                    if(compo->operand_1[0] != '#') {
                        memset(tmp_modifi,0,sizeof(tmp_modifi));
                        sprintf(tmp_modifi,"M%06lX05\n",current_locctr+1);
                        error_ptr = (char*)malloc(sizeof(char));
                        buffer_modifi = (char*)realloc(buffer_modifi, strlen(buffer_modifi)+10);
                        strcat(buffer_modifi,tmp_modifi);
                    }
                    E = 0;
                    PC = current_locctr +4;
                    cur_size_record += 4;
                    if(cur_size_record > max_size_record) {
                        fprintf(objfp,"%02X%s\n",cur_size_record-4,buffer_objcode);
                        fprintf(objfp,"T%06lX",current_locctr);
                        memset(buffer_objcode, 0, sizeof(buffer_objcode));
                        cur_size_record = 4;
                    }
                } else if(strcmp(target_hash->format,"3/4") == 0){    //format 3
                    if(strcmp(compo->operand_2,"X") != 0 && strcmp(compo->operand_2,"\0") != 0) {
                        printf("%s : 유효하지 않은 입력입니다. 3/4형식의 2번째 operand는 X레지스터 외에 올 수 없습니다.\n",compo->line_number);
                        fclose(lstfp);
                        fclose(objfp);
                        fclose(fp);
                        remove(lst_file_name);
                        remove(obj_file_name);
                        return;
                    }
                    PC = current_locctr + 3;
                    objcode = objcode << 4;
                    //X레지스터를 사용하였는가?
                    if(label_locctr != -1) {
                        if(strcmp(compo->operand_2,"X") == 0) {
                            //x : 1
                            objcode += 8;
                        }
                        long disp = label_locctr - PC;
                        //PC relative 로 표현 가능한가?
                        if(-2048 <=  disp && disp <= 2047) {
                            //b:0 p:1 e:0
                            objcode += 2;
                            if(disp < 0) {
                                disp = 0x1000 + disp;
                            }
                        } else if (disp <= 4095) {  //BASE relativ로 표현 가능한가?
                            //b:1 p:0 e:0
                            objcode += 4;
                            disp = label_locctr - BASE_locctr;
                        }
                        objcode = objcode << 12;
                        objcode += disp;
                    } else {
                        if(compo->operand_1[0] == '#') {
                            objcode = objcode << 12;
                            objcode += atoi(strtok_Result);
                        } else {
                            printf("%s : operand에 문제가 있습니다. SYMTAB에 없는 operand입니다.\n",compo->line_number);
                            fclose(lstfp);
                            fclose(objfp);
                            fclose(fp);
                            remove(lst_file_name);
                            remove(obj_file_name);
                            return;
                        }
                    }
                    cur_size_record += 3;
                    if(cur_size_record > max_size_record) {
                        fprintf(objfp,"%02X%s\n",cur_size_record-3,buffer_objcode);
                        fprintf(objfp,"T%06lX",current_locctr);
                        memset(buffer_objcode, 0, sizeof(buffer_objcode));
                        cur_size_record = 3;
                    }
                } else if(strcmp(target_hash->format,"2") == 0){    //format 2
                    PC = current_locctr + 2;
                    objcode = target_hash->opcode;
                    objcode = objcode << 4;
                    if(compo->operand_1[0] == '#' && strcmp(compo->operand_2,"\0") == 0) {
                        strtok_Result = (char*)malloc(sizeof(compo->operand_1));
                        strcpy(strtok_Result,compo->operand_1);
                        strtok_Result = strtok(strtok_Result,"#");
                        int value = atoi(strtok_Result);
                        if(value > 15) {
                            printf("%s : 2형식의 10진수 입력 범위는 0~15입니다.\n",compo->line_number);
                            fclose(lstfp);
                            fclose(objfp);
                            fclose(fp);
                            remove(lst_file_name);
                            remove(obj_file_name);
                            return;
                        }
                        objcode += value;
                    } else if(strcmp(compo->operand_1,"A") == 0) {
                        objcode += 0;
                    } else if(strcmp(compo->operand_1,"X") == 0) {
                        objcode += 1;
                    } else if(strcmp(compo->operand_1,"L") == 0) {
                        objcode += 2;
                    } else if(strcmp(compo->operand_1,"B") == 0) {
                        objcode += 3;
                    } else if(strcmp(compo->operand_1,"S") == 0) {
                        objcode += 4;
                    } else if(strcmp(compo->operand_1,"T") == 0) {
                        objcode += 5;
                    } else if(strcmp(compo->operand_1,"F") == 0) {
                        objcode += 6;
                    } else if(strcmp(compo->operand_1,"PC") == 0) {
                        objcode += 8;
                    } else if(strcmp(compo->operand_1,"SW") == 0) {
                        objcode += 9;
                    } else {
                            printf("%s : 유효하지 않은 입력입니다. 2형식의 1번째 operand는 상수, A,X,L,B,,S,T,F,PC,SW가 올 수 있습니다.\n",compo->line_number);
                            fclose(lstfp);
                            fclose(objfp);
                            fclose(fp);
                            remove(lst_file_name);
                            remove(obj_file_name);
                            return;
                    }
                        objcode = objcode << 4;
                        if(compo->operand_2[0] == '#') {
                            strtok_Result = (char*)malloc(sizeof(compo->operand_2));
                            strcpy(strtok_Result,compo->operand_2);
                            strtok_Result = strtok(strtok_Result,"#");
                            int value = atoi(strtok_Result);
                            if(value > 15) {
                                printf("%s : 2형식의 10진수 입력 범위는 0~15입니다.\n",compo->line_number);
                                fclose(lstfp);
                                fclose(objfp);
                                fclose(fp);
                                remove(lst_file_name);
                                remove(obj_file_name);
                                return;
                            }
                            objcode += value;
                        } else if(strcmp(compo->operand_2,"A") == 0) {
                            objcode += 0;
                        } else if(strcmp(compo->operand_2,"X") == 0) {
                            objcode += 1;
                        } else if(strcmp(compo->operand_2,"L") == 0) {
                            objcode += 2;
                        } else if(strcmp(compo->operand_2,"B") == 0) {
                            objcode += 3;
                        } else if(strcmp(compo->operand_2,"S") == 0) {
                            objcode += 4;
                        } else if(strcmp(compo->operand_2,"T") == 0) {
                            objcode += 5;
                        } else if(strcmp(compo->operand_2,"F") == 0) {
                            objcode += 6;
                        } else if(strcmp(compo->operand_2,"PC") == 0) {
                            objcode += 8;
                        } else if(strcmp(compo->operand_2,"SW") == 0) {
                            objcode += 9;
                        } else if(strcmp(compo->operand_2,"\0")!= 0) {
                            printf("%s : 유효하지 않은 입력입니다. 2형식의 2번째 operand는 상수, A,X,L,B,,S,T,F,PC,SW가 올 수 있습니다.\n",compo->line_number);
                            fclose(lstfp);
                            fclose(objfp);
                            fclose(fp);
                            remove(lst_file_name);
                            remove(obj_file_name);
                            return;
                        }
                    fprintf(lstfp, "%04X",objcode);
                    fprintf(lstfp, "\n");
                    cur_size_record += 2;
                    if(cur_size_record > max_size_record) {
                        fprintf(objfp,"%02X%s\n",cur_size_record-2,buffer_objcode);
                        fprintf(objfp,"T%06lX",current_locctr);
                        memset(buffer_objcode, 0, sizeof(buffer_objcode));
                        cur_size_record = 2;
                    }
                    sprintf(buf_operand_1, "%04X",objcode);
                    strcat(buffer_objcode,buf_operand_1);
                    objcode = 0;
                    //free(compo);
                    continue;
                } else if(strcmp(target_hash->format, "1") == 0){    //format 1;
                    if(strcmp(compo->operand_1,"\0") != 0) {
                        printf("%s : 1형식에는 operand가 존재할 수 없습니다.\n",compo->line_number);
                        fclose(lstfp);
                        fclose(objfp);
                        fclose(fp);
                        remove(lst_file_name);
                        remove(obj_file_name);
                        return;
                    }
                    PC = current_locctr + 1;
                    cur_size_record += 1;
                    if(cur_size_record > max_size_record) {
                        fprintf(objfp,"%02X%s\n",cur_size_record-1,buffer_objcode);
                        fprintf(objfp,"T%02lX",current_locctr);
                        memset(buffer_objcode, 0, sizeof(buffer_objcode));
                        cur_size_record = 1;
                    }
                    sprintf(buf_operand_1, "%02X",objcode);
                }
                fprintf(lstfp, "%06X",objcode);
                fprintf(lstfp, "\n");
                
                sprintf(buf_operand_1, "%06X",objcode);
                strcat(buffer_objcode,buf_operand_1);
            }
            objcode = 0;
            free(compo);
        }
    }
    if(lstfp != NULL)
        fclose(lstfp);
    if(objfp != NULL)
        fclose(objfp);
    printf("[%s], [%s]\n",lst_file_name,obj_file_name);
    SaveHistory();
    fclose(fp);
    CopySymbolTable();
}
int MakeSymbolTable(char *label, long location) {
    int key = label[0] - 65;
    SymbolNode *input_node;
    SymbolNode *present;
    SymbolNode *prev;
    present = SymbolTable[key];
    input_node = (SymbolNode*)malloc(sizeof(SymbolNode));
    input_node->location = (int)location;
    strcpy(input_node->symbol,label);
    //label이 SYMTAB에 존재하는지 확인
    if(SymbolTable[key] == NULL) {
        present = SymbolTable[key] = input_node;
        SymbolTable[key]->next = NULL;
    } else {
        while(present != NULL) {
            if(strcmp(present->symbol,label) == 0) //이미 존재하는 경우
                return -1;
            present = present->next;
        }
        prev = present = SymbolTable[key];
        while(present != NULL) {
            if(strcmp(present->symbol,label) > 0) {
                prev = present;
                present = present->next;
            } else if (strcmp(present->symbol,label) < 0) {
                if(prev == present) { //제일 처음에 삽입해야하는 경우이다.
                    SymbolTable[key] = input_node;
                    SymbolTable[key]->next = present;
                    return 1;
                }
                else {
                    prev->next = input_node;
                    input_node->next = present;
                    return 1;
                }
            }
        }
        prev->next = input_node;
        input_node->next = NULL;
        return 1;
    }
    return 0;
}
void PrintSymbolTable() {
    for(int i = 25; i>=0; i--) {
        SymbolNode *print_node = Recent_SymbolTable[i];
        if(print_node == NULL) {
            continue;
        }
        while(print_node != NULL) {
            printf("\t%s\t%04X",print_node->symbol,print_node->location);
            if(i != 0)
                printf("\n");
            print_node = print_node->next;
        }
    }
    SaveHistory();
}
void CopySymbolTable() {
    FreeSymbolTable(Recent_SymbolTable);
    for(int i = 25; i>=0; i--) {
        SymbolNode *copy_node = SymbolTable[i];
        SymbolNode *present = Recent_SymbolTable[i];
        if(copy_node == NULL) {
            continue;
        }
        while(copy_node != NULL) {
            SymbolNode *new_node = (SymbolNode*)malloc(sizeof(SymbolNode));
            strcpy(new_node->symbol,copy_node->symbol);
            new_node->location = copy_node->location;
            new_node->next = NULL;
            if(Recent_SymbolTable[i] == NULL) {
               present = Recent_SymbolTable[i] = new_node;
            } else {
                present->next = new_node;
                present = present->next;
            }
            copy_node = copy_node->next;
        }
    }
}
void FreeSymbolTable(SymbolNode** Table) {
    for(int i = 25; i>=0; i--) {
        SymbolNode *remove_node = Table[i];
        if(remove_node == NULL) {
            continue;
        } else {
            while(Table[i] != NULL) {
                free(remove_node);
                Table[i] = Table[i]->next;
                remove_node = Table[i];
            }
        }
    }
}
int main(void) {
    FILE *fp = NULL;
    int opcode,key;
    char input_mnemonic[7],input_format[5];
    //"opcode.txt"를 읽은 후 HashTable을 만든다.
    fp = fopen("opcode.txt","r");
    if(fp == NULL){
        printf("opcode.txt을 불러오는데 문제가 있습니다. 프로그램을 다시 시작하십시오.\n");
        exit(0);
    }
    while(0 < fscanf(fp, "%X %s %s",&opcode,input_mnemonic,input_format)) {
        key = HashKey(input_mnemonic);
        HashInput(key,input_mnemonic,opcode,input_format);
    }
    fclose(fp);
    memset(virtualMemory,0,sizeof(virtualMemory));
    memset(bp,0,sizeof(bp));
    head_history = (Node*)malloc(sizeof(Node));
    head_history->next = NULL;
    present = head_history;
    address = -1;
    for(int i=0; i<3; i++) {
        estab[i] = (ESTAB*)malloc(sizeof(ESTAB));
    }
    while(1) {
        printf("sicsim>");
        InputLine();    //instruction에 모든 입력을 저장한다.
        if(instruction[0] != '\0')
            ImplementInstruction();
        memset(instruction,0,sizeof(instruction));
    }
    return 0;
}
