/*
 * setting.c
 *
 *  Created on: 2015/8/27
 *      Author: jk
 */
#include "eat_fs_type.h"
#include "eat_fs.h"
#include "eat_fs_errcode.h"
#include "eat_modem.h"
#include "eat_interface.h"
#include "eat_uart.h"
#include "setting.h"
#include "macro.h"
#include "log.h"
#include "cJSON.h"



#define SETITINGFILE_NAME  L"C:\\setting.txt"

SETTING setting =
{
//		ADDR_TYPE_DOMAIN,
//		{
//				"server.xiaoan110.com",
//		},
		ADDR_TYPE_IP,
		{
				SERVER_IP1,
				SERVER_IP2,
				SERVER_IP3,
				SERVER_IP4,
		},
		SERVER_PORT,
		UPLOAD_PERIOD,
		HEARTBEAT,
		GPS_PERIOD,
		SAMPLE_PERIOD//just for example
};

GPS gps;

/*
 * read setting from flash
 */

eat_bool setting_initial(void)
{
    FS_HANDLE fh;
    FS_HANDLE seekRet;
    eat_fs_error_enum fs_Op_ret;
    UINT readLen;

    cJSON_Hooks mem_hooks;

    mem_hooks.malloc_fn = eat_mem_alloc;
    mem_hooks.free_fn = eat_mem_free;

    LOG_DEBUG("setting initial to default value.");

    cJSON_InitHooks(&mem_hooks);//initial the cJSON memory hook

    setting.addr_type = ADDR_TYPE_IP;

    setting.addr.ipaddr[0] = SERVER_IP1;
    setting.addr.ipaddr[1] = SERVER_IP2;
    setting.addr.ipaddr[2] = SERVER_IP3;
    setting.addr.ipaddr[3] = SERVER_IP4;
    setting.port = SERVER_PORT;

    setting.upload_period= UPLOAD_PERIOD;
    setting.heartbeat_period = HEARTBEAT;
    setting.gps_period = GPS_PERIOD;

    fh = eat_fs_Open(SETITINGFILE_NAME, FS_READ_ONLY);
    if (fh < EAT_FS_NO_ERROR)
    {
        LOG_DEBUG("No Setting File");
        return EAT_FALSE;
    }
    else
    {
        seekRet = eat_fs_Seek(fh, 0, EAT_FS_FILE_BEGIN);
        if (0 > seekRet)
        {
            LOG_ERROR("eat_fs_Seek():Seek File Pointer Fail");
            eat_fs_Close(fh);
            return EAT_FALSE;
        }
        else
        {
            eat_trace("eat_fs_Seek():Seek File Pointer Success");
        }

        eat_trace("eat_fs_Open():Create File Success,and FileHandle is %x", fh);

        fs_Op_ret = (eat_fs_error_enum) eat_fs_Read(fh, &setting, sizeof(SETTING), &readLen);
        if (EAT_FS_NO_ERROR != fs_Op_ret)
        {
            LOG_ERROR("eat_fs_Read() Fail,and Return Error: %d,Readlen is %d", fs_Op_ret, readLen);
            eat_fs_Close(fh);
            return EAT_FALSE;
        }
        else
        {
            eat_fs_Close(fh);
        }
    }

    return EAT_TRUE;
}

/*
 * save setting to flash
 */
eat_bool setting_save(void)
{
    FS_HANDLE fh;   //File handle
    eat_fs_error_enum fs_Op_ret;

    UINT writedLen;
    eat_trace("setting_save");
    fh = eat_fs_Open(SETITINGFILE_NAME, FS_CREATE_ALWAYS | FS_READ_WRITE);

    if (fh < EAT_FS_NO_ERROR)
    {
        eat_trace("eat_fs_Open():Create File Fail,and Return Error is %x", fh);
        return EAT_FALSE;
    }
    else
    {
        eat_trace("eat_fs_Open():Create File Success,and FileHandle is %x", fh);
        fs_Op_ret = (eat_fs_error_enum) eat_fs_Write(fh, &setting, sizeof(SETTING), &writedLen);
        if (EAT_FS_NO_ERROR != fs_Op_ret)
        {
            eat_trace(
                    "eat_fs_Write():eat_fs_Write File Fail,and Return Error is %d,Readlen is %d",
                    fs_Op_ret, writedLen);
            eat_fs_Close(fh);
            return EAT_FALSE;
        }
        else
        {
            eat_trace("eat_fs_Write:eat_fs_Write File Success");
            eat_fs_Close(fh);
        }
    }

    return EAT_TRUE;
}

u32 setting_getUploadPeriod()
{
	return setting.upload_period;
}

void setting_setuploadPeriod(u32 period)
{
	setting.upload_period = period;
}

u32 setting_getSamplePeriod()
{
	return setting.sample_period;
}

void setting_setSamplePeriod(u32 period)
{
	setting.sample_period = period;
}

u32 setting_getHeartbeat()
{
    return setting.heartbeat_period;
}

u32 setting_getGPSPeriod()
{
    return setting.gps_period;
}

void setting_setGPSPeriod(u32 period)
{
	setting.gps_period= period;
}

