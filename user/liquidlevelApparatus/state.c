/*
 * state.c
 *
 *  Created on: 2015/8/27
 *      Author: jk
 */

#include <eat_interface.h>

#include "state.h"

#include "log.h"

typedef struct
{
    eat_bool connected;     //is the socket connected to server
    SOCKET_TRIGGER pfn;
}STATE;


static STATE state =
{
        EAT_FALSE,	//eat_bool connected
		NULL,		//SOCKET_TRIGGER pfn;
};


eat_bool socket_conneted(void)
{
    return state.connected;
}

void set_socket_state(eat_bool socket_state)
{
    state.connected = socket_state;

    LOG_INFO("socket state change to %d", socket_state);

}

void set_socket_trigger(SOCKET_TRIGGER pfn)
{
	state.pfn = pfn;
}

