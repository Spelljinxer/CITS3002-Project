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

    //FOR COMPILATION AND EXECUTION OF THE C Client RUN:
        //cc -std=c11 -o rake-c rake-c.c && ./rake-c [arg1]
        //where arg1 is the Rakefile
    //on a side note, please run the server before running this program, or connections will fail.
//-------------------------------------------------------------------------

char buffer[BUFFSIZE]; // buffer for reading lines in Rakefile

char hosts[BUFFSIZE][BUFFSIZE] = {};  //Contains the HOSTS of which is an array of strings

int portnumber;


/**
 * @brief This struct is used to parse and store all our Rakefile data
 * actioncommand 
 *  - is the command that is to be executed
 * requirements
 *  - array of type string 
 * requirementnum
 * - number of requirement files per command
 * 
 */
struct {
    char actionCommand[BUFFSIZE];
    char **requirements;
    int requirementnum;
} actionsets[BUFFSIZE][BUFFSIZE];

int setcount = 0;  //keeps track of which actionset are are on (actionset1, actionset2, etc.)
int actioncounts[BUFFSIZE]; //keeps track of the numebr of actions per actionset

//---------------------------------------------------------------------------------------------

/**
 * @brief Parses in the Rakefile and stores it in the actionsets struct
 * 
 * @param rakefile 
 */
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
            if (starts_with(buffer, "\t")) //check if line is one tabbed - these are the "actions"
            {

                if (starts_with(buffer, "\t\t")) //check if line is two tabs - these are the "requires"
                {
                    char **splitreqs = strsplit(buffer, &nwords); //Use the given strsplit to split each requirement into an array of strings
                    actionsets[setnum][actionnum-1].requirements = malloc((nwords-1) * sizeof(char*)); //attempt to allocate memory for our requirements array

                    for (int x = 1; x < nwords; x++)
                    {
                        actionsets[setnum][actionnum-1].requirementnum++;
                        actionsets[setnum][actionnum-1].requirements[x-1] = malloc(sizeof(splitreqs[x]));
                        actionsets[setnum][actionnum-1].requirements[x-1] = strdup(splitreqs[x]);  //place the requirements into requirement array
                    }
                }
                else //because it is not two tabbed but only tabbed, then simply add the action to actionset while trimming the whitespace
                {
                    strcpy(actionsets[setnum][actionnum].actionCommand, trim_whitespace(buffer));
                    actionsets[setnum][actionnum].requirementnum = 0; //because there are no requirements detected, it is 0
                    actionnum++;  
                }
            }

            else //We are either a PORT, HOSTS or a new actionset
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
                        strcpy(hosts[i-1], words[i]); //place the hosts into the hosts array
                    }
                }
                else if (strstr(buffer, ":")) 
                {
                    if (setnum != -1) 
                    {
                        actioncounts[setnum] = actionnum; //set the number of actions in the actionset to the current number of actions
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


//------------------------------------------- QUOTE_SERVERS() --------------------------------------------------

/**
 * @brief Strut keeps track of the servers and their ports
 * *host
 *  - the final hostname of the server with the lowest cost
 * port
 * - the port number of the server with the lowest cost
 * port_array
 *  - array of port numbers of each server
 * host_array
 * - array of hostnames of each server
 * 
 */
struct sockinfo
{
    char *host;
    int port; 
    int port_array[BUFFSIZE];
    char **host_array;
};
/**
 * @brief Quotes the servers for the lowest cost and stores them in the struct sockinfo
 * 
 * @param index 
 * @return struct sockinfo containing the hostname and port of the lowest cost server
 */
struct sockinfo quote_servers(int index)
{
    struct sockinfo quoteinfo;
    struct sockinfo finalinfo;
    int connections [BUFFSIZE];
    float min_cost = INFINITY; //as we don't know the cost yet, set it to INFINITY
    int connection_num = 0;
    for(size_t i = 0; i < sizeof(hosts) / sizeof(hosts[0]); i++)  //Perform for every host in the HOSTS array
    {
        if(strlen(hosts[i]) > 0)
        {
            int portnum = portnumber;
            
            int sock_socket = 0,  client_socket; //initialise the socket creation
            struct sockaddr_in serv_addr;
            sock_socket = socket(AF_INET, SOCK_STREAM, 0);

            quoteinfo.host_array = malloc(sizeof(hosts[i])); //allocate memory to hold our hostnames
            quoteinfo.host_array[i] = strdup(hosts[i]); //place the hostname into the hostname array
            quoteinfo.port_array[i] = portnum; //place the port number into the port array
            
            if(char_counter(hosts[i], ":") > 0) //simpl checks if we have more than one hostname in the HOSTS array, as the first host is tied to default PORT
            {
                char *hostname = strtok(hosts[i], ":"); //split the hostname from the port number
                int port_split = atoi(strtok(NULL, ":")); //split the port number from the hostname
                quoteinfo.host_array[i] = strdup(hostname);
                quoteinfo.port_array[i] = port_split;
            }
            
            if(strcmp(hosts[i], "localhost") == 0)
            {
                quoteinfo.host_array[i] = "127.0.0.1";  //The C Client DOES NOT connect to "localhost", hence we need to change it to "127.0.0.1"
            }
            
            if (sock_socket < 0)
            {
                printf("Could not create socket");
                exit(1);
            }
            memset(&serv_addr, '0', sizeof(serv_addr));

            //SEE: https://www.geeksforgeeks.org/socket-programming-cc, for socket connecting in C

            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(quoteinfo.port_array[i]);
            
            //These two simple checks are to ensure that we are not connecting to a server that is not running
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
            int port_int = quoteinfo.port_array[i]; //retrieve the current portnum of the server
            sprintf(port_string, "%d", port_int); //Cast to string
            concatenate_quote(message, quote, comma, hosts[i], port_string); //concatenate the message, more on this in the header
            send(sock_socket , message, strlen(message) , 0 ); //send the message to the server
            connections[connection_num] = sock_socket; //store the socket in the connections array
            connection_num++;
        } 
    }

    fd_set readfds;

    int total_connections = connection_num;
    //iterate over all our connections, using select() to check if there is data to be read
    while(connection_num > 0)
    {
        FD_ZERO(&readfds);
        int max_fd = 0;

            for(int i = 0; i < total_connections; i++)
            {
                if (connections[i] > 0) {
                    FD_SET(connections[i], &readfds);
                }
                max_fd = (max_fd > connections[i]) ? max_fd : connections[i]; //if our max_fd is greater than the current connection, set max_fd to the current connection
            }

        max_fd++;

        int retval = select(max_fd, &readfds, NULL, NULL, NULL); //select() the number of sockets that are ready to be read

        if(retval == -1)
        {
            printf("Select error\n");
            exit(1);
        }
        else if(retval >= 0)
        {
            for (int i = 0; i < total_connections; i++) {

                //if the socket is ready to be read 
                if(FD_ISSET(connections[i], &readfds) && connections[i] > 0) {

                    char buffer[1024] = {0};
                    int valread = read(connections[i], buffer, 1024); //Read the message from the server

                    if(valread == 0)
                    {
                        printf("Server is down\n");
                        connections[i] = 0;
                    }
                    else
                    {
                        bool second_comma = false;
                        char* comma = ",";
                        int second_comma_index; //these simply check for the second comma in the message

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
                        hostname[first_comma_index] = '\0'; //nullbyte to terminate the string

                        //message e.g. "localhost,6238,90"
                        //this will run from "6238" --> second ","
                        for(int i = first_comma_index + 1; i < second_comma_index; i++) 
                        {
                            port[i - first_comma_index - 1] = buffer[i];
                        }
                        port[second_comma_index - first_comma_index - 1] = '\0';  
                        quoteinfo.host = hostname;
                        quoteinfo.port = atoi(port); //nullbyte to terminate the string

                        //message e.g. "localhost,6238,90"
                        //this will run from "90" --> end of string
                        char* cost_string = malloc(sizeof(char) * BUFFSIZE);
                        for(int i = second_comma_index; i < strlen(buffer); i++)
                        {
                            cost_string[i-second_comma_index] = buffer[i+1];
                        }
                        int cost = atoi(cost_string); //nullbyte to terminate the string

                        //if our current cost is less than min_cost
                        //then set the min_cost to the given cost
                        //likewise, swap out the host and port to a different struct
                        if(cost < min_cost) 
                        {
                            min_cost = cost;
                            finalinfo.host = quoteinfo.host;
                            finalinfo.port = quoteinfo.port;
                        }
                    }
                    connections[i] = -1; //remove the socket from the connections array
                    connection_num--;
                    shutdown(connections[i], SHUT_RDWR);  //shutdown the socket to prever read and writing
                }
            }
        }


    }
    return finalinfo; //return the struct containing the lowest cost server and its port            
}

//---------------------------- READ_DATA() --------------------------------------------------

/**
 * @brief Reads extra_data from the server if reported that it exists
 * 
 * @param sock the socket to read from
 * @param extra_data the extra_data to pass into the function to read and parse
 * @param is_File if set to true then the server is reporting a file, and we must open and write.
 * @param is_Err  if set to true then std_err has been detected
 * @return char* the extra_data to read
 */
char *read_data(int sock, char*extra_data, bool is_File, bool is_Err)
{
    int data_left = -1;
    char *f_data = calloc(strlen(extra_data) + 1,sizeof(char));
    if(extra_data[0] != '\0') //check if we are not an empty string
    {
        int first_comma = 0;
        for(int i = 0; i < strlen(extra_data); i++)
        {
            if(extra_data[i] == ',')   //finds the position of the first comma so we can split from there
            {
                first_comma = i;
                break;
            }
        }
        char *data_left_placeholder = malloc(sizeof(char) * BUFFSIZE);
        for(int i = 0; i < first_comma; i++)
        {
            data_left_placeholder[i] = extra_data[i]; //copy the data_left into a placeholder, this contains the amount of bytes left to read
        }
        data_left_placeholder[first_comma] = '\0'; //nullbyte to terminate the string
        data_left = atoi(data_left_placeholder);
        for(int i = first_comma + 1; i < strlen(extra_data); i++)
        {
            f_data[i-first_comma-1] = extra_data[i]; //similarly, start from the first comma and copy the remaining data to read into the f_data
        }
        f_data[strlen(extra_data)-first_comma-1] = '\0'; //nullbyte to terminate the string

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
            int dl_as_int = data_left; //lol, this is so we can use data_left as an int

            max_fdata_length = strlen(f_data);
            extra_data[strlen(f_data)];


            for(int i = dl_as_int; i < max_fdata_length; i++)
            {
                extra_data[i-dl_as_int] = f_data[i]; //simiarly for the code some blocks above, copy the data_left amount of bytes from f_data into extra_data
            }
            extra_data[max_fdata_length-dl_as_int] = '\0';

            //set f_data to f_data[:data_left]
            int f_data_index = 0;
            char*f_data_TWO = malloc(sizeof(char) * (strlen(f_data) + 1));
            while(f_data[f_data_index] != '\0' && f_data_index < dl_as_int )
            {
                f_data_TWO[f_data_index] = f_data[f_data_index]; //a more complicated way of copying data, but it works
                f_data_index++;
            }
            f_data_TWO[f_data_index] = '\0';
            strcpy(f_data, f_data_TWO);
            free(f_data_TWO); //free the memory as we will not us f_data_TWO anymore
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
        int valread = read(sock, buffer_data, 1024); //read the data from the socket 

        if(valread) //if a message has been read, then...
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
                    data_left_placeholder[i] = buffer_data[i]; //copy the buffer_data up to the first comma into a placeholder
                }
                data_left_placeholder[first_comma_index] = '\0'; //nullbyte to terminate string
                data_left = atoi(data_left_placeholder); //cast it to an int as this is the data_left now want.
                char* buffer_data_copy = malloc(sizeof(char) * strlen(buffer_data));
                for(int i = first_comma_index + 1; i < strlen(buffer_data); i++)
                {
                    buffer_data_copy[i-first_comma_index-1] = buffer_data[i];
                }
                buffer_data_copy[strlen(buffer_data)-first_comma_index-1] = '\0'; //nullbyte to terminate string
                strcpy(buffer_data, buffer_data_copy);

            }

            //This if blocks performs functionally the same as line 378.
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
            strcat(f_data, buffer_data); //concatenate the new buffer_data into our f_data for reading
        }
        
    }
    if(is_File == false && is_Err == false)
    {
        printf("OUTPUT--> %s\n", f_data); //simply print the output assuming no error
        return extra_data;
    }
    else if (is_Err == true)
    {
        printf("ERROR-->%s\n", f_data); //but because we have detected the error, print the error
        return extra_data;
    }
    //otherwise it is now a file_type so we must prepare to open and write
    //NOTE: THIS DOES NOT WORK, AS WE HAVE NOT IMPLEMENTED COMPILATION AND LINKING.
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

