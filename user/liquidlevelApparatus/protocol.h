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
	CMD_SETTING	= 1,		//�ɼ�ָ���·���������������� -> ǰ�˲ɼ���
	CMD_SAMPLE	= 2,		//�ɼ������ϱ���ǰ�˲ɼ��� ->  �������������
	CMD_PING	= 3,		//������ǰ�˲ɼ���  -> �������������
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
