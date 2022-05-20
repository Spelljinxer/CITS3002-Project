/**
 *  CITS3002 2022 Sem 1 - Project - Header File for the C client
 * @authors
 *  - Daivik Anil 22987816
 *  - Nathan Eden 22960674
 *  - Reiden Rufin 22986337
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>

//-----------------------------------------------------------------------------------------------------
#define BUFFSIZE 1024

char buffer[BUFFSIZE];

int total_lines = 0;
int total_actionset_count = 0;
int total_actions_count = 0;
int longest_requirements_line = 0;
int host_length = 0;

//-----------------------------------------------------------------------------------------------------

int count_char(char *str, char c) //string, delimiter 
{
    int count = 0;
    for(int i = 0; i<=strlen(str); i++)
    {
        if(str[i] == c)
        {
            count++;
        }
    }
    return count;
}

bool StartsWith(const char *a, const char *b)
{
    if(strncmp(a, b, strlen(b)) == 0) return 1;
    return 0;
}

char *trimwhitespace(char *str)
{
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

char* get_first_seven_chars(char *s)
{
    char *result = malloc(sizeof(char) * 7);
    strncpy(result, s, 7);
    return result;
}

char *concatenate_strings(char *s1, char *s2)
{
    char *result = malloc(sizeof(char) * (strlen(s1) + strlen(s2) + 1));
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}


void test_socket_address(char* hostname, struct sockaddr_in serv_ddr)
{
    if (inet_pton(AF_INET, hostname, &serv_ddr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        exit(1);
    }
}

void test_socket_connecton(int socket, struct sockaddr_in server)
{
    if(connect(socket, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("Could not connect to server\n");
        exit(1);
    }
    else
    {
        printf("Connected to server\n");
    }
}

int test_client_fd(int client_fd, int sock, struct sockaddr_in serv_addr, size_t serv_size)
{
    client_fd = connect(sock, (struct sockaddr*)&serv_addr, serv_size);
    if(client_fd < 0)
    {
        printf("Connection Failed\n");
        return -1;
    }
    else
    {
        printf("Connected to server\n");
    }
    return client_fd;
}

char** split_string_to_array(char *string_to_split, const char* delim)
{
    char *newstring;
    strcpy(newstring, string_to_split);
    char **result = malloc(sizeof(char*) * strlen(string_to_split) + 1);
    int index = 0;
    char*split_char = strtok(newstring, delim);
    while(split_char != NULL)
    {
        result[index] = malloc(sizeof(char) * strlen(split_char) + 1);
        strcpy(result[index], split_char);
        split_char = strtok(NULL, delim);
        index++;
    }
    return result;
}

char *splice_string(char* string, int start, int end)
{
    char *result = malloc(sizeof(char) * (end - start + 1));
    strncpy(result, &string[start], end - start);
    result[end - start] = '\0';
    return result;
}

int get_number_of_elements(char ** arr)
{
    int i = 0;
    while(arr[i] != NULL)
    {
        i++;
    }
    return i;
}

int get_char_index(char *str, int delim)
{
    int first_occurence = 0;
    while(str[first_occurence] != delim)
    {
        first_occurence++;
    }
    return first_occurence;
}


void extract_line_data(char *rakefile)
{
    FILE *fp;
    char longest[BUFFSIZE];
    fp = fopen(rakefile, "r");
    if (fp == NULL)
    {
        printf("Error reading file\n");
        exit(1);
    }
    while(fgets(buffer, BUFFSIZE, fp))
    {
        buffer[strcspn (buffer, "#\r\n")] = 0; //trim comments
        if(strlen(buffer) > 1)
        {
            total_lines++;
        }
        
        if(StartsWith(buffer, "\t")) //ACTIONS
        {
            
            if (StartsWith(buffer, "\t\t")) //REQUIREMENTS
            {
                if(longest_requirements_line < strlen(buffer))
                {
                    longest_requirements_line = strlen(buffer);
                }
            }
            else
            {
                if(strlen(buffer) > 2) //if the line is not empty
                {
                    total_actions_count++;
                }
            }
        }
        else
        {
            if(strstr(buffer, "HOSTS"))
            {
                host_length = strlen(buffer);
            }
            else if (strstr(buffer, ":")) //actionsets
            {
                total_actionset_count++;
            }
        }

    }
    fclose(fp);
    // printf("total_actionset_lines: %d\n", total_actionset_count);
    // printf("total_actions_length: %d\n", total_actions_count);
    // printf("longest_requirements_line: %d\n", longest_requirements_line);
}


