#ifndef MESSAGE_H__
#define MESSAGE_H__

#define MAX_MSG_SIZE 4096

// mesage_status defines the status of the previous request.
typedef enum message_status {
    OK_DONE,
    OK_WAIT_FOR_RESPONSE,
    OK_IMPORTANT,
    OK_SHUTDOWN,
} message_status;

// message is a single packet of information sent between client/server.
// message_status: defines the status of the message.
// length: defines the length of the string message to be sent.
// payload: defines the payload of the message.
typedef struct message {
    message_status status;
    int length;
    char* payload;
} message;

void add_payload(message *m, const char *format, ...);
void add_raw_payload(message *m, char *payload);

void init_message(message *m);

void destroy_message(message *m);

#endif
