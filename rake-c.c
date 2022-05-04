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

char* arr[BUFFSIZE][BUFFSIZE];

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
        char **words = strsplit(buffer, &nwords);
        for(int w=0 ; w<nwords ; ++w) 
        {
            //place PORT and port number in array
            if(strcmp(words[w], "PORT") == 0)
            {
                //places "PORT" in first column
                arr[0][w] = words[w];
                //places port number in second column
                arr[1][w] = words[w+2];
            }
            //place HOST inside third column
            else if(strcmp(words[w], "HOST") == 0)
            {
                arr[2][w] = words[w];
                //place host name inside fourth column
                arr[3][w] = words[w+2];
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
