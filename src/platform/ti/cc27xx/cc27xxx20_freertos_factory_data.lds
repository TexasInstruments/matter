/*
 * Copyright (c) 2021-2024, Texas Instruments Incorporated
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

INCLUDE ti_utils_build_linker.cmd.genmap

STACKSIZE = 3096;


/*0x18000 HSM FW Size*/

/* FLASH END WITH HSM and Factory Data: 0x1E7000*/
/* Factory data start: 0x1E7000 */
/*Factory data length: 0x1000 */
/* NVS Start 0x1E1000*/

MEMORY
{


    /* If device has HSM FW the last part of Flash is reserved for it */
    FLASH (RX) : ORIGIN = ti_utils_build_GenMap_FLASH0_BASE, LENGTH = 0x1E7000

    FLASH_FACTORY_DATA (R) : ORIGIN = 0x1E7000, LENGTH = 0x1000
    /*
     * Customer Configuration Area and Bootloader Backdoor configuration in
     * flash.
     */
    CCFG (R)   : ORIGIN = ti_utils_build_GenMap_CCFG_BASE, LENGTH = ti_utils_build_GenMap_CCFG_SIZE
    SRAM (RWX) : ORIGIN = ti_utils_build_GenMap_RAM0_BASE, LENGTH = ti_utils_build_GenMap_RAM0_SIZE

    /* Security Configuration Area (SCFG) - if device supports it */
    SCFG (R)    : ORIGIN = DEFINED(ti_utils_build_GenMap_SCFG_BASE) ? ti_utils_build_GenMap_SCFG_BASE : 0x0, LENGTH = DEFINED(ti_utils_build_GenMap_SCFG_SIZE) ? ti_utils_build_GenMap_SCFG_SIZE : 0x0

    /* S2RRAM - if device supports it.  Intended for the S2R radio module, but
     * it can also be used by the application with some limitations. Please
     * refer to the s2rram example.
     */
    S2RRAM (RW) : ORIGIN = DEFINED(ti_utils_build_GenMap_S2RRAM_BASE) ? ti_utils_build_GenMap_S2RRAM_BASE : 0x0, LENGTH = DEFINED(ti_utils_build_GenMap_S2RRAM_SIZE) ? ti_utils_build_GenMap_S2RRAM_SIZE : 0x0

    /* Explicitly placed off target for the storage of logging data.
     * The data placed here is NOT loaded onto the target device.
     * This is part of 1 GB of external memory from 0x60000000 - 0x9FFFFFFF.
     * ARM memory map can be found here:
     * https://developer.arm.com/documentation/ddi0337/e/memory-map/about-the-memory-map
     */
    LOG_DATA (R) : ORIGIN = 0x90000000, LENGTH = 0x40000
    LOG_PTR  (R) : ORIGIN = 0x94000008, LENGTH = 0x40000
}

REGION_ALIAS("REGION_TEXT", FLASH);
REGION_ALIAS("REGION_FACTORY_DATA", FLASH_FACTORY_DATA);
REGION_ALIAS("REGION_BSS", SRAM);
REGION_ALIAS("REGION_DATA", SRAM);
REGION_ALIAS("REGION_STACK", SRAM);
REGION_ALIAS("REGION_HEAP", SRAM);
REGION_ALIAS("REGION_LOG", LOG_DATA);
REGION_ALIAS("REGION_ARM_EXIDX", FLASH);
REGION_ALIAS("REGION_ARM_EXTAB", FLASH);

