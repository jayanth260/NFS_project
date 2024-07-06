#include "Read_file.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

// extern int edited;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int check_file_permission(const char *path, int mode)
{
    return access(path, mode);
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
void extractSubstringAfterLastSlash(const char *input, char *output)
{
    // Find the last occurrence of '/'
    const char *lastSlash = strrchr(input, '/');

    if (lastSlash != NULL)
    {
        // Copy the substring after the last '/'
        strcpy(output, lastSlash + 1);
    }
    else
    {
        // If no '/' is found, set the output string to an empty string
        output[0] = '\0';
    }
}
int fileExists(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file)
    {
        fclose(file);
        return 1; // File exists
    }
    else
    {
        return 0; // File doesn't exist
    }
}

int directoryExists(const char *path)
{
    DIR *dir = opendir(path);
    if (dir)
    {
        closedir(dir);
        return 1; // Directory exists
    }
    else
    {
        return 0; // Directory doesn't exist
    }
}

int deleteContents(const char *dirPath)
{
    DIR *dir = opendir(dirPath);
    struct dirent *entry;

    if (!dir)
    {
        perror("Error opening directory");
        // exit(EXIT_FAILURE);
        return -1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            char fullPath[256];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

            DIR *subdir = opendir(fullPath);

            if (subdir)
            {
                // It's a directory, so recursively delete its contents
                closedir(subdir);
                deleteContents(fullPath);

                // Remove the empty subdirectory
                if (rmdir(fullPath) != 0)
                {
                    perror("Error removing directory");
                    return -1;
                }
            }
            else
            {
                // It's a file, remove it
                if (remove(fullPath) != 0)
                {
                    perror("Error removing file");
                    return -1;
                }
            }
        }
    }

    closedir(dir);
}

