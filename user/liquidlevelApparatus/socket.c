/*
 * socket.c
 *
 *  Created on: 2015/8/27
 *      Author: jk
 */
#include <stdio.h>

#include <eat_interface.h>
#include <eat_socket.h>

#include "socket.h"
#include "setting.h"
#include "log.h"
#include "client.h"
#include "msg.h"
#include "state.h"
#include "timer.h"


static s8 socket_id = 0;

static u32 hostname_request_id = 0;	//��ǰֻ������һ����������������ֶα�ʶ��ŷ̫���Ҫ

#define DESC_DEF(x)	case x:\
                        return #x

/*
SOC_READ    = 0x01,   Notify for read
SOC_WRITE   = 0x02,   Notify for write
SOC_ACCEPT  = 0x04,   Notify for accept
SOC_CONNECT = 0x08,   Notify for connect
SOC_CLOSE   = 0x10,    Notify for close
SOC_ACKED   = 0x20   Notify for acked
*/

static char* getEventDescription(soc_event_enum event)
{
	switch (event)
	{
		DESC_DEF(SOC_READ);
		DESC_DEF(SOC_WRITE);
		DESC_DEF(SOC_ACCEPT);
		DESC_DEF(SOC_CONNECT);
		DESC_DEF(SOC_CLOSE);
		DESC_DEF(SOC_ACKED);
		default:
		{
			static char soc_event[10] = {0};
			sprintf(soc_event, "%d", event);
			return soc_event;
		}
	}
}

/*
CBM_DEACTIVATED             = 0x01,  deactivated
CBM_ACTIVATING              = 0x02,  activating
CBM_ACTIVATED               = 0x04,  activated
CBM_DEACTIVATING            = 0x08,  deactivating
CBM_CSD_AUTO_DISC_TIMEOUT   = 0x10,  csd auto disconnection timeout
CBM_GPRS_AUTO_DISC_TIMEOUT  = 0x20,  gprs auto disconnection timeout
CBM_NWK_NEG_QOS_MODIFY      = 0x040,  negotiated network qos modify notification
CBM_WIFI_STA_INFO_MODIFY
*/

static char* getStateDescription(cbm_bearer_state_enum state)
{
	switch (state)
	{
		DESC_DEF(CBM_DEACTIVATED);
		DESC_DEF(CBM_ACTIVATING);
		DESC_DEF(CBM_ACTIVATED);
		DESC_DEF(CBM_DEACTIVATING);
		DESC_DEF(CBM_CSD_AUTO_DISC_TIMEOUT);
		DESC_DEF(CBM_GPRS_AUTO_DISC_TIMEOUT);
		DESC_DEF(CBM_NWK_NEG_QOS_MODIFY);
		DESC_DEF(CBM_WIFI_STA_INFO_MODIFY);
		default:
		{
			static char bearer_state[10] = {0};
			sprintf(bearer_state, "%d", state);
			return bearer_state;
		}
	}
}



void hostname_notify_cb(u32 request_id,eat_bool result,u8 ip_addr[4])
{
	s8 rc = SOC_ERROR;
	sockaddr_struct address={SOC_SOCK_STREAM};


	if (result == EAT_TRUE)
	{
		LOG_DEBUG("hostname notify: %d.%d.%d.%d:%d", ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3], setting.port);

		address.sock_type = SOC_SOCK_STREAM;
		address.addr_len = 4;
		address.addr[0] = ip_addr[0];
		address.addr[1] = ip_addr[1];
		address.addr[2] = ip_addr[2];
		address.addr[3] = ip_addr[3];

		address.port = setting.port; /* TCP server port */

		rc = eat_soc_connect(socket_id, &address);
		if (rc >= 0)
		{
			LOG_INFO("socket id of new connection is :%d", rc);
		}
		else if (rc == SOC_WOULDBLOCK)
		{
         	LOG_INFO("Connection is in progressing...");
		}
		else
		{
			LOG_ERROR("Connect return error:%d", rc);
		}
	}
}

void soc_notify_cb(s8 s,soc_event_enum event,eat_bool result, u16 ack_size)
{
    u8 buffer[128] = {0};
    s32 rc = 0;

    LOG_DEBUG("SOCKET notify:socketid(%d), event(%s)", s, getEventDescription(event));

    switch (event)
    {
        case SOC_READ:
            socket_id = s;		//TODO: according to the demo, but why?

            rc = eat_soc_recv(socket_id, buffer, 128);
            if (rc == SOC_WOULDBLOCK)
            {
                LOG_ERROR("read data from socket block");
            }
            else if (rc > 0)
            {
                client_proc(buffer, rc);
            }
            else
            {
                LOG_ERROR("eat_soc_recv error:rc=%d", rc);
            }

            break;

        case SOC_CONNECT:
            if (result)
            {
            	LOG_INFO("SOC_CONNECT successfully");
            	set_socket_state(EAT_TRUE);
            }
            else
            {
            	LOG_ERROR("SOC_CONNECT error:%d", result);

            	//wait 5 seconds, then reconnect
            	eat_sleep(5000);
            	socket_init();
            }

            break;
        case SOC_CLOSE:
            eat_soc_close(s);
            set_socket_state(EAT_FALSE);

            //restart the AT timer again
            eat_timer_start(TIMER_AT_CMD, 5000);
            break;

        case SOC_ACKED:
            LOG_DEBUG("acked size of send data: %d", ack_size);
            break;

        default:
            break;
    }

}

