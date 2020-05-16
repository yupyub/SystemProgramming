//*********************************************************************
//		My ls Ver 0.1.0
//2007   Made By Paek Jae-Dong
//E-mail: pjd@kw.ac.kr
//Blog: rabe.egloos.com
//*********************************************************************


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
//#include <term.h>

#define PATH_LENGTH 	200

//옵션에대한 값을 지정
#define FLAG_ALL 	0x01
#define FLAG_INODE 	0x02
#define FLAG_LONG 	0x04
#define FLAG_RECUR	0x08
#define FILEINFO 	struct file_info

//파일길이를 지정
#define FILENAME_SIZE	256

struct file_info{
	char filename[FILENAME_SIZE+1];
	unsigned long inode;
	unsigned long mode;
	unsigned long nlink;
	unsigned long uid;
	unsigned long gid;
	unsigned long size;
	//마지막 수정시간 출력
	time_t atime;  
	//이 파일이 차지하는 block수
	unsigned long blocks;
	struct file_info *next;
};


void seek_dir(char *dir,int opt);	//디렉토리를 순회하는 함수
void print_long(FILEINFO *list_head,int opt);	//-l옵션이 있을때 화면에 파일정보를 출력함

//-l옵션이 있을때 파일정보를 링크드리스트에 저장하는 함수
void save_long(FILEINFO **list_head,struct stat *cur_stat,struct dirent *cur_dir,int opt);	

void print_normal(FILEINFO *list_head,int opt);	//-l 옵션이 없을때 화면에 파일정보를 출력하는 함수

//-l 옵션이 없을때 파일정보를 링크드리스트에 저장하는 함수
void save_normal(FILEINFO **list_head,struct stat *cur_stat,struct dirent *cur_dir,int opt);

void free_list(FILEINFO *list_head);	//링크드리스트의 메모리를 해제하는 함수

void sort_list(FILEINFO *list_head);	//링크드리스트에 저장된 자료를 파일이름순으로 정렬하는 함수
int long_len(unsigned long num);	//unsigned long 값을 입력받아 자리수를 리턴하는 함수

int main(int argc,char* argv[])
{
	int i,j;
	int opt=0;	//옵션을 저장하는 변수
	char path[PATH_LENGTH]="./"; // 실행시 path옵션을 안주었을때의 디폴트 경로값


	for(i=1;i<argc;i++)
	{
		for(j=1;argv[i][j] != '\0';j++)
		{
			if(argv[i][0] != '-')	//옵션이 아니고 path 정보일때
			{
				strncpy(path,argv[i],PATH_LENGTH-1);
				break;
			}
			switch(argv[i][j])	//실제 ls에서는 각 옵션의 대소문자를 구분함
			{
				case 'a':
					opt |= FLAG_ALL;
					break;
				case 'l':
					opt |= FLAG_LONG;
					break;
				case 'i':
					opt |= FLAG_INODE;
					break;
				case 'R':	
					opt |= FLAG_RECUR;
					break;
			}
		}
	}

	if(path[strlen(path)-1] != '/')
	{
		sprintf(path+strlen(path),"/");
	}
	printf("%c[1;31m",27);  //색변경
	printf("\ndirectory path:");
	seek_dir(path,opt);		//디렉토리를 순회함


	printf("%c[0m\n",27);  //색변경
	return 0;
}

int long_len(unsigned long num)	//화면에 정렬하여 출력하기위하여 칸수를 계산
{
	int nCount=0;
	while( num%10 || num/10 ){
		num /=10;
		nCount++;
	}

	return nCount;
}

void free_list(FILEINFO *list_head){
	FILEINFO *tmp_list;

	while(list_head != NULL){     //메모리 해제하기
		tmp_list = list_head;
		list_head = list_head->next;
		free(tmp_list);
	}
}

