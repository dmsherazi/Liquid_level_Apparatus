/*
 * msg.c
 *
 *  Created on: 2015/8/27
 *      Author: jk
 */
#include <eat_interface.h>
#include <eat_mem.h>

#include "msg.h"

void* alloc_msg(u16 cmd, size_t length)
{
    MSG_HEADER* msg = eat_mem_alloc(length);

    if (msg)
    {
        msg->signature = htonl(SIGNATURE);
        msg->length = htonl(length - sizeof(MSG_HEADER));
        msg->cmd = htons(cmd);
    }

    return msg;
}



void free_msg(void* msg)
{
    eat_mem_free(msg);
}
