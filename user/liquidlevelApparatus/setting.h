/*
 * setting.h
 *
 *  Created on: 2015/8/27
 *      Author: jk
 */

#ifndef USER_LIQUIDLEVEL_SETTING_H_
#define USER_LIQUIDLEVEL_SETTING_H_

#include <eat_type.h>

#define MAX_DOMAIN_NAME_LEN 32
typedef enum
{
	ADDR_TYPE_IP,
	ADDR_TYPE_DOMAIN
}ADDR_TYPE;

#define MAX_GATEWAY_ID_LEN 100
typedef struct
{
	//Server configuration
	ADDR_TYPE addr_type;
	union
	{
		u8 domain[MAX_DOMAIN_NAME_LEN];
		u8 ipaddr[4];
	}addr;
	u16 port;

	//Timer configuration
	u32 upload_period;
    u32 heartbeat_period;
    u32 gps_period;
    u32 sample_period;

}SETTING;

typedef struct
{
    eat_bool isGPS;
    float longitude;
    float latitude;
}GPS;

extern SETTING setting;
extern GPS gps;

eat_bool setting_initial(void);
eat_bool setting_save(void);

u32 setting_getHeartbeat(void);

u32 setting_getUploadPeriod(void);
void setting_setuploadPeriod(u32 period);

u32 setting_getSamplePeriod(void);
void setting_setSamplePeriod(u32 period);

u32 setting_getGPSPeriod(void);
void setting_setGPSPeriod(u32 period);


#endif /* USER_LIQUIDLEVEL_SETTING_H_ */