//링크드리스트에 들어있는자료를 파일이름순으로 정렬
void sort_list(FILEINFO *list_head){	
	FILEINFO *tmp_list_left;
	FILEINFO *tmp_list_right;

	FILEINFO tmp_list;
	FILEINFO *tmp_listp;


	if( list_head == NULL )
		return ;

	if( list_head->next == NULL)
		return ;

	tmp_list_left = list_head;
	tmp_list_right = list_head->next;


	while( tmp_list_left->next != NULL){

		while(tmp_list_right != NULL){

			//두개의 리스트를 서로 exchange하는 코드
			if( strcmp(tmp_list_left->filename,tmp_list_right->filename) >0 ){	
				memcpy(&tmp_list , tmp_list_left,sizeof(FILEINFO));
				memcpy(tmp_list_left , tmp_list_right,sizeof(FILEINFO));
				memcpy(tmp_list_right , &tmp_list,sizeof(FILEINFO));
				tmp_listp = tmp_list_left->next;
				tmp_list_left->next = tmp_list_right->next;
				tmp_list_right->next = tmp_listp;
			}

			tmp_list_right = tmp_list_right->next;
		}
		tmp_list_left = tmp_list_left->next;
		tmp_list_right = tmp_list_left->next;
	}

}

void seek_dir(char *dir,int opt)
{
	DIR *dp;
	struct dirent *entry;
	struct stat tmp_stat;
	FILEINFO *tmp_list;
	FILEINFO *list_head;

	list_head = NULL;

	printf("%c[1;31m",27);	//색변경
	printf("%s\n",dir);	//경로를 출력

	if((dp = opendir(dir)) == NULL){	//디렉토리 스트림을 Open함
		fprintf(stderr,"directory open error: %s\n",dir);
		return;
	}

	chdir(dir);

	while((entry = readdir(dp)) != NULL){	//디렉토리정보를 한개씩 읽어옴
		lstat(entry->d_name, &tmp_stat);
		if( opt & FLAG_LONG)	//옵션에 따라 파일정보를 저장하는 방식을 달리함
			save_long(&list_head,&tmp_stat,entry,opt);
		else
			save_normal(&list_head,&tmp_stat,entry,opt);
	}


	sort_list(list_head);	//읽어온 자료를 정렬함


	if( opt & FLAG_LONG)	//옵션에 따라 파일정보를 출력하는 방식을 달리함
		print_long(list_head,opt);
	else
		print_normal(list_head,opt);


	tmp_list = list_head;


	while( tmp_list != NULL) {
		if(S_ISDIR(tmp_list->mode)){

			// '.' 과 '..'은 재귀 에서 배제함
			if( strcmp(".",tmp_list->filename) == 0 || strcmp("..",tmp_list->filename) == 0 ){
				tmp_list = tmp_list->next;
				continue;
			}
			if( opt & FLAG_RECUR){          //재귀 옵션이 있을때만 실행

				printf("%c[1;31m",27);  //색변경
				printf("\n\ndirectory path:%s",dir);

				seek_dir(tmp_list->filename,opt);
			}
		}

		tmp_list = tmp_list->next;

	}
	chdir("..");

	closedir(dp);
	free_list(list_head);	//파일정보가 저장된 메모리를 해제함
}
void save_long(FILEINFO **list_head,struct stat *cur_stat,struct dirent *cur_dir,int opt)
{
	FILEINFO *cur_list=(*list_head);

	if( *list_head != NULL)
		while( cur_list->next != NULL)
			cur_list = cur_list->next;

	if( cur_dir->d_name[0] == '.' )
		if( !(opt & FLAG_ALL) )
			return; //all옵션이 없으면 .으로 시작하는 파일은 스킵힌다. 


	if( (*list_head) == NULL){   //리스트의 맨처음일때
		cur_list = (FILEINFO *)malloc(sizeof(FILEINFO));
		cur_list->next = NULL;
		*list_head = cur_list;
	}else{
		cur_list->next = (FILEINFO *)malloc(sizeof(FILEINFO));
		cur_list = cur_list->next;
		cur_list->next = NULL;
	}


	cur_list->inode = cur_stat->st_ino;
	cur_list->mode = cur_stat->st_mode;
	strcpy(cur_list->filename ,cur_dir->d_name);

	//hard-link 수
	cur_list->nlink = cur_stat->st_nlink;

	//User ID  
	cur_list->uid = cur_stat->st_uid;

	//Group ID
	cur_list->gid = cur_stat->st_gid;

	//File Size 
	cur_list->size = cur_stat->st_size;

	//마지막 수정시간 
	cur_list->atime = cur_stat->st_atime;   //시간값을 분석하여 구조체에 저장

	//이 파일이 차지하는 block수
	cur_list->blocks = cur_stat->st_blocks;

}               

