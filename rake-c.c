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

void read_rakefile(char *rakefile)
{
    FILE *fptr = fopen(rakefile, "r");
    
    if (fptr == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    
    int nwords;
    // https://stackoverflow.com/questions/56226129/how-to-skip-a-comment-in-c-programming-with-using-fopen
    while (fgets (buffer, BUFFSIZE, fptr)) {   /* read every line */
        buffer[strcspn (buffer, "#\r\n")] = 0;  /* trim comment or line-ending */
        char **words = strsplit(buffer, &nwords); //split each line based on the spaces

        for(int w=0 ; w<nwords ; ++w) 
        {
            
            //retrieve the Port number
            if(strcmp(words[w], "PORT") == 0)
            {
               portnumber = atoi(words[w+2]);
               printf("port number is %d\n", portnumber);
            }
            //place all the hostnames found on that line into the hosts array
            if(strcmp(words[w], "HOSTS") == 0)
            {
                for(int i=0 ; i<nwords - 2; ++i)
                {
                    strcpy(hosts[i], words[w+2+i]);
                }
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
