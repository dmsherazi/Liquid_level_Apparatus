/*
 * log.c
 *
 *  Created on: 2015/9/9
 *      Author: jk
 */

#include <stdio.h>

#include <eat_interface.h>

#include "log.h"
#include "uart.h"

void log_hex(const char* data, int length)
{
    int i = 0, j = 0;

    print("    ");
    for (i  = 0; i < 16; i++)
    {
        print("%X  ", i);
    }
    print("    ");
    for (i = 0; i < 16; i++)
    {
        print("%X", i);
    }

    print("\r\n");

    for (i = 0; i < length; i += 16)
    {
        print("%02d  ", i % 16 + 1);
        for (j = i; j < 16 && j < length; j++)
        {
            print("%02x ", data[j] & 0xff);
        }
        if (j >= length)
        {
            for (j = 0; j < (16 - length % 16); j++)
            {
                print("   ");
            }
        }
        print("    ");
        for (j = i; j < 16 && j < length; j++)
        {
            if (data[j] < 32)
            {
                print(".");
            }
            else
            {
                print("%c", data[j] & 0xff);
            }
        }

        print("\r\n");
    }
}

void log_binary(const char* data, int length)
{
    char buf[512] = {0};
    int i = 0;

    for (i = 0; i < length && i < 170; i++)
    {
    	sprintf(buf + i * 3, "%02X ", data[i]);
    }
    LOG_INFO("%s", buf);
}