void print_long(FILEINFO *list_head,int opt)
{
	FILEINFO *cur_list;
	unsigned long tmp_perm;
	struct tm *tm_ptr;
	int i;

	cur_list = list_head;

	while(cur_list != NULL){
		tmp_perm = cur_list->mode;


		//File Type에따라서 출력할 색을 달리함
		if(S_ISREG(cur_list->mode)){
			if(cur_list->mode & 01001001)	//실행파일인지 검사
				printf("%c[1;32m",27);
			else
				printf("%c[0m",27);

			printf("REG  ");
		}else if(S_ISDIR(cur_list->mode)){
			printf("%c[1;34m",27);
			printf("DIR  ");
		}else if(S_ISCHR(cur_list->mode)){
			printf("%c[1;37m",27);
			printf("CHR  ");
		}else if(S_ISBLK(cur_list->mode)){
			printf("%c[1;33m",27);
			printf("BLK  ");
		}else if(S_ISFIFO(cur_list->mode)){
			printf("%c[1;37m",27);		
			printf("FIFO ");
		}else if(S_ISLNK(cur_list->mode)){
			printf("%c[1;36m",27);
			printf("LNK  ");
		}else if(S_ISSOCK(cur_list->mode)){
			printf("%c[1;35m",27);
			printf("SOCK ");
		}

		//30->기본색 밝은 회색
		//31->밝은 빨강
		//32->실행파일 녹색
		//33-> 노랑
		//34->디렉토리 밝은파랑
		//35->연보라
		//36-> 밝은 CYAN 링크
		//37->밝은 흰색

		if(opt & FLAG_INODE)
			printf("%u ",(unsigned int)cur_list->inode);	

		//Permission 출력
		for(i=0;i<3;i++)
		{
			if(tmp_perm & S_IRUSR)
				printf("r");
			else
				printf("-");

			if(tmp_perm & S_IWUSR)
				printf("w");
			else
				printf("-");

			if(tmp_perm & S_IXUSR)
				printf("x");
			else
				printf("-");

			tmp_perm <<=3;
		}

		//hard-link 수 출력
		printf(" %2u",(unsigned int)cur_list->nlink);

		//User ID출력
		printf(" %5u",(unsigned int)cur_list->uid);

		//Group ID출력
		printf(" %5u",(unsigned int)cur_list->gid);

		//File Size 출력
		printf(" %12u",(unsigned int)cur_list->size);

		//마지막 수정시간 출력
		tm_ptr = gmtime(&cur_list->atime);	//시간값을 분석하여 구조체에 저장
		printf(" %02d/%02d/%2d %02d:%02d",tm_ptr->tm_year%100,tm_ptr->tm_mon,tm_ptr->tm_mday,tm_ptr->tm_hour,tm_ptr->tm_min);

		//이 파일이 차지하는 block수
		printf("%5u",(unsigned int)cur_list->blocks);

		//파일이름 출력
		printf(" %s",cur_list->filename);


		printf("\n");
		cur_list = cur_list->next;
	}
}

