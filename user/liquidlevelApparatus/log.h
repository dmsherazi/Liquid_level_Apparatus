/*
 * log.h
 *
 *  Created on: 2015/8/27
 *      Author: jk
 */

#ifndef USER_LIQUIDLEVEL_LOG_H_
#define USER_LIQUIDLEVEL_LOG_H_

#include <eat_interface.h>

#include "rtc.h"

#ifdef APP_DEBUG
#define LOG_DEBUG(fmt, ...) eat_trace("%ld[%d][DBG][%s:%d %s]"fmt, rtc_getTimestamp(), eat_get_task_id(), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) eat_trace("%ld[%d][INF][%s:%d %s]"fmt, rtc_getTimestamp(), eat_get_task_id(), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) eat_trace("%ld[%d][ERR][%s:%d %s]"fmt, rtc_getTimestamp(), eat_get_task_id(), __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_HEX(data, len)  log_hex(data, len);
#define LOG_BIN(data, len)  log_binary(data, len);
#else
#define LOG_DEBUG(fmt, ...)
#define LOG_INFO(fmt, ...)
#define LOG_ERROR(fmt, ...)
#define LOG_HEX(data, len)
#define LOG_BIN(data, len)
#endif


void log_hex(const char* data, int length);
void log_binary(const char* data, int length);


#endif /* USER_LIQUIDLEVEL_LOG_H_ */
