/*
 * macro.h
 *
 *  Created on: 2015Äê8ÔÂ29ÈÕ
 *      Author: jk
 */

#ifndef USER_LIQUIDLEVEL_MACRO_H_
#define USER_LIQUIDLEVEL_MACRO_H_


#define WATCHDOG_TIMEOUT (60000)	// 1 minutes

#define WATCHDOG_FEED (WATCHDOG_TIMEOUT / 2)

#define SERVER_IP1 118
#define SERVER_IP2 89
#define SERVER_IP3 104
#define SERVER_IP4 130
#define SERVER_PORT 9880

#define HEARTBEAT (3 * 60 * 1000)	/* 3 minutes */

#define GPS_PERIOD (5 * 1000)
#define UPLOAD_PERIOD (10 * 1000)
#define SAMPLE_PERIOD 500

#endif /* USER_LIQUIDLEVEL_MACRO_H_ */
