#pragma once

#include <stdint.h>

#define LEMON_MESSAGE_MAGIC 0xBEEF

#define LEMON_MESSAGE_PROTCOL_WMEVENT 1
#define LEMON_MESSAGE_PROTCOL_WMCMD 2
#define LEMON_MESSAGE_PROTOCOL_SHELLCMD 3
#define LEMON_MESSAGE_PROTOCOL_SESSIONCMD 4
#define LEMON_MESSAGE_PROTOCOL_SESSIONRESPONSE 5

namespace Lemon {
    typedef struct LemonMessage {
        unsigned short magic = 0xBEEF;
        unsigned short length;
        unsigned int protocol = 0;
        uint8_t data[];
    } __attribute__((packed)) lemon_message_t;
}