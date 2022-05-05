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
#include "strsplit.c"


#define BUFFSIZE 512

//-------------------------------------------------------------------------

char buffer[BUFFSIZE];

char hosts[][BUFFSIZE] = {};
char actionsets[BUFFSIZE][BUFFSIZE][BUFFSIZE] = {};

int portnumber;

struct action{
    char actionCommand[BUFFSIZE][BUFFSIZE];
    char requirements[BUFFSIZE][BUFFSIZE];

    char commandStorage[BUFFSIZE][BUFFSIZE];
    char requirementStorage[BUFFSIZE][BUFFSIZE][BUFFSIZE];

};


bool StartsWith(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

void read_rakefile(char *rakefile, struct action *action, struct action *storage){ 
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
                char** words = strsplit(buffer, &nwords);
                for(int i = 1; i < nwords; i++)
                {
                    strcpy(action->requirements[i], strcat(words[i], " "));
                    strcpy(storage->requirementStorage[setnum][actionnum-1], buffer);
                    strcat(actionsets[setnum][actionnum-1], action->requirements[i]);
                }
            }
            else
            {
                strcpy(action->actionCommand[setnum], buffer);
                strcpy(storage->commandStorage[cmdStorageIndex], action->actionCommand[setnum]);
                cmdStorageIndex++;
                strcpy(actionsets[setnum][actionnum], strcat(action->actionCommand[setnum], " "));
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
    struct action *storage = malloc(sizeof(struct action));
    read_rakefile(argv[1], action, storage); 

    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if(strlen(actionsets[i][j]) > 0)
            {
                printf("actionset[%d][%d] is: %s\n", i, j, actionsets[i][j]);
            }
        }
    }
    for(int i = 0; i < 8; i++)
    {
        if(strlen(storage->commandStorage[i]) > 0)
        {
            printf("command storage[%d] is: %s\n", i, storage->commandStorage[i]);
        }
    }

    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if(strlen(storage->requirementStorage[i][j]) > 0)
            {
                printf("requirement storage[%d][%d] is: %s\n", i, j, storage->requirementStorage[i][j]);
            }
        }
    }
    free(action);
    return 0;
}
