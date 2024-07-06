#include "create_delete.h"
#include "headers.h"
#include <pthread.h>

pthread_mutex_t mutex;

extern Server_set *S;
extern int *Sserver_conn;

int i = 0;

int add_path(int index, char path[])
{

    S->SS[index].paths = (acc_path *)realloc(S->SS[index].paths, (++S->SS[index].no_of_paths) * sizeof(acc_path));
    strcpy(S->SS[index].paths[S->SS[index].no_of_paths - 1].path, path);

    // for (int i = 0; i < S->SS[index].no_of_paths; i++)
    //     printf("%s\n", S->SS[index].paths[i].path);

    S->SS[index].paths->SS_num = index;


    

    return 0;
}

int remove_path(int index, char path[])
{   
  
// printf("%s\n",path);
    char temp[256];
    strcpy(temp,path);
    strcat(temp, "/");
    
    for (int i = 0; i < S->SS[index].no_of_paths; ++i)
    {
        // printf("%d\n",i);
        if (!strcmp(S->SS[index].paths[i].path, path) || strstr(S->SS[index].paths[i].path,temp)!=NULL)
        {
            // Found the target string, shift the remaining elements
            for (int j = i; j < S->SS[index].no_of_paths - 1; ++j)
            {
                bzero(S->SS[index].paths[j].path, 128);
                strcpy(S->SS[index].paths[j].path, S->SS[index].paths[j + 1].path);
                S->SS[index].paths[j].SS_num = S->SS[index].paths[j + 1].SS_num;
            }
            // Decrement the size
            S->SS[index].no_of_paths--;
            // Break to avoid skipping the next element
            // break;
        }
    }

    return 0;
}

int Request_Create_delete(char buffer[], int sock, int index, int add)
{
    // sleep(10);
    // pthread_mutex_lock(&mutex);
    // i++;
    char temp[1024];
    char op[1024];
    strcpy(temp, buffer);
    // printf("SS-%s\n", temp);
    // char tem[1024];
    // strcpy(tem, "hello");
    strcpy(op, strtok(temp, " "));
    send(sock, op, sizeof(op), 0);
    create_delete_details CD_d;
    strcpy(CD_d.op, op);
    // strcpy(CD_d.op,"bvhfdskcnscnasdc");
    strcpy(CD_d.type, strtok(NULL, " "));
    // char file_path[256];
    strcpy(CD_d.file_path, strtok(NULL, " "));
    // printf("%s %s %s \n", CD_d.file_path, CD_d.op, CD_d.type);
    CD_d.add = add;
    send(sock, &CD_d, sizeof(CD_d), 0);
    // printf("ee%d %s\n", i, file_path);
    // if(!strcmp(op,"create")){
    int already_exists=0;
    // char already[2];
    // printf("%d\n", time(NULL));
    size_t read_bytes;
    // while(strlen(already)==0)
    //  read_bytes= recv(sock, already, sizeof(already), 0);
    // printf("%s\n",already);
    // already_exists=atoi(already);
    // if(read_bytes==0)
    while(already_exists==0)
    recv(sock, &already_exists, sizeof(already_exists), 0);
    // printf("%d\n", already_exists);
    if (already_exists == 4)
    {
        // printf("43\n");
        perror("file/dir alreay exists!");
        // close(sock);
        // pthread_mutex_unlock(&mutex);
        return 4;
    }
    else if (already_exists == 7)
    {
        perror("file/dir already deleted.");
        // pthread_mutex_unlock(&mutex);
        return 7;
    }
    // }
    int permission = 0;

    recv(sock, &permission, sizeof(permission), 0);
    if (permission == 3)
    {
        perror("no required permission");
        // close(sock);
        // pthread_mutex_unlock(&mutex);
        return 3;
    }

    char done[10];
    recv(sock, &done, sizeof(done), 0);

    if (!strcmp(done, "notdone"))
    {
        perror("cant create/delete file/dir");
        // pthread_mutex_unlock(&mutex);
        return 5;
    }

    // printf("123\n");
    if (!strcmp(CD_d.op, "create") && add == 1)
    {

        add_path(index, CD_d.file_path);
    }
    else if (!strcmp(CD_d.op, "delete"))
    {
        //    printf("234\n");
        remove_path(index, CD_d.file_path);
    }

    // pthread_mutex_unlock(&mutex);

    return 0;
}

