/*
 * Copyright (c) 2022-2024, Texas Instruments Incorporated
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

STACKSIZE = 0x1000;

MEMORY
{
    /* Application stored in and executes from external flash */
    FLASH_META (RX ) : ORIGIN = 0x10001000, LENGTH = 0x0000001C /* Reserved for Bootloader */
    FLASH_HDR  (RX ) : ORIGIN = 0x1000101C, LENGTH = 0x000000E4
/*  FLASH      (RX ) : ORIGIN = 0x10001100, LENGTH = 0x0003EF00*//*~256 KB */
    FLASH      (RX ) : ORIGIN = 0x10001100, LENGTH = 0x03FFEEFF /*~4   MB */

    /* Application uses internal C(ode)RAM for code/data */
    CRAM_TCM   (RWX) : ORIGIN = 0x00000000, LENGTH = 0x00008000 /* 32  KB */
    CRAM       (RWX) : ORIGIN = 0x08000000, LENGTH = 0x00010000 /* 64  KB */

    /* Application uses internal D(ata)RAM for data */
    DRAM_TCM   (RWX) : ORIGIN = 0x20000000, LENGTH = 0x00020000 /* 128 KB */
    DRAM       (RWX) : ORIGIN = 0x28000000, LENGTH = 0x00045000 /* 276 KB */

    MEM_POOL   (RW ) : ORIGIN = 0x28045000, LENGTH = 0x00003000 /* 12  KB */

    /* Other memory regions */
    PERIPH_API (RW ) : ORIGIN = 0x45602000, LENGTH = 0x0000001F
    PHY_CTX    (RW ) : ORIGIN = 0x45900000, LENGTH = 0x00010000
    PHY_SCR    (RW ) : ORIGIN = 0x45910000, LENGTH = 0x00004800
    DB_MEM     (RW ) : ORIGIN = 0x45A80000, LENGTH = 0x0000FFFF
    CPERAM     (RWX) : ORIGIN = 0x45C00000, LENGTH = 0x00010000 /* 64K  PROGRAM MEMORY  */
    MCERAM     (RWX) : ORIGIN = 0x45C80000, LENGTH = 0x00001000 /* 4K   PROGRAM MEMORY   */
    RFERAM     (RWX) : ORIGIN = 0x45CA0000, LENGTH = 0x00001000 /* 4K   PROGRAM MEMORY   */
    MDMRAM     (RWX) : ORIGIN = 0x45CC0000, LENGTH = 0x00000100 /* 256B PROGRAM MEMORY */

    /* Explicitly placed off target for the storage of logging data.
     * The data placed here is NOT loaded onto the target device.
     * This is part of 1 GB of external memory from 0x60000000 - 0x9FFFFFFF.
     * ARM memory map can be found here:
     * https://developer.arm.com/documentation/ddi0337/e/memory-map/about-the-memory-map
     */

    LOG_DATA   (R  ) : ORIGIN = 0x90000000, LENGTH = 0x00040000 /* 256 KB */
    LOG_PTR    (R  ) : ORIGIN = 0x94000008, LENGTH = 0x00040000 /* 256 KB */
}

REGION_ALIAS("REGION_HDR", FLASH_HDR);
REGION_ALIAS("REGION_TEXT", FLASH);
REGION_ALIAS("REGION_BSS", DRAM);
REGION_ALIAS("REGION_NVM", DRAM_TCM);
REGION_ALIAS("REGION_CONNECTIVITY", DRAM);
REGION_ALIAS("REGION_DATA", DRAM_TCM);
REGION_ALIAS("REGION_STACK", DRAM_TCM);
REGION_ALIAS("REGION_HEAP", DRAM_TCM);
REGION_ALIAS("REGION_LOG", LOG_DATA);
REGION_ALIAS("REGION_ARM_EXIDX", FLASH);
REGION_ALIAS("REGION_ARM_EXTAB", FLASH);
REGION_ALIAS("REGION_TI_RAMFUNC", CRAM_TCM);

