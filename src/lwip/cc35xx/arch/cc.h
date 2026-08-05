/*
 *    Copyright (c) 2020 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/*
 *
 *    Description:
 *      This file defines processor-architecture- and toolchain-
 *      specific constants and types required for building
 *      LwIP against FreeRTOS.
 *
 */
#ifndef LWIP_ARCH_CC_H
#define LWIP_ARCH_CC_H

/* std.h functions required */
#include <stdint.h>

/* Disable lwIP's private definition of 'struct timeval' */
//#define LWIP_TIMEVAL_PRIVATE 0
#include <sys/select.h>

/* Define byte order of the system */
//#define BYTE_ORDER LITTLE_ENDIAN

/* Use lwip provided errors as ti compiler is too granular*/
#define LWIP_PROVIDE_ERRNO  1

uint32_t cc35xx_lwip_rand();
#define LWIP_RAND() ((u32_t) cc35xx_lwip_rand())

/* Setup Packing Macros */
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __attribute__ ((__packed__))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

/* Different handling for unit test, normally not needed */
#ifdef LWIP_NOASSERT_ON_ERROR
#define LWIP_ERROR(message, expression, handler) do { if (!(expression)) { \
  handler;}} while(0)
#endif

struct sio_status_s;
typedef struct sio_status_s sio_status_t;
#define sio_fd_t sio_status_t*
#define __sio_fd_t_defined

typedef uint32_t sys_prot_t;

// Place LwIP pools into their own subsections of .bss to make it easier to see
// their sizes in the linker map file.
extern uint8_t __attribute__((section(".bss.lwip_ND6_QUEUE"))) memp_memory_ND6_QUEUE_base[];
extern uint8_t __attribute__((section(".bss.lwip_IP6_REASSDATA"))) memp_memory_IP6_REASSDATA_base[];
extern uint8_t __attribute__((section(".bss.lwip_RAW_PCB"))) memp_memory_RAW_PCB_base[];
extern uint8_t __attribute__((section(".bss.lwip_TCP_SEG"))) memp_memory_TCP_SEG_base[];
extern uint8_t __attribute__((section(".bss.lwip_PBUF_POOL"))) memp_memory_PBUF_POOL_base[];
extern uint8_t __attribute__((section(".bss.lwip_FRAG_PBUF"))) memp_memory_FRAG_PBUF_base[];
extern uint8_t __attribute__((section(".bss.lwip_PBUF"))) memp_memory_PBUF_base[];
extern uint8_t __attribute__((section(".bss.lwip_TCP_PCB_LISTEN"))) memp_memory_TCP_PCB_LISTEN_base[];
extern uint8_t __attribute__((section(".bss.lwip_REASSDATA"))) memp_memory_REASSDATA_base[];
extern uint8_t __attribute__((section(".bss.lwip_UDP_PCB"))) memp_memory_UDP_PCB_base[];
extern uint8_t __attribute__((section(".bss.lwip_MLD6_GROUP"))) memp_memory_MLD6_GROUP_base[];
extern uint8_t __attribute__((section(".bss.lwip_IGMP_GROUP"))) memp_memory_IGMP_GROUP_base[];
extern uint8_t __attribute__((section(".bss.lwip_TCP_PCB"))) memp_memory_TCP_PCB_base[];
extern uint8_t __attribute__((section(".bss.lwip_SYS_TIMEOUT"))) memp_memory_SYS_TIMEOUT_base[];

#endif /* LWIP_ARCH_CC_H */
