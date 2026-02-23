/*
 * This program creates a STREAM socket client (TCP).
 * Written on 02/09/2026 by Kuete Mouafo Yannick
 *
 * Usage: ./client <ipaddr> <portnumber>
 *
 * The client:
 *   1. Connects to the server at the given IP and port
 *   2. Sends a user-entered string to the server
 *   3. Waits for and prints the server's response
 *   4. Cleans up and exits
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 100

/* ----------------------------------------------------------------
 * parse_arguments
 * ----------------------------------------------------------------
 * Validates command-line arguments and extracts IP and port.
 * Exits with a usage message if arguments are missing.
 */
void parse_arguments(int argc, char *argv[], char *serverIP, int *port)
{
    if (argc < 3) {
        fprintf(stderr, "usage is: client <ipaddr> <portnumber>\n");
        exit(1);
    }

    strcpy(serverIP, argv[1]);
    *port = strtol(argv[2], NULL, 10);

    if (*port <= 0 || *port > 65535) {
        fprintf(stderr, "Error: Invalid port number '%s'. Must be between 1 and 65535.\n", argv[2]);
        exit(1);
    }
}

/* ----------------------------------------------------------------
 * create_client_socket
 * ----------------------------------------------------------------
 * Creates a TCP socket and connects to the server.
 * Returns the socket descriptor.
 */
int create_client_socket(const char *serverIP, int port)
{
    int sd;
    int rc;
    struct sockaddr_in server_address;

    /* Step 1: Create the socket */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("Error: socket() failed");
        exit(1);
    }

    /* Step 2: Fill in the server address data structure */
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(serverIP);

    /* Step 3: Connect to the server */
    rc = connect(sd, (struct sockaddr *)&server_address, sizeof(server_address));
    if (rc < 0) {
        perror("Error: connect() failed");
        close(sd);
        exit(1);
    }

    printf("Connected to server at %s:%d\n", serverIP, port);

    return sd;
}

/* ----------------------------------------------------------------
 * send_message
 * ----------------------------------------------------------------
 * Prompts the user for a string and sends it to the server.
 * Returns 0 on success, -1 on failure.
 */
int send_message(int sd)
{
    char buffer[BUFFER_SIZE];
    int rc;

    /* Prompt the user for a message */
    printf("Enter a message to send: ");
    fgets(buffer, BUFFER_SIZE, stdin);

    /* Remove the trailing newline from fgets */
    buffer[strcspn(buffer, "\n")] = '\0';

    printf("You are sending '%s'\n", buffer);
    printf("The length of the string is %lu bytes\n", strlen(buffer));

    /* Send the string (include the null terminator) */
    rc = send(sd, buffer, strlen(buffer) + 1, 0);
    if (rc < 0) {
        perror("Error: send() failed");
        return -1;
    }

    printf("Sent %d bytes to the server\n", rc);

    return 0;
}

/* ----------------------------------------------------------------
 * receive_response
 * ----------------------------------------------------------------
 * Waits for and prints the server's response.
 * Returns 0 on success, -1 on failure.
 */
int receive_response(int sd)
{
    char buffer[BUFFER_SIZE];
    int rc;

    memset(buffer, 0, BUFFER_SIZE);

    rc = recv(sd, buffer, BUFFER_SIZE - 1, 0);
    if (rc < 0) {
        perror("Error: recv() failed");
        return -1;
    }

    if (rc == 0) {
        printf("Server closed the connection without responding.\n");
        return -1;
    }

    buffer[rc] = '\0';
    printf("Server response: %s\n", buffer);

    return 0;
}

/* ----------------------------------------------------------------
 * cleanup
 * ----------------------------------------------------------------
 * Closes the socket to free resources.
 */
void cleanup(int sd)
{
    if (sd >= 0) {
        close(sd);
    }
    printf("Connection closed.\n");
}

/* ----------------------------------------------------------------
 * main
 * ----------------------------------------------------------------
 * Orchestrates the client lifecycle:
 *   parse args -> connect -> send -> receive response -> cleanup
 */
int main(int argc, char *argv[])
{
    char serverIP[29];
    int port;

    /* Parse and validate command-line arguments */
    parse_arguments(argc, argv, serverIP, &port);

    /* Create socket and connect to server */
    int sd = create_client_socket(serverIP, port);

    /* Send a message to the server */
    if (send_message(sd) == 0) {
        /* Wait for the server's response */
        receive_response(sd);
    }

    /* Clean up */
    cleanup(sd);

    return 0;
}
