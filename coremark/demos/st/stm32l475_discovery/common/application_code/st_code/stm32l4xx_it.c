/**
  ******************************************************************************
  * @file    Cloud/AWS/Src/stm32l4xx_it.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    12-April-2017
  * @brief   Interrupt service routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "stm32l4xx_it.h"
#include "FreeRTOSConfig.h"

extern void xPortSysTickHandler( void );

/* External variables --------------------------------------------------------*/

extern TIM_HandleTypeDef htim6;

/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
}

/**
* @brief This function handles Hard fault interrupt.
*/
/*
void HardFault_Handler(void)
{
  while (1)
  {
  }
}
*/

/**
* @brief This function handles Memory management fault.
*/
//void MemManage_Handler(void)
//{
//  while (1)
//  {
//  }
//}

/**
* @brief This function handles Prefetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  while (1)
  {
  }
}

//void prvKageUsageHandler( void )
//{
//
//}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
//	/* Assumes psp was in use. */
//		__asm volatile
//		(
//				// r0: stack pointer that contains processor state
//				// r1: original basepri value
//				// r2: shadow stack of r0
//				// r3: temp register to hold values
//
//	#ifdef EXCEPTION_NEW_MICRO_BENCHMARK
//				// Setup dummy lr
//				"	push {lr}						\n"
//				"	ldr r14, =0xfffffffd			\n"
//				/* Silhouette: Reset cycle counter */
//				"	movw r2, #4100					\n"
//				"	movt r2, #57344					\n"
//				"	mov r3, #0						\n"
//				"	str r3, [r2]					\n"
//	#endif
//				// Set priority to highest to disable interrupt preemption
//				"	mrs r1, basepri					\n"
//				"	mov r0, %1						\n"
//				"	msr basepri, r0					\n"
//				"	isb								\n"
//				"	dsb								\n"
//			#ifndef USE_PROCESS_STACK	/* Code should not be required if a main() is using the process stack. */
//				"	tst lr, #4						\n"
//				"	ite eq							\n"
//				"	mrseq r0, msp					\n"
//				"	mrsne r0, psp					\n"
//			#else
//				"	mrs r0, psp						\n"
//			#endif
//				"	isb								\n"
//				// Increment the nested untrusted exception counter
//				// r2 and r3 temporarily used here to hold counter
//				"	ldr r2, numNested				\n"
//				"	ldr r3, [r2]					\n"
//				"	add r3, r3, #1					\n"
//				"	str r3, [r2]					\n"
//				// Copy hardware-saved processor state to r0 + offset
//				"	add r2, r0, #4096		\n"
//				"	ldr r3, [r0]					\n" // r0
//				"	str r3, [r2]					\n"
//				"	ldr r3, [r0, #4]				\n" // r1
//				"	str r3, [r2, #4]				\n"
//				"	ldr r3, [r0, #8]				\n" // r2
//				"	str r3, [r2, #8]				\n"
//				"	ldr r3, [r0, #12]				\n" // r3
//				"	str r3, [r2, #12]				\n"
//				"	ldr r3, [r0, #16]				\n" // r12
//				"	str r3, [r2, #16]				\n"
//				"	ldr r3, [r0, #20]				\n" // lr
//				"	str r3, [r2, #20]				\n"
//				"	ldr r3, [r0, #24]				\n" // return addr
//				"	str r3, [r2, #24]				\n"
//				"	ldr r3, [r0, #28]				\n" // xpsr
//				"	str r3, [r2, #28]				\n"
//				"	add r2, r2, #32					\n"
//				"	add r3, r0, #32					\n"
//				"	tst r14, #0x10					\n" /* Is the task using the FPU context?  If so, spill low vfp registers. */
//				"	itttt eq						\n"
//				"	vldmeq r3, {s0-s15}				\n" // s0-s15
//				"	vstmeq r2, {s0-s15}				\n"
//				"	ldreq r3, [r3, #64]				\n" // fpscr
//				"	streq r3, [r2, #64]				\n"
//				// Spill other processor state to kernel shadow stack
//				"	add sp, sp, #4096		\n" // point sp to shadow stack
//				"									\n"
//				"	tst r14, #0x10					\n" /* Is the task using the FPU context?  If so, push high vfp registers. */
//				"	it eq							\n"
//				"	vstmdbeq sp!, {s16-s31}			\n"
//				"									\n"
//				"	mrs r3, control					\n"
//				"	stmdb sp!, {r0, r3-r11, r14}	\n" /* Save the remaining registers and current r0. */
//				"	sub sp, sp, #4096		\n" // restore sp
//				// Set task stacks and shadow stacks to privileged Read-only
//				// (Currently assuming that task stacks is always the 5th region)
//				"	ldr r0, =0xe000ed98				\n"
//				"	mov r2, #4						\n"
//				"	str r2, [r0]					\n"
//				"	ldr r0, =0xe000eda0				\n"
//				"	ldr r2, [r0]					\n"
//				"	orr r2, #67108864				\n" // 0x04000000, privileged read-only bit in AP bits
//				"	str r2, [r0]					\n"
//				// Disable the MPU region of foreground task stack
//				// (Currently assuming that it is always the 8th region)
//				"	ldr r0, =0xe000ed98				\n"
//				"	mov r2, #7						\n"
//				"	str r2, [r0]					\n"
//				"	ldr r0, =0xe000eda0				\n"
//				"	ldr r2, [r0]					\n"
//				"	bic r2, #1						\n"
//				"	str r2, [r0]					\n"
//				// Reset basepri
//				"	msr basepri, r1					\n"
//				"	isb								\n"
//				"	dsb								\n"
//				// Clear r0-r3 before calling C function
//				"	mov r0, #0						\n"
//				"	mov r1, #0						\n"
//				"	mov r2, #0						\n"
//				"	mov r3, #0						\n"
//				// Call the C handler function
//				"	bl %0							\n"
//				// Set priority to highest to disable interrupt preemption
//				"	mrs r1, basepri					\n"
//				"	mov r0, %1						\n"
//				"	msr basepri, r0					\n"
//				"	isb								\n"
//				"	dsb								\n"
//				// Decrement the nested untrusted exception counter
//				// r0 and r2 temporarily used here to hold counter
//				"	ldr r0, numNested				\n"
//				"	ldr r2, [r0]					\n"
//				"	sub r2, r2, #1					\n"
//				"	str r2, [r0]					\n"
//				// If the exception is nested, don't
//				// restore MPU
//				"	teq r2, #0						\n"
//				"	it ne							\n"
//				"	bne skip_mpu_res				\n"
//				// Set task stacks and shadow stacks back to privileged RW
//				// (Currently assuming that task stacks is always the 5th region)
//				"	ldr r0, =0xe000ed98				\n"
//				"	mov r2, #4						\n"
//				"	str r2, [r0]					\n"
//				"	ldr r0, =0xe000eda0				\n"
//				"	ldr r2, [r0]					\n"
//				"	bic r2, #67108864				\n" // 0x04000000, privileged read-only bit in AP bits
//				"	str r2, [r0]					\n"
//				// Enable the region of foreground task stack
//				"	ldr r0, =0xe000ed98				\n"
//				"	mov r2, #7						\n"
//				"	str r2, [r0]					\n"
//				"	ldr r0, =0xe000eda0				\n"
//				"	ldr r2, [r0]					\n"
//				"	orr r2, #1						\n"
//				"	str r2, [r0]					\n"
//				// Restore other processor state from kernel shadow stack
//				"	skip_mpu_res:					\n"
//				"	add sp, sp, #4096		\n" // point sp to shadow stack
//				"	ldmia sp!, {r0, r3-r11, r14}		\n" /* Pop r0 and the registers that are not automatically saved on exception entry. */
//				"	msr control, r3					\n"
//				"									\n"
//				"	tst r14, #0x10					\n" /* Is the task using the FPU context?  If so, pop the high vfp registers too. */
//				"	it eq							\n"
//				"	vldmiaeq sp!, {s16-s31}			\n"
//				"	sub sp, sp, #4096		\n" // restore sp
//				"									\n"
//				// Restore hardware-saved processor state from r0 + offset
//				"	add r2, r0, #4096		\n"
//				"	ldr r3, [r2]					\n" // r0
//				"	str r3, [r0]					\n"
//				"	ldr r3, [r2, #4]				\n" // r1
//				"	str r3, [r0, #4]				\n"
//				"	ldr r3, [r2, #8]				\n" // r2
//				"	str r3, [r0, #8]				\n"
//				"	ldr r3, [r2, #12]				\n" // r3
//				"	str r3, [r0, #12]				\n"
//				"	ldr r3, [r2, #16]				\n" // r12
//				"	str r3, [r0, #16]				\n"
//				"	ldr r3, [r2, #20]				\n" // lr
//				"	str r3, [r0, #20]				\n"
//				"	ldr r3, [r2, #24]				\n" // return addr
//				"	str r3, [r0, #24]				\n"
//				"	ldr r3, [r2, #28]				\n" // xpsr
//				"	str r3, [r0, #28]				\n"
//				"	add r2, r2, #32					\n"
//				"	add r3, r0, #32					\n"
//				"	tst r14, #0x10					\n" /* Is the task using the FPU context?  If so, spill low vfp registers. */
//				"	itttt eq						\n"
//				"	vldmeq r2, {s0-s15}				\n" // s0-s15
//				"	vstmeq r3, {s0-s15}				\n"
//				"	ldreq r2, [r2, #64]				\n" // fpscr
//				"	streq r2, [r3, #64]				\n"
//				// Reset basepri
//				"	msr basepri, r1					\n"
//				"	isb								\n"
//				"	dsb								\n"
//	#if 0
//				"	pop {lr}						\n"
//	#endif
//				// Exception return
//				"	bx r14							\n"
//				// Reference of the nested handler counter
//				"	numNested:  .word uxNestedUntrustedException\n"
//				::"i"(prvKageUsageHandler), "i"(configMAX_SYSCALL_INTERRUPT_PRIORITY): "r0", "r1", "r2", "r3", "memory"
//		);
  while (1)
  {
  }
}

/**
* @brief This function handles System service call via SWI instruction.
*/
/*void SVC_Handler(void) // AWS_EDIT
{
}*/

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
}

/**
* @brief This function handles Pendable request for system service.
*/
/*void PendSV_Handler(void)  // AWS_EDIT
{
}*/

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  HAL_SYSTICK_IRQHandler();
}

/* When using the ST HAL, we must implement their systick callback. So, here,
 * we simply call the RTOS systick handler that resides in the core level abstraction.
 */

void HAL_SYSTICK_Callback( void )
{
	xPortSysTickHandler( );
}


/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles TIM6 global interrupt, DAC channel1 and channel2
 * underrun error interrupts.
 */
void TIM6_DAC_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim6);
}

/**
 * @brief This function handles EXTI line[15:10] interrupts.
 */
void EXTI15_10_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void EXTI1_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
