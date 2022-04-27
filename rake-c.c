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

//-------------------------------------------------------------------------


void read_rakefile(char *rakefile)
{
    int c;
    FILE *fptr = fopen(rakefile, "r");
    char buf[1024];
    
    if(fptr == NULL) //simple check
    {
        printf("Error opening file\n");
        exit(1);
    }

    if (fgets(buf, sizeof buf, fptr))
    {
        char *delim = strstr(buf, "#"); 
        if (delim)
        {
            *delim = '\0'; //ignore the "#" comments in rakefile
        }
        printf("%s\n", buf);
        while ((c = getc(fptr)) != EOF)  
        {
            printf("%c", c); 
        }
        fclose(fptr);
    }
}


int main(int argc, char* argv[])
{
    read_rakefile(argv[1]); //  "./rake-c Rakefile"
    return 0;
}

