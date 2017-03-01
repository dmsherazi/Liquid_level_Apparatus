/*
 * msg.h
 *
 *  Created on: 2015/8/27
 *      Author: jk
 */

#ifndef USER_LIQUIDLEVEL_MSG_H_
#define USER_LIQUIDLEVEL_MSG_H_

#include <stdio.h>

#include "protocol.h"

#define htonl(l) ((((l >> 24) & 0x000000ff)) | \
                  (((l >>  8) & 0x0000ff00)) | \
                  (((l) & 0x0000ff00) <<  8) | \
                  (((l) & 0x000000ff) << 24))
#define ntohl(l) htonl(l)

#define htons(s) ((((s) >> 8) & 0xff) | \
                  (((s) << 8) & 0xff00))
#define ntohs(s) htons(s)

void* alloc_msg(u16 cmd, size_t length);
void* alloc_rspMsg(const MSG_HEADER* pMsg);

void free_msg(void* msg);

#endif /* USER_LIQUIDLEVEL_MSG_H_ */
