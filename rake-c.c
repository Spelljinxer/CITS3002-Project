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

char hosts[BUFFSIZE][BUFFSIZE] = {};

int portnumber;


struct {
    char actionCommand[BUFFSIZE];
    char **requirements;
    int requirementnum;
} actionsets[BUFFSIZE][BUFFSIZE];

int setcount = 0;
int actioncounts[BUFFSIZE];

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
        if(strlen(buffer) > 2)
        { 
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
    }
    actioncounts[setnum] = actionnum;
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
    int connection_num = 0;
    for(size_t i = 0; i < sizeof(hosts) / sizeof(hosts[0]); i++)
    {
        if(strlen(hosts[i]) > 0)
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
                send(sock_socket , message, strlen(message) , 0 );
                connections[connection_num] = sock_socket;
                connection_num++;
            }
        }
        
    }

    fd_set readfds;

    int total_connections = connection_num;
    while(connection_num > 0)
    {
        FD_ZERO(&readfds);
        int max_fd = 0;

            for(int i = 0; i < total_connections; i++)
            {
                if (connections[i] > 0) {
                    FD_SET(connections[i], &readfds);
                }
                max_fd = (max_fd > connections[i]) ? max_fd : connections[i];
            }

        max_fd++;

        int retval = select(max_fd, &readfds, NULL, NULL, NULL);

        if(retval == -1)
        {
            printf("Select error\n");
            exit(1);
        }
        else if(retval >= 0)
        {
            for (int i = 0; i < total_connections; i++) {
                if(FD_ISSET(connections[i], &readfds) && connections[i] > 0) {

                    char buffer[1024] = {0};
                    int valread = read(connections[i], buffer, 1024);

                    if(valread == 0)
                    {
                        printf("Server is down\n");
                        connections[i] = 0;
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
                        hostname[first_comma_index] = '\0';

                        //message e.g. "localhost,6238,90"
                        //this will run from "6238" --> second ","
                        for(int i = first_comma_index + 1; i < second_comma_index; i++)
                        {
                            port[i - first_comma_index - 1] = buffer[i];
                        }
                        port[second_comma_index - first_comma_index - 1] = '\0';
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
                    connections[i] = -1;
                    connection_num--;
                    shutdown(connections[i], SHUT_RDWR);
                }
            }
        }


    }
    return finalinfo;            
}

//---------------------------- READ_DATA() --------------------------------------------------
char *read_data(int sock, char*extra_data, bool is_File, bool is_Err)
{
    int data_left = -1;
    char *f_data = calloc(strlen(extra_data) + 1,sizeof(char));
    if(extra_data[0] != '\0')
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
        data_left_placeholder[first_comma] = '\0';
        data_left = atoi(data_left_placeholder);
        for(int i = first_comma + 1; i < strlen(extra_data); i++)
        {
            f_data[i-first_comma-1] = extra_data[i];
        }
        f_data[strlen(extra_data)-first_comma-1] = '\0';

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

            max_fdata_length = strlen(f_data);
            extra_data[strlen(f_data)];


            for(int i = dl_as_int; i < max_fdata_length; i++)
            {
                extra_data[i-dl_as_int] = f_data[i];
            }
            extra_data[max_fdata_length-dl_as_int] = '\0';

            //set f_data to f_data[:data_left]
            int f_data_index = 0;
            char*f_data_TWO = malloc(sizeof(char) * (strlen(f_data) + 1));
            while(f_data[f_data_index] != '\0' && f_data_index < dl_as_int )
            {
                f_data_TWO[f_data_index] = f_data[f_data_index];
                f_data_index++;
            }
            f_data_TWO[f_data_index] = '\0';
            strcpy(f_data, f_data_TWO);
            free(f_data_TWO);
            data_left = 0;
        }

        else
        {
            free(extra_data);
            extra_data = calloc(strlen(extra_data), sizeof(char));
            data_left -= strlen(f_data);
        }
    }
    while(data_left > 0 || data_left == -1)
    {
        char buffer_data[BUFFSIZE] = { 0 };
        int valread = read(sock, buffer_data, 1024);

        if(valread)
        {
            if(data_left == -1)
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
                data_left_placeholder[first_comma_index] = '\0';
                data_left = atoi(data_left_placeholder);
                char* buffer_data_copy = malloc(sizeof(char) * strlen(buffer_data));
                for(int i = first_comma_index + 1; i < strlen(buffer_data); i++)
                {
                    buffer_data_copy[i-first_comma_index-1] = buffer_data[i];
                }
                buffer_data_copy[strlen(buffer_data)-first_comma_index-1] = '\0';
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


                char*buffer_data_TWO = malloc(sizeof(char) * strlen(buffer_data));
                int buffer_data_index = 0;
                while(buffer_data[buffer_data_index] != '\0' && buffer_data_index < dl_as_int )
                {
                    buffer_data_TWO[buffer_data_index] = buffer_data[buffer_data_index];
                    buffer_data_index++;
                }
                buffer_data_TWO[buffer_data_index] = '\0';
                strcpy(buffer_data, buffer_data_TWO);
                data_left = 0;

            }
            else
            {
                data_left -= strlen(buffer_data);
            }
            strcat(f_data, buffer_data);
        }
        
    }
    if(is_File == false && is_Err == false)
    {
        printf("OUTPUT--> %s\n", f_data);
        return extra_data;
    }
    else if (is_Err == true)
    {
        printf("ERROR-->%s\n", f_data);
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
        filename[comma_index] = '\0';

        for(int i = comma_index + 1; i < strlen(f_data); i++)
        {
            data_to_write[i-comma_index-1] = f_data[i];
        }
        data_to_write[strlen(f_data)-comma_index-1] = '\0';

        FILE *fptr;
        fptr = fopen(filename, "w");
        fprintf(fptr, "%s", data_to_write);
        
        fclose(fptr);
        printf("FILE-->%s\n",filename);

        return extra_data;
    }
}