void socket_setup(void)
{
	s8 rc = SOC_SUCCESS;
	s8 val = EAT_TRUE;
	sockaddr_struct address={SOC_SOCK_STREAM};

    eat_soc_notify_register(soc_notify_cb);

    socket_id = eat_soc_create(SOC_SOCK_STREAM, 0);
    if (socket_id < 0)
    {
		LOG_ERROR("eat_soc_create return error :%d", socket_id);
    	eat_reset_module();//TODO: error handle
		return;
    }
    else
    {
    	LOG_DEBUG("socket id = %d", socket_id);
    }

    rc = eat_soc_setsockopt(socket_id, SOC_NBIO, &val, sizeof(val));
    if (rc != SOC_SUCCESS)
    {
    	LOG_ERROR("eat_soc_setsockopt set SOC_NBIO failed: %d", rc);
    	eat_reset_module();//TODO: error handle
    	return;
    }

    rc = eat_soc_setsockopt(socket_id, SOC_NODELAY, &val, sizeof(val));
    if (rc != SOC_SUCCESS)
    {
    	LOG_ERROR("eat_soc_setsockopt set SOC_NODELAY failed: %d", rc);
    	eat_reset_module();//TODO: error handle
    	return;
    }

    val = SOC_READ | SOC_WRITE | SOC_CLOSE | SOC_CONNECT;
    rc = eat_soc_setsockopt(socket_id, SOC_ASYNC, &val, sizeof(val));
    if (rc != SOC_SUCCESS)
    {
    	LOG_ERROR("eat_soc_setsockopt set SOC_ASYNC failed: %d", rc);
    	eat_reset_module();//TODO: error handle
    	return;
    }

    address.sock_type = SOC_SOCK_STREAM;
    address.addr_len = 4;
    if (setting.addr_type == ADDR_TYPE_IP)
    {
        address.addr[0] = setting.addr.ipaddr[0];
        address.addr[1] = setting.addr.ipaddr[1];
        address.addr[2] = setting.addr.ipaddr[2];
        address.addr[3] = setting.addr.ipaddr[3];

        LOG_DEBUG("connect to %d.%d.%d.%d:%d", address.addr[0], address.addr[1], address.addr[2], address.addr[3], setting.port);
    }
    else
    {
    	u8 ipaddr[4] = {0};
    	u8 len = 4;

    	eat_soc_gethost_notify_register(hostname_notify_cb);
    	hostname_request_id++;
    	rc = eat_soc_gethostbyname(setting.addr.domain, ipaddr, &len, hostname_request_id);
    	if (rc == SOC_SUCCESS)
    	{
            address.addr[0] = ipaddr[0];
            address.addr[1] = ipaddr[1];
            address.addr[2] = ipaddr[2];
            address.addr[3] = ipaddr[3];

            LOG_DEBUG("host:%s -> %d.%d.%d.%d", setting.addr.domain, ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
    	}
    	else if (rc == SOC_WOULDBLOCK)
    	{
    		LOG_INFO("eat_soc_gethostbyname wait callback");
    		return;
    	}
    	else
    	{
    		LOG_ERROR("eat_soc_gethostbyname error!");
    		return;
    	}
    }
    address.port =  setting.port;                /* TCP server port */


    rc = eat_soc_connect(socket_id, &address);
    if(rc >= 0)
    {
    	LOG_INFO("socket id of new connection is :%d", rc);
    }
    else if (rc == SOC_WOULDBLOCK)
    {
     	LOG_INFO("Connection is in progressing...");
    }
    else
    {
    	LOG_ERROR("Connect return error:%d", rc);
    }
}
void bear_notify_cb(cbm_bearer_state_enum state, u8 ip_addr[4])
{
	LOG_INFO("BEAR_NOTIFY: %s", getStateDescription(state));

	if (state & CBM_ACTIVATED)
	{
		LOG_INFO("local ip: %d.%d.%d.%d", ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);

		socket_setup();
	}
}

void socket_init(void)
{
    s8 rc = eat_gprs_bearer_open("CMNET", NULL, NULL, bear_notify_cb);

    switch (rc)
    {
    case CBM_OK:
        rc = eat_gprs_bearer_hold();
        if (rc == CBM_OK)
        {
            LOG_INFO("hold bearer success.");

            eat_timer_stop(TIMER_AT_CMD);

            socket_setup();
        }
        else
        {
        	LOG_ERROR("hold bearer failed");
            eat_timer_start(TIMER_AT_CMD, 5000);
        }

    	break;
    case CBM_WOULDBLOCK:
    	LOG_INFO("opening bearer...");
    	eat_timer_stop(TIMER_AT_CMD);

    	break;
    default:
        LOG_ERROR("open bearer failed, rc = %d", rc);

        eat_timer_start(TIMER_AT_CMD, 5000);

    	break;
    }

    return;
}

s32 socket_sendData(void* data, s32 len)
{
    s32 rc = eat_soc_send(socket_id, data, len);
    if (rc >= 0)
    {
        LOG_DEBUG("socket send data successful:%d", rc);
    }
    else
    {
        LOG_ERROR("sokcet send data failed:%d", rc);
    }

    LOG_BIN(data, len);

    free_msg(data);  //TODO: is it ok to free the msg here???

    return rc;
}
