# include <sys/types.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <signal.h>
# include <sys/wait.h>

# define PORT 5000
# define BUFFER_SIZE 1024

void
handle_client(int
client_socket) {
    char
buffer[BUFFER_SIZE];
int
len;

// Read
command
from client
    len = read(client_socket, buffer, sizeof(buffer) - 1);
if (len > 0)
{
    buffer[len] = '\0'; // Null - terminate
the
string
printf("Received command: %s\n", buffer);

// Execute
the
command and send
the
output
back
FILE * fp = popen(buffer, "r");
if (fp == NULL)
{
    perror("popen");
return;
}

while (fgets(buffer, sizeof(buffer), fp) != NULL) {
write(client_socket, buffer, strlen(buffer));
}

pclose(fp);
}

close(client_socket);
}

int
main()
{
    int
server_socket, client_socket;
struct
sockaddr_in
server_addr, client_addr;
socklen_t
client_len = sizeof(client_addr);

// Create
a
socket
server_socket = socket(AF_INET, SOCK_STREAM, 0);
if (server_socket < 0)
{
perror("socket");
exit(1);
}

// Prepare
the
sockaddr_in
structure
memset( & server_addr, 0, sizeof(server_addr));
server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = INADDR_ANY;
server_addr.sin_port = htons(PORT);

// Bind
the
socket
if (bind(server_socket, (struct sockaddr *) & server_addr, sizeof(server_addr)) < 0)
{
perror("bind");
close(server_socket);
exit(1);
}

// Listen
for incoming connections
listen(server_socket, 5);
printf("Server listening on port %d\n", PORT);

while (1) {
// Accept a new connection
client_socket = accept(server_socket, (struct sockaddr * ) & client_addr, & client_len);
if (client_socket < 0) {
perror("accept");
continue;
}
handle_client(client_socket);
}

close(server_socket);
return 0;
}
