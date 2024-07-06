#include "Read.h"
#include "headers.h"

int init(SServer_addr address,char buffer[]){
    int sock=connect_S(address.SS_ip,address.port);
    // printf("%d\n",sock);
    // bzero(buffer,1024);
    char temp[1024];
    strcpy(temp,buffer);
    char* token=strtok(temp," ");
    if(!strcmp(token,"read")){
    token=strtok(NULL," ");


   return Request_Read(token,sock);
    }
    if(!strcmp(token,"write")){
        token=strtok(NULL," ");

        return Request_Write(token,sock);
    }
    
    

}

int Request_Read(char file_path[], int sock)
{   
    
    size_t length = strlen(file_path);

    char *file_name = (char *)malloc(length + 1);

    strcpy(file_name, file_path);

    
    for (size_t i = 0; i < length; ++i)
    {
        if (file_name[i] == '/')
        {
            file_name[i] = '_';
        }
    }
    // printf("%d",sock);
    // printf("%s\n", file_name);

    char buffer[1024];
    strcpy(buffer, "read");
    // printf("%d\n",sock);
    
    send(sock, buffer, sizeof(buffer), 0);
    bzero(buffer,1024);
    strcpy(buffer,file_path);
    send(sock,buffer,sizeof(buffer),0);


    FILE *file = fopen(file_name, "wb");
    if (file == NULL)
    {
        perror("Error creating file");
        // exit(EXIT_FAILURE);
        return -1;
    }

    bzero(buffer, 1024);
    size_t bytesRead;

    while ((bytesRead = recv(sock, buffer, sizeof(buffer), 0)) > 0)
    {
        if(!strcmp(buffer,"STOP"))
        break;
        printf("%s\n", buffer);
        fwrite(buffer, 1, bytesRead, file);
    }

    mode_t new_permissions = S_IRUSR;
    if (chmod(file_name, new_permissions) != 0)
    {
        perror("Error changing file permissions");
    }

    fclose(file);
    printf("file closed\n");

    close(sock);
    printf("connection closed\n");

     printf("enter done after reading the file.\n");
    char done[6];
    scanf("%s",done);
    int len=strlen(done);
    done[len]='\0';
    printf("%s\n",done);
    if(!strcmp(done,"done")){
    if (remove(file_name) != 0) {
      perror("Error deleting file");
      exit(0);}
    // } else {
    }

    return 0;
    // }
}


int Request_Write(char file_path[],int sock){


    size_t length = strlen(file_path);

    char *file_name = (char *)malloc(length + 1);

    strcpy(file_name, file_path);

    
    for (size_t i = 0; i < length; ++i)
    {
        if (file_name[i] == '/')
        {
            file_name[i] = '_';
        }
    }
    
    // printf("%d",sock);
    // printf("%s\n", file_name);

    char buffer[1024];
    strcpy(buffer, "write");
    // printf("%d\n",sock);
    
    send(sock, buffer, sizeof(buffer), 0);
    bzero(buffer,1024);
    strcpy(buffer,file_path);
    send(sock,buffer,sizeof(buffer),0);

    int write_permission;
    recv(sock,&write_permission,sizeof(write_permission),0);
    if(write_permission==3){
        perror("no write permission");
        close(sock);
        return -1;
    }
    int write_access;
    recv(sock,&write_access,sizeof(write_access),0);
    
    if(write_access==2){
        perror("other client is writing into same file");
        close(sock);
        return -1;
    }

    FILE *file = fopen(file_name, "wb");
    if (file == NULL)
    {
        perror("Error creating file");
        exit(EXIT_FAILURE);
    }

    bzero(buffer, 1024);
    size_t bytesRead;

    while ((bytesRead = recv(sock, buffer, sizeof(buffer), 0)) > 0)
    {   
        if(!strcmp(buffer,"STOP"))
        break;
        printf("%s\n", buffer);
        fwrite(buffer, 1, bytesRead, file);
        bzero(buffer,1024);
    }
    fclose(file);
    printf("file closed\n");

    

     printf("enter done after writing into the file.\n");
    char done[6];
    scanf("%s",done);
    int len=strlen(done);
    done[len]='\0';
    printf("%s\n",done);


    // if(!strcmp(done,"done")){
    // if (remove(file_name) != 0) {
    //   perror("Error deleting file");
    //   exit(0);}
    // // } else {
    // }

// sleep(100);

    if(!strcmp(done,"done")){
        FILE* file1 = fopen(file_name, "rb");
        bzero(buffer,1024);
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file1)) > 0) {
        send(sock, buffer, bytesRead, 0);
    }
    
     fclose(file1);}
     usleep(500000);
    bzero(buffer,1024);
    strcpy(buffer,"STOP");
    send(sock,buffer,sizeof(buffer),0);
   



    close(sock);
    printf("connection closed\n");
    if (remove(file_name) != 0) {
      perror("Error deleting file");
      exit(0);}
    return 0;
}


int Request_getinfo(char file_path[], int sock)
{
    size_t length = strlen(file_path);

    char *file_name = (char *)malloc(length + 1);

    strcpy(file_name, file_path);

    for (size_t i = 0; i < length; ++i)
    {
        if (file_name[i] == '/')
        {
            file_name[i] = '_';
        }
    }
    printf("%d", sock);
    printf("%s\n", file_name);

    char buffer[1024];
    strcpy(buffer, "getinfo");
    printf("%d\n", sock);

    send(sock, buffer, sizeof(buffer), 0);
    bzero(buffer, 1024);
    strcpy(buffer, file_path);
    send(sock, buffer, sizeof(buffer), 0);
    struct get_info info;

    recv(sock, &info,sizeof(info),0);

    // char perm[10];
    // recv(sock,&perm,sizeof(perm),0);
    printf("\nPermission: %s\n",info.perm);
    // usleep(100000);
    // int bytes;
    // while(bytes==0){

    // recv(sock,&bytes,sizeof(bytes),0);
    // }
    printf("\nSize of file: %d bytes\n",info.size);
    // usleep(100000);
    // char mod_time[20];
    // recv(sock,&mod_time,sizeof(mod_time),0);
    printf("\nLast modified time: %s\n",info.time);

    close(sock);
    printf("Connection closed\n");
    return 0;
}