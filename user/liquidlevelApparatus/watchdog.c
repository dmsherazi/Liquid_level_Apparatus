/*
 * watchdog.c
 *
 *  Created on: 2015/8/27
 *      Author: jk
 */

#include <eat_interface.h>
#include <eat_timer.h>
#include "watchdog.h"
#include "macro.h"

void startWatchdog(void)
{
	eat_bool rc = eat_watchdog_start(WATCHDOG_TIMEOUT,0); //60s, reboot if over time

    if(rc)
    {
        eat_trace("open watchdog success");
    }
    else
    {
        eat_trace("open watchdog fail:%d", rc);
    }
}

void stopWatchdog(void)
{
	eat_bool rc = eat_watchdog_stop();
    if(!rc)
    {
        eat_trace("stop watchdog success");
    }
    else
    {
        eat_trace("stop watchdog fail");
    }
}

void feedWatchdog(void)
{
	eat_bool rc = eat_watchdog_feed();
    if(!rc )
    {
        eat_trace("[%s]feed watchdog success", __FUNCTION__);
    }
    else
    {
        eat_trace("[%s]feed watchdog fail:%d", __FUNCTION__, rc);
    }
}

