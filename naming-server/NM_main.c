#include "headers.h"
#include "create_delete.h"

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t muttex3 = PTHREAD_MUTEX_INITIALIZER;

Server_set *S;
Client_details *C;
sem_t *SS_sem;


struct LRU_Caching lru[11];
int index_var=-1;

char *ip = "127.0.0.1";
int port = 5566;

int server_sock;

int break_string(char arr[], SServer_details *SS, int index)
{
  int count = 0;
  for (int i = 0; arr[i] != '\0'; i++)
  {
    if (arr[i] == ' ')
    {
      count++;
    }
  }
  // return count;
  SS->no_of_paths = count + 1;
  if (SS->paths != NULL)
    free(SS->paths);
  SS->paths = (acc_path *)malloc((SS->no_of_paths) * sizeof(acc_path));
  for (int i = 0; i < SS->no_of_paths; i++)
    SS->paths[i].SS_num = index;

  // Tokenize the input string
  char *token = strtok(arr, " ");
  int i = 0;

  while (token != NULL)
  {
    // Store the token in the array
    strcpy(SS->paths[i].path, token);
    int len = strlen(token);
    SS->paths[i].path[len] = '\0';

    i++;

    // Get the next token
    token = strtok(NULL, " ");
  }

  // Print the substrings
  // for (int j = 0; j < i; j++)
  // {
  //   printf(" %s\n", SS->paths[j].path);
  // }

  return 0;
}

int *Sserver_conn;
// int *server_alive;

int no_of_active_S()
{
  int x = 0;
  for (int i = 0; i < S->no_of_SS; i++)
  {
    if (S->SS[i].active == 1)
      x++;
  }
  return x;
}

void *Storage_Sfunc(void *arg)
{
  int *client_conn = (int *)arg;
  // printf("S%d\n", *client_conn);

  // int no_of_paths;
  // recv(*client_conn, &no_of_paths, sizeof(no_of_paths), 0);
  // printf("%d\n",no_of_paths);

  int done = 0;

  if (S->no_of_SS == S->size)
  {
    S->size = 2 * S->size;
    S->SS = (SServer_details *)realloc(S->SS, S->size * sizeof(SServer_details));
    Sserver_conn = (int *)realloc(Sserver_conn, S->size * sizeof(int));
  }

  // S->SS[S->no_of_SS-1].accessible_paths=(char**)malloc(no_of_paths*sizeof(char*));
  // for(int i=0;i<no_of_paths;i++)
  // S->SS[S->no_of_SS-1].accessible_paths[i]=(char*)malloc(64*sizeof(char));

  // ...

  // SServer_details ss;
  // printf("bye\n");
  SS_recv temp;
  while(strlen(temp.SS_ip)<7)
  recv(*client_conn, &temp, sizeof(temp), 0);

  printf("Client: %d %s\n", temp.SS_Cport, temp.SS_ip);
  printf("%s\n", temp.accessible_paths);
  int index = first_time(temp.SS_ip, temp.SS_Cport);
  if (index == -1)
  {
    pthread_mutex_lock(&mutex2);
    index = S->no_of_SS++;
    S->SS[index].S1_idx = -1;
    S->SS[index].S2_idx = -1;
    pthread_mutex_unlock(&mutex2);
  }
  Sserver_conn[index] = *client_conn;
  sem_init(&SS_sem[index], 0, 1);

  break_string(temp.accessible_paths, &S->SS[index], index);

  strcpy(S->SS[index].SS_ip, temp.SS_ip);
  S->SS[index].SS_Cport = temp.SS_Cport;
  S->SS[index].S1_set=0;
  S->SS[index].S2_set=0;

  // printf("Client: %d %s\n", S->SS[index].SS_Cport, S->SS[index].SS_ip);

  // printf("I%d\n", index);

  book_conn(1, index);

  // int i = S->no_of_SS - 1;
  // printf("NN%d\n",no_of_active_S());
  if (no_of_active_S() >2)
  {
    duplicate_S(index, 1);
    if (done == 0)
    {
      duplicate_S(0, 0);
      duplicate_S(1, 0);
      done = 1;
    }
  }

  

  for (int i = 0; i < S->SS[index].no_of_paths; i++)
    printf("%s %d\n", S->SS[index].paths[i].path, S->SS[index].paths[i].SS_num);

  // printf("%s\n", S->SS[S->no_of_SS - 1].accessible_paths[0]);
  // close(*client_conn);
  // printf("[+]Client disconnected.\n\n");
  
  if (check_connection(*client_conn, index) == 6)
    book_conn(2, index);

  pthread_exit(NULL);

  // }
}
void extractSubstring(const char *input, char *output)
{
  // Find the last occurrence of '/'
  const char *lastSlash = strrchr(input, '/');

  if (lastSlash != NULL)
  {
    // Calculate the length of the substring until the last '/'
    size_t length = lastSlash - input + 1; // Include the last '/'

    // Copy the substring to the output buffer
    strncpy(output, input, length);
    output[length - 1] = '\0'; // Null-terminate the output string
  }
  else
  {
    // If no '/' is found, copy the entire input string
    strcpy(output, input);
  }
}

