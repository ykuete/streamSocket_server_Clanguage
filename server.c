/*
 * This program creates a STREAM socket server (TCP).
 * Written on 02/09/2026 by Kuete Mouafo Yannick
 *
 * Usage: ./server <portnumber>
 *
 * The server:
 *   1. Binds to the given port on all network interfaces
 *   2. Listens and accepts one client connection
 *   3. Receives a string from the client
 *   4. Sends a response back to the client
 *   5. Cleans up and exits
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 100
#define BACKLOG 5
#define RESPONSE "Server acknowledged your message!"

/* ----------------------------------------------------------------
 * parse_arguments
 * ----------------------------------------------------------------
 * Validates command-line arguments and returns the port number.
 * Exits with a usage message if arguments are missing or invalid.
 */
int parse_arguments(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "usage is: server <portnumber>\n");
        exit(1);
    }

    int port = atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Error: Invalid port number '%s'. Must be between 1 and 65535.\n", argv[1]);
        exit(1);
    }

    return port;
}

/* ----------------------------------------------------------------
 * create_server_socket
 * ----------------------------------------------------------------
 * Creates a TCP socket, binds it to the given port on all
 * interfaces (INADDR_ANY), and starts listening.
 * Returns the server socket descriptor.
 */
int create_server_socket(int port)
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

    /* Allow port reuse to avoid "Address already in use" errors */
    int opt = 1;
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Error: setsockopt() failed");
        close(sd);
        exit(1);
    }

    /* Step 2: Fill in the address data structure */
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    /* Step 3: Bind the socket to the address and port */
    rc = bind(sd, (struct sockaddr *)&server_address, sizeof(server_address));
    if (rc < 0) {
        perror("Error: bind() failed");
        close(sd);
        exit(1);
    }

    /* Step 4: Listen for incoming connections */
    rc = listen(sd, BACKLOG);
    if (rc < 0) {
        perror("Error: listen() failed");
        close(sd);
        exit(1);
    }

    printf("Server is listening on port %d...\n", port);

    return sd;
}

/* ----------------------------------------------------------------
 * accept_client
 * ----------------------------------------------------------------
 * Waits for and accepts one incoming client connection.
 * Prints client connection info.
 * Returns the new socket descriptor for the client.
 */
int accept_client(int server_sd)
{
    struct sockaddr_in from_address;
    socklen_t fromLength = sizeof(struct sockaddr_in);

    printf("Waiting for a client to connect...\n");

    int new_sd = accept(server_sd, (struct sockaddr *)&from_address, &fromLength);
    if (new_sd < 0) {
        perror("Error: accept() failed");
        close(server_sd);
        exit(1);
    }

    printf("Client connected successfully from %s:%d\n",
           inet_ntoa(from_address.sin_addr),
           ntohs(from_address.sin_port));

    return new_sd;
}

/* ----------------------------------------------------------------
 * receive_message
 * ----------------------------------------------------------------
 * Receives a null-terminated string from the client.
 * Prints the number of bytes received and the message.
 * Returns the number of bytes received, or -1 on error.
 */
int receive_message(int client_sd, char *buffer, int buf_size)
{
    memset(buffer, 0, buf_size);

    int rc = recv(client_sd, buffer, buf_size - 1, 0);

    if (rc < 0) {
        perror("Error: recv() failed");
        return -1;
    }

    if (rc == 0) {
        printf("Client disconnected before sending data.\n");
        return 0;
    }

    /* Make sure the string is null-terminated */
    buffer[rc] = '\0';

    /* Print as required by the assignment */
    printf("Received %d bytes\n", rc);
    printf("Message: %s\n", buffer);

    return rc;
}

/* ----------------------------------------------------------------
 * send_response
 * ----------------------------------------------------------------
 * Sends a fixed response string back to the client.
 * Also prints the response (must match what the client receives).
 * Returns 0 on success, -1 on failure.
 */
int send_response(int client_sd)
{
    const char *response = RESPONSE;

    int rc = send(client_sd, response, strlen(response) + 1, 0);
    if (rc < 0) {
        perror("Error: send() failed");
        return -1;
    }

    printf("Response sent: %s\n", response);

    return 0;
}

/* ----------------------------------------------------------------
 * cleanup
 * ----------------------------------------------------------------
 * Closes all open sockets to free resources.
 * Pass -1 for any socket that is not open.
 */
void cleanup(int server_sd, int client_sd)
{
    if (client_sd >= 0) {
        close(client_sd);
    }
    if (server_sd >= 0) {
        close(server_sd);
    }
    printf("Server shut down. All sockets closed.\n");
}

/* ----------------------------------------------------------------
 * main
 * ----------------------------------------------------------------
 * Orchestrates the server lifecycle:
 *   parse args -> create socket -> accept -> receive -> send -> cleanup
 */
int main(int argc, char *argv[])
{
    char buffer[BUFFER_SIZE];

    /* Parse and validate command-line arguments */
    int port = parse_arguments(argc, argv);

    /* Create server socket, bind, and listen */
    int server_sd = create_server_socket(port);

    /* Accept one client connection */
    int client_sd = accept_client(server_sd);

    /* Receive the client's message */
    int bytes = receive_message(client_sd, buffer, BUFFER_SIZE);

    /* Send a response if we got data */
    if (bytes > 0) {
        send_response(client_sd);
    }

    /* Clean up all sockets */
    cleanup(server_sd, client_sd);

    return 0;
}
