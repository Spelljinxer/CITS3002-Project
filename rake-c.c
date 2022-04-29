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

char (*lines)[BUFFSIZE] = NULL; 

void read_rakefile(char *rakefile)
{
    FILE *fptr = fopen(rakefile, "r");
    
    int i, n = 0;
    int sizecheck = BUFFSIZE;
    
    if(fptr == NULL) //simple check
    {
        printf("Error opening file\n");
        exit(1);
    }
    
    if (!(lines = malloc (BUFFSIZE * sizeof *lines))) { /* allocate MAXL arrays */
        fprintf (stderr, "error: virtual memory exhausted 'lines'.\n");
        exit(1);
    }
    //https://stackoverflow.com/questions/36801833/storing-each-line-of-a-text-file-into-an-array
    while (n < BUFFSIZE && fgets (lines[n], BUFFSIZE, fptr)) { /* read each line */
        lines[n][strcspn (lines[n], "#\r\n")] = 0; /* trim comment or line-ending */
        char *p = lines[n];                  /* assign pointer */
        for (; *p && *p != '\n'; p++) {}     /* find 1st '\n'  */
        *p = 0, n++;                         /* nul-termiante  */
        if(n == sizecheck)  // ATTEMPTS DYNAMIC REALLOCATION IF WE HAVE REACHED THE END OF THE BUFFER
        {
            void *tmp = realloc (lines, 2 * BUFFSIZE * sizeof *lines);
            if(tmp == NULL)
            {
                fprintf (stderr, "error: virtual memory exhausted 'lines'.\n");
                break;
            }
            sizecheck *= 2;
        }
    }
    
    fclose (fptr);   /* close file if not stdin */

    for (i = 0; i < n; i++) printf ("line[%2d] : %s\n", i + 1, lines[i]);

    free (lines);   /* free allocated memory */
}


int main(int argc, char* argv[])
{
    read_rakefile(argv[1]); //  "./rake-c Rakefile"
    return 0;
}
