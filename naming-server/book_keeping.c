#include "headers.h"

extern Server_set *S;
extern int *Sserver_conn;
extern Client_details *C;
extern sem_t *SS_sem;

// 1-> server joined
// 2-> server connection lost
// 3-> client joined
// 4-> client left
int book_conn(int code, int id)
{
    // printf("%d %d\n", S->no_of_SS, Sserver_conn[0]);
    char ip[16];
    int port;

    if (code == 1 || code == 2)
    {
        strcpy(ip, S->SS[id].SS_ip);
        port = S->SS[id].SS_Cport;
    }
    else if (code == 3 || code == 4)
    {
        strcpy(ip, C[id].C_ip);
        port = C[id].port;
    }

    FILE *file = fopen("book.txt", "a"); // "ab" for append in binary mode

    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    // Binary data to be appended
    // const char *str;
    if (code == 1)
    {
        fprintf(file, "storage server %d connected. ip: %s, port: %d.\n", id, ip, port);
        S->SS[id].active = 1;
    }
    else if (code == 2)
    {
        fprintf(file, "storage server %d disconnected. ip: %s, port: %d.\n", id, ip, port);
        S->SS[id].active = 0;
    }
    else if (code == 3)
        fprintf(file, "client %d connected. ip: %s, port: %d.\n", id, ip, port);
    else if (code == 4)
        fprintf(file, "client %d disconnected. ip: %s, port: %d.\n", id, ip, port);

    // Append the string to the file
    // if (fputs(str, file) == EOF) {
    //     perror("Error appending string");
    //     fclose(file);
    //     return 1;
    // }

    // Close the file
    fclose(file);

    printf("String appended successfully.\n");

    return 0;
}

int book_cmd(char cmd[], int C_id, int SS_id, int success)
{

    FILE *file = fopen("book.txt", "a"); // "ab" for append in binary mode

    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    if (success == 0)
    {
        if (SS_id == -1)
        {
            fprintf(file, "invalid path by client %d on operation '%s'.\n", C_id, cmd);
        }
        else
            fprintf(file, "operation '%s' by client %d, involing Storage server %d is  unsuccessful.\n", cmd, C_id, SS_id);
    }
    if (success == 1)
    {
        fprintf(file, "operation '%s' by client %d, involing Storage server %d is a success.\n", cmd, C_id, SS_id);
    }
    fclose(file);

    return 0;
}

void die(const char *message)
{
    perror(message);
    // exit(EXIT_FAILURE);
}

int check_connection(int C_conn, int SS_id)
{

    while (1)
    {

        sem_wait(&SS_sem[SS_id]);
        sleep(1);
        char buffer1[16];
        strcpy(buffer1, "check");

        send(C_conn, buffer1, sizeof(buffer1), 0);

        int sent_time = time(NULL);

        int flags = fcntl(C_conn, F_GETFL, 0);
        if (flags == -1)
        {
            die("fcntl");
        }
        if (fcntl(C_conn, F_SETFL, flags | O_NONBLOCK) == -1)
        {
            die("fcntl");
        }

        while (strcmp(buffer1, "ACK_Alive") && strcmp(buffer1,"ACK_Edited") )
        {
            recv(C_conn, &buffer1, sizeof(buffer1), 0);
            if (time(NULL) - sent_time > 2)
            {
                // perror("Storage server response timeout");
                // return 6;
                //         printf("here\n");
                //         send(C_conn, buffer1, sizeof(buffer1), 0);
                //         printf("here\n");
                //         while (strcmp(buffer1, "ACK_Alive"))
                // {
                //     recv(C_conn, &buffer1, sizeof(buffer1), 0);
                //     if (time(NULL) - sent_time > 2)
                //     {
                perror("Storage server response timeout");
                sem_post(&SS_sem[SS_id]);
                return 6;
                // }
            }
            
        }

        // }
        flags = fcntl(C_conn, F_GETFL, 0);
        if (flags == -1)
        {
            perror("Error getting socket flags");
            return -1;
        }

        // Clear the non-blocking flag
        flags &= ~O_NONBLOCK;

        // Set the modified flags
        if (fcntl(C_conn, F_SETFL, flags) == -1)
        {
            perror("Error setting socket flags");
            return -1;
        }
        sem_post(&SS_sem[SS_id]);
        if(!strcmp(buffer1,"ACK_Edited")){
                update_R(S->SS[SS_id].S1_idx,S->SS[SS_id].S2_idx,SS_id);
            }
        // printf("Alive\n");
        sleep(5);
    }
}

int first_time(char ip[16], int S_port)
{
    for (int i = 0; i < S->no_of_SS; i++)
    {
        if ((S->SS[i].SS_Cport == S_port) && !strcmp(S->SS[i].SS_ip, ip))
        {
            return i;
        }
    }

    return -1;
}