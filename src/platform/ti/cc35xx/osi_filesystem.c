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
#include <osi_kernel.h>

#ifdef RAM_CONTAINER
#include "containers.h"
#endif

/*!
    \brief the below functions are needed for file management of the upper mac.
           currently only supported file is the WiFi part firmware bin file.
           in the sitara AM243x launchpad, the firmware bin is saved in a specific flash offset.

           those functions are redirected from fxxx to osi_fxxx in the linker command (--symbol_map=fread=osi_fread).
           if already have system that use fopen fclose fread etc. this file should not be compiled in.
*/
#define ATTRIBUTE __attribute__ ((used))
#define FW_OFFSET_IN_FLASH (0x800000)
#define RAM_OFFSET_IN_FLASH (0x900000)

int ATTRIBUTE osi_fclose(FILE *_fp)
{
    return 0;
}


FILE * ATTRIBUTE osi_fopen(const char *_fname, const char *_mode)
{
#ifdef RAM_CONTAINER
    if(strcmp("rambtlr",_fname) == 0)
    {
        return (FILE *)(NULL);
    }
    else if(strcmp("fw",_fname) == 0)
    {
        return (FILE *)(gFWbuffer);
    }
    else if(strcmp("cc33xx-conf",_fname) == 0)
    {
        return (FILE *)(gINIbuffer);
    }
    return (FILE *)(NULL);

#else
    if(strcmp("rambtlr",_fname) == 0)
    {
        return (FILE *)(RAM_OFFSET_IN_FLASH);
    }
    else if(strcmp("fw",_fname) == 0)
    {
        return (FILE *)(FW_OFFSET_IN_FLASH);
    }
    return (FILE *)(NULL);
#endif
}

size_t ATTRIBUTE osi_fread(void *_ptr, size_t len, size_t offset, FILE *_fp)
{
#ifdef RAM_CONTAINER
    memcpy(_ptr,(void *)((uint32_t)(_fp) + offset),len);
    return len;
#else
    if(Flash_read(gFlashHandle[CONFIG_FLASH0], (size_t)(_fp) + offset, _ptr, len) == 0)
    {
        return(len);
    }
    else
    {
        return 0;
    }
#endif
}

size_t osi_fwrite(const void *_ptr, size_t _size, size_t _count, register FILE *_fp)//
{
    assert(0);

    return 0;
}


/*!
    \brief initialize the SPI module
    \param data - input buffer
    \param length - length in bytes to read
    \return 0 - success, -1 - failed
    \note
    1. if initializing all the MCU drivers at the beginning of the world, this function can stay empty.
       this function is called in the wlan_start and will stay initialized even if used wlan_stop
    2. the SPI must have the following parameters
       - master
       - 4 pin mode SPI
       - chip select active low
       - polarity 0 phase 0
       - max frequency 40000000Hz
       - data block 32bits
    \warning
*/
size_t ATTRIBUTE osi_filelength(const char * FileName)
{
    return 0;
}
//

