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
    static char seq = 0;
    MSG_HEADER* msg = eat_mem_alloc(length);

    if (msg)
    {
        msg->signature = htons(SIGNATURE);
        msg->length = htons(length - sizeof(MSG_HEADER));
        msg->cmd = cmd;
        msg->seq = seq++;
    }

    return msg;
}



void free_msg(void* msg)
{
    eat_mem_free(msg);
}
