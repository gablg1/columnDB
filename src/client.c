#define _XOPEN_SOURCE
/**
 * client.c
 *  CS165 Fall 2015
 *
 * This file provides a basic unix socket implementation for a client
 * used in an interactive client-server database.
 * The client receives input from stdin and sends it to the server.
 * No pre-processing is done on the client-side.
 *
 * For more information on unix sockets, refer to:
 * http://beej.us/guide/bgipc/output/html/multipage/unixsock.html
 **/
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "common.h"
#include "message.h"
#include "utils.h"


/**
 * connect_client()
 *
 * This sets up the connection on the client side using unix sockets.
 * Returns a valid client socket fd on success, else -1 on failure.
 *
 **/
int connect_client() {
    int client_socket;
    size_t len;
    struct sockaddr_un remote;

    log_info("Attempting to connect...\n");

    if ((client_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        log_err("L%d: Failed to create socket.\n", __LINE__);
        return -1;
    }

    remote.sun_family = AF_UNIX;
    strncpy(remote.sun_path, SOCK_PATH, strlen(SOCK_PATH) + 1);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family) + 1;
    if (connect(client_socket, (struct sockaddr *)&remote, len) == -1) {
        perror("client connect failed: ");
        return -1;
    }

    log_info("Client connected at socket: %d.\n", client_socket);
    return client_socket;
}

int main(void)
{
    int client_socket = connect_client();
    if (client_socket < 0) {
        exit(1);
    }

    message_t send_message;
    message_t recv_message;

    // Always output an interactive marker at the start of each command if the
    // input is from stdin. Do not output if piped in from file or from other fd
    char* prefix = "";
    if (isatty(fileno(stdin))) {
        prefix = "db_client> ";
    }

    char *output_str = NULL;
    int len = 0;

    // Continuously loop and wait for input. At each iteration:
    // 1. output interactive marker
    // 2. read from stdin until eof.
    while (printf("%s", prefix), output_str = fgets(send_message.message,
           DEFAULT_MESSAGE_BUFFER_SIZE, stdin), !feof(stdin)) {
        if (output_str == NULL) {
            log_err("fgets failed.\n");
            break;
        }

        // Only process input that is greater than 1 character.
        // Ignore things such as new lines.
        // Otherwise, convert to message and send directly to the server.
        if (strlen(send_message.message) > 1) {
            if (send(client_socket, &(send_message), sizeof(message_t), 0) == -1) {
                log_err("Failed to send message.");
                exit(1);
            }

            // For now, always wait for server response (even if it is just an OK message)
            if ((len = recv(client_socket, &(recv_message), sizeof(message_t), 0)) > 0) {
                if (recv_message.status == OK_WAIT_FOR_RESPONSE) {
                    if ((len = recv(client_socket, &(recv_message), sizeof(message_t), 0)) > 0) {
                        int message_length = strlen(recv_message.message);
                        if (message_length < DEFAULT_MESSAGE_BUFFER_SIZE) {
                            recv_message.message[message_length] = '\0';
                        }
                        printf("%s\n", recv_message.message);
                    }
                }
            }
            else {
                if (len < 0) {
                    log_err("Failed to receive message.");
                }
                else {
		    log_info("Server closed connection\n");
		}
                exit(1);
            }
        }
    }

    close(client_socket);
    return 0;
}
