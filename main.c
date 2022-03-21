#include <stdio.h>
#include<string.h>
#include<unistd.h>

void substring(char* s,int len)
{
  int i=0;
  int j=0;
  char temp[len-5];
  while (i<len)
  {
    if(i<=4)
    {
      i++;
      continue;
    }
    temp[j]=s[i];
    j++;
    i++;
  }
  printf("%s",temp);
}


void tcpConnection(char *s,int len)
{
  int i=0;
  int j=0;
  char buff[len-9];
  while (i<len)
  {
    if(i<=8)
    {
      i++;
      continue;
    }
    buff[j]=s[i];
    j++;
    i++;
  }
}

int main()
{
  char cmd[100];
  char dir[100];
  int bool=0;
  while(1)
  {
    getcwd(dir,100);
    printf("%s yes master-",dir);
    fgets (cmd, 30, stdin);
    printf("Command entered: %s", cmd); 
     if (strncmp(cmd,"EXIT",4)==0)
    {
      break;
    }
    if (strncmp(cmd,"ECHO",4)==0 && bool==0)
    {
      substring(cmd,strlen(cmd));
    }
    if(strncmp(cmd,"TCP PORT",8)==0)
    {
      bool=1;
    }
  }
  return 0;
}
