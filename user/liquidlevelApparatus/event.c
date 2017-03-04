/*
 * event.c
 *
 *  Created on: 2015/8/27
 *      Author: jk
 */
#include <string.h>

#include <eat_interface.h>
#include <eat_uart.h>
#include <eat_gps.h>

#include "timer.h"
#include "watchdog.h"
#include "thread_msg.h"
#include "log.h"
#include "uart.h"
#include "socket.h"
#include "setting.h"
#include "msg.h"
#include "state.h"
#include "client.h"
#include "macro.h"
#include "utils.h"
#include "adc.h"


typedef int (*EVENT_FUNC)(const EatEvent_st* event);
typedef struct
{
	EatEvent_enum event;
	EVENT_FUNC pfn;
}EVENT_PROC;


/*
 * local event function definition
 */
int event_timer(const EatEvent_st* event);
int event_threadMsg(const EatEvent_st* event);
int event_mod_ready_rd(const EatEvent_st* event);
int event_readADData(const EatEvent_st* event);

static EVENT_PROC msgProcs[] =
{
    {EAT_EVENT_TIMER,				event_timer},
    {EAT_EVENT_MDM_READY_RD,        event_mod_ready_rd},
    {EAT_EVENT_MDM_READY_WR,        EAT_NULL},
    {EAT_EVENT_UART_READY_RD,       event_uart_ready_rd},
    {EAT_EVENT_UART_READY_WR,		EAT_NULL},
    {EAT_EVENT_UART_SEND_COMPLETE,	EAT_NULL},
    {EAT_EVENT_USER_MSG,            event_threadMsg},
    {EAT_EVENT_ADC,                 event_readADData},
};


#define DESC_DEF(x) case x:\
                            return #x

static char* getEventDescription(EatEvent_enum event)
{
    switch (event)
    {
        DESC_DEF(EAT_EVENT_TIMER);
        DESC_DEF(EAT_EVENT_KEY);
        DESC_DEF(EAT_EVENT_INT);
        DESC_DEF(EAT_EVENT_MDM_READY_RD);
        DESC_DEF(EAT_EVENT_MDM_READY_WR);
        DESC_DEF(EAT_EVENT_MDM_RI);
        DESC_DEF(EAT_EVENT_UART_READY_RD);
        DESC_DEF(EAT_EVENT_UART_READY_WR);
        DESC_DEF(EAT_EVENT_ADC);
        DESC_DEF(EAT_EVENT_UART_SEND_COMPLETE);
        DESC_DEF(EAT_EVENT_USER_MSG);
        DESC_DEF(EAT_EVENT_IME_KEY);
        default:
        {
            static char soc_event[10] = {0};
            sprintf(soc_event, "%d", event);
            return soc_event;
        }
    }
}
int event_mod_ready_rd(const EatEvent_st* event)
{
	u8 buf[256] = {0};
	u16 len = 0;
	u8* buf_ptr = NULL;

	len = eat_modem_read(buf, 256);
	LOG_DEBUG("modem recv: %s", buf);

	if (!len)
	{
	    LOG_ERROR("modem receive nothing");
	    return -1;
	}
	buf_ptr = (u8*) strstr((const char *) buf, "+CGATT: 1");
	if (buf_ptr != NULL)
	{
		socket_init();
		eat_timer_stop(TIMER_AT_CMD);
	}

	return 0;
}


int event_proc(EatEvent_st* event)
{
	int i = 0;

    LOG_DEBUG("event: %s", getEventDescription(event->event));

	for (i = 0; i < sizeof(msgProcs) / sizeof(msgProcs[0]); i++)
	{
		if (msgProcs[i].event == event->event)
		{
			EVENT_FUNC pfn = msgProcs[i].pfn;
			if (pfn)
			{
				return pfn(event);
			}
			else
			{
		        LOG_ERROR("event(%d) not processed!", event->event);
			}
		}
	}

	return -1;
}

