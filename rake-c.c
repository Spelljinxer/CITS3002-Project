/**
 *  CITS3002 2022 Sem 1 - Project - rake client C
 * @authors
 *  - Daivik Anil 22987816
 *  - Nathan Eden 22960674
 *  - Reiden Rufin 22986337
 */

#include <stdio.h>
#include "strsplit.c"
#include "c-client.h"
//-------------------------------------------------------------------------

char buffer[BUFFSIZE];

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

//---------------------------------------------------------------------------------------------

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


    while (fgets (buffer, BUFFSIZE, fptr))
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
                }
            }
            else
            {
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
                if (setnum != -1) {
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

//---------------------------- QUOTE_SERVERS() --------------------------------------------------
struct sockinfo
{
    char *host; 
    int port; 
    int port_array[BUFFSIZE];
    char **host_array;
};

struct sockinfo quote_servers(int index)
{
    struct sockinfo quoteinfo;
    struct sockinfo finalinfo;
    int connections [BUFFSIZE];
    float min_cost = INFINITY;
    int valread;
    for (int i = 0; i <= hosts[0][BUFFSIZE]; i++)
    {
        int portnum = portnumber;
        
        int sock_socket = 0,  client_socket;
        struct sockaddr_in serv_addr;
        sock_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(strlen(hosts[i]) > 0 )
        {
            quoteinfo.host_array = malloc(sizeof(hosts[i]));
            quoteinfo.host_array[i] = strdup(hosts[i]);
            quoteinfo.port_array[i] = portnum;
            
            if(char_counter(hosts[i], ":") > 0)
            {
                char *hostname = strtok(hosts[i], ":");
                int port_split = atoi(strtok(NULL, ":"));
                quoteinfo.host_array[i] = strdup(hostname);
                quoteinfo.port_array[i] = port_split;
            }
            
            if(strcmp(hosts[i], "localhost") == 0)
            {
                //IDK WHY BUT C DOESNT CONNECT 'LOCALHOST' ONLY DIRECT IP
                quoteinfo.host_array[i] = "127.0.0.1"; 
            }

            if (sock_socket < 0)
            {
                printf("Could not create socket");
                exit(1);
            }
            memset(&serv_addr, '0', sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(quoteinfo.port_array[i]);
            if(inet_pton(AF_INET, quoteinfo.host_array[i], &serv_addr.sin_addr)<=0)
            {
                printf("Invalid address/ Address not supported\n");
                exit(1);
            }
            if (connect(sock_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
            {
                printf("Connection Failed\n");
                exit(1);
            }
            
            char*quote = "quote,";
            char *comma = ",";

            char*message = malloc(sizeof(char) * BUFFSIZE);
            char port_string[BUFFSIZE];
            int port_int = quoteinfo.port_array[i];
            sprintf(port_string, "%d", port_int); //Cast to string
            concatenate_quote(message, quote, comma, hosts[i], port_string);
            //printf("OUTGOING--> %s\n", message);
            send(sock_socket , message, strlen(message) , 0 );
            connections[i] = sock_socket;
        }
    }
    memset(quoteinfo.host_array, 0, sizeof(quoteinfo.host_array));
    memset(quoteinfo.port_array, 0, sizeof(quoteinfo.port_array)); //idk why i empty it, could be removed

    int connections_index = 1;
    while(connections[connections_index] != 0)
    {
        fd_set readfds;
        struct timeval tv;
        int retval;
        FD_ZERO(&readfds);
        FD_SET(connections[connections_index], &readfds);
        retval = select(connections[connections_index]+1, &readfds, NULL, NULL, &tv);
        if(retval == -1)
        {
            printf("Select error\n");
            exit(1);
        }
        else if(retval)
        {
            char buffer[1024] = {0};
            valread = read(connections[connections_index], buffer, 1024);
            if(valread == 0)
            {
                printf("Server is down\n");
                connections[connections_index] = 0;
            }
            else
            {
                bool second_comma = false;
                char* comma = ",";
                int second_comma_index;

                //retrieve the index of the second comma
                for(int i = 0; i < strlen(buffer); i++)
                {
                    if(buffer[i] == comma[0])
                    {
                        if(second_comma)
                        {
                            second_comma_index = i;
                        }
                        second_comma = true;
                    }
                }
                char*hostname = malloc(sizeof(char) * BUFFSIZE);
                char*port = malloc(sizeof(char) * 512);
                int first_comma_index = 0;
                for(int i = 0; i < strlen(buffer); i++)
                {
                    if(buffer[i] == comma[0])
                    {
                        first_comma_index = i;
                        break;
                    }
                }
                //message e.g. "localhost,6238,90"
                //this will run from "localhost" --> first ","
                for(int i = 0; i < first_comma_index; i++)
                {
                    hostname[i] = buffer[i];
                }
                
                //message e.g. "localhost,6238,90"
                //this will run from "6238" --> second ","
                for(int i = first_comma_index + 1; i < second_comma_index; i++)
                {
                    port[i - first_comma_index - 1] = buffer[i];
                }

                quoteinfo.host = hostname;
                quoteinfo.port = atoi(port);

                //message e.g. "localhost,6238,90"
                //this will run from "90" --> end of string
                char* cost_string = malloc(sizeof(char) * BUFFSIZE);
                for(int i = second_comma_index; i < strlen(buffer); i++)
                {
                    cost_string[i-second_comma_index] = buffer[i+1];
                }
                int cost = atoi(cost_string);

                if(cost < min_cost)
                {
                    min_cost = cost;
                    finalinfo.host = quoteinfo.host;
                    finalinfo.port = quoteinfo.port;
                }
            }
        }
        
        close(connections[connections_index]);
        connections_index++;
        if(connections[connections_index] == 0)
        {
            break;
        }
    }
    // printf("finalinfo.host = %s\n", finalinfo.host);
    // printf("finalinfo.port = %d\n", finalinfo.port);
    return finalinfo;            
}

//---------------------------- READ_DATA() --------------------------------------------------
char *read_data(int sock, char*extra_data, bool is_File)
{
    float data_left = INFINITY;
    char *f_data = malloc(sizeof(char) * strlen(extra_data));
    if(extra_data[0] == '\0')
    {
        int first_comma = 0;
        for(int i = 0; i < strlen(extra_data); i++)
        {
            if(extra_data[i] == ',')
            {
                first_comma = i;
                break;
            }
        }
        char *data_left_placeholder = malloc(sizeof(char) * BUFFSIZE);
        for(int i = 0; i < first_comma; i++)
        {
            data_left_placeholder[i] = extra_data[i];
        }
        data_left = atof(data_left_placeholder);
        for(int i = first_comma + 1; i < strlen(extra_data); i++)
        {
            f_data[i-first_comma-1] = extra_data[i];
        }

        if(strlen(f_data) > data_left)
        {
            int max_fdata_length = 0;
            for(int i = data_left; i < strlen(f_data); i++)
            {
                if(f_data[i] == '\n')
                {
                    max_fdata_length = i;
                    break;
                }
            }
            //set extra_data to f_data[data_left:]
            int dl_as_int = data_left;
            extra_data = malloc(sizeof(char) *  strlen(f_data));
            for(int i = dl_as_int; i < max_fdata_length; i++)
            {
                extra_data[i-dl_as_int] = f_data[i];
            }
            extra_data[max_fdata_length-dl_as_int] = '\0';

            //set f_data to f_data[:data_left]
            int f_data_index = 0;
            char*f_data_TWO = malloc(sizeof(char) * strlen(f_data));
            while(f_data[f_data_index] != '\0' && f_data_index < dl_as_int )
            {
                f_data_TWO[f_data_index] = f_data[f_data_index];
                f_data_index++;
            }
            strcpy(f_data, f_data_TWO);
            free(f_data_TWO);
            data_left = 0;
        }
        
        else
        {
            memset(extra_data, 0 , sizeof(extra_data)); //CHECK IF THIS ACTUALLY WORKS
            strcpy(extra_data, " ");
            data_left -= strlen(f_data);
        }
    }
    while(data_left > 0)
    {
        char buffer_data[BUFFSIZE] = { 0 };
        int valread;
        valread = read(sock, buffer_data, BUFFSIZE); 
        if(valread)
        {
            if(data_left == INFINITY)
            {
                int first_comma_index = 0;
                for(int i = 0; i < strlen(buffer_data); i++)
                {
                    if(buffer_data[i] == ',')
                    {
                        first_comma_index = i;
                        break;
                    }
                }
                char *data_left_placeholder = malloc(sizeof(char) * BUFFSIZE);
                for(int i = 0; i < first_comma_index; i++)
                {
                    data_left_placeholder[i] = buffer_data[i];
                }
                data_left = atof(data_left_placeholder);
                char* buffer_data_copy = malloc(sizeof(char) * strlen(buffer_data));
                for(int i = first_comma_index + 1; i < strlen(buffer_data); i++)
                {
                    buffer_data_copy[i-first_comma_index-1] = buffer_data[i];
                }
                strcpy(buffer_data, buffer_data_copy);

            }
            
            if(strlen(buffer_data) > data_left)
            {
                int dl_as_int = data_left;
                extra_data = malloc(sizeof(char) *  strlen(buffer_data));
                for(int i = dl_as_int; i < strlen(buffer_data); i++)
                {
                    extra_data[i-dl_as_int] = buffer_data[i];
                }
                extra_data[strlen(buffer_data)-dl_as_int] = '\0';
                //printf("extra_data = %s\n", extra_data);

                char*buffer_data_TWO = malloc(sizeof(char) * strlen(buffer_data));
                int buffer_data_index = 0;
                while(buffer_data[buffer_data_index] != '\0' && buffer_data_index < dl_as_int )
                {
                    buffer_data_TWO[buffer_data_index] = buffer_data[buffer_data_index];
                    buffer_data_index++;
                }
                strcpy(buffer_data, buffer_data_TWO);
                //printf("buffer_data = %s\n", buffer_data);
                data_left = 0;

            }
            else
            {
                data_left -= strlen(buffer_data);
            }
            strcat(f_data, buffer_data);
        }
        
    }
    if(is_File == false)
    {
        return extra_data;
    }
    else
    {
        char*filename = malloc(sizeof(char) * strlen(f_data));
        char*data_to_write = malloc(sizeof(char) * strlen(f_data));
        int comma_index = 0;
        for(int i = 0; i < strlen(f_data); i++)
        {
            if(f_data[i] == ',')
            {
                comma_index = i;
                break;
            }
        }
        for(int i = 0; i < comma_index; i++)
        {
            filename[i] = f_data[i];
        }
        for(int i = comma_index + 1; i < strlen(f_data); i++)
        {
            data_to_write[i-comma_index-1] = f_data[i];
        }
        filename[comma_index] = '\0';

        FILE *fptr;
        fptr = fopen(filename, "w");
        fprintf(fptr, "%s", data_to_write);
        fclose(fptr);
        return extra_data;
    }
}

//---------------------------- PROCESS_ACTIONS() --------------------------------------------------

//printf("%s\n", actionsets[s_index][a_index].actionCommand);
void process_actions()
{
    int valread;
    int connections[BUFFSIZE];
    for (int s_index = 0; s_index < setcount; s_index++)
    {
        bool shit = false;
        int sock = 0, client_fd;
        for(int a_index = 0; a_index < actioncounts[s_index]; a_index++)
        {
            char*curraction = actionsets[s_index][a_index].actionCommand;
            char*remote = get_first_seven_chars(curraction);
            if(strlen(curraction) > 1)
            {

                struct sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;


                if(strcmp(remote, "remote-") == 0)
                {
                    char* remote_action = malloc(sizeof(char) * strlen(curraction));
                    for(int i = 7; i < strlen(curraction); i++)
                    {
                        remote_action[i-7] = curraction[i];
                    }
                    remote_action[strlen(curraction)-7] = '\0';
                    strcpy(curraction, remote_action);
                    printf("R-OUTGOING--> %s\n", curraction);
                    serv_addr.sin_port = htons(portnumber);

                    sock = socket(AF_INET, SOCK_STREAM, 0);
                    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
                    {
                        printf("\nInvalid address/ Address not supported \n");
                        exit(1);
                    }
                    client_fd = test_client_fd(client_fd, sock, serv_addr, sizeof(serv_addr), portnumber);
                }
                else
                {
                    printf("OUTGOING--> %s\n", curraction);
                    struct sockinfo info = quote_servers(s_index);
                    char*hostname_exec = info.host;
                    if(strcmp(hostname_exec, "localhost") == 0)
                    {
                        hostname_exec = "127.0.0.1";
                    }
                    int port_exec = info.port;
                    serv_addr.sin_port = htons(info.port);
                    sock = socket(AF_INET, SOCK_STREAM, 0);
                    if(inet_pton(AF_INET, hostname_exec, &serv_addr.sin_addr) <= 0)
                    {
                        printf("\nInvalid address/ Address not supported \n");
                        exit(1);
                    }
                    client_fd = test_client_fd(client_fd, sock, serv_addr, sizeof(serv_addr), port_exec);
                }

                char*action = "action,";
                char*msg = malloc(sizeof(action) * strlen(curraction) + 1);
                strcpy(msg, action);
                strcat(msg, curraction);
                send(sock, msg, strlen(msg), 0);
                connections[s_index] = sock;
            }
            printf("\n"); //next actionset
            
            for(int i = 0; i < setcount; i++)
            {
                if(connections[i] != 0)
                {
                    char buffer[1024] = {0};
                    fd_set readfds;
                    FD_ZERO(&readfds);
                    FD_SET(connections[i], &readfds);
                    struct timeval tv;
                    tv.tv_sec = 0;
                    tv.tv_usec = 100000;
                    int retval = select(connections[i] + 1, &readfds, NULL, NULL, &tv);
                    if(retval > 0)
                    {
                        if(FD_ISSET(connections[i], &readfds))
                        {
                            valread = read(connections[i], buffer, 1024);
                            if(valread == 0)
                            {
                                printf("Connection closed\n");
                                connections[i] = 0;
                            }
                            else
                            {
                                //printf("INCOMING<---%s\n", buffer);
                                float data_left = INFINITY;
                                char *data_left_placeholder = malloc(sizeof(char) * strlen(buffer));
                                char*data = malloc(sizeof(char) * strlen(buffer));
                                char*extra_data = malloc(sizeof(char) * strlen(buffer));
                                char*f_data = malloc(sizeof(char) * strlen(buffer));
                                strcpy(data, buffer);

                                int first_comma_index = 0;
                                for(int i = 0; i < strlen(buffer); i++)
                                {
                                    if(buffer[i] == ',')
                                    {
                                        first_comma_index = i;
                                        break;
                                    }
                                }

                                if(data_left == INFINITY)
                                {
                                    char *data_left_placeholder = malloc(sizeof(char) * strlen(data));
                                    for(int i = 0; i < first_comma_index; i++)
                                    {
                                        data_left_placeholder[i] = data[i];
                                    }
                                    data_left = atof(data_left_placeholder);

                                    char*data_placeholder = malloc(sizeof(char) * strlen(data));
                                    for(int i = first_comma_index + 1; i < strlen(data); i++)
                                    {
                                        data_placeholder[i-first_comma_index-1] = data[i];
                                    }
                                    strcpy(data, data_placeholder);
                                }

                                if(strlen(data) <= data_left)
                                {
                                    for(int i = data_left; i < strlen(data); i++)
                                    {
                                        extra_data[i] = data[i];
                                    }
                                    char *ds_save = malloc(sizeof(char) * strlen(data));
                                    for(int i = 0 ; i < data_left; i ++)
                                    {
                                        ds_save[i] = data[i];
                                    }
                                    strcpy(data, ds_save);
                                    data_left = 0;
                                }
                                else
                                {
                                    data_left -= strlen(data);
                                }
                                strcat(f_data, data);
                                
                                

                                char *data_exitcode_hold = malloc(strlen(f_data) + 1);
                                char *data_stdout_hold = malloc(strlen(f_data) + 1);
                                char *data_stderr_hold = malloc(strlen(f_data) + 1);
                                char *data_fcount_hold = malloc(strlen(f_data) + 1);


                                struct comma_indices ci;
                                
                                int comma_index_one = init_comma_indices(f_data).comma_index_one;
                                int comma_index_two = init_comma_indices(f_data).comma_index_two;
                                int comma_index_three = init_comma_indices(f_data).comma_index_three;

                                int data_exitcode = get_exit_code(f_data, comma_index_one);
                                int data_stdout = get_stdout(f_data, comma_index_one, comma_index_two);
                                int data_stderr = get_stderr(f_data, comma_index_two, comma_index_three);
                                int data_fcount = get_fcount(f_data, comma_index_three);
                                
                            
                                if(data_exitcode != 0)
                                {
                                    shit = true;
                                }
                                printf("data_stdout : %d\n", data_stdout);
                                if(data_stdout == 1)
                                {

                    
                                    char*output = malloc(sizeof(char) * BUFFSIZE);
                                    output, extra_data = read_data(sock, extra_data, false);

                                    printf("OUTPUT-->%s\n", extra_data);
                                }
                                
                                if(data_stderr == 1)
                                {
                                    char*output = malloc(sizeof(char) * BUFFSIZE);
                                    output, extra_data = read_data(sock, extra_data, false);

                                    printf("ERROR-->%s\n", extra_data);
                                }

                                for(int i = 0; i < data_fcount; i++)
                                {
                                    extra_data = read_data(sock, " ", true);
                                }
                            } 
                        }
                    }
                }
                if(shit)
                {
                    printf("TERMINATED--> actionset %d\n", s_index+1);
                    break;
                }
            }

        }
    }
}

//--------------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) 
{
    //extract_line_data(argv[1]);
    read_rakefile(argv[1]);
    process_actions();

    printf("-------- REQUIREMENTS --------\n");
    // for(int i = 0; i < 10; i++)
    // {
    //     for(int j = 0; j < 10; j++)
    //     {
    //         //printf("actionsets[%d][%d]: %s\n", i, j, actionsets[i][j].actionCommand);
    //         if (actionsets[i][j].requirementnum > 0)
    //         {
    //             for (int z = 0; z < actionsets[i][j].requirementnum; z++) 
    //             {
    //                 if(strlen (actionsets[i][j].requirements[z]) > 0)
    //                 {   
    //                     int len = strlen(actionsets[i][j].requirements[z]);
                        
    //                     printf("actionsets[%d][%d] req %d: %s \n", i, j, z, actionsets[i][j].requirements[z]);
    //                 }
    //             }
    //         }
    //     }
    //  }
     printf("--------------------ACTION COMMANDS------------------------\n");
    //  for(int i = 0; i < 10; i++)
    //  {
    //      for(int j = 0; j < actioncounts[i]; j++)
    //      {
    //          if(strlen(actionsets[i][j].actionCommand) > 0)
    //          {
    //              printf("actionsets[%d][%d]: %s \n", i, j, actionsets[i][j].actionCommand);
    //          }
    //      }
    //  }
    return 0;
}