SECTIONS {

    PROVIDE (_resetVecs_base_address =
        DEFINED(_resetVecs_base_address) ? _resetVecs_base_address : 0x0);

    .resetVecs (_resetVecs_base_address) : AT (_resetVecs_base_address) {
        KEEP (*(.resetVecs))
    } > REGION_TEXT

    .ramVecs (NOLOAD) : ALIGN(256) {
        KEEP (*(.ramVecs)) /* Section from interrupt.c (driverlib) */
    } > REGION_DATA

    /* Define base address for the DAC arrays and struct */
    PROVIDE (_factory_data_base_address =
        DEFINED(_factory_data_base_address) ? _factory_data_base_address : 0x1E7000);

    .factory_data(_factory_data_base_address) : AT (_factory_data_base_address) {
        KEEP (*(.factory_data_struct))
    } > REGION_FACTORY_DATA 

    /*
     * UDMALPF3_CONFIG_BASE below must match UDMALPF3_CONFIG_BASE defined
     * by ti/drivers/dma/UDMALPF3.h
     * The user is allowed to change UDMALPF3_CONFIG_BASE to move it away from
     * the default address 0x2000_0400, but remember it must be 1024 bytes aligned.
     */
    UDMALPF3_CONFIG_BASE = 0x20000400;

    /*
     * Define absolute addresses for the DMA channels.
     * DMA channels must always be allocated at a fixed offset from the DMA base address.
     * --------- DO NOT MODIFY -----------
     */
    DMA_CHANNEL0_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x000);
    DMA_CHANNEL1_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x010);
    DMA_CHANNEL2_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x020);
    DMA_CHANNEL3_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x030);
    DMA_CHANNEL4_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x040);
    DMA_CHANNEL5_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x050);
    DMA_CHANNEL6_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x060);
    DMA_CHANNEL7_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x070);
    DMA_CHANNEL8_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x080);
    DMA_CHANNEL9_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x090);
    DMA_CHANNEL10_CONTROL_TABLE_ENTRY_ADDRESS = (UDMALPF3_CONFIG_BASE + 0x0A0);
    DMA_CHANNEL11_CONTROL_TABLE_ENTRY_ADDRESS = (UDMALPF3_CONFIG_BASE + 0x0B0);

    /* 9-16 DMA channels => the alt. control table is at offset 0x100 */
    DMA_CHANNEL0_ALT_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x100);
    DMA_CHANNEL1_ALT_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x110);
    DMA_CHANNEL2_ALT_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x120);
    DMA_CHANNEL3_ALT_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x130);
    DMA_CHANNEL4_ALT_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x140);
    DMA_CHANNEL5_ALT_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x150);
    DMA_CHANNEL6_ALT_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x160);
    DMA_CHANNEL7_ALT_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x170);
    DMA_CHANNEL8_ALT_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x180);
    DMA_CHANNEL9_ALT_CONTROL_TABLE_ENTRY_ADDRESS  = (UDMALPF3_CONFIG_BASE + 0x190);
    DMA_CHANNEL10_ALT_CONTROL_TABLE_ENTRY_ADDRESS = (UDMALPF3_CONFIG_BASE + 0x1A0);
    DMA_CHANNEL11_ALT_CONTROL_TABLE_ENTRY_ADDRESS = (UDMALPF3_CONFIG_BASE + 0x1B0);


    /*
     * Allocate Channel n descriptors at absolute addresses.
     * --------- DO NOT MODIFY -----------
     */
    UDMALPF3_channel0ControlTableEntry_is_placed = 0;
    .dmaChannel0ControlTableEntry DMA_CHANNEL0_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL0_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel0ControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel1ControlTableEntry_is_placed = 0;
    .dmaChannel1ControlTableEntry DMA_CHANNEL1_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL1_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel1ControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel2ControlTableEntry_is_placed = 0;
    .dmaChannel2ControlTableEntry DMA_CHANNEL2_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL2_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel2ControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel3ControlTableEntry_is_placed = 0;
    .dmaChannel3ControlTableEntry DMA_CHANNEL3_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL3_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel3ControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel4ControlTableEntry_is_placed = 0;
    .dmaChannel4ControlTableEntry DMA_CHANNEL4_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL4_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel4ControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel5ControlTableEntry_is_placed = 0;
    .dmaChannel5ControlTableEntry DMA_CHANNEL5_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL5_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel5ControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel6ControlTableEntry_is_placed = 0;
    .dmaChannel6ControlTableEntry DMA_CHANNEL6_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL6_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel6ControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel7ControlTableEntry_is_placed = 0;
    .dmaChannel7ControlTableEntry DMA_CHANNEL7_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL7_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel7ControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel8ControlTableEntry_is_placed = 0;
    .dmaChannel8ControlTableEntry DMA_CHANNEL8_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL8_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel8ControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel9ControlTableEntry_is_placed = 0;
    .dmaChannel9ControlTableEntry DMA_CHANNEL9_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL9_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel9ControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel10ControlTableEntry_is_placed = 0;
    .dmaChannel10ControlTableEntry DMA_CHANNEL10_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD) : AT (DMA_CHANNEL10_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel10ControlTableEntry)} > REGION_DATA
    UDMALPF3_channel11ControlTableEntry_is_placed = 0;
    .dmaChannel11ControlTableEntry DMA_CHANNEL11_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD) : AT (DMA_CHANNEL11_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel11ControlTableEntry)} > REGION_DATA

    UDMALPF3_channel0AltControlTableEntry_is_placed = 0;
    .dmaChannel0AltControlTableEntry DMA_CHANNEL0_ALT_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL0_ALT_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel0AltControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel1AltControlTableEntry_is_placed = 0;
    .dmaChannel1AltControlTableEntry DMA_CHANNEL1_ALT_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL1_ALT_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel1AltControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel2AltControlTableEntry_is_placed = 0;
    .dmaChannel2AltControlTableEntry DMA_CHANNEL2_ALT_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL2_ALT_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel2AltControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel3AltControlTableEntry_is_placed = 0;
    .dmaChannel3AltControlTableEntry DMA_CHANNEL3_ALT_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL3_ALT_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel3AltControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel4AltControlTableEntry_is_placed = 0;
    .dmaChannel4AltControlTableEntry DMA_CHANNEL4_ALT_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL4_ALT_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel4AltControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel5AltControlTableEntry_is_placed = 0;
    .dmaChannel5AltControlTableEntry DMA_CHANNEL5_ALT_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL5_ALT_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel5AltControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel6AltControlTableEntry_is_placed = 0;
    .dmaChannel6AltControlTableEntry DMA_CHANNEL6_ALT_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL6_ALT_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel6AltControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel7AltControlTableEntry_is_placed = 0;
    .dmaChannel7AltControlTableEntry DMA_CHANNEL7_ALT_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL7_ALT_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel7AltControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel8AltControlTableEntry_is_placed = 0;
    .dmaChannel8AltControlTableEntry DMA_CHANNEL8_ALT_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL8_ALT_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel8AltControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel9AltControlTableEntry_is_placed = 0;
    .dmaChannel9AltControlTableEntry DMA_CHANNEL9_ALT_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD)   : AT (DMA_CHANNEL9_ALT_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel9AltControlTableEntry)}   > REGION_DATA
    UDMALPF3_channel10AltControlTableEntry_is_placed = 0;
    .dmaChannel10AltControlTableEntry DMA_CHANNEL10_ALT_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD) : AT (DMA_CHANNEL10_ALT_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel10AltControlTableEntry)} > REGION_DATA
    UDMALPF3_channel11AltControlTableEntry_is_placed = 0;
    .dmaChannel11AltControlTableEntry DMA_CHANNEL11_ALT_CONTROL_TABLE_ENTRY_ADDRESS (NOLOAD) : AT (DMA_CHANNEL11_ALT_CONTROL_TABLE_ENTRY_ADDRESS) {*(.dmaChannel11AltControlTableEntry)} > REGION_DATA

    /* if a ROM-only symbol is present, then ROM is being used.
     * Reserve memory for surgically placed config constants.
     */
    _rom_rodata_start = 0x2000;
    _rom_rodata_size = DEFINED(ROM_RODATA_SIZE) ? 0 : DEFINED(ROM_RODATA_SIZE_NO_OAD) ? 0x330 : 0;

    .rom_rodata_reserve (_rom_rodata_start): {
        . += _rom_rodata_size;
    } > REGION_TEXT AT> REGION_TEXT

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
        *(.rodata.*)
        *(.rodata_*)
        KEEP (*(.timestampPFormat))
    } > REGION_TEXT AT> REGION_TEXT

    .data : ALIGN(4) {
        __data_load__ = LOADADDR (.data);
        __data_start__ = .;
        *(.data)
        *(.data.*)
        . = ALIGN (2);
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

    /* 12 pages of NV Memory (0x800 each)  */
    .nvs (0x1E1000) (NOLOAD) : AT (0x1E1000) ALIGN(0x800) {  
        *(.nvs)
    } > REGION_TEXT

    .ccfg : {
        KEEP (*(.ccfg))
    } > CCFG AT> CCFG

    .scfg :
    {
        KEEP(*(.scfg));
    } > SCFG

    .bss : {
        __bss_start__ = .;
        *(.shbss)
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN (4);
        __bss_end__ = .;
    } > REGION_BSS AT> REGION_BSS

    /* Placing the section .s2rram in S2RRAM region. Only uninitialized
     * objects may be placed in this section.
     */
    .s2rram (NOLOAD) : {
        *(.s2rram)
    } > S2RRAM AT> S2RRAM

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

    .log_data (COPY) : {
        KEEP (*(.log_data))
    } > REGION_LOG
    .log_ptr (COPY) : {
        KEEP (*(.log_ptr*))
    } > LOG_PTR
}

ENTRY(resetISR)
