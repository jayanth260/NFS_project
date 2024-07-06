#include "headers.h"
#include "read_accesPath.h"
#include "Read_file.h"

extern int edited;

int SS_Nmport;
int SS_Cport;

extern set_writepaths WP_set;
pthread_mutex_t mutexx1=PTHREAD_MUTEX_INITIALIZER;

int SS_Initialize(void){
    int nm_port; // Naming Server Port for Connection
    char nm_ip[16];
    strcpy(nm_ip, "127.0.0.1");
    SServer_details ss;
    strcpy(ss.SS_ip, "127.0.0.1");

    nm_port = 5566;
    ss.SS_Cport = SS_Cport;
    // int SS_Nmport = 5500;
     read_access_paths(&ss);
    // printf("%s\n",ss.SS_ip);

    // Create a socket for communication with the Naming Server
    int ss_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (ss_socket == -1)
    {
        perror("Socket creation error");
        exit(1);
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SS_Nmport);
    server_addr.sin_addr.s_addr = inet_addr(ss.SS_ip);

    int n = bind(ss_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0)
    {
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d for NM\n", SS_Nmport);

    // // Naming Server address
    struct sockaddr_in nm_address;
     memset(&nm_address, '\0', sizeof(nm_address));
    nm_address.sin_family = AF_INET;
    nm_address.sin_port = htons(nm_port);
    // inet_pton(AF_INET, nm_ip, &nm_address.sin_addr);

    // Connect to the Naming Server
    int client_conn;
    if ((client_conn= connect(ss_socket, (struct sockaddr *)&nm_address, sizeof(nm_address)) )== -1)
    {
        perror("Connection error");
        exit(1);
    }
    char buffer[1024];
    bzero(buffer,1024);
    strcpy(buffer,"S");
    send(ss_socket,buffer,sizeof(buffer),0);
    usleep(500000);
    // printf("%s\n",ss.accessible_paths);
    // Send the initialization message to the Naming Server
    // strcpy(ss.accessible_paths,"hello");
    // send(ss_socket,&no_of_paths,sizeof(no_of_paths),0);
    // printf("%d\n",ss.no_of_paths);
    // ss.no_of_paths=0;

    // printf("%s\n",ss.accessible_paths);
    usleep(500000);
    send(ss_socket, &ss, sizeof(ss), 0);

    // printf("%d %d\n",ss_socket,client_conn);

    // Close the socket after sending the message
    // close(client_conn);

    printf("SS has been initialized and sent its details to the Naming Server.\n");
    
    return ss_socket;
}

//communication with NM server happens in this
void* NM_thread_func(){
   int C_conn= SS_Initialize();


  // pthread_t *create_deleteT=(pthread_t*)malloc(sizeof(pthread_t));
  // int no_of_trheads=0;
  // int size=1;

  while(1){
// char buffer[1024];
//     recv(C_conn,buffer,sizeof(buffer),0);
//     printf("%s\n",buffer);
//     if(!strcmp(buffer, "create")|| !strcmp(buffer,"delete")){
//     create_delete(C_conn);


  //  }
  // printf("hello\n");
  // if(no_of_trheads==size){
  //   size=2*size;
  //   create_deleteT=(pthread_t*)realloc(create_deleteT,size*sizeof(pthread_t));
  // }
  

  char buffer2[1024];
  bzero(buffer2,1024);
    int bytesRead = recv(C_conn, buffer2, sizeof(buffer2), 0);
// bzero(buffer,1024);
    if (bytesRead > 0) {
        buffer2[bytesRead] = '\0'; // Null-terminate the received data
        // printf("%s %d\n", buffer2,strlen(buffer2));

        if (!strcmp(buffer2, "create") || !strcmp(buffer2, "delete")) {
  //         pthread_mutex_lock(&mutexx1);
  // no_of_trheads++;
  // pthread_mutex_unlock(&mutexx1);
            create_delete(C_conn);
            // create_delete(C_conn);
        }
        else if(!strcmp(buffer2,"check")){
          bzero(buffer2,1024);
          if(edited==1){
            strcpy(buffer2,"ACK_Edited");
            edited=0;
          }else
          strcpy(buffer2,"ACK_Alive");
          send(C_conn,buffer2,sizeof(buffer2),0);
        }
        else if(!strcmp(buffer2,"copy")){
          // printf("hello\n");
          copy_init(C_conn);
        }
    }
  }
    
    pthread_exit(NULL);
}



//communication with Clients happens here
void* C_conn_func(){
    char *ip="127.0.0.1";
    // int SS_Cport=6000;

    int Sserver_sock;
    struct sockaddr_in Sserver_addr;
  Sserver_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (Sserver_sock < 0){
    perror("[-]Socket error");
    exit(1);
  }
//   printf("[+]TCP server socket created.\n");
    memset(&Sserver_addr, '\0', sizeof(Sserver_addr));
  Sserver_addr.sin_family = AF_INET;
  Sserver_addr.sin_port = htons(SS_Cport);
  Sserver_addr.sin_addr.s_addr = inet_addr(ip);


int n = bind(Sserver_sock, (struct sockaddr*)&Sserver_addr, sizeof(Sserver_addr));
  if (n < 0){
    perror("[-]Bind error");
    exit(1);
  }
  printf("[+]Bind to the port number: %d for Client\n", SS_Cport);

    listen(Sserver_sock, 5);
  printf("Listening...\n");
    pthread_t *C_Request_thread;
    C_Request_thread=(pthread_t*)malloc(sizeof(pthread_t));
    int size=1;
    int no_of_C=0;

    int *client_conn=(int*)malloc(sizeof(int));
  while(1){
    struct sockaddr_in client_addr;

    socklen_t addr_size = sizeof(client_addr);
    if(no_of_C==size){
      size=2*size;
      C_Request_thread=(pthread_t*)realloc(C_Request_thread,size*sizeof(pthread_t));
      client_conn=(int*)malloc(size*sizeof(int));
    }
    // printf("%d\n",no_of_C);
    no_of_C++;
   client_conn[no_of_C-1] = accept(Sserver_sock, (struct sockaddr*)&client_addr, &addr_size);
   printf("[+]Client connected.\n");
   char buffer[1024];
   bzero(buffer,1024);
   
   recv(client_conn[no_of_C-1], &buffer, sizeof(buffer), 0);
   printf("Client: %s\n",buffer);
   if(!strcmp(buffer,"read"))
   {
    // printf("enter\n");
    pthread_create(&C_Request_thread[no_of_C-1],NULL,Read,&client_conn[no_of_C-1]);
   }
   if(!strcmp(buffer,"write")){
    pthread_create(&C_Request_thread[no_of_C-1],NULL,Write,&client_conn[no_of_C-1]);
   }
   if(!strcmp(buffer,"getinfo")){
    pthread_create(&C_Request_thread[no_of_C-1],NULL,getinfo,&client_conn[no_of_C-1]);
   }




    
  }

}

int main(int argc,char* argv[])
{
  SS_Cport=atoi(argv[1]);
  SS_Nmport=atoi(argv[2]);
  
    // set_writepaths* WP_set=(set_writepaths*)malloc(sizeof(set_writepaths));
    WP_set.no_of_writepaths=0;
    WP_set.size=1;
    WP_set.WP=(write_path*)malloc(sizeof(write_path));

    pthread_t NM_thread,C_thread;

    pthread_create(&NM_thread,NULL,NM_thread_func,NULL);
    pthread_create(&C_thread,NULL,C_conn_func,NULL);


    

    pthread_join(NM_thread,NULL);
    pthread_join(C_thread,NULL);
   

    return 0;
}
