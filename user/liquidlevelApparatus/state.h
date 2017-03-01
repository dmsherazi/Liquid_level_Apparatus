/*
 * state.h
 *
 *  Created on: 2015/8/27
 *      Author: jk
 */

#ifndef USER_LIQUIDLEVEL_STATE_H_
#define USER_LIQUIDLEVEL_STATE_H_

#include <eat_type.h>

#include "protocol.h"

eat_bool socket_conneted(void);
void set_socket_state(eat_bool connected);

typedef void (*SOCKET_TRIGGER)(eat_bool state);
void set_socket_trigger(SOCKET_TRIGGER pfn);

#endif /* USER_LIQUIDLEVEL_STATE_H_ */
