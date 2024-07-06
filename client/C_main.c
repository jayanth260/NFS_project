#include "headers.h"

void die(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int connect_S(char SS_ip[],int SS_port ){
  

  int sock;
  struct sockaddr_in addr;
  // socklen_t addr_size;
  // char buffer[1024];
  // int n;
  // printf("%s %d\n",SS_ip,SS_port);
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0){
    perror("[-]Socket error");
    return -1;
  }
  printf("[+]TCP server socket created.\n");

  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port =htons(SS_port);
  addr.sin_addr.s_addr = inet_addr(SS_ip);

  connect(sock, (struct sockaddr*)&addr, sizeof(addr));
  printf("Connected to the server.\n");
  // char buffer[1024];
  // strcpy(buffer,"test");
  // send(sock,buffer,sizeof(buffer),0);

  return sock;
 
}

int main(){


  // while(1){

  char buffer[1024];
  // while(strlen(buffer)<2)
  fgets(buffer,sizeof(buffer),stdin);
  int len=strlen(buffer);
  buffer[len-1]='\0';
  
  char *ip = "127.0.0.1";
  int port = 5566;

  // char buffer[1024];
  int sock=connect_S(ip,port);



  bzero(buffer,1024);
  strcpy(buffer,"C");
  
  send(sock, buffer, strlen(buffer), 0);
  usleep(500000);
  bzero(buffer, 1024);
  strcpy(buffer, "create file dir1/file6.txt");
  printf("Client: %s\n", buffer);
  send(sock, buffer, strlen(buffer), 0);
  // strcpy(buffer,"file1.txt");
  int sent_time=time(NULL);
  char buffer1[1024];
  bzero(buffer1,1024);
  int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1) {
        die("fcntl");
    }
    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1) {
        die("fcntl");
    }

  while(strcmp(buffer1,"ACK")){
    recv(sock,&buffer1,sizeof(buffer1),0);
    if(time(NULL)-sent_time > 2)
    {
      printf("Request timeout\n");
      continue;
    }

  }
   flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1) {
        perror("Error getting socket flags");
        return -1;
    }

    // Clear the non-blocking flag
    flags &= ~O_NONBLOCK;

    // Set the modified flags
    if (fcntl(sock, F_SETFL, flags) == -1) {
        perror("Error setting socket flags");
        return -1;
    }

  SServer_addr address;
  // while(1){
    // for(int i=0;i<2;i++){
      char temp[1024];
      strcpy(temp,buffer);
      char op[8];
      strcpy(op,strtok(temp," "));

  if(!strcmp(op,"read")|| !strcmp(op,"write") || !strcmp(op,"getinfo")){
      int valid_path;
  recv(sock,&valid_path,sizeof(valid_path),0);
if(valid_path==1)
{
  perror("invalid Path");

}
else{
  recv(sock, &address, sizeof(address), 0);
  printf("Server: %s %d\n", address.SS_ip,address.port);
  }// }
  // }
  close(sock);
  printf("Disconnected from the server.\n");
  if( valid_path==0){
    init( address, buffer);
  }
  }
  else if(!strcmp(op,"create") || !strcmp(op,"delete")){
    int flag;
    recv(sock,&flag,sizeof(flag),0);
    // printf("%d\n",flag);
  if(flag==0)
  printf("delete/create done\n");
  else if(flag==1)
  printf("invalid path\n");
  else if(flag==3)
  printf("no required permission for file to create in that directory\n");
  else if(flag==5)
  printf("creation/deletion failed\n");
  else if(flag==4)
  printf("file/dir already exists\n");
  else if(flag==7)
  printf("file/dir already deleted\n");

  close(sock);

  }
  else if(!strcmp(op,"copy")){
    int flag;
    recv(sock,&flag,sizeof(flag),0);
    printf("%d\n",flag);
      if(flag==0)
      printf("copy succes\n");
      else if(flag==1)
      printf("invalid path\n");
      else if(flag==3)
  printf("no required permission for file to create in that directory\n");
  else if(flag==5)
  printf("creation/deletion failed\n");
  else if(flag==4)
  printf("file/dir already exists\n");


  close(sock);
    
    
  }



  // return ;
  // }

  return 0;
}
