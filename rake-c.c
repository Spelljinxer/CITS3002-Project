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

//2d Array of strings containing hosts
char hosts[][BUFFSIZE] = {};
//cretae a 3d array of strings containing the actions
char actionsets[BUFFSIZE][BUFFSIZE][BUFFSIZE] = {};
int portnumber;

struct action{
    char actionCommand[BUFFSIZE][BUFFSIZE];
    char requirements[BUFFSIZE][BUFFSIZE];
};

struct storage {
    char commandStorage[BUFFSIZE][BUFFSIZE];
    char requirementsStorage[BUFFSIZE][BUFFSIZE];
};


bool StartsWith(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

void read_rakefile(char *rakefile, struct action *action, struct storage *storage) { 
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
                char** words = strsplit(buffer, &nwords);
                for(int i = 1; i < nwords; i++)
                {
                    strcpy(action->requirements[i], strcat(words[i], " "));
                    strcpy(storage->requirementsStorage[setnum], action->requirements[i]);
                    strcat(actionsets[setnum][actionnum-1], action->requirements[i]);
                }
            }
            else
            {
                strcpy(action->actionCommand[setnum], buffer);
                strcpy(storage->commandStorage[setnum], action->actionCommand[setnum]);
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
    struct storage *storage = malloc(sizeof(struct storage));

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
       printf("commandStorage at %d is: %s\n", i, storage->commandStorage[i]);
    }
    free(action);
    return 0;
}
