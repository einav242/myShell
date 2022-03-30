#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
 
#define PORT_NUM 5060
#define SIZE 65536 //2^16
#define NUM 256

static int number = 0; 
static double time_amount = 0;
 

void substring(char *s, int len)
{
  s += 5;
  printf("%s", s);
}

void dir1(char *cmd, int len) //this function shows all the files in the folder
{
  DIR *dir;
  struct dirent *d_entp;
  dir = opendir(".");//Open a directory stream on NAME "."
  if (dir) 
  {
    while ((d_entp = readdir(dir)) != NULL) 
    {
      printf("%s\n", d_entp->d_name);// print file 
    }
    closedir(dir);//Close the directory stream dir
  }
  else // if there are no files in the folder
  {
    printf("No Doc");
  }
}

int check_len(char *cmd, int len, int start, int end)
{
  int i = 0;
  int ans = 0;
  int numbOfSpace = 0;
  while (i < len)
  {
    if (cmd[i] == ' ' || cmd[i] == '\0')
    {
      numbOfSpace++;
      i++;
    }
    if (numbOfSpace < start)
    {
      i++;
      continue;
    }
    if (numbOfSpace == end)
    {
      break;
    }
    ans++;
    i++;
  }
  return ans - 1;
}

void cd(char *cmd, int len) //this function changes the directory we are in
{
  int i = 0;
  int j = 0;
  int len1 = check_len(cmd, len, 1, 2);
  char *temp = (char *)malloc(sizeof(char) * len1);
  memset(temp, 0, len1);
  while (j < len1)
  {
    if (i <= 2)
    {
      i++;
      continue;
    }
    temp[j] = cmd[i];
    j++;
    i++;
  }
  temp[j] = '\0';
  int ans;
  ans = chdir(temp);//Change the process's working directory to the temp is open on
  if (ans == -1)  
  {
    printf("ERROR\n");
  }
}

void sys(char *cmd, int len)
{
    char delim[] = " \n\r\t";
    char* argv[256] = {0};
    int i = 0;
    char *token = strtok(cmd, delim);//Divide the cmd into tokens separated by characters in DELIM.
    while (token != NULL)
    {
      argv[i] = token;
      i++;
      token = strtok(NULL, " ");
    }
    argv[i] = NULL;
    int pid = fork();
    if(pid == -1)
    {
      perror("ERROER");
      return;
    }
    if(pid == 0)//child
    { 
      execvp(argv[0], argv);
    }
    else // parents
    { 
      wait(NULL);
    }
}


int main()
{
   int server_s = -1; 
   int enable_reuse = 1;
   int client_s = -1;
   char buffer[SIZE] = {0};
  char dir[1024]; //into this array we put the path to the folder where we are

   struct sockaddr_in server_a;
   struct sockaddr_in client_a;

   socklen_t client_a_len = sizeof(client_a);
   socklen_t len;
  
   //open socket TCP 
   printf("Socket creating\n");
   if ((server_s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
       printf("Error: can not create server socket: %d", errno); // if the creat server socket is faild you get this error
   }
     
    //sets a socket option
   printf("Setsockopt\n");
   
   bzero(&server_a, sizeof(server_a)); 
   server_a.sin_family = AF_INET;
   server_a.sin_addr.s_addr = INADDR_ANY;
   server_a.sin_port = htons(PORT_NUM);
     
   // Bind the socket to the port with any IP at this port
   if (bind(server_s, (struct sockaddr *)&server_a,sizeof(server_a)) == -1)
   {
       printf("Failed: bind() failed , Error code : %d", errno); // if bind is faild you get this mesage
       close(server_s); //and close server socket
       return -1;
   }
   printf("Bind success\n"); // if bind is scussess you get this mesage 
   printf("\n"); 
    if (listen(server_s, 5) < 0)   // listening to incoming connections and put the server in a passive mode 
    {    
        printf("Filed: listen failed , Error code : %d", errno); // if listen is faild you get this error
        close(server_s);//and close server socket
        return -1;
    }
    int i = 0;
    int bytes_r = 0;
    char bufcc[NUM];
    len = sizeof(bufcc);
    memset(&client_a, 0, sizeof(client_a));
    client_a_len = sizeof(client_a);
    client_s = accept(server_s, (struct sockaddr *)&client_a, &client_a_len);
    if (client_s == -1)
    {
    printf("Failed: accep failed ,Error code : %d", errno);//if accept failed you get this error
    close(server_s);
    return -1;
    } 
     getcwd(dir, 1024);
      printf("%s yes master-", dir); //prompt
   while (1)
   {
       if ((bytes_r = recv(client_s, buffer, sizeof(buffer), 0)) <= 0 )
        {
            if (listen(server_s, 5) < 0) //always listing if the client connenction again
            {    
                printf("Filed: listen failed , Error code : %d", errno); // if listen is faild you get this error
                close(server_s);//and close server socket
                return -1;
            }
            int i = 0;
            int bytes_r = 0;
            char bufcc[NUM];
            len = sizeof(bufcc);
            memset(&client_a, 0, sizeof(client_a));
            client_a_len = sizeof(client_a);
            client_s = accept(server_s, (struct sockaddr *)&client_a, &client_a_len); 
            if (client_s == -1)
            {
            printf("Failed: accep failed ,Error code : %d", errno);//if accept failed you get this error
            close(server_s);
            return -1;
            } 
            continue;
        }
        if (strncmp(buffer, "ECHO", 4) == 0) // print the output after the ECHO
        {
            substring(buffer,strlen(buffer));
        }
        else if (strncmp(buffer, "DIR", 3) == 0) 
        {
            dir1(buffer,strlen(buffer));
        }
        else if (strncmp(buffer, "CD", 2) == 0)
        {
          cd(buffer,sizeof(buffer));
        }
        // else
        // {
        //     system(buffer);
        // }
        else
        {
            buffer[strlen(buffer)-1] ='\0';
            sys(buffer, sizeof(buffer));
        }
        memset(buffer,0,strlen(buffer));//initialize the array for another message
      getcwd(dir, 1024);
      printf("%s yes master-\n", dir); //prompt
   }
   close(server_s);
   return 0;
}