#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<string.h>
#include<fcntl.h>
#include<time.h>
#include<sys/time.h>

#define VMRSS_LINE 22

unsigned int get_proc_mem(unsigned int pid){
	char file_name[64]={0};
	sprintf(file_name,"/proc/%d/status",pid);
	
    FILE *fd;
	char line_buff[512]={0};
	fd =fopen(file_name,"r");
	if(fd==NULL) return 0;
	char name[64];
	int vmrss;
	for (int i=0; i<VMRSS_LINE-1;i++){
		fgets(line_buff,sizeof(line_buff),fd);
	}
	
	fgets(line_buff,sizeof(line_buff),fd);
	sscanf(line_buff,"%s %d",name,&vmrss);
	fclose(fd);
 
	return vmrss;//程序使用的物理内存
}

int main(){
    int pid=getpid();
    printf("pid=%d\n",pid);
    struct timeval tv;
    long startTime=0;
    long endTime=0;
    gettimeofday(&tv,NULL);
    startTime=tv.tv_sec*1000000+tv.tv_usec;//tv_sec=s;tv_usec=us;
    printf("startTime:%ld\n",startTime);

    //int fd=open("test.txt",O_RDONLY);
    int fd=open("/home/kjr/OSbook/bible10.txt",O_RDONLY);
    if(fd==-1){
        printf("can't open the file");
        return 1;
    }

    struct stat sb;
    if(fstat(fd,&sb)==-1) printf("fstat error!");
    char *mmapped;
    if((mmapped=mmap(NULL,sb.st_size,PROT_READ,MAP_SHARED,fd,0))==(void *)-1) printf("mmapped error!");
    close(fd);
    //printf("%s\n",mmapped);
    
    char find_str[100]={"\0"};
    printf("input string to find:\n");
    scanf("%s",find_str);
    int find_str_length=strlen(find_str);
    int line=1;
    int count=0;
    int len=strlen(mmapped);
    //printf("%d\n",len);
    if(strstr(mmapped,find_str) || strstr(mmapped,"\n")){
        for(int i=0;i<len;i++){
            if(mmapped[i]=='\n') line++;
            if(strstr(mmapped,find_str)){
                if(mmapped[i]==find_str[0]){
                    int j=1;
                    for(;j<find_str_length;j++){
                        if(mmapped[i+j]!=find_str[j] || (i+j)>=len) break;
                    }
                    if(j==find_str_length){
                        count++;
                        printf("the line of the find_str:%d\n",line);
                    }
                }
                else continue;
            }
            else continue;
        }
    }
    if(count==0) printf("can't find the string\n");
    else printf("the number of the find_str in the file is:%d\n",count);

    gettimeofday(&tv,NULL);
    endTime=tv.tv_sec*1000000+tv.tv_usec;
    printf("endTime:%ld\n",endTime);
    printf("used time is %ld us\n",(endTime-startTime));

    printf("used memory=%d kB\n",get_proc_mem(pid));
    return 0;
}