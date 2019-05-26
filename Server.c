/*
Buffer si deve ridimensionare?
*/

#include <stdio.h>      
#include <sys/types.h>
#include <sys/socket.h>   
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <ctype.h>
#include <math.h>

#define true 1
#define false 0

const char WRONGARGS[] = "Missing parameter\n\nSYNTAX: $server <port number>\n";
const int MAX_LENGTH = 512;
const char ALLOWEDCMDS[4][4] = { "TEXT", "HIST", "EXIT", "QUIT"};

char WELCOMEMSG[] = "Welcome, connection enstablished successfully.";
char GOODBYEMSG[] = "It was fun while it lasted, goodbye!";
char PROTOCOLFORMAT[] = "%s %s %s\n";
char OKRES[] = "OK";
char ERRORMSG[] = "ERR";

int currentSocket = 0, port = 0, incomingSocket;
struct sockaddr_in server;
char buffer[512] = "";

char* responseBuilder(char result[], char type[], char content[]);
char* getCommand(char fulls[]);
int isAllowedCommand(char cmd[]);
int responseLength(char result[], char type[], char content[]);
void flushAll();
int getCounter(char buff[]);
int getCountLength(char buff[]);
int countAlnum(char txt[]);
int isMessageCorrect(char buff[]);
char* removeProtocolText(char b[]);
void sendHistText();
int cmpfunc(const void *a, const void *b);

//Contains all the text received from the client via TEXT cmd
char *rcvdstr = NULL;
 
