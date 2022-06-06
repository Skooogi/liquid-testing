/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "channel/bsp_uart.h"

#include "stm32h7xx.h"
// #include "bsp_usart.h"

//Call different stdint for different compilers Document H
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
#include <stdint.h>
extern uint32_t SystemCoreClock;
#endif

//Assert
#define vAssertCalled(char, int) printf("Error:%s,%d\r\n", char, int)
#define configASSERT(x) \
    if ((x) == 0)       \
    vAssertCalled(__FILE__, __LINE__)

/************************************************************************
 *               FreeRTOS Basic configuration options 
 *********************************************************************/
/* Set 1: RTOS uses preemptive scheduler; Set 0: RTOS uses cooperative scheduler (time slice)
 * 
 * Note: in terms of multi task management mechanism, the operating system can be divided into preemptive and cooperative.
 * Cooperative operating system is a task that actively releases the CPU and switches to the next task.
 * The timing of task switching depends entirely on the running task.
 */
#define configUSE_PREEMPTION 0

//1 enable time slice scheduling (default enabled)
#define configUSE_TIME_SLICING 1

/* Some hardware running FreeRTOS has two ways to choose the next task to perform:
 * General methods and hardware specific methods (hereinafter referred to as "special methods").
 * 
 * General method:
 *      1.configUSE_PORT_OPTIMISED_TASK_SELECTION Is 0 or the hardware does not support this special method.
 *      2.It can be used for all FreeRTOS supported hardware
 *      3.Fully implemented in C, the efficiency is slightly lower than that of the special method.
 *      4.It is not mandatory to limit the maximum number of available priorities
 * Special methods:
 *      1.Configure must be_ PORT_ OPTIMISED_ TASK_ Selection is set to 1.
 *      2.Assembly instructions that depend on one or more specific architectures (generally similar to calculating leading zeros [CLZ] instructions).
 *      3.More efficient than general methods
 *      4.Generally, the maximum number of available priorities is forced to be 32
 * If the macro of the MCU is not set to 0, it should be calculated by hardware!
 */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1

/* Set 1: enable low-power tickless mode; Set to 0: keep the system tick interrupt running all the time
 * If low power consumption is turned on, it may cause problems in downloading, because the program is sleeping, the following methods can be used to solve it
 * 
 * Download method:
 *      1.Connect the development version normally
 *      2.Press and hold the reset button and release the reset button at the moment of clicking download
 *     
 *      1.Connect BOOT 0 to high level (3.3V) through jumper cap
 *      2.Power on again and download
 *    
 * 			1.Use FlyMcu to erase the chip and then download it
 *			STMISP -> Clear chip (z)
 */
#define configUSE_TICKLESS_IDLE 0

/*
 * Write the actual CPU core clock frequency, that is, the CPU instruction execution frequency, which is usually called Fclk
 * Fclk In order to supply the clock signal to the CPU core, the main frequency of the CPU is XX MHz,
 * It refers to this clock signal. Accordingly, 1/Fclk is the cpu clock cycle;
 */
#define configCPU_CLOCK_HZ (SystemCoreClock)

//Frequency of RTOS system beat interruption. That is, the number of interrupts in one second. Each interrupt RTOS will schedule tasks
#define configTICK_RATE_HZ ((TickType_t)1000)

//Maximum priority available
#define configMAX_PRIORITIES (32)

//Stack size used by idle tasks
#define configMINIMAL_STACK_SIZE ((unsigned short)128)

//Task name string length
#define configMAX_TASK_NAME_LEN (16)

//The data type of the system beat counter variable. 1 indicates 16 bit unsigned integer and 0 indicates 32-bit unsigned integer
#define configUSE_16_BIT_TICKS 0

//Idle tasks give up CPU usage rights to other user tasks with the same priority
#define configIDLE_SHOULD_YIELD 1

//Enable queue
#define configUSE_QUEUE_SETS 0

//Enable the task notification function, which is enabled by default
#define configUSE_TASK_NOTIFICATIONS 1

//Use mutually exclusive semaphores
#define configUSE_MUTEXES 0

//Use recursive mutex semaphores
#define configUSE_RECURSIVE_MUTEXES 0

//Use count semaphore when is 1
#define configUSE_COUNTING_SEMAPHORES 0

/* Set the semaphores that can be registered and the number of message queues */
#define configQUEUE_REGISTRY_SIZE 10

#define configUSE_APPLICATION_TASK_TAG 0

/*****************************************************************
              FreeRTOS Configuration options related to memory requests                                               
*****************************************************************/
//Support dynamic memory application
#define configSUPPORT_DYNAMIC_ALLOCATION 1
//Support static memory
#define configSUPPORT_STATIC_ALLOCATION 0
//Total heap size of all systems
#define configTOTAL_HEAP_SIZE ((size_t)(10 * 1024))

