#include "headers.h"

extern Server_set *S;

SServer_addr get_Sserver(char file_path[], char op[])
{

  SServer_addr address;
  for (int i = 0; i < S->no_of_SS; i++)
  {
    for (int j = 0; j < S->SS[i].no_of_paths; j++)
    {
      // printf("%s\n",S->SS[i].paths[j].path);
      // printf("in\n");
      // printf("%s\n", S->SS[i].accessible_paths[j]);
      if (!strcmp(file_path, S->SS[i].paths[j].path) && S->SS[i].paths[j].SS_num == i)
      {
        if (S->SS[i].active == 0)
        {
          if (!strcmp(op, "read"))
          {
            if (S->SS[i].S1_idx != -1 && S->SS[S->SS[i].S1_idx].active == 1)
            {
              address.port = S->SS[S->SS[i].S1_idx].SS_Cport;
              strcpy(address.SS_ip, S->SS[S->SS[i].S1_idx].SS_ip);
              return address;
            }
            else if (S->SS[i].S2_idx != -1 && S->SS[S->SS[i].S2_idx].active == 1)
            {
              address.port = S->SS[S->SS[i].S2_idx].SS_Cport;
              strcpy(address.SS_ip, S->SS[S->SS[i].S2_idx].SS_ip);
              return address;
            }
          }
          else
          {
            address.port = -1;
            strcpy(address.SS_ip, "nope");
            return address;
          }
        }
        else
        {
          // printf("%d\n", *client_conn);
          // printf("%s\n",file_path);
          address.port = S->SS[i].SS_Cport;
          // printf("%d\n",address.port);
          strcpy(address.SS_ip, S->SS[i].SS_ip);
          address.index = i;

          return address;
        }
      }
    }
  }

  address.port = -1;
  strcpy(address.SS_ip, "nope");
  return address;
}