char **buffer_temp;
int no_of_buffer_temp = 0;
int size_of_bt = 1;

void *Client_func(void *arg)
{
  int *client_conn = (int *)arg;
  // printf("%d\n", *client_conn);

  char buffer[1024];
  bzero(buffer, 1024);
  recv(*client_conn, buffer, sizeof(buffer), 0);

  int len = strlen(buffer);
  buffer[len] = '\0';
  if (no_of_buffer_temp == size_of_bt)
  {
    size_of_bt = 2 * size_of_bt;
    buffer_temp = (char **)realloc(buffer_temp, size_of_bt * sizeof(char *));
  }
  // buffer_temp[no_of_buffer_temp]=(char*)
  buffer_temp[no_of_buffer_temp] = (char *)malloc(1024 * sizeof(char));
  pthread_mutex_lock(&mutex1);
  no_of_buffer_temp++;
  pthread_mutex_unlock(&mutex1);
  strcpy(buffer_temp[no_of_buffer_temp - 1], buffer);
  printf("Client: %d %s\n", *client_conn, buffer);

  char op[1024];
  char file_path_source[1024];
  char file_path_dest[1024];

  char *token = strtok(buffer, " ");
  strcpy(op, token);
  strcpy(lru[++index_var].operation, token);
  if (!strcmp(op, "create"))
  {
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    extractSubstring(token, file_path_source);
  }
  else if (!strcmp(op, "delete"))
  {
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    strcpy(file_path_source, token);
  }
  else if (!strcmp(op, "copy"))
  {
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    strcpy(file_path_source, token);
    token = strtok(NULL, " ");
    strcpy(file_path_dest, token);
  }
  else
  {
    token = strtok(NULL, " ");
    strcpy(file_path_source, token);
    // printf("hey\n");
  }
  char buffer1[1024];
  strcpy(buffer1, "ACK");
  send(*client_conn, &buffer1, sizeof(buffer1), 0);
  book_conn(3, no_of_buffer_temp - 1);
  SServer_addr address_source = get_Sserver(file_path_source,op);
  SServer_addr address_dest;
  address_dest.port = 0;
  if (!strcmp(op, "copy"))
    address_dest = get_Sserver(file_path_dest,op);

<<<<<<< HEAD
  // printf("%d\n", S->no_of_SS);
  printf("%s\n", file_path_source);

  // int v = 0;
  // int i;
=======
  printf("%d\n", S->no_of_SS);
  printf("%s\n", file_path);
  strcpy(lru[index_var].path, file_path);
  
  int v = 0;
  int i;


  /// here i would like to write +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++===========


  for (i = 0; i < S->no_of_SS; i++)
  {
    for (int j = 0; j < S->SS[i].no_of_paths; j++)
    {
      // printf("in\n");
      // printf("%s\n", S->SS[i].accessible_paths[j]);
      if (!strcmp(file_path, S->SS[i].accessible_paths[j]))
      {
        // printf("%d\n", *client_conn);
        lru[index_var].SS_No=i;
        address.port = S->SS[i].SS_Cport;
        strcpy(address.SS_ip, S->SS[i].SS_ip);
>>>>>>> origin/main

  // printf("%d\n", *client_conn);
  int valid_path = 0;
  if (address_source.port == -1 || address_dest.port == -1)
  {
    valid_path = 1;
    book_cmd(buffer_temp[no_of_buffer_temp - 1], no_of_buffer_temp - 1, -1, 0);
    book_conn(4, no_of_buffer_temp - 1);
    send(*client_conn, &valid_path, sizeof(valid_path), 0);
    close(*client_conn);
    return NULL;
  }
  else
  {
    if (strcmp(op, "create") && strcmp(op, "delete") && strcmp(op, "copy"))
    {
      send(*client_conn, &valid_path, sizeof(valid_path), 0);

      send(*client_conn, &address_source, sizeof(address_source), 0);
      book_cmd(buffer_temp[no_of_buffer_temp - 1], no_of_buffer_temp - 1, address_source.index, 1);
    }
  }

  if (!strcmp(op, "create") || !strcmp(op, "delete"))
  {
    // printf("S%d\n", Sserver_conn[i]);
    // printf("1%d\n",*client_conn);
    // // pthread_mutex_lock(&mutex1);
    // printf("2%d\n",*client_conn);
    // printf("%s\n",buffer_temp[no_of_buffer_temp-1]);
    sem_wait(&SS_sem[address_source.index]);
    int flag = Request_Create_delete(buffer_temp[no_of_buffer_temp - 1], Sserver_conn[address_source.index], address_source.index, 1);
    sem_post(&SS_sem[address_source.index]);
    // pthread_mutex_unlock(&mutex1);
    printf("%d\n",flag);
    send(*client_conn, &flag, sizeof(flag), 0);
    if (flag == 0)
      book_cmd(buffer_temp[no_of_buffer_temp - 1], no_of_buffer_temp - 1, address_source.index, 1);
    else
      book_cmd(buffer_temp[no_of_buffer_temp - 1], no_of_buffer_temp - 1, address_source.index, 0);
  }
  else if (!strcmp(op, "copy"))
  {
    sem_wait(&SS_sem[address_source.index]);
    sem_wait(&SS_sem[address_dest.index]);
    int flag = Request_copy(buffer_temp[no_of_buffer_temp - 1], address_source.index, address_dest.index);
    sem_post(&SS_sem[address_dest.index]);
    sem_post(&SS_sem[address_source.index]);

    printf("%d\n", flag);

    send(*client_conn, &flag, sizeof(flag), 0);

    if (flag == 0)
      book_cmd(buffer_temp[no_of_buffer_temp - 1], no_of_buffer_temp - 1, address_source.index, 1);
    else
      book_cmd(buffer_temp[no_of_buffer_temp - 1], no_of_buffer_temp - 1, address_source.index, 0);
  }

  book_conn(4, no_of_buffer_temp - 1);
  close(*client_conn);
  printf("[+]Client disconnected.\n\n");
  // }
  // return;
  // for LRU caching checking purpose........
  // for(int xy=0;xy<=index_var;xy++){
  // printf("SS_No= %d\nPath=%s\n Operation= %s\n",lru[xy].SS_No,lru[xy].path,lru[xy].operation);
  // }

  pthread_exit(NULL);
}

int main()
{

  struct sockaddr_in server_addr;

  // socklen_t addr_size;
  // char buffer[1024];
  int n;

  S = (Server_set *)malloc(sizeof(Server_set));
  S->SS = (SServer_details *)malloc(sizeof(SServer_details));
  S->size = 1;
  S->no_of_SS = 0;
  Sserver_conn = (int *)malloc(sizeof(int));

  buffer_temp = (char **)malloc(sizeof(char));

  C = (Client_details *)malloc(sizeof(Client_details));
  SS_sem = (sem_t *)malloc(sizeof(sem_t));
  // server_alive=(int*)malloc(sizeof(int));

  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock < 0)
  {
    perror("[-]Socket error");
    exit(1);
  }
  printf("[+]TCP server socket created.\n");

  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip);

  n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (n < 0)
  {
    perror("[-]Bind error");
    exit(1);
  }
  printf("[+]Bind to the port number: %d\n", port);

  listen(server_sock, 5);
  printf("Listening...\n");

  pthread_t *Storage_Sthread, *Client_thread;
  Storage_Sthread = (pthread_t *)malloc(sizeof(pthread_t));
  Client_thread = (pthread_t *)malloc(sizeof(pthread_t));
  int size_SS_arr = 1;
  int size_C_arr = 1;
  int no_of_SS = 0;
  int no_of_C = 0;
  // int client_addr[6];
  // for(int i=0;i<2;i++){
  // printf("%d\n",i);
  int *client_conn = (int *)malloc(2 * sizeof(int));
  while (1)
  {
    struct sockaddr_in client_addr;

    socklen_t addr_size = sizeof(client_addr);
    if (no_of_C == size_C_arr)
    {
      size_C_arr = 2 * size_C_arr;
      Client_thread = (pthread_t *)realloc(Client_thread, size_C_arr * sizeof(pthread_t));
      C = (Client_details *)realloc(C, size_C_arr * sizeof(Client_details));
      client_conn = (int *)realloc(client_conn, (size_C_arr + size_SS_arr) * sizeof(int));
    }
    if (no_of_SS == size_SS_arr)
    {
      size_SS_arr = 2 * size_SS_arr;
      Storage_Sthread = (pthread_t *)realloc(Storage_Sthread, size_SS_arr * sizeof(pthread_t));
      client_conn = (int *)realloc(client_conn, (size_C_arr + size_SS_arr) * sizeof(int));
      SS_sem = (sem_t *)realloc(SS_sem, size_SS_arr * sizeof(sem_t));
    }
    client_conn[no_of_C + no_of_SS] = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
    if(client_conn[no_of_C + no_of_SS]>1000)
    {
      perror("connect error");
      continue;
    }
    printf("[+]Client connected.\n");
    // printf("%d\n",client_conn[no_of_C + no_of_SS]);
    int conn=client_conn[no_of_C+no_of_SS];
    char buffer[1024];
    recv(client_conn[no_of_C + no_of_SS], &buffer, sizeof(buffer), 0);

    // printf("%d %s %d\n", , buffer, client_conn[no_of_C+no_of_SS]);
    if (!strcmp(buffer, "C"))
    {
      // char client_ip[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(client_addr.sin_addr), C[no_of_C].C_ip, INET_ADDRSTRLEN);
      C[no_of_C].port = ntohs(client_addr.sin_port);

      pthread_create(&Client_thread[no_of_C], NULL, Client_func, &conn);
      no_of_C++;
    }
    else if (!strcmp(buffer, "S"))
    {
      pthread_create(&Storage_Sthread[no_of_SS], NULL, Storage_Sfunc, &conn);
      no_of_SS++;
    }
  }

  for (int a = 0; a < no_of_C; a++)
    pthread_join(Client_thread[a], NULL);
  for (int b = 0; b < no_of_SS; b++)
    pthread_join(Storage_Sthread[b], NULL);

  // for(int )
  // Storage_Sfunc(server_sock);
  // Client_func(server_sock);

  return 0;
}
