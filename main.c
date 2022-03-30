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


#define SERVER_IP_ADDRESS "127.0.0.1" //IPv4 address of server
#define PORT_NUM 5060
#define SIZE 65536 //2^16

int sizeOfFile(char *name) // return the size of data file if the file not exists return -1
{
  FILE *file1;
  int count = 0;
  char c = ' ';
  if ((file1 = fopen(name, "rb")) == NULL)//if the file not exists return -1
  {
    return -1;
  }

  while (fread(&c, sizeof(c), 1, file1) == 1)//over the data file return count
  {
    count += 1;
  }
  fclose(file1);
  return count;
}

int copy(char *src, char *dest) // copy the data in src file to the dest file
{
  int count = sizeOfFile(src); // size of file retun the size of file if the file not exists return -1
  if (count == -1)
  {
    printf("There is no file named %s\n", src);
    return -1;
  }
  char *data = (char *)malloc(sizeof(char) * count);
  memset(data, 0, sizeof(data));
  FILE *file1;
  if ((file1 = fopen(src, "rb")) == NULL) 
  {
    return -1;
  }
  fread(data, count, 1, file1);//read data from a file file1 and store it in a data.
  FILE *file2;
  if ((file2 = fopen(dest, "wb")) == NULL)
  {
    return -1;
  }
  fwrite(data, count, 1, file2);///write data from a file2 and store it in a data.
  fclose(file1);
  fclose(file2);
  free(data);
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

void sendToCopy(char *cmd, int len)
{
  int len1 = check_len(cmd, len, 1, 2) + 1; 
  int len2 = check_len(cmd, len, 2, 3) + 1;
  char *src = (char *)malloc(sizeof(char) * len1);
  char *dest = (char *)malloc(sizeof(char) * len2);
  memset(src, 0, sizeof(src));
  memset(dest, 0, sizeof(dest));
  int i = 0;
  int count = 0;
  int j1 = 0;
  int j2 = 0;
  int bool1 = 0;
  while (i < len || j1 < len1)
  {
    if (cmd[i] == '\0')
    {
      break;
    }
    if (cmd[i] == ' ')
    {
      if (count < 2)
      {
        count++;
      }
      i++;
      continue;
    }
    if (count == 1)
    {
      bool1 = 1;
    }
    if (count == 2)
    {
      bool1 = 0;
    }
    if (bool1 == 1)
    {
      src[j1] = cmd[i];
      j1++;
    }
    i++;
  }
  src[j1] = '\0';
  cmd = cmd + len1 + 6;
  i = 0;
  while (i < len2 - 1)
  {
    dest[i] = cmd[i];
    i++;
  }
  dest[i] = '\0';
  copy(src, dest); //func copy the src file to the dest file
  free(src);
  free(dest);
}

void substring(char *s, int len)
{
  s += 5;
  printf("%s", s);
}

void del(char *cmd, int len) // delete file 
{
  int i = 0;
  int j = 0;
  int len1 = check_len(cmd, len, 1, 2);
  char *temp = (char *)malloc(sizeof(char) * len1);
  memset(temp, 0, len1);
  while (j < len1)
  {
    if (i <= 6)
    {
      i++;
      continue;
    }
    temp[j] = cmd[i];
    j++;
    i++;
  }
  temp[j] = '\0';
  int b = unlink(temp);//Remove the link temp
  free(temp);
}

int tcpConnection() //this function open a connention with the server
{
  int sock = 0;
  struct sockaddr_in server_a;        //is a server address
  bzero(&server_a, sizeof(server_a)); //Initializes the socket address
  server_a.sin_family = AF_INET;      // ipv4
  server_a.sin_port = htons(PORT_NUM);
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) //SOCK_STREM-TCP
  {
    printf("\n Error: in socket creation \n"); // if did not open soket you get this error
    return -1;
  }
  if (connect(sock, (struct sockaddr *)&server_a, sizeof(server_a)) < 0)
  {
    printf("Failed: connect failed, Error code : %d \n", errno); // if the connect is faild you get this error
    return -1;
  }
  printf("Connected to server\n"); // else you get that connected is work
  return sock;
}

void sendm(char *s, int len, int sock) //This function receives a socket number and a message and sends to the server that the same socket is connected to
{
  int bytes_s = send(sock, s, len, 0); //send the data to the server
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
  char cmd[1024]; //into this array we put the command
  char dir[1024]; //into this array we put the path to the folder where we are
  int bool = 0;
  int sock = 0;
  while (1)
  {
    getcwd(dir, 1024);
    printf("%s yes master-", dir); //prompt
    memset(cmd, 0, strlen(cmd));
    fgets(cmd, 1024, stdin); //receive information from the user
    if (strncmp(cmd, "EXIT", 4) == 0) // Exit from the shell
    {
      break;
    }
    else if (strncmp(cmd, "ECHO", 4) == 0) // print the output after the ECHO
    {
      if (bool == 0) //if we are in local print the informetion
      {
        substring(cmd, strlen(cmd));
      }
      else //if we are in TCP connection send the information to the server
      {
        sendm(cmd, strlen(cmd), sock);
      }
    }
    else if (strncmp(cmd, "TCP PORT", 8) == 0) //all commands starting with "ECHO" will go to the server now
    {
      bool = 1;
      sock = tcpConnection();
    }
    else if (strncmp(cmd, "LOCAL", 5) == 0 && bool == 1) //all commands starting with "ECHO" will print in the terminal now
    {
      bool = 0;
      close(sock); // close the socket
    }
    else if (strncmp(cmd, "DIR", 3) == 0) //this function shows all the files in the folder
    {
      if(bool==1)
      {
        sendm(cmd,strlen(cmd),sock);
      }
      else
      {
        dir1(cmd, sizeof(cmd));
      }
    }
    else if (strncmp(cmd, "CD", 2) == 0) // changes the directory to a partition that comes as a parameter after CD 
    {
      if(bool==1)
      {
        sendm(cmd,sizeof(cmd),sock);
      }
      cd(cmd, sizeof(cmd)); //cndir is a system call
    }
    else if (strncmp(cmd, "COPY", 4) == 0) // copy file source to dest file
    {
      //* fwrite is a system call *//
      //* fopen is a system call *//
      //* fread is a system call *//
      sendToCopy(cmd, sizeof(cmd));
    }
    else if (strncmp(cmd, "DELETE", 6) == 0)// delete file 
    {
      del(cmd, sizeof(cmd)); //unlink is a system call
    }
    // else
    // {
    //   if(bool==1)
    //   {
    //      sendm(cmd, strlen(cmd), sock);
    //   }
    //   else
    //   {
    //     int a = system(cmd); //system is a libary function in c
    //   }
    // }
    else
    {
      if (bool==1)
      {
        sendm(cmd,sizeof(cmd),sock);
      }
      else
      {
        cmd[strlen(cmd)-1] ='\0';
        sys(cmd, sizeof(cmd));
      }
    }
  }
  return 0;
}
