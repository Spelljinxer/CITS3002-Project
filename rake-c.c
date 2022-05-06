/** 
 *  CITS3002 2022 Sem 1 - Project
 * @authors
 *  - Daivik Anil 22987816
 *  - Nathan Eden 22960674
 *  - Reiden Rufin 22986337
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "strsplit.c"


#define BUFFSIZE 512

//-------------------------------------------------------------------------

char buffer[BUFFSIZE];

char hosts[][BUFFSIZE] = {};
char actionsets[BUFFSIZE][BUFFSIZE][BUFFSIZE] = {};

int portnumber;

struct action{
    char actionCommand[BUFFSIZE];
    char requirements[BUFFSIZE][BUFFSIZE];

    char commandStorage[BUFFSIZE][BUFFSIZE];
    char requirementStorage[BUFFSIZE][BUFFSIZE][BUFFSIZE];
};


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

char *removeToken(char *buffer) 
{
    const char *delimiters = " ,";
    size_t i, skip;

    skip = strcspn(buffer, delimiters); /* skip the word */
    skip += strspn(buffer + skip, delimiters); /* skip the delimiters */

    for (i = 0; buffer[skip + i] != '\0'; i++) {
        buffer[i] = buffer[skip + i];
    }
    buffer[i] = '\0';

    return buffer;
}

void read_rakefile(char *rakefile, struct action *action){ 
    FILE *fptr = fopen(rakefile, "r");
    
    if (fptr == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    
    int setnum = -1;
    int actionnum = -1;
    int nwords;

    int cmdStorageIndex = 0;
    while (fgets (buffer, BUFFSIZE, fptr)) 
    {  
        buffer[strcspn (buffer, "#\r\n")] = 0;  /* trim comment or line-ending */       

        if (StartsWith(buffer, "\t")) //check if line is one tabbed - these are the "actions"
        {
            
            if (StartsWith(buffer, "\t\t")) //check if line is two tabs - these are the "requires"
            {
                char *str = strdup(buffer);
                char *trimmed = trimwhitespace(str);
                char *word = removeToken(trimmed);

                strcpy(action->requirements[cmdStorageIndex], word);
                cmdStorageIndex ++;
                strcat(actionsets[setnum][actionnum-1], word);
            }
            else
            {
                char *trail = " ";
                char* command = strcpy(action[setnum].actionCommand, buffer);
                strcat(command, trail);
                strcpy(actionsets[setnum][actionnum], command);
                // printf("actiontest at [%d]: %s\n", setnum, action[setnum].actiontest);
                
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
               setnum++;
               actionnum = 0;
           }
        }
    }
    fclose(fptr);
}



int main(int argc, char* argv[])
{
    struct action *action = malloc(sizeof(struct action));
    read_rakefile(argv[1], action); 

    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            if(strlen(actionsets[i][j]) > 0)
            {
                printf("actionsets[%d][%d]: %s\n", i, j, actionsets[i][j]);
            }
        }
    }
    free(action);
    return 0;
}
