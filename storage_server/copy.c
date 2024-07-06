#include "Read_file.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

pthread_mutex_t mutex2=PTHREAD_MUTEX_INITIALIZER;

extern set_writepaths WP_set;

// extern int edited;

int copy_file_S(int C_conn,char file_path[]){
    
    // printf("%s\n",file_path);
    FILE *file = fopen(file_path, "rb");
    
    if (file == NULL)
    {
        perror("Error opening file");
    }
    else
    {   printf("file opened\n");
        char buffer[1024];
        size_t bytesRead;

        while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
        {
            send(C_conn, buffer, bytesRead, 0);
        }
    }
    fclose(file);
    printf("file closed\n");
    usleep(200000);
    // sleep(2);
    char buffer[1024];
    strcpy(buffer,"STOP");
    send(C_conn,buffer,sizeof(buffer),0);


}

int copy_file_D(int C_conn,char file_path[]){

    int write_permission=0;
    if (access(file_path, W_OK) != 0) {
        write_permission=3;
        send(C_conn,&write_permission,sizeof(write_permission),0);
        // close(C_conn);
        
        return;
    }

send(C_conn,&write_permission,sizeof(write_permission),0);
// pthread_mutex_lock(&mutex2);
// // printf("1\n");
//     int v=0;
//     int write_access=0;
//     printf("%d\n",WP_set.no_of_writepaths);
//     int index;
//     // printf("4\n");
//     for(int i=0;i<WP_set.no_of_writepaths ;i++){
//         printf("2\n");
//         if(!strcmp(WP_set.WP[i].path,file_path)){
            
//             if(WP_set.WP[i].write_access==1){
//                 write_access=2;
//                 send(C_conn,&write_access,sizeof(write_access),0);
//                 // close(C_conn);
//                 pthread_mutex_unlock(&mutex2);
//                 return ;
//             }
//             v=1;
//             WP_set.WP[WP_set.no_of_writepaths].write_access=1;
//             index=i;

//         }
//     }
//     if(v==0){
//         // printf("3\n");
//         if(WP_set.size==WP_set.no_of_writepaths){
//             WP_set.size=2*WP_set.size;
//             WP_set.WP=(write_path*)realloc(WP_set.WP,WP_set.size*sizeof(write_path) );

//         }
//         strcpy(WP_set.WP[ WP_set.no_of_writepaths].path,file_path);
//         WP_set.WP[WP_set.no_of_writepaths].write_access=1;
//         WP_set.no_of_writepaths++;
//         index=WP_set.no_of_writepaths-1;
        
//     }
//     pthread_mutex_unlock(&mutex2);
//     send(C_conn,&write_access,sizeof(write_access),0);
//     printf("here\n");
    // printf("hell\n");
   char buffer[1024];
    FILE *file = fopen(file_path, "w");
    if (file == NULL)
    {
        perror("Error creating file");
        // exit(EXIT_FAILURE);
        return -1;
    }

    size_t bytesRead;

    while ((bytesRead = recv(C_conn, buffer, sizeof(buffer), 0)) > 0)
    {
        if(!strcmp(buffer,"STOP"))
        break;
        printf("%s\n",buffer);
        fwrite(buffer, 1, bytesRead, file);
    }

    fclose(file);
    printf("file closed\n");

    // WP_set.WP[index].write_access=0;

}

void listDirectoriesRecursively(const char *path, char *dirPaths) {
    // printf("%d\n",path);
    // printf("%s\n",dirPaths);
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;

    dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        // exit(EXIT_FAILURE);
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue; // Skip current and parent directory entries
        }

        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

        if (stat(fullPath, &statbuf) == -1) {
            perror("Error getting file status");
            // exit(EXIT_FAILURE);
            return -1;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            strcat(dirPaths, fullPath);
            strcat(dirPaths, "\n");

            // Recursively list directories inside this one
            listDirectoriesRecursively(fullPath, dirPaths);
        }
    }

    closedir(dir);
}


void sendDirectoryListRecursively(int clientSocket, const char *directory) {
    char dirPaths[1024];
    memset(dirPaths, 0, sizeof(dirPaths));
    // printf("%s\n",directory);
    
    listDirectoriesRecursively(directory, dirPaths);

    // printf("%s %d\n",dirPaths,strlen(dirPaths));
    // char buffer[1024];
    // strcpy(buffer,"bchsjcs");
    // send(clientSocket,buffer,sizeof(buffer),0);
    // send(clientSocket,buffer,sizeof(buffer),0);
    // usleep(200000);
    send(clientSocket,dirPaths, sizeof(dirPaths), 0);
}


int copy_dir_S(int C_conn,char file_path[]){

    sendDirectoryListRecursively(C_conn, file_path);
    // edited=1;

}

int copy_init(int C_conn){
    copy_details C_d;
    size_t read_bytres;
    while(strcmp(C_d.type,"file") && strcmp(C_d.type,"dir") )
   read_bytres= recv(C_conn,&C_d,sizeof(C_d),0);
//    printf("SS%s \n",C_d.filepath);
//    printf("%ld\n",read_bytres);
    if(!strcmp(C_d.type,"file") && C_d.S_D=='S'){
        // printf("source\n");
        copy_file_S(C_conn,C_d.filepath);

    }
    else if(!strcmp(C_d.type,"file") && C_d.S_D=='D'){
        // printf("dest\n");
        copy_file_D(C_conn,C_d.filepath);
    }
    else if(!strcmp(C_d.type,"dir") && C_d.S_D=='S'){
        // printf("S\n");
        copy_dir_S(C_conn,C_d.filepath);
    }
}