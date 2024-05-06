
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/sctp.h> 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#define BUFFER_SIZE 1024


void get_message(char *buffer, const char *string, size_t size) {
    memcpy(buffer, string, size);
}



int main(int argc, char const** argv) {

    // set default address and port
    const char *address = "127.0.0.1";
    if (argc >= 3) { address = argv[2]; }
    int port = 9999;
    if (argc >= 4) { port = atoi(argv[3]); }
    char *message = "hello";
    if (argc >= 2) { message = (char*)argv[1]; }

    int status;
    int valread; 
    int client_fd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = { 0 };

    if ((client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP)) < 0) {
        printf("ERROR: socket failed \n");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, address, &serv_addr.sin_addr) <= 0) {
        printf("ERROR: Invalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }
 
    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        printf("ERROR: connection failed \n");
        exit(EXIT_FAILURE);
    }

    
    message = NULL;
    size_t size; 
    while (getline(&message, &size, stdin) != -1) {
        // make message
        get_message(buffer, message, strlen(message));
        
        // send hello
        sctp_sendmsg(client_fd, buffer, strlen(message), NULL, 0, 0, 0, 0, 0, 0);
        printf("[+] message sent\n");

        // clear buffer
        memset(buffer, 0, sizeof(buffer));
        
        // subtract 1 for the null terminator at the end
        valread = sctp_recvmsg(client_fd, buffer, BUFFER_SIZE - 1,
                               (struct sockaddr *) NULL, 0, NULL, NULL);
        printf("%s\n", buffer);

    }
 
    // closing the connected socket
    close(client_fd);
    return 0;

}

