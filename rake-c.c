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

//#define MAX_LINE_LENGTH 1024

//-------------------------------------------------------------------------


void read_rakefile(char *rakefile)
{
    int c;
    FILE *fptr;

    if ((fptr = fopen(rakefile, "r")) == NULL)
    {
        printf("Error reading file\n");
    }
    else
    {
        while ((c = getc(fptr)) != EOF)
        {
            printf("%c", c);
        }
        fclose(fptr);
    }
}


int main(int argc, char* argv[])
{
    read_rakefile(argv[1]);
    return 0;
}

