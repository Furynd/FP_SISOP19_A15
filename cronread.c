#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include<pthread.h>

pthread_t tid;

typedef struct argstruct
{
  short minute;
  short hour;
  short daym;
  short month;
  short dayw;
  int hash;
  char *cmd;
}argstr;

void * cronjob( void *args){
  argstr *arg = (argstr *)args;
  while(1){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);  
    if(tm.tm_sec==0)break;
    sleep(1);
  }
  while(1)
  {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    // sprintf(folder, "%d:%d:%d-%d:%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min);
    if(arg->minute == -1 || arg->minute == tm.tm_min)
      if(arg->hour == -1 || arg->hour == tm.tm_hour)
        if((arg->daym == tm.tm_mday && arg->dayw == -1) || (arg->dayw == tm.tm_wday && arg->daym == -1)
            || (arg->daym == -1 && arg->dayw == -1) || (arg->daym == tm.tm_mday && arg->dayw == tm.tm_wday))
          if(arg->month == -1 || arg->month == (tm.tm_mon+1))
            system(arg->cmd);
    sleep(60);
  }
}

short GetInput(FILE *pFile){
    short r=0;
    char c;
    while(1){
        c = fgetc(pFile);
        if(c == '*')return -1;
        if(c==' '||c=='\n')continue;
        else break;
    }
    r= c-'0';
    while(1){
        c = fgetc(pFile);
        if(c>='0' && c <= '9') r=10*r+c-'0';
        else break;
    }
    return r;
}

int main(int argc, char *argv[] )  {
    argstr args;
    FILE *pFile = fopen("/home/furynd/sisop/fp/FP_SISOP19_A15/crontab.data", "r");
    if(pFile == NULL) {
        perror("fopen");
        printf("can't open file\n");
        return 0;
    }
    size_t len = 255;
    args.minute = GetInput(pFile);
    args.hour = GetInput(pFile);
    args.daym = GetInput(pFile);
    args.month = GetInput(pFile);
    args.dayw = GetInput(pFile);
    args.dayw %= 7;
    args.cmd = malloc(sizeof(char) * len);
    fgets(args.cmd ,len, pFile);

    printf("%d %d %d %d %d %s \n",args.minute, args.hour, args.daym, args.month, args.dayw, args.cmd);

    if(pthread_create(&(tid), NULL, cronjob, (void*)&args)){
        printf("can't create thread\n");
        exit(0);
    }
    pthread_join(tid, NULL);
    
    return 0;
}