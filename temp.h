#define MAX_MEMORY_SIZE 1048576
#define MAX_INPUT_SIZE 100
unsigned char virtualMemory[MAX_MEMORY_SIZE];    //1Mbyte(2^20 = 1048576 = 16*65536) 할당
char instruction[MAX_INPUT_SIZE];  //한 라인 입력시 저장 위해 사용
char factor[3][MAX_INPUT_SIZE];    //인자에 대한 정보 저장.
long address;         //dump할 때 내부 메모리 저장을 위한 변수
int is_opcode = 0;    //opcode 명령어일 경우 표시를 해주기 위한 변수.
int is_type_or_assemble = 0;    //type or assemble 명령어일 경우 표시를 해주기 위한 변수.
int idx_command_start = 0;  //instruction에서 인자를 제외한 순수 명령어의 시작 위치가 저장된다.
int idx_command_end = 0;    //instruction에서 인자를 제외한 순수 명령어의 마지막 위치가 저장된다.
long progaddr = 0;   //porgaddr 실행시 주소가 저장된다.
long csaddr = 0; //ESTAB구성 시 각 섹션별 시작주소가 갱신된다.
long cslth = 0; //Header record에서 프로그램의 길이를 저장한다.
long execaddr = 0;  //loader  pass2에서 다음 섹션으로 점프하기 위한 주소를 저장한다.
char bp[1000][8];   //breakpoint를 문자열로 저장한다.
int bp_idx = 0; //breakpoint의 인덱스
int run_bp_idx = 0; //run에서 사용되는 breakpoint의 인덱스
long address_value = 0;   //run에서 사용되는 가상메모리 주소
long A = 0, X = 0, L = 0, PC = 0, B = 0, S = 0,T = 0;    //레지스터
char SW;
int end_flag = 0;   //프로그램의 마지막 수행인지 체크하는 플래그
int load_error_flag = 0; //로드 중 에러가 발생했는지 체크하는 플래그
//ESTAB 구조체이다.
typedef struct ESTAB{
    int is_csec;    //is_csec = 0 : symbol name이 들어온 경우, is_csec = 1 : control section name이 들어온 경
    char csec_name[7];  //각 컨트롤 섹션의 이름
    char symbol[7];    //각 컨트롤 섹션에서 define한 symbol
    long address;    //컨트롤 섹션 및 symbol의 주소
    long length;     //컨트롤 섹션의 길이
    struct ESTAB* next;
}ESTAB;
//history의 linked list 구현을 위한 구조체
typedef struct Node{
    char name_instruction[MAX_INPUT_SIZE];
    struct Node *next;
}Node;
//hastable에 opcode에 관한 데이터를 저장하기 위한 구조체
typedef struct HashNode{
    int opcode;
    char format[5];
    char mnemonic[7];
    struct HashNode* next;
}HashNode;
//symbol table에 symbol과 location에 관한 데이터를 저장하기 위한 구조체
typedef struct SymbolNode {
    char symbol[10];
    int location;
    struct SymbolNode* next;
}SymbolNode;
//intermediate file로 부터 인자들을 저장하는 구조체이다.
typedef struct ComponentInfo {
    char line_number[10];
    char locctr[10];
    char label[10];
    char opcode[10];
    char operand_1[100];
    char operand_2[10];
}ComponentInfo;
ESTAB *estab[3];
//key = 0: 첫번째로 로드된 프로그램
//key = 1: 두번째로 로드된 프로그램
//key = 2: 세번째로 로드된 프로그램
Node *head_history; //history의 제일 처음을 가르키는 포인터
Node *present;  //history의 가장 최근 노드를 가르키는 포인터
HashNode *HashTable[20];    //사이즈가 20인 hashtable
HashNode *target_hash;
SymbolNode *SymbolTable[26];    //알파벳순으로 저장하기 위하여 사이즈를 26으로 설정
SymbolNode *Recent_SymbolTable[26];
//인자가 없는 명령어 리스트이다.
char list_single_instruction[12][11] = {"help", "h", "dir", "d", "quit", "q", "history", "hi", "reset", "opcodelist", "symbol","run"};
//인자가 있는 명령어 리스트이다.
char list_plural_instruction[12][10] = {"dump","du","edit","e","fill","f","opcode","type","assemble","progaddr","loader","bp"};
/************************************************************
 *ExecuteInstruction
 *opcode에 대응하는 기능 수행
 ************************************************************/
