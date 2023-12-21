#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define RRQ 1
#define WRQ 2
#define DAT 3
#define ACK 4
#define ERR 5

/**
 * @brief Creates a RRQ request or a WRD request.
 *
 * @param opcode the operation code (RRQ or WRQ)
 * @param file_name the name of the file to handle
 * @param mode the transfer mode ('octet' for binary transfer)
 * @param request the character array in which the request is stored
 */
void create_request(int opcode, char *file_name, char *mode, char *request) {
    int i = 0;
    request[i++] = 0;
    request[i++] = opcode;
    strcpy(request+i, file_name);
    i = i+strlen(file_name)+1;
    strcpy(request+i, mode);
}

/**
 * @brief Implements the 'gettftp' feature.
 *
 * @param file_name the name of the file to read
 * @param the address of the TFTP server
 */
void tftp_get(char *file_name, char *server) {
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error during socket creation");
        exit(EXIT_FAILURE);
    }

    // Build the server address
    struct sockaddr_in servaddr;
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(server, "tftp", &hints, &res) != 0) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    // Build and send the RRQ request
    char request[512];
    create_request(RRQ, file_name, "octet", request);
    if (sendto(sockfd, request, strlen(request)+1, 0, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Error during the sending of the RRQ request");
        exit(EXIT_FAILURE);
    }

    // Receive and handle DAT and ACK packets
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    char buffer[516];
    int n;

    while (1) {
        // Receive a packet
        n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len);
        if (n < 0) {
            perror("Error during reception of the DAT packet");
            exit(EXIT_FAILURE);
        }

        // Extract the opcode
        int opcode = (buffer[0] << 8) | buffer[1];

        if (opcode == DAT) {
            // Handle the data packet
            // TODO: Write the data to a file or process it as needed

            // Send an ACK packet
            char ack[4] = {0, ACK, buffer[2], buffer[3]};
            if (sendto(sockfd, ack, sizeof(ack), 0, (struct sockaddr *)&cliaddr, len) < 0) {
                perror("Error during the sending of the ACK packet");
                exit(EXIT_FAILURE);
            }

            // If the data packet is less than 512 bytes (plus 4 bytes for the opcode and block number),
            // then it is the last packet and we break the loop
            if (n < 516) {
                break;
            }
        } else if (opcode == ERR) {
            // Handle the error packet
            // TODO: Print the error message or handle the error as needed
            break;
        }
    }

    // Close the socket
    close(sockfd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s server file_name\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *server = argv[1];
    char *file_name = argv[2];

    tftp_get(file_name, server);

    return EXIT_SUCCESS;
}
