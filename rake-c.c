/**
 *  CITS3002 2022 Sem 1 - Project - rake client C
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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "strsplit.c"


#define BUFFSIZE 100
//-------------------------------------------------------------------------

char buffer[BUFFSIZE];

char hosts[][BUFFSIZE] = {};

int portnumber;


struct {
    char actionCommand[BUFFSIZE];
    char **requirements;
    int requirementnum;
} actionsets[BUFFSIZE][BUFFSIZE];

int setcount = 0;
int actioncounts[BUFFSIZE];

// actionsets[actionsetnum][???] will be the max length for the first array
// actionsets[0][actioncounts[0]] will be the max length for the second array of the first index
// actionsets[1][actioncounts[1]] will be the max length for the second array of the second index
// requirements are already dynamic, so there's no need to change it

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


    while (fgets (buffer, BUFFSIZE, fptr))
    {
        buffer[strcspn (buffer, "#\r\n")] = 0;  /* trim comment or line-ending */

        if (StartsWith(buffer, "\t")) //check if line is one tabbed - these are the "actions"
        {

            if (StartsWith(buffer, "\t\t")) //check if line is two tabs - these are the "requires"
            {

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
                strcpy(actionsets[setnum][actionnum].actionCommand, trimwhitespace(buffer));
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
                if (setnum != -1) {
                    actioncounts[setnum] = actionnum;
                }
                setnum++;
                setcount++;

                actionnum = 0;

            }
        }
    }
    actioncounts[setnum] = actionnum;
    //printf("total_action_count at [%d]: %d\n", setnum, total_action_count);
    fclose(fptr);
}

//--------------------------------------------------------------------------------------------------------------------------


void send_message(int sock, int valread, char buffer[], char* message)
{
    printf("Sending message from C Client...\n");
    send(sock , message, strlen(message) , 0 );
    valread = read(sock, buffer, 1024);
    printf("Received the message: %s\n", buffer);
}

void quote_servers()
{
    float placeholder = 'inf';
    int min_cost = (int) placeholder;

}

//--------------------------------------------------------------------------------------------------------------------------


int main(int argc, char* argv[]) {
    read_rakefile(argv[1]);

    //for(int i = 0; i < 10; i++)
    //{
        // for(int j = 0; j < 10; j++)
        // {
             //printf("actionsets[%d][%d]: %s\n", i, j, actionsets[i][j].actionCommand);
          //   if (actionsets[i][j].requirementnum > 0) {
               //  for (int z = 0; z < actionsets[i][j].requirementnum; z++) {

                     //printf("actionsets[%d][%d] req %d: %s \n", i, j, z, actionsets[i][j].requirements[z]);
             //        printf("actionsets[%d][%d]: %s \n", i, j, &actionsets[i][j].actionCommand);
           //      }
         //    }
       //  }
     //}
     for(int i = 0; i < 10; i++)
     {
         for(int j = 0; j < actioncounts[i]; j++)
         {
             if(strlen(actionsets[i][j].actionCommand) > 0)
             {
                 printf("actionsets[%d][%d]: %s \n", i, j, actionsets[i][j].actionCommand);
             }
         }
     }
    quote_servers();
    while (true) {
        int sock = 0, valread;
        struct sockaddr_in serv_addr;
        char *message = "Hello from C Client";
        char buffer[BUFFSIZE] = {0};

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("\n Socket creation error \n");
            return -1;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(portnumber);

        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
        }

        if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            printf("\nConnection Failed \n");
            return -1;
        }

        send_message(sock, valread, buffer, message);
    }


    return 0;
}
