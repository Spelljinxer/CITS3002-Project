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


#define BUFFSIZE 1024

//-------------------------------------------------------------------------

char buffer[BUFFSIZE];

//2d Array of strings containing hosts
char hosts[][BUFFSIZE] = {};
//3d Array of strings containg the actionsets
char*** actionsets;
int portnumber;

bool StartsWith(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

void read_rakefile(char *rakefile)
{
    FILE *fptr = fopen(rakefile, "r");
    
    if (fptr == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    
    int setnum = 0;
    int actionnum = 1;
    int nwords;
    // https://stackoverflow.com/questions/56226129/how-to-skip-a-comment-in-c-programming-with-using-fopen
    while (fgets (buffer, BUFFSIZE, fptr)) 
    {  
        buffer[strcspn (buffer, "#\r\n")] = 0;  /* trim comment or line-ending */       

        if (StartsWith(buffer, "\t")) //check if line is one tabbed - these are the "actions"
        {
            printf("%s\n", buffer);
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
               actionnum = 1;
           }
        }
    }
    fclose(fptr);
}



int main(int argc, char* argv[])
{
    read_rakefile(argv[1]); //assuming Rakefile is the next argument
    return 0;
}