int copy_file(copy_details copy_d_S, copy_details copy_d_D, int sour_sock, int dest_sock)
{

    char op[8];
    strcpy(op, "copy");
    size_t length = strlen(copy_d_S.filepath);

    char *file_name = (char *)malloc(length + 1);

    strcpy(file_name, copy_d_S.filepath);

    for (size_t i = 0; i < length; ++i)
    {
        if (file_name[i] == '/')
        {
            file_name[i] = '_';
        }
    }

    // char buffer[1024];
    printf("file opened\n");
    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
        perror("Error creating file");
        // exit(EXIT_FAILURE);
    }

    size_t bytesRead;
    char buffer1[1024];

    memset(buffer1, 0, sizeof(buffer1));
    while ((bytesRead = recv(sour_sock, buffer1, sizeof(buffer1), 0)) > 0)
    {
        if (!strcmp(buffer1, "STOP"))
            break;
        // printf("DD%s\n", buffer1);
        fprintf(file, "%s", buffer1);
    }

    fclose(file);
    printf("file closed\n");
    send(dest_sock, op, sizeof(op), 0);
    send(dest_sock, &copy_d_D, sizeof(copy_d_D), 0);
    int write_permission = 0;
    recv(dest_sock, &write_permission, sizeof(write_permission), 0);
    if (write_permission == 3)
    {
        perror("no permission to write");
        return 3;
    }

    // int write_access = 0;

    // recv(dest_sock, &write_access, sizeof(write_access), 0);
    // if (write_access == 2)
    // {
    //     perror("other client is writing to same file");
    //     return 2;
    // }
    file = fopen(file_name, "r");

    if (file == NULL)
    {
        perror("Error opening file");
    }
    else
    {
        printf("file opened\n");
        char buffer1[1024];
        size_t bytesRead;

        while ((bytesRead = fread(buffer1, 1, sizeof(buffer1), file)) > 0)
        {
            printf("%s\n", buffer1);
            send(dest_sock, buffer1, bytesRead, 0);
        }
    }
    fclose(file);
    printf("file closed\n");
    usleep(500000);

    strcpy(buffer1, "STOP");
    send(dest_sock, buffer1, sizeof(buffer1), 0);

    if (remove(file_name) != 0) {
      perror("Error deleting file");
      return -1;
      
      }
    
    

    return 0;
}

void removeCommonPrefix(char *target, const char *common)
{
    size_t commonLength = strlen(common);
    size_t targetLength = strlen(target);

    if (strncmp(target, common, commonLength) == 0)
    {
        size_t newLength = targetLength - commonLength;
        memmove(target, target + commonLength, newLength + 1);
    }
}

int copy_all_files(copy_details copy_d_S, copy_details copy_d_D, int sour_sock, int dest_sock, int dest_index, int source_index)
{

    char temp1[256];
    strcpy(temp1, copy_d_S.filepath);
    char temp2[256];
    char *token = strtok(temp1, "/");
    while (token != NULL)
    {
        strcpy(temp2, token);
        token = strtok(NULL, "/");
    }

    char first_dir[256];
    strcpy(first_dir, copy_d_D.filepath);
    int len1 = strlen(first_dir);
    first_dir[len1] = '\0';

    strcat(first_dir, "/");
    strcat(first_dir, temp2);
    // printf("%s\n", first_dir);

    char prefix[] = "create file ";

    // Calculate the length needed for the result
    size_t resultLength = strlen(prefix) + strlen(first_dir) + 1; // +1 for null terminator

    char result[resultLength];
    strncpy(result, prefix, sizeof(result));
    strncat(result, first_dir, sizeof(result) - strlen(prefix) - 1);
    for (int i = 0; i < S->SS[source_index].no_of_paths; i++)
    {
        // printf("FF%d\n",i);
        char *temp = strstr(S->SS[source_index].paths[i].path, copy_d_S.filepath);
        if (temp != NULL)
        {
            int len1 = strlen(copy_d_S.filepath);
            if (S->SS[source_index].paths[i].path[len1] == '/')
            {

                char temp1[256];
                strcpy(temp1, S->SS[source_index].paths[i].path);
                removeCommonPrefix(temp1, copy_d_S.filepath);
                char temp3[256];
                strcpy(temp3, result);
                strcat(temp3, temp1);
                // printf("%s\n", temp3);

                if (Request_Create_delete(temp3, dest_sock, dest_index, 1) != 4)
                {
                    // copy_details C_Sfiled;
                    // strcpy(C_Sfiled.filepath, S->SS[source_index].paths[i].path);
                    // strcpy(C_Sfiled.type, "file");
                    // C_Sfiled.S_D = 'S';
                    // copy_details C_Dfiled;
                    // strcpy(C_Dfiled.filepath, temp3);
                    // strcpy(C_Dfiled.type, "file");
                    // C_Dfiled.S_D = 'D';

                    char temp4[1024];
                    removeCommonPrefix(temp3,"create file ");
                    strcpy(temp4,"copy file ");
                    strcat(temp4,S->SS[source_index].paths[i].path);
                    strcat(temp4, " ");
                    strcat(temp4,temp3);
                    // printf("done\n");
                    printf("%s\n",temp4);
                       int check=Request_copy(temp4,source_index,dest_index);         // copy_file(C_Sfiled, C_Dfiled, sour_sock, dest_sock);
                    if(check!=0)
                    return check;
                    // printf("done2\n");
                }
                // printf("ff%d\n",i);
            }
        }
    }

    return 0;
}