void save_normal(FILEINFO **list_head,struct stat *cur_stat,struct dirent *cur_dir,int opt)
{
	FILEINFO *cur_list=*list_head;

	if( *list_head != NULL)
		while( cur_list->next != NULL)
			cur_list = cur_list->next;


	if( cur_dir->d_name[0] == '.')
		if( !(opt & FLAG_ALL) )
			return; //all옵션이 없으면 .과 ..은 스킵한다.


	if( (*list_head) == NULL){	//리스트의 맨처음일때
		cur_list = (FILEINFO *)malloc(sizeof(FILEINFO));
		cur_list->next = NULL;
		*list_head = cur_list;
	}else{
		cur_list->next = (FILEINFO *)malloc(sizeof(FILEINFO));
		cur_list = cur_list->next;
		cur_list->next = NULL;
	}

	cur_list->inode	= cur_stat->st_ino;
	cur_list->mode = cur_stat->st_mode;
	strcpy(cur_list->filename ,cur_dir->d_name);


}

void print_normal(FILEINFO *list_head,int opt)
{
	FILEINFO *cur_list;
	int i,j;
	int col_length_name[15];	//정렬을위해 칸 너비를 저장하는 배열
	int col_length_inode[15];
	int col_num=0;
	int nCount;
	int index;
	int term_x,term_y;	//터미널의 크기를 읽어오는 함수

	for(i=0;i<15;i++){
		col_length_name[i] = 0;
		col_length_inode[i] = 0;
	}


	//터미널 화면의 크기 정보를 얻어옴
	//setupterm(NULL,fileno(stdout),(int *)0);
	term_y = 100;//tigetnum("lines");	//현재 터미널의 크기를 얻어온다.
	term_x = 50;//tigetnum("cols");


	for(j=15;j>0;j--){
		cur_list = list_head;
		index  =0;
		nCount = 0;

		//파일 정보 출력시 세로정렬을 위한 최적의 칸 너비를 계산하는  코드부분
		while(cur_list != NULL){	
			if( col_length_name[index%j] < strlen(cur_list->filename) )
				col_length_name[index%j] = strlen(cur_list->filename);

			if( opt & FLAG_INODE){
				if( col_length_inode[index%j] < long_len(cur_list->inode) )
					col_length_inode[index%j] =long_len(cur_list->inode);
			}

			nCount = 0;
			for(i=0;i<j;i++){
				nCount += col_length_name[i]+2;
				if( opt & FLAG_INODE)
					nCount += col_length_inode[i]+1;
			}

			if(term_x < nCount)	
				break;

			nCount = 0;

			index ++;
			index = index %j;
			cur_list = cur_list->next;
		}
		if(cur_list == NULL){
			col_num = j;
			break;
		}

	}

	cur_list = list_head;
	index = 0;

	while(cur_list != NULL){
		if(S_ISREG(cur_list->mode)){
			if(cur_list->mode & 01001001) //실행파일인지 검사
				printf("%c[1;32m",27);
			else
				printf("%c[0m",27);

		}else if(S_ISDIR(cur_list->mode)){
			printf("%c[1;34m",27);
		}else if(S_ISCHR(cur_list->mode)){
			printf("%c[1;37m",27);
		}else if(S_ISBLK(cur_list->mode)){
			printf("%c[1;33m",27);
		}else if(S_ISFIFO(cur_list->mode)){
			printf("%c[1;37m",27);
		}else if(S_ISLNK(cur_list->mode)){
			printf("%c[1;36m",27);
		}else if(S_ISSOCK(cur_list->mode)){ 
			printf("%c[1;35m",27);
		}

		if( opt & FLAG_INODE )
			printf(" %*u",col_length_inode[index],(unsigned int)cur_list->inode);

		printf(" %*s ",col_length_name[index],cur_list->filename);
		cur_list = cur_list->next;
		if( index == col_num -1)
			printf("\n");
		index ++;
		index = index % col_num;

	}
}

