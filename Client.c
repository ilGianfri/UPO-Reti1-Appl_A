#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

const char WRONGARGS[] = "Missing parameter\n\nSYNTAX: $%s <server address> <port number>\n";
const char HELP[] = "\nThis software lets you send one or more strings to a server that will calculate the frequency of each character and return it.\nThe available options for this client are:\na. Insert text\nb. Analyze text\nc. Exit program (with text analysis)\nd. Exit program (without text analysis)\n\nWhat would you like to do? ";
const int MAX_LENGTH = 512;

int currentSocket = 0, port = 0, exit_req = 0;
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
// int splitAndSendText(char fulls[]);

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
        clearBuffer();

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
                if (buffer[3] == 'S' && buffer[4] == 'T' && buffer[5] == 'A' && buffer[6] == 'R' && buffer[7] == 'T')   //OK START
                {  
                    printf("%s", removeProtocolText(buffer));             

                    showSelection();
                }
                else if (buffer[3] == 'T' && buffer[4] == 'E' && buffer[5] == 'X' && buffer[6] == 'T')  //OK TEXT
                {
                    fprintf(stdout, "Server has received the message successfully.\n");
                    showSelection();
                }
                else if (buffer[3] == 'Q' && buffer[4] == 'U' && buffer[5] == 'I' && buffer[6] == 'T')  //OK QUIT
                {
                    fprintf(stdout, "%s", removeProtocolText(buffer));
                    close(currentSocket);
                    break;
                }
                else if (buffer[3] == 'E' && buffer[4] == 'X' && buffer[5] == 'I' && buffer[6] == 'T')  //OK EXIT
                {
                    fprintf(stdout, "%s", removeProtocolText(buffer));
                    close(currentSocket);
                    break;
                }
                else if (buffer[3] == 'H' && buffer[4] == 'I' && buffer[5] == 'S' && buffer[6] == 'T')  //OK HIST
                {
                    if (buffer[8] == 'E' && buffer[9] == 'N' && buffer[10] == 'D')
                    {
                        fprintf(stderr, "Server has analyzed and sent the result correctly.\n\n");          
                        showSelection();
                    }
                    else fprintf(stdout, "%s", removeProtocolText(buffer));   
                }
            }
            else if (buffer[0] == 'E' && buffer[1] == 'R' && buffer[2] == 'R')  //ERROR
            {
                //HIST ERROR
                if (buffer[4] == 'H' && buffer[5] == 'I' && buffer[6] == 'S' && buffer[7] == 'T')
                {
                    fprintf(stderr, "Server reported an issue with the HIST request. Server message: %s\nError %d\n", removeProtocolText(buffer), serverResponse);          
                }
                else if (buffer[4] == 'T' && buffer[5] == 'E' && buffer[6] == 'X' && buffer[7] == 'T')  //ERR TEXT
                {
                    fprintf(stderr, "%s", removeProtocolText(buffer));
                }
                else
                {
                    fprintf(stderr, "%s", removeProtocolText(buffer));
                }

                close(currentSocket);
                exit(1);
            }      
        }

        //Clears the buffer
        clearBuffer();
        flushAll();
    }
}

//Counts the alfanumeric char contained in the given string
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

//Clears both streams (in/out)c
void flushAll()
{
    fflush(stdin);
    fflush(stdout);
}

//Calculates the length of the composed string
int getMessageLength(char result[], char type[], char content[])
{
	char toreturn[MAX_LENGTH];
    sprintf(toreturn, PROTOCOLFORMAT, result, type, content);
	return strlen(toreturn);
}

//Shows the selection to the user and sends the message
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

        //If the text is too long, splits it in 2 messages
        if (strlen(str) > 502)
        {
            int len = strlen(str);
            int len1 = len/2;
            int len2 = len - len1; // Compensate for possible odd length
            char *s1 = malloc(len1+1); // one for the null terminator
            memcpy(s1, str, len1);
            s1[len1] = '\0';
            char *s2 = malloc(len2+1); // one for the null terminator
            memcpy(s2, str+len1, len2);
            s2[len2] = '\0';
            write(currentSocket, textMessageBuilder("TEXT", s1, countAlnum(s1)), strlen(textMessageBuilder("TEXT", s1, countAlnum(s1))));
            free(s1);
            write(currentSocket, textMessageBuilder("TEXT", s2, countAlnum(s2)), strlen(textMessageBuilder("TEXT", s2, countAlnum(s2))));
            free(s2);
        }
        else
            write(currentSocket, textMessageBuilder("TEXT", str, countAlnum(str)), strlen(textMessageBuilder("TEXT", str, countAlnum(str))));
        break;
        case 'b': case 'B':
            write(currentSocket, "HIST\n", 5);
        break;
        case 'c': case 'C':
            exit_req = 1;
            write(currentSocket, "EXIT\n", 5);
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