/***************************************************************
             FreeRTOS Configuration options related to hook functions                                            
**************************************************************/
/* Set 1: use Idle Hook (Idle Hook is similar to callback function); Set to 0: ignore idle hooks
 * 
 * The idle task hook is a function, which is implemented by the user,
 * FreeRTOS Specifies the name and parameters of the function: void vApplicationIdleHook(void),
 * This function is called every idle task cycle
 * For RTOS tasks that have been deleted, idle tasks can free the stack memory allocated to them.
 * Therefore, it must be ensured that idle tasks can be executed by the CPU
 * It is common to use the idle hook function to set the CPU to enter the power saving mode
 * API functions that cause idle tasks to block cannot be called
 */
#define configUSE_IDLE_HOOK 0

/* Set 1: use time slice hook; Set to 0: ignore time slice hook
 * 
 * 
 * The time slice hook is a function, which is implemented by the user,
 * FreeRTOS Specifies the name and parameters of the function: void V applicationtickhook (void)
 * Time slice interrupts can be called periodically
 * Functions must be very short and cannot use a lot of stacks,
 * Cannot call with 'FromISR' or 'from'_ API function at the end of "ISR"
 */
/*xTaskIncrementTick The function is called in the xPortSysTickHandler interrupt function. Therefore, the execution time of the vapplicationtickehook() function must be very short*/
#define configUSE_TICK_HOOK 0

//Use memory request failure hook function
#define configUSE_MALLOC_FAILED_HOOK 0

/*
 * Enables the stack overflow detection function when it is greater than 0. If this function is used 
 * The user must provide a stack overflow hook function, if used
 * This value can be 1 or 2 because there are two stack overflow detection methods */
#define configCHECK_FOR_STACK_OVERFLOW 0

/********************************************************************
          FreeRTOS Configuration options related to runtime and task status collection   
**********************************************************************/
//Enable runtime statistics
#define configGENERATE_RUN_TIME_STATS 0
//Enable visual trace debugging
#define configUSE_TRACE_FACILITY 0
/* Configure with macro_ TRACE_ When facility is 1 at the same time, the following three functions will be compiled
 * prvWriteNameToBuffer()
 * vTaskList(),
 * vTaskGetRunTimeStats()
*/
#define configUSE_STATS_FORMATTING_FUNCTIONS 1

/********************************************************************
                FreeRTOS Configuration options related to collaboration                                                
*********************************************************************/
//Enable the collaboration. After enabling the collaboration, you must add the file croutine c
#define configUSE_CO_ROUTINES 0
//Number of valid priorities of the collaboration
#define configMAX_CO_ROUTINE_PRIORITIES (2)

/***********************************************************************
                FreeRTOS Configuration options related to software timers      
**********************************************************************/
//Enable software timer
#define configUSE_TIMERS 0
//Software timer priority
#define configTIMER_TASK_PRIORITY (configMAX_PRIORITIES - 1)
//Software timer queue length
#define configTIMER_QUEUE_LENGTH 10
//Software timer task stack size
#define configTIMER_TASK_STACK_DEPTH (configMINIMAL_STACK_SIZE * 2)

/************************************************************
            FreeRTOS Optional function configuration options                                                     
************************************************************/
#define INCLUDE_xTaskGetSchedulerState 1
#define INCLUDE_vTaskPrioritySet 1
#define INCLUDE_uxTaskPriorityGet 1
#define INCLUDE_vTaskDelete 1
#define INCLUDE_vTaskCleanUpResources 1
#define INCLUDE_vTaskSuspend 1
#define INCLUDE_vTaskDelayUntil 1
#define INCLUDE_vTaskDelay 1
#define INCLUDE_eTaskGetState 1
#define INCLUDE_xTimerPendFunctionCall 0
//#define INCLUDE_xTaskGetCurrentTaskHandle       1
//#define INCLUDE_uxTaskGetStackHighWaterMark     0
//#define INCLUDE_xTaskGetIdleTaskHandle          0

/******************************************************************
            FreeRTOS Options related to interrupt configuration                                                 
******************************************************************/
#ifdef __NVIC_PRIO_BITS
#define configPRIO_BITS __NVIC_PRIO_BITS
#else
#define configPRIO_BITS 4
#endif
//Interrupt lowest priority
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 15

//Highest interrupt priority that the system can manage
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

#define configKERNEL_INTERRUPT_PRIORITY (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS)) /* 240 */

#define configMAX_SYSCALL_INTERRUPT_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/****************************************************************
            FreeRTOS Configuration options related to interrupt service functions                         
****************************************************************/
#define xPortPendSVHandler PendSV_Handler
#define vPortSVCHandler SVC_Handler

/* The following is what you need to use perceptio tracealyzer. Configure when you don't need it_ TRACE_ Facility is defined as 0 */
#if (configUSE_TRACE_FACILITY == 1)
#include "trcRecorder.h"
#define INCLUDE_xTaskGetCurrentTaskHandle 1 / / enable an optional function (this function is used by the Trace source code. The default value is 0, which means it is not used)
#endif

#endif /* FREERTOS_CONFIG_H */
