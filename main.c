#include <stdio.h>
#include<string.h>
#include<unistd.h> 
#include <time.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>  
#include <netinet/tcp.h>
#include <errno.h> 
               
#define SERVER_IP_ADDRESS "127.0.0.1" //IPv4 address of server
#define PORT_NUM 5060    
#define SIZE 65536 //2^16  

void substring(char* s,int len)
{
  int i=0;
  int j=0;
  char temp[len-4];
  while (i<len)
  {
    if(i<4)
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
  char cmd[100];
  char dir[100];
  int bool=0;
  int sock=0;
  getcwd(dir,100);
  while(1)
  {
    printf("%s yes master-",dir);
    memset(cmd,0,strlen(cmd));
    fgets (cmd, 30, stdin);
    printf("Command entered: %s", cmd); 
     if (strncmp(cmd,"EXIT",4)==0)
    {
      if(bool==1)
      {
        sendm("CLOSE CONNECTION",17,sock);
        close(sock);
      }
      break;
    }
    if (strncmp(cmd,"ECHO",4)==0)
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
    if(strncmp(cmd,"TCP PORT",8)==0)
    {
      bool=1;
      sock=tcpConnection();
    }
    if(strncmp(cmd,"LOCAL",5)==0 && bool==1)
    {
        bool=0;
        close(sock);
        getcwd(dir,100);
    }

  }
  return 0;
}
