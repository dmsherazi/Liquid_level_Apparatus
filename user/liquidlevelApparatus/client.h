/*
 * client.h
 *
 *  Created on: 2015/8/27
 *      Author: jk
 */

#ifndef USER_LIQUIDLEVEL_CLIENT_H_
#define USER_LIQUIDLEVEL_CLIENT_H_


int client_proc(const void* m, int msgLen);
void msg_heartbeat(void);
void msg_upload(char* devid, double pressure);


#endif /* USER_LIQUIDLEVEL_CLIENT_H_ */
