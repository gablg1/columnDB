#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "message.h"
#include <string.h>
#include <assert.h>


void add_payload(message *m, const char *format, ...) {
    va_list v;
    va_start(v, format);

    // lets make sure only one payload was added
    assert(m->payload == NULL);

    m->payload = malloc(MAX_MSG_SIZE);
    vsnprintf(m->payload, MAX_MSG_SIZE, format, v);
    va_end(v);
    m->length = strlen(m->payload);
}

void init_message(message *m) {
    m->payload = NULL;
}

void destroy_message(message *m) {
    if (m->payload != NULL)
        free(m->payload);
}

