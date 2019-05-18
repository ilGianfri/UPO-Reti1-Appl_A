#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>


const char WRONGARGS[] = "Missing parameter\n\nSYNTAX: $%s <server address> <port number>\n";
const char HELP[] = "\nThe available options for this client are:\na. Insert text\nb. Analyze text\nc. Exit program (with text analysis)\nd. Exit program (without text analysis)\n\nWhat would you like to do? ";
const int MAX_LENGTH = 512;

int currentSocket = 0, port = 0;
struct sockaddr_in scktAddr;
char buffer[512] = "";
char PROTOCOLFORMAT[] = "%s %s %s\n";
char OKRES[] = "OK";
char ERRORMSG[] = "ERR";

void clearBuffer();
char* removeProtocolText(char b[]);
char* getResponseResult(char fulls[]);
char* getResponseType(char fulls[]);
char showSelection();

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
    
    while(1)
    {
        //Listens on the socket for messages
        int serverResponse = read(currentSocket, buffer, sizeof(buffer));
        if (serverResponse == -1)
        {
            fprintf(stderr, "Connection has been closed.\n");
            close(currentSocket);
            exit(1);
        }
        else
        {
            // Distinguish between a OK response from the server and an error
            if (buffer[0] == 'O' && buffer[1] == 'K') //OK
            {
                printf("%s", removeProtocolText(buffer));
                
                if (buffer[3] == 'S')   //START
                {  
                    char selection = showSelection();
                    
                    switch (selection)
                    {
                        case 'a':
                        fprintf(stdout, "Please insert the text to analyze: ");
                        fflush(stdout);
                        char str[512];
                        fgets(str, sizeof(str), stdin);
                        fprintf(stdout, "%s", str);
                        break;
                    case 'b':
                    break;
                    case 'c':
                    break;
                    case 'd':
                    write(currentSocket,"QUIT\n", 6);
                    break;
                    default:
                    //error
                        break;
                    }
                }
                else if (buffer[3] == 'Q')  //QUIT
                {
                    fprintf(stdout ,"%s", removeProtocolText(buffer));
                    close(currentSocket);
                    break;
                }
            }
            else    //ERROR
            {
                fprintf(stderr, "Received an error from the server. Connection will be closed. Error %d.\n", serverResponse);
                close(currentSocket);
                exit(1);
            }      
        }

        //Clears the buffer
        clearBuffer();
    }
}

char showSelection()
{
    fprintf(stdout, HELP);
    char choice = 0;
    scanf(" %c", &choice);
    fflush(stdin);

    return choice;
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

	char cmd[pointer - 1];
	memcpy(cmd, &fulls[0], pointer - 1);
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