int copy_dir(copy_details copy_d_S, copy_details copy_d_D, int sour_sock, int dest_sock, int dest_index, int source_index)
{
    char receivedDirPaths[1024];
    char receivedFilePaths[1024];

    memset(receivedDirPaths, 0, sizeof(receivedDirPaths));
    // Receive directory paths
    // recv(sour_sock,receivedDirPaths,sizeof(receivedDirPaths),0);
    recv(sour_sock, receivedDirPaths, sizeof(receivedDirPaths), 0);
    // printf("%d\n",strlen(receivedDirPaths));
    if (strlen(receivedDirPaths) == 0)
        recv(sour_sock, receivedDirPaths, sizeof(receivedDirPaths), 0);
    printf("Received directory paths:\n%s\n", receivedDirPaths);

    // Receive file paths
    // receivePaths(clientSocket, receivedFilePaths);
    // printf("\nReceived file paths:\n%s", receivedFilePaths);

    char temp1[256];
    strcpy(temp1, copy_d_S.filepath);
    char temp2[256];
    char *token = strtok(temp1, "/");
    while (token != NULL)
    {
        strcpy(temp2, token);
        token = strtok(NULL, "/");
    }

    char first_dir[256];
    strcpy(first_dir, copy_d_D.filepath);
    int len1 = strlen(first_dir);
    first_dir[len1] = '\0';

    strcat(first_dir, "/");
    strcat(first_dir, temp2);
    // printf("%s\n", first_dir);

    char prefix[] = "create dir ";

    // Calculate the length needed for the result
    size_t resultLength = strlen(prefix) + strlen(first_dir) + 1; // +1 for null terminator

    char result[resultLength];
    strncpy(result, prefix, sizeof(result));
    strncat(result, first_dir, sizeof(result) - strlen(prefix) - 1);
    // printf("%s\n", result);
int ret;
    ret=Request_Create_delete(result, dest_sock, dest_index, 1);
    if(ret!=0)
    return ret;
    char **array = NULL;
    int num = 0;
    char *token1 = strtok(receivedDirPaths, "\n");

    int *add = (int *)malloc(sizeof(int));
    while (token1 != NULL)
    {
        num++;
        char *newString = strdup(token1);
        array = (char **)realloc(array, sizeof(char *) * (int)(num + 1));
        add = (int *)realloc(add, (num + 1) * sizeof(int));
        add[num - 1] = 0;
        for (int i = 0; i < S->SS[source_index].no_of_paths; i++)
        {
            if (!strcmp(newString, S->SS[source_index].paths[i].path))
                add[num - 1] = 1;
        }
        array[num - 1] = newString;
        token1 = strtok(NULL, "\n");
    }

    for (int i = 0; i < num; i++)
    {

        removeCommonPrefix(array[i], copy_d_S.filepath);
        char temp3[256];
        strcpy(temp3, result);
        strcat(temp3, array[i]);
        // printf("%s\n", temp3);
        // int add=0;

        ret=Request_Create_delete(temp3, dest_sock, dest_index, add[i]);
        if(ret!=0)
        return ret;
    }
    for (int i = 0; i < num; i++)
    {
        free(array[i]);
    }

    free(array);

    return copy_all_files(copy_d_S,copy_d_D,sour_sock,dest_sock,dest_index,source_index);

}
int temp_time=500000;

int Request_copy(char buffer[], int source_index, int dest_index)
{
    char temp[1024];
    strcpy(temp, buffer);
    int sour_sock = Sserver_conn[source_index];
    int dest_sock = Sserver_conn[dest_index];

    char op[8];
    strcpy(op, strtok(temp, " "));

    send(sour_sock, op, sizeof(op), 0);

    copy_details copy_d_S;
    copy_details copy_d_D;
    // strcpy(copy_d.op,op);
    strcpy(copy_d_S.type, strtok(NULL, " "));
    strcpy(copy_d_D.type, copy_d_S.type);
    strcpy(copy_d_S.filepath, strtok(NULL, " "));
    strcpy(copy_d_D.filepath, strtok(NULL, " "));
    copy_d_S.S_D = 'S';
    copy_d_D.S_D = 'D';
    temp_time=temp_time+500;
    usleep(temp_time);
    send(sour_sock, &copy_d_S, sizeof(copy_d_S), 0);

    if (!strcmp(copy_d_D.type, "file"))
    {
        return copy_file(copy_d_S, copy_d_D, sour_sock, dest_sock);
    }
    else if (!strcmp(copy_d_D.type, "dir"))
    {

        return copy_dir(copy_d_S, copy_d_D, sour_sock, dest_sock, dest_index, source_index);
    }

    return -1;
}