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
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/stat.h>

char *strdup(const char *s);
char *strsep(char **stringp, const char *delim);

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/un.h>

#include "common.h"
#include "message.h"
#include "utils.h"

#define DEFAULT_STDIN_BUFFER_SIZE 1024

// uncomment this if you want to compile for production
#define IMPORTANT_ONLY

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

    message send_message;
    message recv_message;

    // Always output an interactive marker at the start of each command if the
    // input is from stdin. Do not output if piped in from file or from other fd
    char* prefix = "";
    if (isatty(fileno(stdin))) {
        prefix = "db_client > ";
    }

    char *output_str = NULL;
    int len = 0;

    // Continuously loop and wait for input. At each iteration:
    // 1. output interactive marker
    // 2. read from stdin until eof.
    char read_buffer[DEFAULT_STDIN_BUFFER_SIZE];
    send_message.payload = read_buffer;

    // clear output
    //printf("\n");
    while (printf("%s", prefix), output_str = fgets(read_buffer,
           DEFAULT_STDIN_BUFFER_SIZE, stdin), !feof(stdin)) {
        if (output_str == NULL) {
            log_err("fgets failed.\n");
            break;
        }

        // Only process input that is greater than 1 character.
        // Ignore things such as new lines.
        // Otherwise, convert to message and send the message and the
        // payload directly to the server.
        send_message.length = strlen(read_buffer);
        if (send_message.length > 1) {
            char loading = false;
            int load_fd = -1;

            // if the message is load(something) we need some preprocessing
            char *tmp = strdup(send_message.payload);
            int LOAD_OFFSET = 5;
            tmp[LOAD_OFFSET] = '\0';
            char *filepath;
            struct stat filestat;
            if (strcmp(tmp, "load(") == 0) {
                char *filename = tmp + LOAD_OFFSET + 1;
                filepath = strsep(&filename, "\"");
                if (filename == NULL)
                    continue;

                // first we grab the file's length
                load_fd = open(filepath, O_RDONLY);
                if (load_fd < 0) {
                    printf("Could not open file %s\n", filepath);
                    continue;
                }
                int r = fstat(load_fd, &filestat);
                if (r < 0) {
                    printf("Could not stat file w/ fd %d\n", load_fd);
                    continue;
                }

                loading = true;
            }
            free(tmp);


            // Send the message_header, which tells server payload size
            if (send(client_socket, &(send_message), sizeof(message), 0) == -1) {
                log_err("Failed to send message header.");
                exit(1);
            }

            // Send the payload (query) to server
            if (send(client_socket, send_message.payload, send_message.length, 0) == -1) { log_err("Failed to send query payload.");
                exit(1);
            }

            if (loading) {
                if (send(client_socket, &(filestat.st_size), sizeof(off_t), 0) == -1) { log_err("Failed to send load file size.");
                    exit(1);
                }

                if (sendfile(client_socket, load_fd, NULL, filestat.st_size) == -1) {
                    log_err("Failed to send file. Error: %s", strerror(errno));
                    exit(1);
                }
                // sends token telling server file is done
                char *token = "feedface\n";
                if (send(client_socket, token, strlen(token), 0) == -1) { log_err("Failed to send token.");
                    exit(1);
                }
                close(load_fd);
            }
#ifdef IMPORTANT_ONLY
#else
            long current = get_microtime();
#endif

            // Always wait for server response (even if it is just an OK message)
            if ((len = recv(client_socket, &(recv_message), sizeof(message), 0)) > 0) {
#ifdef IMPORTANT_ONLY
#else
                long elapsed = get_microtime() - current;
                printf("Server's response took %ld microseconds\n", elapsed);
#endif
                if (recv_message.status == OK_SHUTDOWN) {
                    break;
                }

                if ((recv_message.status == OK_WAIT_FOR_RESPONSE || recv_message.status == OK_IMPORTANT) &&
                    (int) recv_message.length > 0) {
                    // Calculate number of bytes in response package
                    int num_bytes = (int) recv_message.length;
                    char payload[num_bytes + 1];

                    // Receive the payload and print it out
                    if ((len = recv(client_socket, payload, num_bytes, 0)) > 0) {
                        payload[num_bytes] = '\0';
#ifdef IMPORTANT_ONLY
                        if (recv_message.status == OK_IMPORTANT)
                            printf("%s", payload);
#else
                        printf("%s\n", payload);
#endif
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
