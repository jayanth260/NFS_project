#include "Read_file.h"
#include <sys/stat.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

set_writepaths WP_set;
int edited = 0;

void *Read(void *arg)
{
    int *C_conn = (int *)arg;
    char file_path[1024];
    bzero(file_path, 1024);
    recv(*C_conn, &file_path, sizeof(file_path), 0);
    // printf("%s\n",file_path);
    FILE *file = fopen(file_path, "rb");

    if (file == NULL)
    {
        perror("Error opening file");
    }
    else
    {
        printf("file opened\n");
        char buffer[1024];
        size_t bytesRead;

        while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
        {
            send(*C_conn, buffer, bytesRead, 0);
        }
    }
    fclose(file);
    printf("file closed\n");

    char buffer[1024];
    strcpy(buffer, "STOP");
    send(*C_conn, buffer, sizeof(buffer), 0);

    close(*C_conn);
    printf("connection cosed\n");
}

void *Write(void *arg)
{

    int *C_conn = (int *)arg;
    char file_path[1024];
    bzero(file_path, 1024);
    recv(*C_conn, &file_path, sizeof(file_path), 0);
    // printf("%s\n", file_path);
    int write_permission = 0;
    if (access(file_path, W_OK) != 0)
    {
        write_permission = 3;
        send(*C_conn, &write_permission, sizeof(write_permission), 0);
        close(*C_conn);

        return;
    }

    send(*C_conn, &write_permission, sizeof(write_permission), 0);
    // printf("hell\n");
    pthread_mutex_lock(&mutex1);
    // printf("1\n");
    int v = 0;
    int write_access = 0;
    // printf("%d\n", WP_set.no_of_writepaths);
    // printf("4\n");
    for (int i = 0; i < WP_set.no_of_writepaths; i++)
    {
        // printf("2\n");
        if (!strcmp(WP_set.WP[i].path, file_path))
        {

            if (WP_set.WP[i].write_access == 1)
            {
                write_access = 2;
                send(*C_conn, &write_access, sizeof(write_access), 0);
                close(*C_conn);
                pthread_mutex_unlock(&mutex1);
                return;
            }
            v = 1;
            WP_set.WP[WP_set.no_of_writepaths].write_access = 1;
        }
    }
    if (v == 0)
    {
        // printf("3\n");
        if (WP_set.size == WP_set.no_of_writepaths)
        {
            WP_set.size = 2 * WP_set.size;
            WP_set.WP = (write_path *)realloc(WP_set.WP, WP_set.size * sizeof(write_path));
        }
        strcpy(WP_set.WP[WP_set.no_of_writepaths].path, file_path);
        WP_set.WP[WP_set.no_of_writepaths].write_access = 1;
        WP_set.no_of_writepaths++;
    }
    pthread_mutex_unlock(&mutex1);
    send(*C_conn, &write_access, sizeof(write_access), 0);

    FILE *file = fopen(file_path, "rb");

    if (file == NULL)
    {
        perror("Error opening file");
    }
    else
    {
        printf("file opened\n");
        char buffer[1024];
        size_t bytesRead;

        while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
        {
            send(*C_conn, buffer, bytesRead, 0);
        }
    }
    fclose(file);
    printf("file closed\n");

    usleep(500000);

    char buffer1[1024];
    strcpy(buffer1, "STOP");
    int len = strlen(buffer1);
    buffer1[len] = '\0';
    send(*C_conn, buffer1, sizeof(buffer1), 0);
    // printf("%s %d\n", buffer1, strlen(buffer1));

    file = fopen(file_path, "wb");
    if (file == NULL)
    {
        perror("Error creating file");
        // exit(EXIT_FAILURE);
        return -1;
    }

    bzero(buffer1, 1024);
    size_t bytesRead;

    while ((bytesRead = recv(*C_conn, buffer1, sizeof(buffer1), 0)) > 0)
    {
        if (!strcmp(buffer1, "STOP"))
            break;
        fwrite(buffer1, 1, bytesRead, file);
        bzero(buffer1, 1024);
    }

    fclose(file);

    close(*C_conn);

    printf("connection cosed\n");

    pthread_mutex_lock(&mutex1);
    for (int i = 0; i < WP_set.no_of_writepaths; i++)
    {
        if (!strcmp(WP_set.WP[i].path, file_path))
        {
            WP_set.WP[i].write_access = 0;
        }
    }
    pthread_mutex_unlock(&mutex1);

    edited = 1;
}

void *getinfo(void *arg){
    int *C_conn = (int *)arg;
    char file_path[1024];
    bzero(file_path,1024);
    recv(*C_conn, &file_path, sizeof(file_path), 0);
    printf("%s\n",file_path);
    struct get_info info;
    struct stat file_stat;
    if (stat(file_path, &file_stat) != 0)
    {
        perror("Error getting file information");
        return 1; // Return an error code or handle accordingly
    }

    char perm[10];
    // Display file permissions
    printf("File Permissions: ");
    // printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
    if((file_stat.st_mode & S_IRUSR)){
        perm[0] = 'r';
    }
    else{
        perm[0] = '-';
    }
    if((file_stat.st_mode & S_IWUSR)){
        perm[1] = 'w';
    }
    else{
        perm[1] = '-';
    }
    if((file_stat.st_mode & S_IXUSR)){
        perm[2] = 'x';
    }
    else{
        perm[2] = '-';
    }
    if((file_stat.st_mode & S_IRGRP)){
        perm[3] = 'r';
    }
    else{
        perm[3] = '-';
    }
    if((file_stat.st_mode & S_IWGRP)){
        perm[4] = 'w';
    }
    else{
        perm[4] = '-';
    }
    if((file_stat.st_mode & S_IXGRP)){
        perm[5] = 'x';
    }
    else{
        perm[5] = '-';
    }
    if((file_stat.st_mode & S_IROTH)){
        perm[6] = 'r';
    }
    else{
        perm[6] = '-';
    }
    if((file_stat.st_mode & S_IWOTH)){
        perm[7] = 'w';
    }
    else{
        perm[7] = '-';
    }
    if((file_stat.st_mode & S_IXOTH)){
        perm[8] = 'x';
    }
    else{
        perm[8] = '-';
    }
    printf("%s\n\n",perm);

    // char info.perm[10];
    bzero(info.perm, 10);
    strcpy(info.perm, perm);
    // send(*C_conn, i, sizeof(i), 0);
    // usleep(100000);

    // int bytes;
    // printf("File Size: %d bytes\n", file_stat.st_size);
    info.size = file_stat.st_size;
    printf("%d\n\n",info.size);

    // send(*C_conn, &bytes, sizeof(bytes), 0);
    // usleep(100000);
    // Convert last modification time to a human-readable format
    char mod_time[20];
    strftime(mod_time, sizeof(mod_time), "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_mtime));
    printf("Last Modified Time: %s\n", mod_time);
    // char buffer2[1024];
    bzero(info.time, 20);
    strcpy(info.time, mod_time);
    send(*C_conn, &info, sizeof(info), 0);
    // send(*C_conn, mod_time, sizeof(mod_time), 0);




    printf("Connection closed\n");
    return 0;
}
