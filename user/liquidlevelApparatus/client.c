/*
 * client.c
 *
 *  Created on: 2015/8/27
 *      Author: jk
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <eat_interface.h>
#include <eat_mem.h>

#include "client.h"
#include "socket.h"
#include "msg.h"
#include "log.h"
#include "uart.h"
#include "state.h"
#include "setting.h"
#include "cJSON.h"

#define TAG_CMD "CMD"
#define TAG_SIGNATURE "SIGNATURE"
#define TAG_HEADER "HEADER"
#define TAG_TIMESTAMP "TIMESTAMP"
#define TAG_LONGITUDE "LONGITUDE"
#define TAG_LATITUDE "LATITUDE"
#define TAG_PRESSURE "PRESSURE"
#define TAG_DEVICEID "DEVICEID"


typedef int (*MSG_PROC)(const void* msg);
typedef struct
{
    int cmd;
    MSG_PROC pfn;
}MC_MSG_PROC;

void msg_heartbeat(void);


static int msg_heartbeatyRsp(const void* msg);



static MC_MSG_PROC msgProcs[] =
{
		{CMD_PING, msg_heartbeatyRsp},
};




int client_proc(const void* m, int msgLen)
{
    MSG_HEADER* msg = (MSG_HEADER*)m;
    size_t i = 0;

    if (msgLen < sizeof(MSG_HEADER))
    {
        LOG_ERROR("receive message length not enough: %zu(at least(%zu)", msgLen, sizeof(MSG_HEADER));

        return -1;
    }

    if (msg->signature != ntohl(SIGNATURE))
    {
        LOG_ERROR("receive message head signature error:%x", msg->signature);
        return -1;
    }

    for (i = 0; i < sizeof(msgProcs) / sizeof(msgProcs[0]); i++)
    {
        if (msgProcs[i].cmd == ntohs(msg->cmd))
        {
            MSG_PROC pfn = msgProcs[i].pfn;
            if (pfn)
            {
                return pfn(msg);
            }
            else
            {
                LOG_ERROR("Message %d not processed", msg->cmd);
                return -1;
            }
        }
    }

    LOG_ERROR("unknown message %d", msg->cmd);
    return -1;
}

void msg_heartbeat(void)
{
	size_t msgLen = sizeof(MSG_HEADER) + sizeof(short);
	MSG_HEADER* msg = alloc_msg(CMD_PING, msgLen);

	LOG_INFO("Gateway heartbeat");

	socket_sendData(msg, msgLen);
}


void msg_upload(char* devid, double pressure)
{
    char* content = NULL;
    cJSON *msg = cJSON_CreateObject();

    LOG_INFO("current data upload: %s=%fmA", devid, pressure / 10.0 * 16.0 + 4);
	LOG_INFO("pressure data upload: %s=%fKpa", devid, pressure);

    cJSON_AddNumberToObject(msg, TAG_CMD, CMD_SAMPLE);
    cJSON_AddStringToObject(msg, TAG_DEVICEID, devid);
    cJSON_AddNumberToObject(msg, TAG_PRESSURE, pressure);

    content = cJSON_PrintUnformatted(msg);

	socket_sendData(content, strlen(content));

    cJSON_Delete(msg);

    LOG_DEBUG("UPLOAD FINISH\n");

	return;
}


static int msg_heartbeatyRsp(const void* msg)
{
	return 0;
}

