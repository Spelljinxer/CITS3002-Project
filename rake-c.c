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

struct sockinfo
{
    char *host; //THIS HAS TO BE TURNED INTO AN ARRAY OF HOSTNAMES
    int port; //THIS HAS TO BE TURNED INTO AN ARRAY OF PORTNUMS CORRESPONDING TO HOSTNAME
};

//No its not done, no it doesnt work, c makes me wanna kill myself
struct sockinfo quote_servers(int index){
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
    struct sockinfo info;
    info.host = "127.0.0.1";
    info.port = portnumber;
    return info;                //THEY'RE JUST HARDCODED FOR NOW FOR TESTING 
}

//WHY DO WE HAVE TO WRITE IN C?????
//printf("%s\n", actionsets[s_index][a_index].actionCommand);
void process_actions()
{
    printf("\n-------TESTING PROCESS ACTIONS-----\n");
    for (int s_index = 0; s_index < setcount; s_index++)
    {
        bool shit = false;
        int connections[BUFFSIZE];
        for (int a_index = 0; a_index < actioncounts[s_index]; a_index++)
        {
            char* curraction = actionsets[s_index][a_index].actionCommand;
            char*remote = get_first_seven_chars(curraction);
            if(strlen(curraction) > 0)
            {
                int sock = 0, valread, client_fd;
                struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                if (strcmp(remote, "remote-") == 0)
                { 
                    printf("R-OUTGOING--> %s\n", curraction);
                    serv_addr.sin_port = htons(portnumber);
                    sock = socket(AF_INET, SOCK_STREAM, 0);
                    if(sock < 0)
                    {
                        printf("\n Error : Could not create socket \n");
                    }
                    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) 
                    {
                        printf("\nInvalid address/ Address not supported \n");
                        exit(1);
                    }
                    client_fd = test_client_fd(client_fd, sock, serv_addr, sizeof(serv_addr));
                }
                else
                {
                    printf("OUTGOING--> %s\n", curraction);
                    struct sockinfo info = quote_servers(a_index);
                    serv_addr.sin_port = htons(info.port);
                    sock = socket(AF_INET, SOCK_STREAM, 0);

                    if(sock < 0)
                    {
                        printf("\n Error : Could not create socket \n");
                    }
                    if (inet_pton(AF_INET, info.host, &serv_addr.sin_addr) <= 0) 
                    {
                        printf("\nInvalid address/ Address not supported \n");
                        exit(1);
                    }
                    client_fd = test_client_fd(client_fd, sock, serv_addr, sizeof(serv_addr));         
                }

                char *action = "action,";
                char *msg = malloc(strlen(action) + strlen(curraction) + 1);
                strcpy(msg, action);
                strcat(msg, curraction);
                send(sock, msg, strlen(msg), 0);
                connections[s_index] = sock;
            }

        }
        printf("\n"); //next actionset
        //TBH IDK WTF I DID OR WTF ITS EVEN DOING BUT IT WORKS FOR NOW
        while(connections)
        {
            fd_set readfds;
            int max_fd = 0;
            FD_ZERO(&readfds);
            for (int i = 0; i < setcount; i++)
            {
                if (connections[i] > 0)
                {
                    FD_SET(connections[i], &readfds);
                    max_fd = connections[i] > max_fd ? connections[i] : max_fd;
                }
            }
            int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
            if ((activity < 0) && (errno!=EINTR)) 
            {
                printf("select error");
            }
            for (int i = 0; i < setcount; i++)
            {
                if (connections[i] > 0)
                {
                    if (FD_ISSET(connections[i], &readfds))
                    {
                        char buffer[1024] = {0};
                        int valread;
                        valread = read(connections[i], buffer, 1024);
                        if(valread > 0)
                        {
                            valread = read(connections[i], buffer, 1024);
                            printf("INCOMING<--%s\n", buffer);
                        }
                        
                    }
                }
            }
            break;
        }
    }
}

//WELL THE FIRST IF(EXTRA_DATA) PASSES CORRECTLY
//IDK HOW TF TO TEST THE WHILE LOOP THAT READS GG TO THAT
//ALSO KIANA HOV KILLS HIMEKO BUT SHE LATERS BECOMES FLAMESCION POG
char* read_data(int sock, char *extra_data, bool is_File)
{
    float data_left = INFINITY;
    float inf_check = INFINITY;
    char *f_data = malloc(sizeof(char) * strlen(extra_data) + 1);
    if(extra_data)
    {
        char *newstring = malloc(sizeof(char) * strlen(extra_data) + 1);
        strcpy(newstring, extra_data);
        
        char* first_element = strtok(newstring, ",");
        data_left = atof(first_element);
        
        int comma_index = get_char_index(extra_data, ',') + 1;
        f_data = strdup(&extra_data[comma_index]);
        
        
        if(strlen(f_data) > data_left)
        {
            extra_data = realloc(f_data, sizeof(char) * strlen(f_data) + 1);
            extra_data = splice_string(f_data, data_left, strlen(f_data));

            f_data = splice_string(f_data, 0, (int) data_left);
            data_left = 0;
        }
        else
        {
            extra_data = " ";
            data_left -= strlen(f_data);
        }
    }
    while(data_left > 0)
    {
        char buffer_data[1024] = { 0 };
        int valread = recv(sock, buffer_data, 1024, 0);
        if(valread > 0)
        {
            if(data_left == inf_check)
            {
                char *newstring = malloc(sizeof(char) * strlen(buffer_data) + 1);
                char *first_element = strtok(newstring, ",");
                data_left = atof(first_element);
                int comma_index = get_char_index(buffer_data, ',') + 1;
                strcpy(buffer_data, (&buffer_data[comma_index]));
            }
            if(strlen(buffer_data) > data_left)
            {
                extra_data = realloc(extra_data, sizeof(char) * strlen(extra_data) + 1);
                extra_data = splice_string(buffer_data, data_left, strlen(extra_data));

                char* buff_spliced = splice_string(buffer_data, 0, strlen(buffer_data));
                strcpy(buffer_data, buff_spliced);
                data_left = 0;
            }
            else
            {
                data_left -= strlen(buffer_data);
            }
            strcat(f_data, buffer_data);
        }
    }
    if(!is_File)
    {
        return extra_data;
    }
    else
    {
        char *filename = malloc(sizeof(char) * strlen(f_data) + 1);
        strcpy(filename, f_data);
        filename = strtok(filename, ",");

        int first_comma = 0;
        while(f_data[first_comma] != ',')
        {
            first_comma++;
        }
        FILE *fptr = fopen(filename, "w");
        for(int i = first_comma+1; i < strlen(f_data); i++)
        {
            fprintf(fptr, "%c", f_data[i]);
        }
        fclose(fptr);
        return extra_data;
    }
}

//--------------------------------------------------------------------------------------------------------------------------


int main(int argc, char* argv[]) {
    extract_line_data(argv[1]);
    read_rakefile(argv[1]);
    process_actions();
    bool is_File = true;
    char *extra_data = "29, split, this, message, into, parts";
    char *d = read_data(0, extra_data, is_File);
    //for(int i = 0; i < 1 0; i++)
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
     printf("--------------------REAL DATA------------------------\n");
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
