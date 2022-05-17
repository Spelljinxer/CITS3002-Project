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
#include <arpa/inet.h>
#include <unistd.h>

//-----------------------------------------------------------------------------------------------------
#define BUFFSIZE 1024

char buffer[BUFFSIZE];

int total_lines = 0;
int total_actionset_count = 0;
int total_actions_count = 0;
int longest_requirements_line = 0;
int host_length = 0;

//-----------------------------------------------------------------------------------------------------

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
}