int create_delete(int C_conn)
{
    // printf("vgdsvcgdc\n");

    char parent_dir[256];
    // printf("hey\n");
    create_delete_details CD_d;
    recv(C_conn, &CD_d, sizeof(CD_d), 0);
    // printf("%s %s %s\n", CD_d.file_path, CD_d.op, CD_d.type);
    extractSubstring(CD_d.file_path, parent_dir);
    char file_dir[256];
    extractSubstringAfterLastSlash(CD_d.file_path, file_dir);

    // printf("CS%d\n", C_conn);
    int permission = 0;
    int already_exists = 0;
    if (!strcmp(CD_d.op, "create"))
    {
        // printf("a\n");
        // pthread_mutex_lock(&mutex);
        if (!strcmp(CD_d.type, "file"))
        {
            // printf("b\n");
            if (fileExists(CD_d.file_path))
            {
                already_exists = 4;
                // sleep(1);
                // printf("%d\n", already_exists);
                // char already[2];
                // strcpy(already,"4");
                send(C_conn, &already_exists, sizeof(already_exists), 0);
                // printf("%d\n", already_exists);
                // close(C_conn);
                // pthread_mutex_unlock(&mutex);
                return -1;
            }
        }
        if (!strcmp(CD_d.type, "dir"))
        {
            // printf("c\n");
            if (directoryExists(CD_d.file_path))
            {
                already_exists = 4;
                // char already[2];
                // strcpy(already,"4");
                // send(C_conn, already, sizeof(already), 0);
                send(C_conn, &already_exists, sizeof(already_exists), 0);
                // close(C_conn);
                // pthread_mutex_unlock(&mutex);
                return -1;
            }
        }
        // printf("d\n");
        // already_exists=0;
        //     char already[2];
        //             strcpy(already,"0");
        //             send(C_conn, already, sizeof(already), 0);
        already_exists = 1;
        send(C_conn, &already_exists, sizeof(already_exists), 0);
        if (check_file_permission(parent_dir, W_OK) != 0)
        {
            // printf("e\n");

            permission = 3;
            send(C_conn, &permission, sizeof(permission), 0);
            // close(C_conn);
            // pthread_mutex_unlock(&mutex);
            return -1;
        }
        permission=1;
        send(C_conn, &permission, sizeof(permission), 0);

        if (!strcmp(CD_d.type, "file"))
        {
            // printf("hey\n");
            // FILE *file = fopen(CD_d.file_path, "w");
            int file_descriptor = open(CD_d.file_path, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);

            if (file_descriptor == -1)
            {
                // Handle error if open() fails
                perror("Error creating file");
                return 1;
            }
        }
        else
        {
            // mkdir(CD_d.file_path, S_IRWXU);
            mkdir(CD_d.file_path, S_IRWXU | S_IRWXG | S_IRWXO);
        }
        if (CD_d.add == 1)
            add_path(CD_d.file_path);
        char done[8];
        strcpy(done, "done");
        // edited=1;
        send(C_conn, done, sizeof(done), 0);
        // pthread_mutex_unlock(&mutex);
        return 0;
    }
    else if (!strcmp(CD_d.op, "delete"))
    {
        // printf("1\n");
        if (!strcmp(CD_d.type, "file"))
        {
            // printf("2\n");
            if (!fileExists(CD_d.file_path))
            {
                // printf("3\n");
                already_exists = 7;
                // char already[2];
                // strcpy(already,"7");
                // send(C_conn, already, sizeof(already), 0);
                // printf("CS%d%d ",C_conn,already_exists);
                send(C_conn, &already_exists, sizeof(already_exists), 0);
                // printf("90\n");
                // printf("CS%d",C_conn);
                // close(C_conn);
                // pthread_mutex_unlock(&mutex);
                return -1;
            }
        }
        if (!strcmp(CD_d.type, "dir"))
        {
            if (!directoryExists(CD_d.file_path))
            {
                already_exists = 7;
                // char already[2];
                // strcpy(already,"7");
                // send(C_conn, already, sizeof(already), 0);
                send(C_conn, &already_exists, sizeof(already_exists), 0);
                // close(C_conn);
                // pthread_mutex_unlock(&mutex);
                return -1;
            }
        }
        // char already[2];
        //         strcpy(already,"0");
        //         send(C_conn, already, sizeof(already), 0);
        already_exists = 1;
        send(C_conn, &already_exists, sizeof(already_exists), 0);

        // already_exists = 4;
        // send(C_conn, &already_exists, sizeof(already_exists), 0);
        // // close(C_conn);
        //// pthread_mutex_unlock(&mutex);
        // return NULL;

        int permission = 0;
        if (check_file_permission(parent_dir, W_OK) != 0)
        {
            // printf("4\n");
            permission = 3;
            send(C_conn, &permission, sizeof(permission), 0);
            // close(C_conn);
            // pthread_mutex_unlock(&mutex);
            return -1;
        }
        // printf("5\n");
        permission=1;
        send(C_conn, &permission, sizeof(permission), 0);

        if (!strcmp(CD_d.type, "file"))
        {
            // printf("6\n");
            if (remove(CD_d.file_path) != 0)
            {
                perror("Error deleting file");
                char done[10];
                strcpy(done, "notdone");
                send(C_conn, done, sizeof(done), 0);
                // pthread_mutex_unlock(&mutex);
                return -1;
            }
        }
        else
        {
            if (deleteContents(CD_d.file_path) == -1 || rmdir(CD_d.file_path)!=0)
            {
                perror("Error deleting directory");
                char done[10];
                strcpy(done, "notdone");
                send(C_conn, done, sizeof(done), 0);
                // pthread_mutex_unlock(&mutex);
                return -1;
            }
        }
        // printf("done\n");
        remove_path(CD_d.file_path);
        char done[8];
        // edited=1;
        strcpy(done, "done");
        send(C_conn, done, sizeof(done), 0);
        // printf("8\n");
        // pthread_mutex_unlock(&mutex);
        // close(C_conn);
        return 0;
    }
}