//---------------------------- PROCESS_ACTIONS() --------------------------------------------------

void process_actions()
{
    struct sockinfo info;
    bool error_found = false;
    int connections[1024];
    int valread;
        
    int sock = 0, client_fd;
    for(int s_index = 0; s_index < setcount; s_index++)
    {
        int connection_num = 0;
    
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;

        for(int a_index = 0; a_index < actioncounts[s_index]; a_index++)
        {
            char*curraction = actionsets[s_index][a_index].actionCommand;

            char*remote = malloc(sizeof(char) * BUFFSIZE);
            for(int i = 0; i < 7; i++)
            {
                remote[i] = curraction[i];
            }
            remote[strlen(curraction)] = '\0';
            if(strcmp(remote, "remote-") == 0)
            {
                char*remote_action = malloc(sizeof(char) * strlen(curraction));
                for(int i = 7; i < strlen(curraction); i++)
                {
                    remote_action[i-7] = curraction[i];
                }
                remote_action[strlen(curraction)-7] = '\0';
                memset(curraction, 0 , sizeof(curraction));
                strcpy(curraction, remote_action);

                printf("OUTGOING--> %s\n", curraction);
                info = quote_servers(s_index);
                char*new_hostname = malloc(sizeof(char) * strlen(info.host));
                strcpy(new_hostname, info.host);
                if(strcmp(new_hostname, "localhost") == 0)
                {
                    new_hostname = "127.0.0.1";
                }
                serv_addr.sin_port = htons(info.port);
                sock = socket(AF_INET, SOCK_STREAM, 0);
                inet_pton(AF_INET, "new_hostname", &serv_addr.sin_addr);
                if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    printf("\nConnection Failed\n");
                    exit(1);
                }
                connection_num++;
            }
            else
            {
                printf("R-OUTGOING--> %s\n", curraction);
                serv_addr.sin_port = htons(portnumber);
                sock = socket(AF_INET, SOCK_STREAM, 0);
                inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
                if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
                {
                    printf("\nConnection Failed\n");
                    exit(1);
                }
                connection_num++;
            }

            char*action = "action,";
            char*msg = malloc(sizeof(action) * strlen(curraction) + 1);
            strcpy(msg, action);
            strcat(msg, curraction);
            send(sock, msg, strlen(msg), 0);
            connections[a_index] = sock;
        }

        fd_set readfds;

        int total_connections = connection_num;
        while(connection_num > 0)
        {
            FD_ZERO(&readfds);
            int max_fd = 0;
            for(int i = 0; i < total_connections; i++)
            {
                if (connections[i] > 0) {
                    FD_SET(connections[i], &readfds);
                }
                max_fd = (max_fd > connections[i]) ? max_fd : connections[i];
            }

            max_fd++;

            int retval = select(max_fd, &readfds, NULL, NULL, NULL);
            if(retval == -1)
            {
                printf("ERROR: select()\n");
                exit(1);
            }
            
            else if(retval >= 0)
            {
                for (int i = 0; i < total_connections; i++) {
                    if(FD_ISSET(connections[i], &readfds) && connections[i] > 0)
                    {
                        printf("\nNEW-ACTION--> %d\n", total_connections - connection_num + 1);
                        char buffer_in[BUFFSIZE] = {0};
                        valread = read(connections[i], buffer_in, 1024);
                        int data_exitcode;
                        int data_stdout;
                        int data_stderr;
                        int data_fcount;
                        int data_left = -1;
                        char *data_left_placeholder = malloc(sizeof(char) * strlen(buffer_in));
                        char*data = malloc(sizeof(char) * strlen(buffer_in));
                        char* extra_data = calloc(strlen(buffer_in),sizeof(char));
                        char*f_data = malloc(sizeof(char) * strlen(buffer_in));
                        strcpy(data, buffer_in);
                        int first_comma_index = 0;
                        for(int i = 0; i < strlen(buffer_in); i++)
                        {
                            if(buffer_in[i] == ',')
                            {
                                first_comma_index = i;
                                break;
                            }
                        }
                        while(data_left > 0 || data_left == -1)
                        {
                            if (data_left == -1)
                            {
                                char *data_left_placeholder = malloc(sizeof(char) * strlen(data));
                                for (int i = 0; i < first_comma_index; i++)
                                {
                                    data_left_placeholder[i] = data[i];
                                }
                                data_left_placeholder[first_comma_index] = '\0';
                                data_left = atoi(data_left_placeholder);

                                char *data_placeholder = malloc(sizeof(char) * strlen(data));
                                for (int i = first_comma_index + 1; i < strlen(data); i++)
                                {
                                    data_placeholder[i - first_comma_index - 1] = data[i];
                                }
                                data_placeholder[strlen(data) - first_comma_index - 1] = '\0';
                                strcpy(data, data_placeholder);
                            }

                            if (strlen(data) > data_left)
                            {
                                extra_data = malloc((strlen(data)-data_left + 1)*sizeof(char));
                                int data_left_new = strlen(data) - data_left;

                                for(int x = 0;x < data_left_new; x++){
                                    extra_data[x] = data[x + data_left];
                                }

                                char* new_data = malloc((data_left + 1 * sizeof(char)));
                                for(int x = 0;x < data_left; x++){
                                    new_data[x] = data[x];
                                }
                                free(data);
                                data = malloc(sizeof(new_data));
                                strcpy(data, new_data);
                                data_left = 0;
                            }
                            else
                            {
                                data_left -= strlen(data);
                            }
                            strcat(f_data, data);
                        }

                        struct comma_indices ci;

                        int comma_index_one = init_comma_indices(f_data).comma_index_one;
                        int comma_index_two = init_comma_indices(f_data).comma_index_two;
                        int comma_index_three = init_comma_indices(f_data).comma_index_three;

                        data_exitcode = get_exit_code(f_data, comma_index_one);
                        data_stdout = get_stdout(f_data, comma_index_one, comma_index_two);
                        data_stderr = get_stderr(f_data, comma_index_two, comma_index_three);
                        data_fcount = get_fcount(f_data, comma_index_three);

                        if(data_exitcode != 0)
                        {
                            error_found = true;
                        }

                        if(data_stdout == 1)
                        {
                            extra_data = read_data(connections[i], extra_data, false, false);
                        }
                        else
                        {
                            printf("OUTPUT--> None\n");
                        }

                        if(data_stderr == 1)
                        {
                            extra_data = read_data(connections[i], extra_data, false, true);
                        }

                        free(extra_data);
                        shutdown(connections[i], SHUT_RDWR);
                        connections[i] = -1;
                        connection_num--;
                    }
                }
            }
        }
        if(error_found)
            {
                printf("\nTERMINATED--> actionset %d returned errors\n", s_index+1);
                exit(1);
            }
    }
}


//--------------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) 
{
    read_rakefile(argv[1]);
    process_actions();
    
    return 0;
}