int main(int argc, char *argv[])
{
	//Checks that the parameters are correct, otherwise prints the help and exits
	if (argc <= 1)
	{
		fprintf(stderr, WRONGARGS);
		return 1;
	}

	//Saves the specified port
	port = atoi(argv[1]);

	//Creates the socket
	currentSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//Throws an error is the socket could not be created for any reson
	if (currentSocket == -1)
	{
		fprintf(stderr, "Could not create a socket. Please verify your network settings.\n");
		return 1;
	}
	else
	{
		//Socket created successfully, prints diagnostic message
		fprintf(stdout, "Socket created successfully.\n");
	}
	
	//Initializes the socket with protocol, port, INADDR_ANY to use all local addresses
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	int scktstatus = bind(currentSocket, (struct sockaddr *)&server, sizeof(server));
	if (scktstatus == 0)
		fprintf(stdout, "Socket ready. Waiting for connection...\n");
	else
	{
		fprintf(stderr, "Socket not ready. Cannot bind to address.\n");
		return 1;
	}
	
	//Listens for incoming connections, maximum 5 queued
	scktstatus = listen(currentSocket, 5);

	int incomingSocket = 0;
	//Now that the socket is ready, keeps waiting for an incoming connection
	while(1)
	{
		free(rcvdstr);
		scktstatus = 0;
		struct sockaddr_in client = { 0 };
		socklen_t sizeClient = (socklen_t) sizeof(client);
        incomingSocket = accept(currentSocket,(struct sockaddr *)&client, &sizeClient);
		if (incomingSocket == -1)
		{
			fprintf(stderr, "Cannot accept incoming connection(s).\n");
			close(currentSocket);
			return 1;
		}
		

		fprintf(stdout, "Sending START message to the client.\n");
		write(incomingSocket, responseBuilder(OKRES, "START", WELCOMEMSG), MAX_LENGTH);

		//Keeps listening on the current socket
		while(scktstatus != -1)
		{
        	bzero(buffer, sizeof(buffer));
			//Waits for a message from the client
			scktstatus = read(incomingSocket, buffer, sizeof(buffer));
			if (scktstatus > 0)
			{
				char *cmd = getCommand(buffer);
				if (isAllowedCommand(cmd) == 1)
				{
					switch(cmd[0])
					{
						case 'T':	//TEXT
						fprintf(stdout, "Received TEXT from the client. \n");
						int count = getCounter(buffer);
						int calculatedcount = countAlnum(buffer) - 4 - getCountLength(buffer);
						if (isMessageCorrect(buffer) && calculatedcount == count)
						{
							fprintf(stdout, "Received message is semantically correct. Replying to the client.\n");
							char num[getCountLength(buffer)];
							sprintf(num, "%d", calculatedcount);

							// First message
							if (rcvdstr == NULL)
							{
								rcvdstr = (char *)malloc(strlen(removeProtocolText(buffer)));
								strcpy(rcvdstr, removeProtocolText(buffer));
							}
							else
							{
								rcvdstr = (char *)realloc(rcvdstr, sizeof(rcvdstr) + strlen(removeProtocolText(buffer)));
								strcat(rcvdstr, removeProtocolText(buffer));
							}
							
							write(incomingSocket, responseBuilder(OKRES, "TEXT", num), responseLength(OKRES, "TEXT", num));
						}
						else	//ERROR
						{
							fprintf(stdout, "Received message is not semantically correct. Replying to the client & closing connection.\n");
							if (calculatedcount != count)
								write(incomingSocket, responseBuilder(ERRORMSG, "TEXT", "Text counter mismatch."), responseLength(ERRORMSG, "TEXT", "Text counter mismatch."));
							else
								write(incomingSocket, responseBuilder(ERRORMSG, "TEXT", "Received message is not semantically correct."), responseLength(ERRORMSG, "TEXT", "Received message is not semantically correct."));
							
							close(incomingSocket);
							scktstatus = -1;
						}						
						break;
						case 'H':	//HIST
						if (rcvdstr != NULL)
							sendHistText(incomingSocket);
						else
						{
							write(incomingSocket, responseBuilder(ERRORMSG, "HIST", "No text has been given. Connection will be closed."), responseLength(OKRES, "HIST", "No text has been given. Connection will be closed."));
							close(incomingSocket);
							free(rcvdstr);
							scktstatus = -1;
						}
						break;
						case 'E':	//EXIT

						if (rcvdstr != NULL)
						{
							sendHistText(incomingSocket);
							write(incomingSocket, responseBuilder(OKRES, "EXIT", GOODBYEMSG), responseLength(OKRES, "EXIT", GOODBYEMSG));
						}
						else
						{
							write(incomingSocket, responseBuilder(ERRORMSG, "HIST", "No text has been given. Connection will be closed."), responseLength(OKRES, "HIST", "No text has been given. Connection will be closed."));
						}

						close(incomingSocket);
						free(rcvdstr);
						scktstatus = -1;	
						break;
						case 'Q':	//QUIT
						fprintf(stdout, "Sending closing message to client.\n");
						write(incomingSocket, responseBuilder(OKRES, "QUIT", GOODBYEMSG), responseLength(OKRES, "QUIT", GOODBYEMSG));
						close(incomingSocket);
						free(rcvdstr);
						scktstatus = -1;
						break;
					}
				}
				else 
				{
					fprintf(stdout, "Received an invalid command. Closing socket");
					write(incomingSocket, responseBuilder(ERRORMSG, "SYNTAX", "Command is not valid. Connection closed."), responseLength(ERRORMSG, "SYNTAX", "Command is not valid. Connection closed."));
					close(incomingSocket);
				}
			}
			else
			{
				fprintf(stderr, "There was an error getting the message from the client. Connection has been terminated.\n");
				close(incomingSocket);
				scktstatus = -1;
			}
		}	
	}

	return 0;
}

