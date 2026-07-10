
/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *    All rights reserved.
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

#include <exception/Exception.h>

/* FreeRTOS handlers */
extern void SVC_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);

/* Custom handler functions */

/* The entry point for the application startup code */
extern void resetISR(void);

/* Linker variables that marks the top and bottom of the stack */
extern void *__stack;
extern unsigned long _stack_end;

/* Initial vector table definition */
__attribute__((section(".resetVecs"), used)) void (*const resetVectors[])(void) = {
    /* The initial stack pointer */
    (void (*)(void))((uint32_t)&_stack_end),
    /* The reset handler */
    resetISR,
    /* The NMI handler */
    Exception_handlerSpin,
    /* The hard fault handler */
    Exception_handlerSpin,
    /* The MPU fault handler */
    Exception_handlerSpin,
    /* The bus fault handler */
    Exception_handlerSpin,
    /* The usage fault handler */
    Exception_handlerSpin,
    /* The secure fault handler */
    Exception_handlerSpin,
    /* Reserved */
    Exception_handlerSpin,
    /* Reserved */
    Exception_handlerSpin,
    /* Reserved */
    Exception_handlerSpin,
    /* SVCall handler */
    SVC_Handler,
    /* Debug monitor handler */
    Exception_handlerSpin,
    /* Reserved */
    Exception_handlerSpin,
    /* The PendSV handler */
    PendSV_Handler,
    /* The SysTick handler */
    SysTick_Handler,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
};