void ExecuteInstruction(long opcode, unsigned char *update_memory, long imm_value, long TA, int E);
/************************************************************
 *ReturnRegisterAddress
 *Register의 실제 주소 반환
 ************************************************************/
long *ReturnRegisterAddress(char R);
/************************************************************
 *GetXBPE
 *xbpe값을 분석하여 저장
 ************************************************************/
void GetXBPE(long half_byte_value, int *xbpe);
/************************************************************
 *Run
 *메모리에 로드된 프로그램을 수행
 ************************************************************/
void Run(void);
/************************************************************
 *Debug
 *bp관련 기능을 수행한다.
 *bp address
 *bp clear
 *bp
 ************************************************************/
void Debug(void);
/************************************************************
 *SetProgaddr
 *loader 또는 run 명령어를 수행할 때 시작하는 주소를 지정한다.
 *sicsim이 시작되면 default로 progaddr는 0x00 주소로 지정된다.
 ************************************************************/
void SetProgaddr(void);
/************************************************************
 *LoaderPass2
 *LodaerPass2을 진행하는 함수이다.
 ************************************************************/
void LoaderPass2(FILE *fp[3], int file_num);
/************************************************************
 *LoaderPass1
 *LodaerPass1을 진행하는 함수이다.
 ************************************************************/
void LoaderPass1(FILE *fp[3], int file_num);
/************************************************************
 *LinkingLodaer
 *loader 명령어에 대한 기능을 수행하는 함수이다.
 *최대 3개의 obj 파일을 읽어 linking 수행 후 가상 메모리에 결과를 기록한다
 *실행이 성공적으로 끝나면 load map(ESTAB)을 화면에 출력한다
 *pass1 또는 pass2에서 에러가 발생할 경우 에러 내용이 화면에 출력된다.
 ************************************************************/
void LinkingLoader(void);
/************************************************************
 *SearchESTAB
 *ESTAB에서 find_symbol이 존재하는지 확인
 *존재할 경우 해당 노드를 반환한다.
 ************************************************************/
ESTAB* SearchESTAB(char find_symbol[], int file_num);
/************************************************************
 *HashESTAB
 *ESTAB을 구성하는 함수이다.
 *csname이나 symbol이 존재하는지 확인하여 존재할 경우 1 return
 *존재하지 않는 경우 ESTAB에 해당 정보를 삽입.
 ************************************************************/
int HashESTAB(char csname[], char symbol[], long indicated_address,int file_num);
/************************************************************
 *SaveHistory
 *head_hisory에 수행한 명령을 추가한다.
 ************************************************************/
void SaveHistory(void);
/************************************************************
 *PrintHistory
 *현재까지 사용한 명령어를 출력한다.
 *가장 최근에 사용한 명령어가 최하단에 출력된다.
 ************************************************************/
void PrintHistory(void);
/************************************************************
 *PrintListOfInstruction
 *shell에서 실행 가능한 모든 명령어 리스트를 출력한다.
 ************************************************************/
void PrintListOfInstruction(void);
/************************************************************
 *PrintFileDirectory
 *현재 디렉토리에 있는 파일 혹은 다른 디렉토리들을 출력한다.
 *실행 파일일 경우 이름 옆에 '*' 표시한다.
 *디렉토리일 경우 이름 옆에 '/' 표시한다
 ************************************************************/
void PrintFileDirectory(void);
/************************************************************
 *CheckFactor
 *명령어의 인자의 유효성을 판단한다.
 *유효한 인자일 경우 '인자의 개수'를 리턴한다.
 *유효하지 않은 인자일 경우 '-1'을 리턴한다.
 ************************************************************/
int CheckFactor(void);
/************************************************************
 *Dump
 *할당되어 있는 메모리를 출력한다.
 *인자가 없는 경우 10라인을 출력한다
 *start만 있는 경우 start부터 160개를 출력한다.
 *start와 end 모두 있는 경우 start부터 end까지의 내용을 출력한다.
 ************************************************************/
