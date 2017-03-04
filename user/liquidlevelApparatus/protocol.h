/*
 * protocol.h
 *
 *  Created on: 2015/8/27
 *      Author: jk
 */

#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#define SIGNATURE 0xFEFE


enum
{
	CMD_SETTING	= 1,		//采集指令下发：网关适配服务器 -> 前端采集板
	CMD_SAMPLE	= 2,		//采集数据上报：前端采集板 ->  网关适配服务器
	CMD_PING	= 3,		//心跳：前端采集板  -> 网关适配服务器
};

#pragma pack(push, 1)

/*
 * Message header definition
 */
typedef struct
{
    short signature;
    char cmd;
    char seq;
    short length;
}__attribute__((__packed__)) MSG_HEADER;

#define MSG_HEADER_LEN sizeof(MSG_HEADER)

typedef struct
{
    MSG_HEADER header;
    int timestamp;
    float longitude;
    float latitude;
    double pressure;
    char deviceID[];
}LIQUIDLEVEL_INFO;


#pragma pack(pop)

#endif /* _PROTOCOL_H_ */
