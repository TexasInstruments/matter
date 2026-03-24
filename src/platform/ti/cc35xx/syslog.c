/*
 * Copyright (c) 2024, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdarg.h>
#include "osi_kernel.h"
#include "errors.h"
#include "syslog.h"

#include "ti/drivers/UART2.h"

UART2_Handle logger = NULL;

#define SYSLOG_STRING_LIMIT 256


int32 logger_write(char *buf, uint32 len);


char* inner_report(const char *pcFormat, va_list arg)
{
    int iRet = 0;
    char * pcTemp;
    char * pcBuff;
    int iSize = 256;

    pcBuff = (char *) os_zalloc(iSize);
    if (pcBuff == NULL)
    {
        return NULL;
    }

    while (1)
    {
        iRet = vsnprintf(pcBuff, iSize, pcFormat, arg);

        if ((iRet > -1) && (iRet < iSize))
        {
            break;
        }
        else
        {
            iSize *= 2;
            if ((pcTemp = os_realloc(pcBuff, iSize)) == NULL)
            {
                os_free(pcBuff);
                return NULL;
            }
            else
            {
                memset(pcTemp, 0, iSize);
                pcBuff = pcTemp;
            }
        }
    }

    return pcBuff;
}

int32 logger_write(char * buf, uint32 len)
{
    if (len == 0)
    {
        return len;
    }
    buf[len - 1] = '\0';
#if UART_1
    if (logger == NULL)
    {
        UART2_Params params;
        UART2_Params_init(&params);
        params.baudRate       = 115200;
        params.readReturnMode = UART2_ReadReturnMode_FULL;
        params.readMode       = UART2_Mode_BLOCKING;
        params.writeMode      = UART2_Mode_BLOCKING;
        logger                = UART2_open(1, &params);
        if (logger == NULL)
        {
            while (1)
            {
            }
        }
    }

    uint32 written = 0;
    UART2_write(logger, buf, len, &written);

    return written;

#else
    Report("\n\r");
    Report(buf);
    return len;
#endif
}

void openlog(const char * ident, int option, int facility) {}

void syslog(int priority, const char * format, ...)
{
    va_list list;
    va_start(list, format);
    vsyslog(priority, format, list);
    va_end(list);
}

void closelog(void) {}

void vsyslog(int priority, const char * format, va_list ap)
{
    char * pcBuff = inner_report(format, ap);
    if (!pcBuff)
    {
        return;
    }
    char newLine[] = "\n\r";

    logger_write(pcBuff, MIN(strlen(pcBuff), SYSLOG_STRING_LIMIT));
    logger_write(newLine, sizeof(newLine) - 1);
    os_free(pcBuff);
}

void gtrace_report(int group, char * fmt, ...)
{
    va_list list;
    va_start(list, fmt);
    char * pcBuff = inner_report(fmt, list);
    va_end(list);

    if (!pcBuff)
    {
        return;
    }

    char newLine[] = "\n\r";

    logger_write(pcBuff, MIN(strlen(pcBuff), SYSLOG_STRING_LIMIT));
    logger_write(newLine, sizeof(newLine) - 1);

    os_free(pcBuff);
}
