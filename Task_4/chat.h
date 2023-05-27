#ifndef CHAT_H
#define CHAT_H

#include "reactor.h"

#define MAX_BUFFER_SIZE 256

void handleNewConnection(void* context);
void handleClientData(void* context);

#endif  // CHAT_H
