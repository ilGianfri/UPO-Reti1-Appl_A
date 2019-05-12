// LabReti1_1819_Appl_A.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>

const char HELP[] = "Missing parameter\n\nSYNTAX: $server <port number>\n";
const int MAX_LENGTH = 512;

int currentSocket = 0;
int port = 0;
struct sockaddr_in server;

void main(int argc, char argv[])
{
	//Checks that the parameters are correct, otherwise prints the help and exits
	if (argc <= 1)
	{
		fprintf(stderr, HELP);
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
		printf("Socket created successfully.\n")
	}
	
	//Initializes the socket with protocol, port, INADDR_ANY to use all local addresses
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	int bindStatus = bind(socket, (struct sockaddr *)&server,sizeof(server))
	if (bindStatus == 0)
	{
		printf("Socket ready.\n");
	}
	else
	{
		fprintf(stderr, "Socket not ready. Cannot bind to address.\n");
		return;
	}
}

/*

*/
//char** responseBuilder(char result[], char type[], char content[])
//{
//	return result + " " +
//}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
