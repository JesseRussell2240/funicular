/*
 * udpserver.c
 *
 * This example shows how to code up a UDP (connectionless)
 * server. Note that we no longer have a listening queue or
 * perform an accept. This is because UDP does not support 
 * the concept of reliable connections - UDP simply provides a
 * lightweight means of sending messages from one system to 
 * another. You, as a programmer, must figure out what to do
 * in case a message is dropped, lost, or delayed!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAXINPUTLEN		 4096
char input[MAXINPUTLEN] = "";

int main(int argc, char *argv[])
{
	int udpSocket = 0;
	int udpPort = 0;
	int status = 0;
	int size = 0;
	int clientLength = 0;
	struct sockaddr_in serverMachine = { 0 };
	struct sockaddr_in clientMachine = { 0 };

	// ensure we have correct arguments

	if (argc < 2) {
		printf ("Usage: udpserver <port>\n");
		return 1;
	}	/* endif */

	udpPort = atoi(argv[1]);

	// allocate a socket

	udpSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udpSocket < 0) {
		printf ("Error: can't get socket!\n");
		return 2;
	}	/* endif */

	// zero out socket structures and fill
	// in with details for our UDP server

	memset (&serverMachine, 0, sizeof (serverMachine));
	memset (&clientMachine, 0, sizeof (clientMachine));

	serverMachine.sin_family = AF_INET;
	serverMachine.sin_addr.s_addr = htonl (INADDR_ANY);
	serverMachine.sin_port = htons (udpPort);

	// bind the socket structure to the socket
	// to establish our server

	status = bind (udpSocket, (struct sockaddr *)&serverMachine, 
			sizeof (serverMachine));
	if (status < 0) {
		printf ("Error: can't bind\n");
		return 3;
	}	/* endif */

	while (1) {
		// get an incoming message. Note the use of
		// recvfrom() which will allow us to obtain the
		// socket information for the client application
		// currently talking to us. Consider having 5
		// clients ... each client could send at any time,
		// thus, knowing which one sent the message becomes
		// important in the UDP environment!

		size = recvfrom (udpSocket, input, MAXINPUTLEN, 0, 
			(struct sockaddr *) &clientMachine, &clientLength);
		if (size < 0) {
			printf ("Error: recvfrom failure\n");
			return 4;
		}	/* endif */

		// for diagnostics, output visually the message the server got,
		// and for grins, uppercase the string on the return path

		printf ("ip: %lX\n", clientMachine.sin_addr.s_addr);
		printf ("ClientLength: %d size: %d Message received: %s\n", clientLength, size, input);
		strupr (input);

		// just echo the message back to client machine for this example

		clientLength = sizeof (clientMachine);
		status = sendto (udpSocket, input, size, 0, 
			(struct sockaddr *) &clientMachine, clientLength);
		if (status < 0) {
			printf ("Error: sendto error\n");
			return 5;
		}	/* endif */
	}	/* end while loop */

	/* as a server, this should never be reached */

	return 0;
}

