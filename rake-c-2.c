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


#define BUFFSIZE 50
//-------------------------------------------------------------------------

char buffer[BUFFSIZE];

char hosts[][BUFFSIZE] = {};

int portnumber;

struct {
    char actionCommand[BUFFSIZE];
    char **requirements;
    int requirementnum;
} actionsets[BUFFSIZE][BUFFSIZE];


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

    int cmdStorageIndex = 0;
    while (fgets (buffer, BUFFSIZE, fptr))
    {
        buffer[strcspn (buffer, "#\r\n")] = 0;  /* trim comment or line-ending */

        if (StartsWith(buffer, "\t")) //check if line is one tabbed - these are the "actions"
        {

            if (StartsWith(buffer, "\t\t")) //check if line is two tabs - these are the "requires"
            {
                int nwords;
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
                // TODO: remove tab from start of actioncommand
                strcpy(actionsets[setnum][actionnum].actionCommand, buffer);
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
                setnum++;
                actionnum = 0;
            }
        }
    }
    fclose(fptr);
}



int main(int argc, char* argv[])
{
    read_rakefile(argv[1]);

    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            printf("actionsets[%d][%d]: %s\n", i, j, actionsets[i][j].actionCommand);
            if (actionsets[i][j].requirementnum > 0) {
                for (int z = 0; z < actionsets[i][j].requirementnum; z++) {
                    printf("actionsets[%d][%d] req %d: %s \n", i, j, z, actionsets[i][j].requirements[z]);
                }
            }
        }
        printf("Port: %d\n",portnumber);
    }
    return 0;
}