/**
 * @brief the bread and butter of the entire C client.
 * Processes the actions that we previously stored in our struct containing Rakefile information
 * 
 */
void process_actions()
{
    struct sockinfo info;
    bool error_found = false;
    int connections[1024];
    int valread;
        
    int sock = 0, client_fd;
    for(int s_index = 0; s_index < setcount; s_index++) //For each actionset
    {
        int connection_num = 0;
    
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;

        for(int a_index = 0; a_index < actioncounts[s_index]; a_index++) //For every action in the current actionset
        {
            char*curraction = actionsets[s_index][a_index].actionCommand; //retrieve our action

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

                printf("R-OUTGOING--> %s\n", curraction);
                info = quote_servers(s_index);                  //BECAUSE WE ARE REMOTE, WE QUOTE THE SERVERS FOR THE LOWEST COST
                char*new_hostname = malloc(sizeof(char) * strlen(info.host));
                strcpy(new_hostname, info.host);
                if(strcmp(new_hostname, "localhost") == 0)
                {
                    new_hostname = "127.0.0.1"; //Like in quote_servers, we must change localhosts to "127.0.0.1"
                }
                serv_addr.sin_port = htons(info.port);      //SEE: https://www.geeksforgeeks.org/socket-programming-cc, for socket connecting in C
                sock = socket(AF_INET, SOCK_STREAM, 0);
                inet_pton(AF_INET, new_hostname, &serv_addr.sin_addr);
                if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) //Connect
                {
                    printf("\nConnection Failed\n");
                    exit(1);
                }
                connection_num++;
            }
            else
            {
                printf("OUTGOING--> %s\n", curraction);
                serv_addr.sin_port = htons(portnumber);
                sock = socket(AF_INET, SOCK_STREAM, 0);
                inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);   //SEE: https://www.geeksforgeeks.org/socket-programming-cc, for socket connecting in C
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
        //performing our select() to wait for the responses from the servers
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
                        valread = read(connections[i], buffer_in, 1024); //read in messages from the server
                        int data_exitcode;
                        int data_stdout;
                        int data_stderr;
                        int data_fcount;
                        int data_left = -1;
                        char *data_left_placeholder = malloc(sizeof(char) * strlen(buffer_in)); //a placeholder for the data_left to read
                        char*data = malloc(sizeof(char) * strlen(buffer_in));
                        char* extra_data = calloc(strlen(buffer_in),sizeof(char));
                        char*f_data = malloc(sizeof(char) * strlen(buffer_in));
                        strcpy(data, buffer_in);
                        int first_comma_index = 0;
                        for(int i = 0; i < strlen(buffer_in); i++)  //find the first comma for splitting
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
                                data_left = atoi(data_left_placeholder); //similar to our quoting and reading, retrieve the data_left to read

                                char *data_placeholder = malloc(sizeof(char) * strlen(data));
                                for (int i = first_comma_index + 1; i < strlen(data); i++)
                                {
                                    data_placeholder[i - first_comma_index - 1] = data[i];
                                }
                                data_placeholder[strlen(data) - first_comma_index - 1] = '\0'; 
                                strcpy(data, data_placeholder); //now the rest of the data that is left of the message
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

                        struct comma_indices ci; //SEE THE HEADER FILE FOR THESE

                        int comma_index_one = init_comma_indices(f_data).comma_index_one;   //initialize the position of the first comma
                        int comma_index_two = init_comma_indices(f_data).comma_index_two;   //initialize the position of the second comma
                        int comma_index_three = init_comma_indices(f_data).comma_index_three;  //initialize the position of the third comma

                        data_exitcode = get_exit_code(f_data, comma_index_one); //the exticode lies between then start of the string and first comma
                        data_stdout = get_stdout(f_data, comma_index_one, comma_index_two); //the stdout lies between the first and second comma
                        data_stderr = get_stderr(f_data, comma_index_two, comma_index_three); //the stderr lies between the second and third comma
                        data_fcount = get_fcount(f_data, comma_index_three); //the fcount lies between the third and the end of the string

                        if(data_exitcode != 0) //an error has occured
                        {
                            error_found = true; 
                        }

                        if(data_stdout == 1) //we can output to the terminal with the extra_data
                        {
                            extra_data = read_data(connections[i], extra_data, false, false);
                        }
                        else
                        {
                            printf("OUTPUT--> None\n");
                        }

                        if(data_stderr == 1) //stderr has occured, pass in is_Err as true
                        {
                            extra_data = read_data(connections[i], extra_data, false, true);
                        }

                        free(extra_data); //free the extra_data as we don't need it anymore.
                        shutdown(connections[i], SHUT_RDWR);
                        connections[i] = -1;
                        connection_num--;
                    }
                }
            }
        }
        if(error_found)
            {
                printf("\nTERMINATED--> actionset %d returned errors\n", s_index+1); //terminate the at the end of the current actionset
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