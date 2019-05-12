#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include<pthread.h>
#include<sys/wait.h>
#include<time.h>

#define key 10007
int status=0;
int status_push=0;
int status_rm=0;
int thr_bit[key];
int cari(){
    int i;
    for(i=0; i<key; i++) if(thr_bit[i]==0) return i;
}

typedef struct node{
	char str[1000];
    int t;
	struct node *kanan;
	struct node *kiri;
}node;

typedef struct root{
	node *rt;
}root;

root list[key];
int hashing(char s[]){
	int i,len=strlen(s),hash=0;
	for(i=0; i<len; i++)
		hash+=s[i];
	hash%=key;
	
	return hash;
}

int push(char s[], int t){
    while(status_push);
    status_push=1;
	int hash=hashing(s);
	
	// alloc memory
	if(list[hash].rt==NULL)
	{
		node* node_tmp=(node*)malloc(sizeof(node));
		strcpy(node_tmp->str,s);
        node_tmp->t=t;
        thr_bit[t]=1;
		node_tmp->kanan=NULL;
		node_tmp->kiri=NULL;
		list[hash].rt=node_tmp;
        status_push=0;
		return 1;
	}
	else
	{
		node* tmp=list[hash].rt;
		while(1)
		{
			if(!strcmp(tmp->str,s)) 
            {
                status_push=0;
                return 0;
            }
			if(tmp->kanan==NULL)
			{
				node* node_tmp=(node*)malloc(sizeof(node));
				strcpy(node_tmp->str,s);
                node_tmp->t=t;
                thr_bit[t]=1;
				node_tmp->kiri=tmp;
				node_tmp->kanan=NULL;
				tmp->kanan=node_tmp;
                status_push=0;
				return 1;
			}
			tmp=tmp->kanan;
		}
	}
}

void rm(char s[]){
	int hash=hashing(s);
	node* tmp=list[hash].rt;
	list[hash].rt=tmp->kanan;
	
	while(strcmp(tmp->str,s)) tmp=tmp->kanan;
	if(tmp->kanan!=NULL) tmp->kanan->kiri=tmp->kiri;
	if(tmp->kiri!=NULL) tmp->kiri->kanan=tmp->kanan;
    thr_bit[tmp->t]=0;
	
	free(tmp);
}

void *print_message_function( void *ptr );

int same(char s[], int arr[])
{
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  
  if(s[2]!='*' && s[4]!='*')
  {
      char tp; 
      tp=s[2];
      s[2]='*';

      if( ( arr[0]==timeinfo->tm_min || s[0]=='*') &&
        ( arr[1]==timeinfo->tm_hour || s[1]=='*') &&
        ( arr[2]==timeinfo->tm_mday || s[2]=='*') &&
        ( arr[3]==timeinfo->tm_mon+1 || s[3]=='*') &&
        ( arr[4]==timeinfo->tm_wday || s[4]=='*') ) return 1;

      s[2]=tp;
      tp=s[4];
      s[4]='*';

      if( ( arr[0]==timeinfo->tm_min || s[0]=='*') &&
        ( arr[1]==timeinfo->tm_hour || s[1]=='*') &&
        ( arr[2]==timeinfo->tm_mday || s[2]=='*') &&
        ( arr[3]==timeinfo->tm_mon+1 || s[3]=='*') &&
        ( arr[4]==timeinfo->tm_wday || s[4]=='*') ) return 1;
  }
else
{
  if( ( arr[0]==timeinfo->tm_min || s[0]=='*') &&
  ( arr[1]==timeinfo->tm_hour || s[1]=='*') &&
  ( arr[2]==timeinfo->tm_mday || s[2]=='*') &&
  ( arr[3]==timeinfo->tm_mon+1 || s[3]=='*') &&
  ( arr[4]==timeinfo->tm_wday || s[4]=='*') ) return 1;
}

  return 0;
}

int main() 
{
    pthread_t thr[100];
    memset(thr_bit,0,sizeof(thr_bit));
    int flag=1;
    struct stat stats;
    char modif[1000];
    char modif_tmp[1000];
    char message[1001];

    // set NULL list
    int i;
    for(i=0; i<key; i++)
    list[i].rt==NULL;

  pid_t pid, sid;
  pid = fork();
  if (pid < 0) { exit(EXIT_FAILURE); }
  if (pid > 0) { exit(EXIT_SUCCESS); }
  umask(0);
  sid = setsid();
  if (sid < 0) { exit(EXIT_FAILURE); }
  if ((chdir("/")) < 0) { exit(EXIT_FAILURE); }

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  while(1) 
  {
    stat("/home/mrx/crontab.data", &stats);
    strftime(modif_tmp, 20, "%d-%m-%y-%H-%M-%S", localtime(&(stats.st_ctime)));
    if(strcmp(modif,modif_tmp))
    {
        FILE *fptrr;
        fptrr = fopen("/home/mrx/crontab.data","r");
        while(fgets(message,1000,fptrr) != NULL)
        {
            if(message[0] != 10)
            {
                message[strlen(message)-1]='\0';
                int idx=cari();
                if(push(message,idx))
                {
                    int iret;
                    status=1;
                    pthread_create( &thr[idx], NULL, print_message_function, (void*) message);
                }
            }
            while(status);
        }
        fclose(fptrr);
        strcpy(modif,modif_tmp);
    }
    sleep(1);
  }
  
  return 0;
}

void *print_message_function( void *ptr )
{
    char *mess;
    mess = (char *) ptr;
    char message_tmp[1001];
    strcpy(message_tmp,mess);
    status=0;
    char mass[1000];
    strcpy(mass,message_tmp);
    int flag=0;
    int f=1;

    int i,j=0;
    char c[6];
    int arr[5];
    memset(arr,0,sizeof(arr));

    for(i=0; i<5; i++)
    {
        while(message_tmp[j]==' ') j++;
        do{
        arr[i]*=10;
        arr[i]+=(message_tmp[j]-'0');
        c[i]=message_tmp[j];
        j++;
        }while(message_tmp[j]!=' ');
    }
    j++;
    c[5]='\0';
    memmove(mass,mass+j,strlen(mass)-j+1);

    while(1)
    {
        if(!same(c,arr)) f=1;
        else if(f)
         {
            f=0;
            FILE *fptr;
            char messag[1000];
            fptr = fopen("/home/mrx/crontab.data","r");
            while(fgets(messag,1000,fptr) != NULL)
            {
                if(messag[0] != 10)
                {
                    messag[strlen(messag)-1]='\0';
                    if(!strcmp(messag,message_tmp))
                    {
                        flag=1;
                        break;
                    }
                }
            }
            fclose(fptr);

            if(flag)
            {
                system(mass);
                flag=0;
                sleep(60);
            }
            else
            {
                while(status_rm);
                status_rm=1;
                rm(message_tmp);
                status_rm=0;
                break;
            }
        }
        sleep(1);
    }

}
