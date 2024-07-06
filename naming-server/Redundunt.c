#include "headers.h"
#include "create_delete.h"

extern Server_set *S;
extern int *Sserver_conn;
extern sem_t *SS_sem;

int change_S_num(int S_index, int D_index)
{
    for (int i = 0; i < S->SS[D_index].no_of_paths; i++)
    {
        for (int j = 0; j < S->SS[S_index].no_of_paths; j++)
        {
            if ( S->SS[S_index].paths[j].SS_num==S_index && !strcmp( S->SS[D_index].paths[i].path, S->SS[S_index].paths[j].path) && strcmp(S->SS[D_index].paths[i].path, "." ))
            {
                S->SS[D_index].paths[i].SS_num = S_index;
                break;
            }
        }
    }
    return 0;
}

int delete_files(int S_index, int D_index)
{

    char temp[1024];
    strcpy(temp, "delete dir ");
    strcat(temp, S->SS[S_index].paths[0].path);
    // printf("%s\n", temp);

    sem_wait(&SS_sem[D_index]);
    Request_Create_delete(temp, Sserver_conn[D_index], D_index, 0);
    sem_post(&SS_sem[D_index]);
    return 0;
}

int update_R(int RS1_index,int RS2_index,int S_index ){
    

    if(RS1_index!=-1 && S->SS[RS1_index].active==1 ){
        char temp1[1024];
            strcpy(temp1, "copy dir ");
            strcat(temp1, S->SS[S_index].paths[0].path);
            strcat(temp1, " ");
            strcat(temp1, ".");
    delete_files(S_index,RS1_index);
    sem_wait(&SS_sem[S_index]);
            sem_wait(&SS_sem[RS1_index]);
            Request_copy(temp1, S_index, RS1_index);

            sem_post(&SS_sem[RS1_index]);
            sem_post(&SS_sem[S_index]);
    }
    if(RS2_index!=-1 && S->SS[RS2_index].active==1){
        char temp1[1024];
            strcpy(temp1, "copy dir ");
            strcat(temp1, S->SS[S_index].paths[0].path);
            strcat(temp1, " ");
            strcat(temp1, ".");
    delete_files(S_index,RS2_index);
    sem_wait(&SS_sem[S_index]);
            sem_wait(&SS_sem[RS2_index]);
            Request_copy(temp1, S_index, RS2_index);

            sem_post(&SS_sem[RS2_index]);
            sem_post(&SS_sem[S_index]);

    }

    return 0;

}

int duplicate_S(int index, int now)
{
    
        int first = 0;
        if (S->SS[index].S1_idx == -1)
        {
            for (int j = index - 1; j >= 0; j--)
            {
                if (S->SS[j].active == 1)
                {
                    S->SS[index].S1_idx = j;
                    break;
                }
            }
            if (S->SS[index].S1_idx == -1)
            {
                for (int j = index + 1; j < S->no_of_SS; j++)
                {
                    if (S->SS[index].active == 1)
                    {
                        S->SS[index].S1_idx = j;
                        break;
                    }
                }
            }
            first = 1;
        }
        if(S->SS[index].S1_idx!=-1){
        if (now == 1 || first == 1)
        {

            char temp1[1024];
            strcpy(temp1, "copy dir ");
            strcat(temp1, S->SS[index].paths[0].path);
            strcat(temp1, " ");
            strcat(temp1, ".");

            if (first == 0)
                delete_files(index, S->SS[index].S1_idx);

            sem_wait(&SS_sem[index]);
            sem_wait(&SS_sem[S->SS[index].S1_idx]);
            Request_copy(temp1, index, S->SS[index].S1_idx);

            sem_post(&SS_sem[S->SS[index].S1_idx]);
            sem_post(&SS_sem[index]);
            change_S_num(index, S->SS[index].S1_idx);
        }
        S->SS[index].S1_set=1;
        }
        // if (S->SS[S->SS[index].S1_idx].S1_idx == -1 || S->SS[S->SS[index].S1_idx].S2_idx == -1)
        //     duplicate_S(S->SS[index].S1_idx, 0);
    
    

        first = 0;
        if (S->SS[index].S2_idx == -1)
        {
            for (int j = index - 1; j >= 0; j--)
            {
                if (S->SS[j].active == 1 && S->SS[index].S1_idx != j)
                    S->SS[index].S2_idx = j;
            }
            if (S->SS[index].S2_idx == -1)
            {
                for (int j = index + 1; j < S->no_of_SS; j++)
                {
                    if (S->SS[index].active == 1 && S->SS[index].S1_idx != j)
                    {
                        S->SS[index].S2_idx = j;
                        break;
                    }
                }
            }
            first = 1;
        }
        if(S->SS[index].S2_idx!=-1){
        if ( now == 1 || first == 1)
        {

            char temp1[1024];
            strcpy(temp1, "copy dir ");
            strcat(temp1, S->SS[index].paths[0].path);
            strcat(temp1, " ");
            strcat(temp1, ".");

            if (first == 0)
                delete_files(index, S->SS[index].S2_idx);

            sem_wait(&SS_sem[index]);
            sem_wait(&SS_sem[S->SS[index].S1_idx]);
            Request_copy(temp1, index, S->SS[index].S2_idx);
            sem_post(&SS_sem[S->SS[index].S1_idx]);
            sem_post(&SS_sem[index]);
            change_S_num(index, S->SS[index].S2_idx);
        }
        S->SS[index].S2_set=1;
        }
        // if (S->SS[S->SS[index].S2_idx].S1_idx == -1 || S->SS[S->SS[index].S2_idx].S2_idx == -1)
        //     duplicate_S(S->SS[index].S2_idx, 0);

    
    
    return 0;
}