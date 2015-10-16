/** server.c
 * CS165 Fall 2015
 *
 * This file provides a basic unix socket implementation for a server
 * used in an interactive client-server database.
 * The server should allow for multiple concurrent connections from clients.
 * Each client should be able to send messages containing queries to the
 * server.  When the server receives a message, it must:
 * 1. Respond with a status based on the query (OK, UNKNOWN_QUERY, etc.)
 * 2. Process any appropriate queries, if applicable.
 * 3. Return the query response to the client.
 *
 * For more information on unix sockets, refer to:
 * http://beej.us/guide/bgipc/output/html/multipage/unixsock.html
 **/
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "cs165_api.h"
#include "message.h"
#include "parser.h"
#include "utils.h"
#include "list.h"

// includes that use global structures
#include "dbs.h"
#include "variables.h"

#define DEFAULT_QUERY_BUFFER_SIZE 1024

/* Bison parser */
#include "parser/grammar.tab.h"

void parse_dsl(char *str, db_operator *op, message *send_msg);

/**
 * parse_command takes as input the send_message from the client and then
 * parses it into the appropriate query. Stores into send_message the
 * status to send back.
 * Returns a db_operator.
 **/
db_operator* parse_command(message* recv_message, message* send_message) {
    send_message->status = OK_WAIT_FOR_RESPONSE;

    // this will be used and freed in execute_db_operator
    db_operator *dbo = malloc(sizeof(db_operator));

    // Uses Bison and flex to parse the string
    parse_dsl(recv_message->payload, dbo, send_message);

    return dbo;
}

/** execute_db_operator takes as input the db_operator and executes the query.
 * It should return the result (currently as a char*, although I'm not clear
 * on what the return type should be, maybe a result struct, and then have
 * a serialization into a string message).
 **/
void execute_db_operator(db_operator* query, message *send_msg) {
    switch (query->type) {
        case NOOP:
            // in this case the payload was already added by parse_dsl
            break;
        case INSERT: {
            list *values = query->values1;
            table *tbl = query->tbl;

            relational_insert(tbl, values);

            // now we destroy the list
            // TODO: move this to cleanup
            destroy_list(query->values1);

            add_payload(send_msg, "Row added succesfully to table %s", query->tbl->name);
            break;
        } case ERROR:
            break;
        default:
            add_payload(send_msg, "Unsupported query type");
            break;
    }
    // TODO: move this to cleanup_db_operator
    free(query);
}

/*
 * Here we free all of the global structures that were allocated,
 * before shutting down the DB server.
 */
void cleanup_globals(void) {
    drop_dbs();
    destroy_vars();
}

/**
 * handle_client(client_socket)
 * This is the execution routine after a client has connected.
 * It will continually listen for messages from the client and execute queries.
 **/
void handle_client(int client_socket) {
    int done = 0;
    int length = 0;

    log_info("Connected to socket: %d.\n", client_socket);

    // Create two messages, one from which to read and one from which to receive
    message send_message;
    init_message(&send_message);

    // recv_message will live on the stack, so we don't need to init it
    message recv_message;

    // Continually receive messages from client and execute queries.
    // 1. Parse the command
    // 2. Handle request if appropriate
    // 3. Send status of the received message (OK, UNKNOWN_QUERY, etc)
    // 4. Send response of request.
    do {
        length = recv(client_socket, &recv_message, sizeof(message), 0);
        if (length < 0) {
            log_err("Client connection closed!\n");
            exit(1);
        } else if (length == 0) {
            done = 1;
        }

        if (!done) {
            char recv_buffer[recv_message.length];
            length = recv(client_socket, recv_buffer, recv_message.length,0);
            recv_message.payload = recv_buffer;
            recv_message.payload[recv_message.length] = '\0';

            // 1. Parse command
            db_operator* query = parse_command(&recv_message, &send_message);

            // 2. Handle request
            execute_db_operator(query, &send_message);

            // 3. Send status of the received message (OK, UNKNOWN_QUERY, etc)
            if (send(client_socket, &(send_message), sizeof(message), 0) == -1) {
                log_err("Failed to send message.");
                exit(1);
            }

            // 4. Send response of request
            if (send(client_socket, send_message.payload, send_message.length, 0) == -1) {
                log_err("Failed to send message.");
                exit(1);
            }

            destroy_message(&send_message);
        }
    } while (!done);

    log_info("Connection closed at socket %d!\n", client_socket);
    close(client_socket);

    cleanup_globals();
}


/**
 * setup_server()
 *
 * This sets up the connection on the server side using unix sockets.
 * Returns a valid server socket fd on success, else -1 on failure.
 **/
int setup_server() {
    int server_socket;
    size_t len;
    struct sockaddr_un local;

    log_info("Attempting to setup server...\n");

    if ((server_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        log_err("L%d: Failed to create socket.\n", __LINE__);
        return -1;
    }

    local.sun_family = AF_UNIX;
    strncpy(local.sun_path, SOCK_PATH, strlen(SOCK_PATH) + 1);
    unlink(local.sun_path);

    /*
    int on = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
    {
        log_err("L%d: Failed to set socket as reusable.\n", __LINE__);
        return -1;
    }
    */

    len = strlen(local.sun_path) + sizeof(local.sun_family) + 1;
    if (bind(server_socket, (struct sockaddr *)&local, len) == -1) {
        log_err("L%d: Socket failed to bind.\n", __LINE__);
        return -1;
    }

    if (listen(server_socket, 5) == -1) {
        log_err("L%d: Failed to listen on socket.\n", __LINE__);
        return -1;
    }

    return server_socket;
}

// Currently this main will setup the socket and accept a single client.
// After handling the client, it will exit.
// You will need to extend this to handle multiple concurrent clients
// and remain running until it receives a shut-down command.
int main(void)
{
    int server_socket = setup_server();
    if (server_socket < 0) {
        exit(1);
    }

    // Populate the global dsl commands
    dsl_commands = dsl_commands_init();

    log_info("Waiting for a connection %d ...\n", server_socket);

    struct sockaddr_un remote;
    socklen_t t = sizeof(remote);
    int client_socket = 0;

    if ((client_socket = accept(server_socket, (struct sockaddr *)&remote, &t)) == -1) {
        log_err("L%d: Failed to accept a new connection.\n", __LINE__);
        exit(1);
    }

    handle_client(client_socket);

    return 0;
}