void Dump(void);
/************************************************************
 *Edit
 *입력한 address번지의 메모리 값을 value에 지정한 값으로 변경한다.
 ************************************************************/
void Edit(void);
/************************************************************
 *Fill
 *start번지부터 end번지까지의 메모리 값을 value에 지정한 값으로 변경한다.
 ************************************************************/
void Fill(void);
/************************************************************
 *Reset
 *메모리의 모든 값을 0으로 초기화한다.
 ************************************************************/
void Reset(void);
/************************************************************
 *HashKey
 *input_mnemonic에 대한 HashTable에서의 키 값을 생성하여 반환한다.
 *input_mnemonic의 첫 글자와 마지막 글자의 합을 20으로 나눈 나머지가 키 값이다.
 ************************************************************/
int HashKey(char *input_mnemonic);
/************************************************************
 *HashInput
 *opcode.txt에 있는 정보를 HashTable에 삽입하는 함수이다.
 ************************************************************/
void HashInput(int key, char *mnemonic, int value, char *format);
/************************************************************
 *FindHash
 *target으로 들어온 mnemonic에 대한 opcode value를 리턴한다
 ************************************************************/
int FindHash(int find_key, char *target);
/************************************************************
 *Opcode
 *mnemonic을 입력하면 해당 opcode를 출력한다
 ************************************************************/
void Opcode(void);
/************************************************************
 *PrintOpcodeList
 *opcode에 대한 HashTable에 저장된 정보를 출력한다.
 ************************************************************/
void PrintOpcodeList(void);
/************************************************************
 *CheckCorrectInstruction
 *입력된 instruction을 분석하여 올바른 명령어인 경우 inst_call에 해당 명령어를 저장하는 함수이다.
 ************************************************************/
void CheckCorrectInstruction(char* inst_call);
/************************************************************
 *ImplementInstruction
 *inst_call에 저장된 명령어에 대응하는 함수를 호출한다
 ************************************************************/
void ImplementInstruction(void);
/************************************************************
 *InputLine
 *MAX_INPUT_SIZE-1까지 문자열을 입력받는다
 *MAX_INPUT_SIZE (101)에는 '\0'이 들어간다
 ************************************************************/
void InputLine(void);
/************************************************************
 *FreeMemory
 *quit기능을 실행할 경우 프로그램 종료 전 동적 메모리 할당 한 부분을 free 해준다.
 ************************************************************/
void FreeMemory(void);
/************************************************************
 *ReadAndPrintFile
 *입력받은 파일명에 대하여 현재 디렉토리에 존재하는지 찾고 파일의 내용을 출력한다.
 ************************************************************/
void ReadAndPrintFile(void);
/************************************************************
 *Assemble
 *입력 받은 .asm 파일로 리스팅 파일(.lst)과 오브젝트 파일(.obj)을 생성한다.
 *
 ************************************************************/
void Assemble(void);
/************************************************************
 *MakeSymbolTable
 *label과 location에 대한 테이블을 만드는 함수이다.
 *label table은 사전역순으로 정렬되어있다.
 *label의 첫글자-'A'에 대한 값을 key값으로 설정한다.(0~25)
 *동일한 symbol이 존재하는 경우 -1을 리턴한다
 ************************************************************/
int MakeSymbolTable(char *label, long location);
/************************************************************
 *PrintSymbolTable
 *assemble 과정 중에 생성된 symbol table을 화면에 출력한다.
 ************************************************************/
void PrintSymbolTable(void);
/************************************************************
 *FindSymbol
 *target에 해당하는 symbol을 찾아 존재하는 경우 symbol의 location을
 *반환한다.
 *존재하지 않는 경우 -1을 반환한다.
 ************************************************************/
int FindSymbol(char *target);
/************************************************************
 *FreeSymbolTable
 *SymbolTable에 할당된 메모리를 free한다.
 ************************************************************/
void FreeSymbolTable(SymbolNode **Table);
/************************************************************
CopySymbolTable
*assemble에 성공한 경우 생성된 SymbolTable을 Recent_SymbolTable에
*복사한다.
************************************************************/
void CopySymbolTable(void);
