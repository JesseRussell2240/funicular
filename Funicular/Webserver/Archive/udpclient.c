/*
 * udpclient.c
 *
 * This example shows how to code up a UDP (connectionless)
 * client. Note that we no longer need to connect to a server,
 * instead, we indicate what IP/port we wish to communicate with,
 * and we also need to allocate and bind a socket that we'll use
 * for initiating the communications. For clients, it really doesn't
 * matter what the port is, hence, we can specify a value of zero for
 * the port number when binding our client side socket structure.
 *
 * Remember, with UDP, there's no guarantee that the communications
 * even works out, hence, once the message is sent, there may not be
 * a response if the server never got the message!
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
	char ip[100];
	int udpSocket = 0;
	int udpPort = 0;
	int status = 0;
	int size = 0;
	int length;
	int serverLength = 0;
	struct sockaddr_in serverMachine = { 0 };
	struct hostent *host;

	// ask the end user what ip and port number the
	// server uses, and ask for a message string. Make
	// sure the IP maps to a valid internet IP before
	// proceeding.

	printf ("Enter server IP address: ");
	gets (ip);
	if ((host = gethostbyname (ip)) == NULL) {
		printf ("Error: can't get host information!\n");
		return 1;
	}	/* endif */

	printf ("Enter server port number: ");
	gets (input);
	udpPort = atoi (input);

	printf ("Enter a string to send to server: ");
	gets (input);
	
	// allocate a socket

	udpSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udpSocket < 0) {
		printf ("Error: can't get socket!\n");
		return 2;
	}	/* endif */

	// zero out socket structures and fill
	// in with details for our UDP server

	memset (&serverMachine, 0, sizeof (serverMachine));

	serverMachine.sin_family = AF_INET;
	memcpy (&serverMachine.sin_addr, host->h_addr, host->h_length);
	serverMachine.sin_port = htons (udpPort);

	// send a message and get the response. Note the use of
	// sendto() where we pass our arbitrary message data (in this
	// case, just a simple C string), indicating how many bytes we
	// have in the message (length of the string plus 1 for the
	// string's null terminator).
	//
	// Then, notice how we use
	// recvfrom() which will allow us to obtain the
	// results from the server.

	length = strlen (input) + 1;
	size = sizeof (serverMachine);
	status = sendto (udpSocket, input, length, 0, 
		(struct sockaddr *) &serverMachine, size);
printf ("status: %d size: %d\n", status, size);
	if (status < 0) {
		printf ("Error: sendto error (short write)\n");
		return 5;
	}	/* endif */

	// retrieve results from server (the original string is converted
	// to all capital letters! (zero out the input buffer just to
	// prove we actually get something back!)

	memset (input, 0, MAXINPUTLEN);
	size = recvfrom (udpSocket, input, MAXINPUTLEN, 0, 
		(struct sockaddr *) &serverMachine, &serverLength);
	if (size < 0) {
		printf ("Error: recvfrom failure\n");
		return 4;
	}	/* endif */

	// for diagnostics, output visually the message the server sent

	printf ("Message: %s\n", input);

	// and that's all!

	return 0;
}

