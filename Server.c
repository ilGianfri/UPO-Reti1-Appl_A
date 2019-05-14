// LabReti1_1819_Appl_A.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>      
#include <sys/types.h>
#include <sys/socket.h>   
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#define true 1
#define false 0

const char WRONGARGS[] = "Missing parameter\n\nSYNTAX: $server <port number>\n";
const int MAX_LENGTH = 512;
const char ALLOWEDCMDS[4][4] = { "TEXT", "HIST", "EXIT", "QUIT"};

char WELCOMEMSG[] = "Welcome, connection enstablished successfully.";
char OKRES[] = "OK";
char ERRORMSG[] = "ERR";

int currentSocket = 0;
int port = 0;
struct sockaddr_in server;
char buffer[512] = "";

char* responseBuilder(char result[], char type[], char content[]);
char* getCommand(char fulls[]);
int isAllowedCommand(char cmd[]);

void main(int argc, char *argv[])
{
	//Checks that the parameters are correct, otherwise prints the help and exits
	if (argc <= 1)
	{
		fprintf(stderr, WRONGARGS);
		return;
	}

	//Saves the specified port
	port = atoi(argv[1]);

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
	
	//Initializes the socket with protocol, port, INADDR_ANY to use all local addresses
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	int status = bind(currentSocket, (struct sockaddr *)&server, sizeof(server));
	if (status == 0)
	{
		printf("Socket ready. Waiting for connection...\n");
	}
	else
	{
		fprintf(stderr, "Socket not ready. Cannot bind to address.\n");
		return;
	}

	//Listens for incoming connections, maximum 5 queued
	status = listen(currentSocket, 5);

	//Now that the socket is ready, keeps waiting for an incoming connection
	while(1)
	{
		struct sockaddr_in client = { 0 };
		int sizeClient = sizeof(client);
        int incomingSocket = accept(currentSocket,(struct sockaddr *)&client, &sizeClient);
		if (incomingSocket == -1)
		{
			fprintf(stderr, "Cannot accept incoming connection(s).\n");
			close(currentSocket);
			return;
		}

		printf("Sending message to client.\n");
		write(incomingSocket, responseBuilder(OKRES, "START", WELCOMEMSG), MAX_LENGTH);

		//Waits for a message from the client
		status = read(incomingSocket, buffer, sizeof(buffer));
		if (status > 0)
		{
			char *cmd = getCommand(buffer);
			if (isAllowedCommand(cmd))
			{
				switch(cmd[0])
				{
					case 'T':

					break;
				}
			}
			else write(incomingSocket, responseBuilder(ERRORMSG, "SYNTAX", "Command is not valid."), MAX_LENGTH);
		}
		else
		{
			fprintf(stderr, "There was an error getting the message from the client. Connection will be terminated.\n");
			close(currentSocket);
			return;
		}
		
	}
	
}

// Spero copy le prime 4 lettere in cmd
char* getCommand(char fulls[])
{
	char cmd[4];
	memcpy(cmd, &fulls[0], 4);
	char *ptr = cmd;

	return ptr;
}

// This method builds the response messages to ensure they respect the protocol
char* responseBuilder(char result[], char type[], char content[])
{
    char toreturn[MAX_LENGTH];
    sprintf(toreturn, "%s %s %s\n", result, type, content);
    char *ptr = toreturn;

    return ptr;
}

// Checks if the given text is one of the allowed commands
int isAllowedCommand(char cmd[])
{
	for (int i = 0; i < 4; i++)
	{
		if (strcmp(cmd, ALLOWEDCMDS[i]) == 0)
			return true;
	}
	return false;
}