SECTIONS {

 /*   GROUP { */
        /* The first flash sector (4KB bytes)
         * and the first 0x1C bytes of the second flash sector is reserved for
         * metadata used by the bootloader. The interrupt vectors must be 512B
         * aligned so the vector table must be at 0x10000000 + 0x1000 + 0x200 = 
         * 0x10001200
         * TODO: Incorrect alignment of 256B resulting in 0x10001100 is
         * currently used, to match what the bootloader currently expects. This
         * needs to be fixed when the bootloader uses the correct alignment.
         * See TIDRIVERS-6649.
         * The bytes from the end of bootloader metadata to the vector table
         * (0x1000101C to 0x100010FF, both inlcuded) must be filled with 0xFF.
         */
/*        .reserved:                   { . += 0x101C; } (NOLOAD) *//* Reserved for bootloader metadata */
/*        .padding : fill = 0xFFFFFFFF { . += 0x00E4; } */
/*        .resetVecs:                  {} PALIGN(4) */
/*    } > FLASH_BASE */

    .resetPad (0x1000101C) : {
        FILL(0xFF);
        _padStart = .;
        /* Need something in the output section otherwise uninitialized (NOBITS) */
        BYTE(0xFF);
        . += (0xE4 - 0x01);
        _padEnd = .;
    } > REGION_HDR

    PROVIDE (_resetVecs_base_address =
        DEFINED(_resetVecs_base_address) ? _resetVecs_base_address : 0x10001100);

    .resetVecs (_resetVecs_base_address) : AT (_resetVecs_base_address) {
        KEEP (*(.resetVecs))
    } > REGION_TEXT

    .ramVecs (NOLOAD) : ALIGN(1024) {
        KEEP (*(.ramVecs))
    } > REGION_DATA

    .text : {
        CREATE_OBJECT_SYMBOLS
        *(.text)
        *(.text.*)
        . = ALIGN(0x4);
        KEEP (*(.ctors))
        . = ALIGN(0x4);
        KEEP (*(.dtors))
        . = ALIGN(0x4);
        __init_array_start = .;
        KEEP (*(.init_array*))
        __init_array_end = .;
        *(.init)
        *(.fini*)
    } > REGION_TEXT AT> REGION_TEXT

    PROVIDE (__etext = .);
    PROVIDE (_etext = .);
    PROVIDE (etext = .);

    .rodata : {
        *(.rodata)
        *(.rodata*)
    } > REGION_TEXT AT> REGION_TEXT

    .data : ALIGN (4) {
        __data_load__ = LOADADDR (.data);
        __data_start__ = .;
        *(.data)
        *(.data*)
        *(.TI.ramfunc)
        . = ALIGN (4);
        __data_end__ = .;
    } > REGION_DATA AT> REGION_TEXT

    .ARM.exidx : {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > REGION_ARM_EXIDX AT> REGION_ARM_EXIDX

    .ARM.extab : {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > REGION_ARM_EXTAB AT> REGION_ARM_EXTAB

    .nvm : {
        *(.bss.xmemDataToWriteBuffer)
        *(.bss.dataToWrite)
        *(.bss.dataRead)
        *(.bss.verifyBuffer)
        . = ALIGN (4);
    } > REGION_NVM AT> REGION_NVM
    
    .bss : {
        __bss_start__ = .;
        *(.shbss)
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN (4);
        __bss_end__ = .;
    } > REGION_BSS AT> REGION_BSS

    .heap : {
        __heap_start__ = .;
        end = __heap_start__;
        _end = end;
        __end = end;
        KEEP(*(.heap))
        __heap_end__ = .;
        __HeapLimit = __heap_end__;
    } > REGION_HEAP AT> REGION_HEAP

    .stack (NOLOAD) : ALIGN(0x8) {
        _stack = .;
        __stack = .;
        KEEP(*(.stack))
        . += STACKSIZE;
        _stack_end = .;
        __stack_end = .;
    } > REGION_STACK AT> REGION_STACK

    .connectivity_shared_status (0x2803FF00) : ALIGN(0x4) {
        KEEP (*(.connectivity_shared_status_section))
    } > REGION_CONNECTIVITY AT> REGION_TEXT

    .ti_ramfunc : ALIGN(0x4) { 
        KEEP (*(.TI.ramfunc))  
    } > REGION_TI_RAMFUNC AT> REGION_TEXT 

    .log_data (COPY) : {
        KEEP (*(.log_data))
    } > REGION_LOG
    .log_ptr (COPY) : {
        KEEP (*(.log_ptr*))
    } > LOG_PTR
}

ENTRY(resetISR)