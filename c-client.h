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

struct comma_indices{
    int comma_index_one;
    int comma_index_two;
    int comma_index_three;
};

struct comma_indices init_comma_indices(char *fdata)
{
    struct comma_indices ci;
    ci.comma_index_one = 0;
    ci.comma_index_two = 0;
    ci.comma_index_three = 0;
    for (int i = 0; i < strlen(fdata); i++)
    {
        if (fdata[i] == ',')
        {
            if (ci.comma_index_one == 0)
            {
                ci.comma_index_one = i;
            }
            else if (ci.comma_index_two == 0)
            {
                ci.comma_index_two = i;
            }
            else if (ci.comma_index_three == 0)
            {
                ci.comma_index_three = i;
            }
        }
    }
    return ci;
}

int get_exit_code(char *f_data, int comma_index_one)
{
    char *data_exitcode_hold = malloc(strlen(f_data) + 1);
    for(int i = 0; i < comma_index_one; i++)
    {
        data_exitcode_hold[i] = f_data[i];
    }
    data_exitcode_hold[comma_index_one] = '\0';
    int exit_code = atoi(data_exitcode_hold);
    free(data_exitcode_hold);
    return exit_code;
}

int get_stdout(char *f_data, int comma_index_one, int comma_index_two)
{
    char *data_stdout_hold = malloc(strlen(f_data) + 1);
    for(int i = comma_index_one + 1; i < comma_index_two; i++)
    {
        data_stdout_hold[i - comma_index_one - 1] = f_data[i];
    }
    data_stdout_hold[comma_index_two] = '\0';
    int stdout_code = atoi(data_stdout_hold);
    free(data_stdout_hold);
    return stdout_code;
}

int get_stderr(char *f_data, int comma_index_two, int comma_index_three)
{
    char *data_stderr_hold = malloc(strlen(f_data) + 1);
    for(int i = comma_index_two + 1; i < comma_index_three; i++)
    {
        data_stderr_hold[i - comma_index_two - 1] = f_data[i];
    }
    data_stderr_hold[comma_index_three] = '\0';
    int stderr_code = atoi(data_stderr_hold);
    free(data_stderr_hold);
    return stderr_code;
}

int get_fcount(char *f_data, int comma_index_three)
{
    char *data_fcount_hold = malloc(strlen(f_data) + 1);
    for(int i = comma_index_three + 1; i < strlen(f_data); i++)
    {
        data_fcount_hold[i - comma_index_three - 1] = f_data[i];
    }
    data_fcount_hold[strlen(f_data) - comma_index_three - 1] = '\0';
    int fcount_code = atoi(data_fcount_hold);
    free(data_fcount_hold);
    return fcount_code;
}

//-------------------------------------------------------------------------------------------------------------

/**
 * @brief simply checks if the string starts with another string
 * 
 * @param a 
 * @param b 
 * @return true if it does start with b
 * @return false otherwise
 */
bool starts_with(const char *a, const char *b) 
{
    if(strncmp(a, b, strlen(b)) == 0) 
    {
        return 1;
    }
    return 0;
}
/**
 * @brief checks for white space in str and removes it if it exists, returns str with whitespace removed
 * 
 * @param str 
 * @return str with whitespace removed
 */
 
char *trim_whitespace(char *str) 
{
    char *end;

    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';

    return str;
}

/**
 * @brief counts the number time delim appears and returns the count
 * 
 * @param str 
 * @param delim
 * @return int count of delim in str 
 */
int char_counter(char *str, char* delim) 
{
    int count = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == delim[0])
            count++;
    }
    return count;
}

/**
 * @brief returns a string containing the first seven characters of s
 * 
 * @param s 
 * @return char* result of seven characters
 */
char* get_first_seven_chars(char *s) //returns a string containing the first seven characters of s
{
    char *result = malloc(sizeof(char) * 7);
    strncpy(result, s, 7);
    return result;
}

/**
 * @brief //returns a string containing s1 and s2 (in that order)
 * 
 * @param s1 
 * @param s2
 * @return char* result of s1 and s2
 */
char *concatenate_strings(char *s1, char *s2) 
{
    char *result = malloc(sizeof(char) * (strlen(s1) + strlen(s2) + 1));
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

/**
 * @brief returns a modified message containing quote + hostname and port separated by a comma
 * 
 * @param message = message to be modified ""
 * @param quote = quote to be added to message "Quote"
 * @param comma = comma to be added between hostname and port ","
 * @param hostname = hostname to be added to message "localhost"
 * @param port = port number to be added to message "6239"
 */
void concatenate_quote(char* message, char* quote, char*comma, char*hostname, char*port) 
{
    strcpy(message, quote);
    strcat(message, hostname);
    strcat(message, comma);
    strcat(message, port);
}

/**
 * @brief returns the number of chars in arr
 * 
 * @param arr
 * @return int i count of elements
 */
int get_number_of_elements(char ** arr) 
{
    int i = 0;
    while(arr[i] != NULL)
    {
        i++;
    }
    return i;
}
/**
 * @brief returns the index of the first occurrence of delim inside of str
 * 
 * @param str 
 * @param delim 
 * @return int first occurrence index of delim inside of str
 */
int get_char_index(char *str, int delim) 
{
    int first_occurence = 0;
    while(str[first_occurence] != delim)
    {
        first_occurence++;
    }
    return first_occurence;
}

//---------------------------------------------------

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