void event_gps_proc(void)
{
    unsigned char buf[1024] = {0};
    unsigned char* buf_gps = NULL;
    int rc;

    double iGpstime = 0.0;
    int satellite = 0;
    float latitude = 0.0;
    float longitude = 0.0;
    float altitude = 0.0;
    float speed = 0.0;
    float course = 0.0;

    /*
     * the output format of eat_gps_nmea_info_output
     * $GPSIM,<latitude>,<longitude>,<altitude>,<UTCtime>,<TTFF>,<num>,<speed>,<course>
     * note:
            <TTFF>:time to first fix(in seconds)
            <num> :satellites in view for fix
     * example:$GPSIM,114.5,30.15,28.5,1461235600.123,3355,7,2.16,179.36
     */

    rc = eat_gps_nmea_info_output(EAT_NMEA_OUTPUT_SIMCOM, buf, 1024);
    if(rc == EAT_FALSE)
    {
        LOG_ERROR("get gps error ,and return is %d",rc);
    }

    LOG_DEBUG("%s",buf);

    buf_gps = string_bypass(buf, "$GPSIM,");

    rc = sscanf(buf_gps,"%f,%f,%f,%lf,%*d,%d,%f,%f",\
        &latitude,&longitude,&altitude,&iGpstime,&satellite,&speed,&course);

    if(longitude > 0 && latitude > 0)//get GPS
    {
        setting_setGPSPeriod(60 * 60 *1000);
        gps.isGPS = EAT_TRUE;
        gps.latitude = latitude;
        gps.longitude = longitude;
    }
    else
    {
        gps.isGPS = EAT_FALSE;
    }
}


int event_timer(const EatEvent_st* event)
{
    switch (event->data.timer.timer_id)
    {
        case TIMER_WATCHDOG:
            LOG_INFO("TIMER_WATCHDOG expire!");
            feedWatchdog();
            if(!socket_conneted())
            {
                //reconnect the socket
	            eat_timer_start(TIMER_AT_CMD, 5000);
            }
            eat_timer_start(event->data.timer.timer_id, WATCHDOG_FEED);
            break;

        case TIMER_AT_CMD:
            LOG_INFO("TIMER_AT_CMD expire!");
            eat_modem_write("AT+CGATT?\n", 10);
            eat_timer_start(event->data.timer.timer_id, 5000);
            break;

        case TIMER_HEARTBEAT:
            LOG_INFO("TIMER_HEARTBEAT expire!");
            eat_timer_start(event->data.timer.timer_id, setting_getHeartbeat());
            msg_heartbeat();
            break;

        case TIMER_UPLOAD:
            LOG_INFO("TIMER_SAMPLE expire!");
            eat_timer_start(event->data.timer.timer_id, setting_getUploadPeriod());
            if(socket_conneted())
            {
                msg_upload("device001",adc_calculatePressure());
                LOG_INFO("upload data!");//to upload
            }
            break;

        case TIMER_GPS:
            LOG_DEBUG("TIMER_GPS expire!");
            event_gps_proc();
            eat_timer_start(event->data.timer.timer_id, setting_getGPSPeriod());
            break;

        default:
            LOG_ERROR ("timer(%d) not processed", event->data.timer.timer_id);
            break;
    }
    return 0;
}

int event_threadMsg(const EatEvent_st* event)
{
    MSG_THREAD* msg = (MSG_THREAD*) event->data.user_msg.data_p;
//    u8 msgLen = event->data.user_msg.len;

    switch (msg->cmd)
    {
        case CMD_THREAD_SMS:
            LOG_DEBUG("receive thread command CMD_SMS");
            break;

        default:
            LOG_ERROR("unknown thread command:%d", msg->cmd);
    }

    freeMsg(msg);

    return 0;
}

int event_readADData(const EatEvent_st* event)
{
    if(event->data.adc.pin == EAT_PIN23_ADC1)
    {
        adc_storeVoltage(event->data.adc.v);
    }
    else
    {
        LOG_ERROR("ADC read error");
    }

    return 0;
}

