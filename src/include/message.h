#ifndef MESSAGE_H__
#define MESSAGE_H__

#define DEFAULT_MESSAGE_BUFFER_SIZE 1024

// mesage_status defines the status of the previous request.
typedef enum message_status_ {
    OK_DONE,
    OK_WAIT_FOR_RESPONSE,
    UNKNOWN_COMMAND,
    INCORRECT_FORMAT
} message_status;

// message_t is a single packet of information sent between client and server.
// message_status: defines the status of the previous message.
// length: defines the length of the string message to be sent.
// message_string: contains the message.
typedef struct message_ {
    message_status status;
    int length;
    char message[DEFAULT_MESSAGE_BUFFER_SIZE];
} message_t;

#endif
