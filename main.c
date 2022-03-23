#include <stdio.h>
#include<string.h>
#include<unistd.h> 
#include <time.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>  
#include <netinet/tcp.h>
#include <errno.h> 
#include<stdlib.h>
#include<sys/wait.h>    
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>    
#include<dirent.h>    
#define SERVER_IP_ADDRESS "127.0.0.1" //IPv4 address of server
#define PORT_NUM 5060    
#define SIZE 65536 //2^16  

int sizeOfFile(char *name)
{
    FILE *file1;
    int count=0;
    char c=' ';
    if((file1=fopen(name,"rb"))==NULL)
    {
        return -1;
    }

    while(fread(&c,sizeof(c),1,file1)==1)
    {
        count+=1;
    }
    fclose(file1);
    return count;
}

int copy(char*src,char*dest)
{
    int count=sizeOfFile(src);
    if (count==-1)
    {
      printf("There is no file named %s\n",src);
      return -1;
    }
    char* data=(char*)malloc(sizeof(char)*count);
    memset(data,0,sizeof(data));
    FILE *file1;
    if((file1=fopen(src,"rb"))==NULL)
    {
        return -1;
    }
    fread(data,count,1,file1);
    FILE *file2;
    if((file2=fopen("temp2","wb"))==NULL)
    {
        return -1;
    }
    fwrite(data,count,1,file2);
    fclose(file1);
    fclose(file2);
    free(data);
}

int check_len(char*cmd,int len,int start,int end)
{
  int i=0;
  int ans=0;
  int numbOfSpace=0;
  while (i<len)
  {
    if(cmd[i]==' ' || cmd[i]=='\0')
    {
      numbOfSpace++;
      i++;
    }
    if(numbOfSpace<start)
    {
      i++;
      continue;
    }
    if(numbOfSpace==end)
    {
      break;
    }
    ans++;
    i++;
  }
  return ans-1;
  
}

void sendToCopy(char* cmd,int len)
{
  int len1=check_len(cmd,len,1,2)+1;
  char* src=(char*)malloc(sizeof(char)*len1);
  memset(src,0,sizeof(src));
  int i=0;
  int count=0;
  int j1=0;
  int j2=0;
  int bool1=0;
  while(i<len || j1<len1)
  {
    if(cmd[i]=='\0')
    {
      break;
    }
    if(cmd[i]==' ')
    {
      if(count<2)
      {
        count++;
      }
      i++;
      continue;
    }
    if (count==1)
    {
      bool1=1;
    }
    if(count==2)
    {
      bool1=0;
    }
    if(bool1==1)
    {
      src[j1]=cmd[i];
      j1++;
    }
    i++;
  }
  src[j1]='\0';
  cmd=cmd+len1+6;
  copy(src,cmd);
  free(src);
}

void substring(char* s,int len)
{
  s+=5;
  printf("%s",s);
}

void del(char*cmd,int len)
{
  int i=0;
  int j=0;
  int len1=check_len(cmd,len,1,2);
  char *temp=(char*)malloc(sizeof(char)*len1);
  memset(temp,0,len1);
  while (j<len1)
  {
    if(i<=6)
    {
      i++;
      continue;
    }
    temp[j]=cmd[i];
    j++;
    i++;
  }
  temp[j]='\0';
  int b=unlink(temp);
  if(b<0)
  {
    printf("The file does not exist or is open");
  }
  free(temp);
}

int tcpConnection()
{
    int sock = 0;
    struct sockaddr_in server_a; //sa is a server address
    bzero(&server_a, sizeof(server_a)); //Initializes the socket address
    server_a.sin_family = AF_INET;      // ipv4
    server_a.sin_port = htons(PORT_NUM);
        //create an endpoint for communication
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) //SOCK_STREM-TCP
    {
      printf("\n Error: in socket creation \n"); // if did not open soket you get this error
      return -1;
    }
    if (connect(sock, (struct sockaddr *)&server_a, sizeof(server_a)) < 0)
    {
      printf("Failed: connect failed, Error code : %d \n", errno);// if the connect is faild you get this error
      return -1;
    }
      printf("Connected to server\n"); // else you get that connected is work 
      return sock;
}

void sendm(char* s,int len,int sock)
{
  s+=5;
  int bytes_s = send(sock, s, len, 0); 
  if (bytes_s == -1)
  {
    printf("Error: in sending file: %d", errno);
  }
  else if (bytes_s < len) 
  {
    printf("Sent just %d bytes from the requisite %d\n", bytes_s, len); 
  }
  else if (bytes_s == 0)
  {
    printf("the TCP has closed before the send \n");
  }
}

int main()
{
  char cmd[1024];
  char dir[1024];
  int bool=0;
  int sock=0;
  getcwd(dir,1024);
  while(1)
  {
    printf("%s yes master-",dir);
    memset(cmd,0,strlen(cmd));
    fgets (cmd, 1024, stdin);
    printf("Command entered: %s", cmd); 
     if (strncmp(cmd,"EXIT",4)==0)
    {
      break;
    }
    else if (strncmp(cmd,"ECHO",4)==0)
    {
      if(bool==0)
      {
        substring(cmd,strlen(cmd));
      }
      else
      {
        sendm(cmd,strlen(cmd),sock);
      }
    }
    else if(strncmp(cmd,"TCP PORT",8)==0)
    {
      bool=1;
      sock=tcpConnection();
    }
     else if(strncmp(cmd,"LOCAL",5)==0 && bool==1)
    {
        bool=0;
        close(sock);
    }
    else if(strncmp(cmd, "DIR", 3) == 0)
    {
      DIR *dir;
      struct dirent *d_entp;
      dir = opendir(".");
      if (dir)
      {
        while ((d_entp = readdir(dir)) != NULL)
        {
          printf("%s\n", d_entp->d_name);
        }
        closedir(dir);
      }
      else
      {
        printf("No Doc");
      }
    }
    else if(strncmp(cmd,"COPY",4)==0)
    {
      sendToCopy(cmd,sizeof(cmd));
    }
    else if(strncmp(cmd,"DELETE",6)==0) 
    {
      del(cmd,sizeof(cmd)); //unlink is a system call
    }
    else
    {
       int a=system(cmd); //system is a libary function in c
    }
  }
  return 0;
}
