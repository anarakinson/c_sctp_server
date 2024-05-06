
#include <arpa/inet.h>
#define _GNU_SOURCE
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define BUFFER_SIZE 1024


void get_message(char *buffer, const char *string, size_t size) {
    memcpy(buffer, string, size);
}


int main(int argc, char** argv) {

    if (argc <= 1) {
        printf("USAGE: server <port> <optional:num_clients>\n");
        exit(EXIT_FAILURE);
    }
    int port = atoi(argv[1]);
    int num_clients = 3;
    if (argc >= 3) { num_clients = atoi(argv[2]); }

    // -----------------------------------------------------
    int server_fd;
    int new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int flags;
    struct sctp_sndrcvinfo sndrcvinfo;

    char buffer[BUFFER_SIZE] = {0};

    // create a socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP)) == 0) {
        perror("ERROR: socket failed!");
        exit(EXIT_FAILURE);
    }

    // set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("ERROR: setsockopt failed!");
        exit(EXIT_FAILURE);
    }

    // setup the server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("ERROR: bind fails!");
        exit(EXIT_FAILURE);
    }

    // start listening for incoming connections
    if (listen(server_fd, num_clients) < 0) {
        perror("ERROR: listening failed!");
        exit(EXIT_FAILURE);
    }

    // print a message showing that the server is listening on a port
    printf("[+] Server listening on port %d\n", port);

    // accept incoming connections 
    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) >= 0) {

        // accepting the connection
        printf("[+] Connection accepted\n");

        // read data from the client and print it
        ssize_t valread;
        while ((valread = sctp_recvmsg(new_socket, buffer, BUFFER_SIZE,
               (struct sockaddr *) NULL, 0, &sndrcvinfo, NULL)) > 0) {
            // Print the received data
            printf("Received data:\n%s\n", buffer);
            
            // send response
            get_message(buffer, "hello from server", strlen("hello from server"));
            sctp_sendmsg(new_socket, buffer, BUFFER_SIZE - 1, NULL, 0, 0, 0, 0, 0, 0);
            // Reset buffer for the next read
            memset(buffer, 0, sizeof(buffer));

        }

    }

    // if accept failed exit
    perror("ERROR: accept failed");
    exit(EXIT_FAILURE);

    // close the socket
    close(server_fd);
    return EXIT_SUCCESS;

}
