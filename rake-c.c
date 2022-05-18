/**
 *  CITS3002 2022 Sem 1 - Project - rake client C
 * @authors
 *  - Daivik Anil 22987816
 *  - Nathan Eden 22960674
 *  - Reiden Rufin 22986337
 */

#include <stdio.h>
#include <stdlib.h>
#include "strsplit.c"
#include "c-client.h"
//-------------------------------------------------------------------------


char hosts[][BUFFSIZE] = {};

int portnumber;

struct {
    char actionCommand[BUFFSIZE];
    char **requirements;
    int requirementnum;
} actionsets[BUFFSIZE][BUFFSIZE];

int setcount = 0;
int actioncounts[BUFFSIZE];

// actionsets[actionsetnum][???] will be the max length for the first array
// actionsets[0][actioncounts[0]] will be the max length for the second array of the first index
// actionsets[1][actioncounts[1]] will be the max length for the second array of the second index
// requirements are already dynamic, so there's no need to change it

//-------------------------------------------------------------------------


void read_rakefile(char *rakefile){
    FILE *fptr = fopen(rakefile, "r");

    if (fptr == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    int setnum = -1;
    int actionnum = -1;
    int nwords;

    while (fgets (buffer, 12, fptr))
    {
        buffer[strcspn (buffer, "#\r\n")] = 0;  /* trim comment or line-ending */
        
        if (StartsWith(buffer, "\t")) //check if line is one tabbed - these are the "actions"
        {
            
            if (StartsWith(buffer, "\t\t")) //check if line is two tabs - these are the "requires"
            {
                char **splitreqs = strsplit(buffer, &nwords);
                actionsets[setnum][actionnum-1].requirements = malloc((nwords-1) * sizeof(char*));
                
                for (int x = 1; x < nwords; x++)
                {
                    actionsets[setnum][actionnum-1].requirementnum++;
                    actionsets[setnum][actionnum-1].requirements[x-1] = malloc(sizeof(splitreqs[x]));
                    actionsets[setnum][actionnum-1].requirements[x-1] = strdup(splitreqs[x]);
                    if(longest_requirements_line > BUFFSIZE)
                    {
                        //do some shit idk
                    }
                }
            }
            else
            {

                if(total_actions_count > BUFFSIZE)
                {
                    //do some shit idk
                }
                strcpy(actionsets[setnum][actionnum].actionCommand, trimwhitespace(buffer));
                actionsets[setnum][actionnum].requirementnum = 0;
                actionnum++;
            }
        }

        else
        {
            if(strstr(buffer, "PORT"))
            {
                char **words = strsplit(buffer, &nwords);
                portnumber = atoi(words[2]);
            }
            else if(strstr(buffer, "HOSTS"))
            {
                char **words = strsplit(buffer, &nwords);
                for(int i = 2; i < nwords; i++)
                {
                    strcpy(hosts[i-1], words[i]);
                }
            }
            else if (strstr(buffer, ":"))
            {
                if(total_actionset_count > BUFFSIZE)
                {
                    //do some shit idk
                }
                if (setnum != -1) 
                {
                    actioncounts[setnum] = actionnum;
                }
                setnum++;
                setcount++;

                actionnum = 0;

            }
        }
    }
    actioncounts[setnum] = actionnum;
    //printf("total_action_count at [%d]: %d\n", setnum, total_action_count);
    fclose(fptr);
}

//--------------------------------------------------------------------------------------------------------------------------


// void send_message(int sock, int valread, char buffer[], char* message)
// {
//     printf("Sending message from C Client...\n");
//     send(sock , message, strlen(message) , 0 );
//     valread = read(sock, buffer, 1024);
//     printf("Received the message: %s\n", buffer);
// }

//No its not done, no it doesnt work, c makes me wanna kill myself
int quote_servers(int index){
    int connections [BUFFSIZE];
    float min_cost = __FLT_MAX__;
    char* requirements;
    for (int i = 0; i <= hosts[0][BUFFSIZE]; i++)
    {
        int port = portnumber;
        printf("this is the value of i: %d\n", i);
        if (count_char(hosts[0][i], ':') > 0)
        {
            port = atoi(strtok(hosts[0][i], ":"));
            hosts[0][i] = strtok(hosts[0][i], ":");
        }
        int cumsock;
        cumsock = socket(AF_INET , SOCK_STREAM , 0);
        if (cumsock == -1){
            printf("Could not create socket\n");
        }
        struct sockaddr_in server;
        struct sockaddr_in {
            short sin_family;
            unsigned short sin_port;
            struct in_addr sin_addr;
            char sin_zero[8];
        };        
        struct in_addr {
            unsigned long s_addr;
        };
        server.sin_addr.s_addr = inet_addr(hosts[0][i]);
        server.sin_family = AF_INET;
        server.sin_port = htons(port);

        if(connect(cumsock, (struct sockaddr *)&server, sizeof(server)) < 0){
            printf("Connect failed. Error\n");
            return;
        }
        else{
            printf("Connected\n");
        }

        if(actionsets[0][index].requirements > 1){
            requirements = " %s", actionsets[0][index].requirements[0];
        }
        else{
            requirements = "None";
        }

        printf("\tRequirements: %s\n", requirements);
        char* message = "quote,%s,%d,%s", hosts[0][i], port, requirements;
        //socket shit
        if(send(cumsock , message , strlen(message) , 0) < 0){
            printf("Send failed\n");
            return;
        }
        else{
            printf("Message sent\n");
        }
        connections[i] = cumsock;
    }
    while(connections){
        //select can select this dick in its mouth
        int select_return = select(1, connections, NULL, NULL, 5);
        break;
    }
    return 0;
}

//--------------------------------------------------------------------------------------------------------------------------


int main(int argc, char* argv[]) {
    extract_line_data(argv[1]);
    // printf("total_actionset_lines: %d\n", total_actionset_count);
    // printf("total_actions_length: %d\n", total_actions_count);
    // printf("longest_requirements_line: %d\n", longest_requirements_line);
    read_rakefile(argv[1]);

    //for(int i = 0; i < 10; i++)
    //{
        // for(int j = 0; j < 10; j++)
        // {
             //printf("actionsets[%d][%d]: %s\n", i, j, actionsets[i][j].actionCommand);
          //   if (actionsets[i][j].requirementnum > 0) {
               //  for (int z = 0; z < actionsets[i][j].requirementnum; z++) {

                     //printf("actionsets[%d][%d] req %d: %s \n", i, j, z, actionsets[i][j].requirements[z]);
             //        printf("actionsets[%d][%d]: %s \n", i, j, &actionsets[i][j].actionCommand);
           //      }
         //    }
       //  }
     //}
     for(int i = 0; i < 10; i++)
     {
         for(int j = 0; j < actioncounts[i]; j++)
         {
             if(strlen(actionsets[i][j].actionCommand) > 0)
             {
                 printf("actionsets[%d][%d]: %s \n", i, j, actionsets[i][j].actionCommand);
             }
         }
     }
    // quote_servers();
    // while (true) {
    //     int sock = 0, valread;
    //     struct sockaddr_in serv_addr;
    //     char *message = "Hello from C Client";
    //     char buffer[BUFFSIZE] = {0};

    //     if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    //         printf("\n Socket creation error \n");
    //         return -1;
    //     }

    //     serv_addr.sin_family = AF_INET;
    //     serv_addr.sin_port = htons(portnumber);

    //     if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    //         printf("\nInvalid address/ Address not supported \n");
    //         return -1;
    //     }

    //     if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    //         printf("\nConnection Failed \n");
    //         return -1;
    //     }

    //     send_message(sock, valread, buffer, message);
    // }


    return 0;
}
