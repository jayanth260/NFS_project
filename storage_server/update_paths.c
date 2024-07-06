#include "Read_file.h"

int add_path(char path[]){

    FILE *file = fopen("input.txt", "a"); // "ab" for append in binary mode

    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    fprintf(file, "%s\n",path);

    fclose(file);
}

int remove_path(char path[]){

    char temp_path[1024];
    strcpy(temp_path,path);
    int len=strlen(temp_path);
    temp_path[len]='\n';
    temp_path[len+1]='\0';
    // strcat(temp_path,'\n');
    // printf("%s",temp_path);

    FILE *file = fopen("input.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Read the entire content of the file into memory
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *fileContent = (char *)malloc(fileSize + 1);
    if (fileContent == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return;
    }

    fread(fileContent, 1, fileSize, file);
    fclose(file);

    // Null-terminate the content
    fileContent[fileSize] = '\0';

    // Search for the string and remove it
    char *match = strstr(fileContent, temp_path);
    if (match != NULL) {
        memmove(match, match + strlen(temp_path), strlen(match + strlen(temp_path)) + 1);
    }

    // Open the file for writing and write the modified content
    file = fopen("input.txt", "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        free(fileContent);
        return;
    }

    fwrite(fileContent, 1, strlen(fileContent), file);

    fclose(file);
    free(fileContent);
    
}