// Computes and replies with the hist
void sendHistText(int socket)
{
	char *res = (char*)malloc(sizeof(MAX_LENGTH));
	memset(res, '\0', strlen(res));
	strcpy(res, "OK HIST ");

	qsort(rcvdstr, (size_t) strlen(rcvdstr), (size_t) sizeof(char), cmpfunc);

	//removes all the white spaces
	int i = 0;
	while (rcvdstr[i] == ' ')
	    memmove(rcvdstr, rcvdstr+1, strlen(rcvdstr));

	char prev = 0;
	int count = 0;
	for (int i = 0; i < strlen(rcvdstr); i++)
	{
		if (isalnum(rcvdstr[i]))
		{
			if (prev == 0)
				prev = rcvdstr[i];
			else
			{
				if (rcvdstr[i] == prev)
					count++;
				else
				{
					count++;
					char t[6];
					sprintf(t, "%c:%d ", prev, count);

					if (strlen(res) <= 504)
						strcat(res, t);
					else
					{
						res[strlen(res)] = '\n';

						write(socket, res, strlen(res));
						
						memset(res, '\0', strlen(res));
						strcpy(res, "OK HIST ");
						strcat(res, t);
					}
					

					count = 0;
					prev = rcvdstr[i];
				}				
			}		
		}
	}

	write(socket, "OK HIST END\n", 12);
}


int cmpfunc(const void *a, const void *b) 
{
	return *(char*)a - *(char*)b;
}


// Clears the streams
void flushAll()
{
    fflush(stdin);
    fflush(stdout);
}

// Removes all the protocol text leaving only the text
char* removeProtocolText(char b[])
{
    char toreturn[504];
	bzero(toreturn, sizeof(toreturn));
    char *ptr = toreturn;
    memcpy(toreturn, &b[5], strlen(b) - 7 - getCountLength(b));
    return ptr;
}

int isMessageCorrect(char buff[])
{
	int correct = 0;
	int totmsglength = ((int)strlen(buff)) - 1;
	//Message is correctly formated
	if (buff[totmsglength] == '\n')
		correct = true;

	//Checks that there's a space after the command, all commands have a length of 4
	if (buff[4] == ' ')
		correct = true;
	else correct = false;

	return correct;
}

//Counts the alphanumeric characters in the text
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

// Gets the length of the number at the end of the TEXT message (e.g 100 = 3, 20 = 2, 5 = 1)
int getCountLength(char buff[])
{
	int length = 0;
	//Gets the message length
	int totmsglength = ((int)strlen(buff)) - 1;
	//Message is correctly formated, gets the text length
	if (buff[totmsglength] == '\n')
	{
		for (int i = totmsglength - 1; i > totmsglength - 3; i--)
		{
			if (isdigit(buff[i]))
				length++;
		}
	}
	return length;
}

// Gets the number at the end of the TEXT message
int getCounter(char buff[])
{
	//Gets the message length
	int totmsglength = ((int)strlen(buff)) - 1;
	//Message is correctly formated, gets the text length
	if (buff[totmsglength] == '\n')
	{
		int length = getCountLength(buff);
							
		char num[length];
		memcpy(num, &buff[totmsglength - length], length);
		//fprintf(stdout, "%d\n", atoi(num));
		return atoi(num);
	}

	return 0;
}

// Gets the command from the received text
char* getCommand(char fulls[])
{
	char cmd[4];
	memcpy(cmd, &fulls[0], 4);
	char *ptr = cmd;

	return ptr;
}

int responseLength(char result[], char type[], char content[])
{
	char toreturn[MAX_LENGTH];
    sprintf(toreturn, PROTOCOLFORMAT, result, type, content);
	return strlen(toreturn);
}

// This method builds the response messages to ensure they respect the protocol
char* responseBuilder(char result[], char type[], char content[])
{
    char toreturn[MAX_LENGTH];
    sprintf(toreturn, PROTOCOLFORMAT, result, type, content);
    char *ptr = toreturn;

    return ptr;
}

// Checks if the given text is one of the allowed commands
int isAllowedCommand(char cmd[])
{
	for (int i = 0; i < 4; i++)
	{
		if (memcmp(cmd, ALLOWEDCMDS[i], 4) == 0)
			return true;
	}
	return false;
}