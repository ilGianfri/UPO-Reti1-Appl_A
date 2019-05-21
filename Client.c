#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>


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
int getMessageLength(char result[], char type[], char content[]);
char* textMessageBuilder(char command[], char type[], int content);
void showSelection();
int countAlnum(char txt[]);
void flushAll();
int indexOfLastAllowedSpace(char fulls[]);

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
        void flushAll();

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
                if (buffer[3] == 'S' && buffer[4] == 'T')   //START
                {  
                    printf("%s", removeProtocolText(buffer));             

                    showSelection();
                }
                else if (buffer[3] == 'T' && buffer[4] == 'E')  //OK TEXT
                {
                    showSelection();
                }
                else if (buffer[3] == 'Q' && buffer[4] == 'U')  //QUIT
                {
                    fprintf(stdout, "%s", removeProtocolText(buffer));
                    close(currentSocket);
                    break;
                }
            }
            else    //ERROR
            {
                //TODO: HANDLE DIFF ERRORS
                fprintf(stderr, "Connection has been closed by the server. Error %d.\n", serverResponse);
                close(currentSocket);
                exit(1);
            }      
        }

        //Clears the buffer
        clearBuffer();
        flushAll();
    }
}

int countAlnum(char txt[])
{
    int c = 0;
    for (int i = 0; i < (int)strlen(txt); i++)
    {
        if (isalnum(txt[i]))
            c++;
    }
    return c;
}

void flushAll()
{
    fflush(stdin);
    fflush(stdout);
}

int indexOfLastAllowedSpace(char fulls[])
{
    // If the string is too long
    if (strlen(fulls) > 504)
    {
        int i = 504;
        // Finds the position of the first whitespace where the string will be split
        while (fulls[i] != ' ')
            i--;

        return i;
    }
    
    return 0;
}


int getMessageLength(char result[], char type[], char content[])
{
	char toreturn[MAX_LENGTH];
    sprintf(toreturn, PROTOCOLFORMAT, result, type, content);
	return strlen(toreturn);
}

void showSelection()
{
    fprintf(stdout, HELP);
    char choice = 0;
    scanf(" %c", &choice);
    fflush(stdin);

    void flushAll();
    
    switch (choice)
    {
        case 'a': case 'A':
        fprintf(stdout, "Please insert the text to analyze: ");
        char str[504];
        bzero(str, sizeof(str));
        //workaround
        fgets(str, sizeof(str), stdin);
        fgets(str, sizeof(str), stdin);
        strtok(str, "\n");
        //fprintf(stdout, "%s", str);
        //printf("%s", textMessageBuilder("TEXT", str, countAlnum(str)));

        if (strlen(str) > 504)
        {
            int c = indexOfLastAllowedSpace(str);
            //TODO
        }
        else
            write(currentSocket, textMessageBuilder("TEXT", str, countAlnum(str)), strlen(textMessageBuilder("TEXT", str, countAlnum(str))));
        break;

        case 'b': case 'B':
            write(currentSocket, "HIST\n", 5);
        break;
        case 'c': case 'C':
            write(currentSocket, "HIST\n", 5);
        break;
        case 'd': case 'D':
        write(currentSocket,"QUIT\n", 5);
        break;
        default:
        fprintf(stderr, "%c s not a valid choice.\n", choice);
        close(currentSocket);
        exit(1);
        break;
    }
}

// This method builds the the  messages to ensure they respect the protocol
char* textMessageBuilder(char command[], char type[], int content)
{
    char toreturn[MAX_LENGTH];
    snprintf(toreturn, sizeof(toreturn), "%s %s %d\n", command, type, content);
    //sprintf(toreturn, PROTOCOLFORMAT, command, type, content);
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




