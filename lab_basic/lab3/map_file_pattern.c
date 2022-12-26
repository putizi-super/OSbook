#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<string.h>
#include<fcntl.h>
#include<time.h>
#include<sys/time.h>
#include<sys/types.h>
#include<regex.h>

int main(){
    int fd=open("new.txt",O_RDONLY);
    //int fd=open("/home/kjr/OSbook/bible10.txt",O_RDONLY);
    if(fd==-1){
        printf("can't open the file");
        return 1;
    }

    struct stat sb;
    if(fstat(fd,&sb)==-1) printf("fstat error!");
    char *mmapped;
    if((mmapped=mmap(NULL,sb.st_size,PROT_READ,MAP_SHARED,fd,0))==(void *)-1) printf("mmapped error!");
    close(fd);

    int status=0;
    regmatch_t pmatch[1];
    regex_t reg;
    int count=0;
    char pattern[]="^From ilug-admin@linux.ie.*Aug.*";//查找ilug-admin@linux.ie在八月份发送的邮件
     
    status=regcomp(&reg,pattern,REG_EXTENDED|REG_NEWLINE);
    if(status!=0){
        printf("compile error!\n");
        return -1;
    }

    char output[1024]={"\0"};
    while(1){
        status=regexec(&reg,mmapped,1,pmatch,0);
        if(status==0){
            count++;
            strncpy(output,mmapped+pmatch[0].rm_so,pmatch[0].rm_eo-pmatch[0].rm_so);
            printf("matched:%s\n",output);
            mmapped += pmatch[0].rm_eo;
        }
        else break;
    }
    regfree(&reg);
    printf("the number of matched string:%d\n",count);
    return 0;
}
