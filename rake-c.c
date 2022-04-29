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

#define BUFFSIZE 1024

//-------------------------------------------------------------------------


void read_rakefile(char *rakefile)
{
    FILE *fptr = fopen(rakefile, "r");
    char buf[BUFFSIZE];
    
    if(fptr == NULL) //simple check
    {
        printf("Error opening file\n");
        exit(1);
    }
    // https://stackoverflow.com/questions/56226129/how-to-skip-a-comment-in-c-programming-with-using-fopen
    while (fgets (buf, BUFFSIZE, fptr)) {   /* read every line */
        buf[strcspn (buf, "#\r\n")] = 0;  /* trim comment or line-ending */
        puts (buf);                      /* output line w/o comment - replace this later when we add it to a DS */
    }
    if (fptr != stdin) fclose (fptr);
}


int main(int argc, char* argv[])
{
    read_rakefile(argv[1]); //  "./rake-c Rakefile"
    return 0;
}

