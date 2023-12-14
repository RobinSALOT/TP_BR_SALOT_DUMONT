#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define RRQ 1
#define WRQ 2
#define DAT 3
#define ACK 4
#define ERR 5

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
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Build the server address
    struct sockaddr_in servaddr;
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(w, "tftp", &hints, &res) != 0) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    // Build and send the RRQ request
    char request[512];
    create_request(RRQ, file_name, "octet", request);
    if (sendto(sockfd, request, strlen(request)+1, 0, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Erreur lors de l'envoi de la requête RRQ");
        exit(EXIT_FAILURE);
    }

    // TODO: Complete this function to receive and handle DAT and ACK packets.
}

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
