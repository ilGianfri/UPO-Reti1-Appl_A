#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>


const char WRONGARGS[] = "Missing parameter\n\nSYNTAX: $%s <server address> <port number>\n";
const char HELP[] = "\nThe available options for this client are:\na. Insert text\nb. Analyze text\nc. Exit program (with text analysis)\nd. Exit program (without text analysis)\n\nWhat would you like to do?\n";
const int MAX_LENGTH = 512;

int currentSocket = 0, port = 0;
struct sockaddr_in scktAddr;
char buffer[512] = "";
char PROTOCOLFORMAT[] = "%s %s %s\n";
char OKRES[] = "OK";
char ERRORMSG[] = "ERR";

void clearBuffer();
char* removeProtocolText(char b[]);

void main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, WRONGARGS, argv[0]);
		return;
    }

    currentSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //Saves the specified port
	port = atoi(argv[2]);

	//Creates the socket
	currentSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//Throws an error is the socket could not be created for any reson
	if (currentSocket == -1)
	{
		fprintf(stderr, "Could not create a socket. Please verify your network settings.\n");
		return;
	}
	else
	{
		//Socket created successfully, prints diagnostic message
		printf("Socket created successfully.\n");
	}

    //Sets the socket port and IP
    memset(&scktAddr, '\0', sizeof(scktAddr));
    scktAddr.sin_family = AF_INET;
    scktAddr.sin_addr.s_addr=inet_addr(argv[1]);
    scktAddr.sin_port = htons(port);

    int connectionStatus = connect(currentSocket, (struct sockaddr *)&scktAddr, sizeof(scktAddr));
    if (connectionStatus == 0)
        printf("Connection to %s on port %d enstablished successfully.\n", argv[1], port);
    else
    {
        fprintf(stderr, "Could not connect to %s on port %d.\n", argv[1], port);
        close(currentSocket);
        return;
    }
    
    int serverResponse = 0;
    while(1)
    {
        //Listens on the socket for messages
        serverResponse = read(currentSocket, buffer, sizeof(buffer));
        if (serverResponse > 0)
        {
            printf("%s", removeProtocolText(buffer));
            clearBuffer();

            //TODO: CHECK IF IT's OK
            char* res = getResponseResult(buffer);
            if (memcmp(res, OKRES, sizeof(res)) == 0)
            {
                //TODO: CHECK SECOND PART OF THE METHOD
                printf(HELP);

                char choice = 0;
                scanf(" %c", &choice);
                switch(choice)
                {
                    case 'd': case 'D':
                    write(currentSocket,"QUIT\n", 6);
                    break;
                }
            }
            else
            {
                //TODO: CLOSE CONNECTION AND CLOSE
            }
            
        }
        else
        {
            fprintf(stderr, "Could not read the received message Error %d.\n", serverResponse);
            exit(1);
        }
    }
}

// This method builds the response messages to ensure they respect the protocol
char* responseBuilder(char command[], char type[], char content[])
{
    char toreturn[MAX_LENGTH];
    sprintf(toreturn, PROTOCOLFORMAT, command, type, content);
    char *ptr = toreturn;

    return ptr;
}



// Gets the result from the server response
char* getResponseResult(char fulls[])
{
    int spacecount = 0;
    int pointer = 0;

     while (spacecount < 1)
    {
        if (fulls[pointer] == ' ')
            spacecount++;

        pointer++;
    }

	char cmd[pointer + 1];
	memcpy(cmd, &fulls[0], pointer);
	char *ptr = cmd;

	return ptr;
}

// Clears the buffer
void clearBuffer()
{
    bzero(buffer, sizeof(buffer));
    //memset(buffer, 0, sizeof(buffer));
}

// Removes all the protocol text leaving only the message
char* removeProtocolText(char b[])
{
    int spacecount = 0;
    int pointer = 0;

    while (spacecount < 2)
    {
        if (b[pointer] == ' ')
            spacecount++;

        pointer++;
    }

    char toreturn[MAX_LENGTH];
    char *ptr = toreturn;
    memcpy(toreturn, &b[pointer], strlen(b)-pointer + 1);

    return ptr;
}




