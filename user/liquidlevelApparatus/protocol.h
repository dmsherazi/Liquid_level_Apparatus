/*
 * protocol.h
 *
 *  Created on: 2015/8/27
 *      Author: jk
 */

#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#define SIGNATURE ((unsigned int)0xFEFE)


enum
{
	CMD_SETTING	= 1001,		//�ɼ�ָ���·���������������� -> ǰ�˲ɼ���
	CMD_SAMPLE	= 1002,		//�ɼ������ϱ���ǰ�˲ɼ��� ->  �������������
	CMD_PING	= 1003,		//������ǰ�˲ɼ���  -> �������������
};

#pragma pack(push, 1)

/*
 * Message header definition
 */
typedef struct
{
    u32 signature;
    float longitude;
    float latitude;
    u32 length;
    u16 cmd;
    char param[];
}__attribute__((__packed__)) MSG_HEADER;

#define MSG_HEADER_LEN sizeof(MSG_HEADER)

typedef struct
{
	short length;
	char data[];
}PARAM;


#pragma pack(pop)

#endif /* _PROTOCOL_H_ */
