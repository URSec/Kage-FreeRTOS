/*
 * Amazon FreeRTOS V1.4.9
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */


/*
 * Debug setup instructions:
 * 1) Open the debug configuration dialog.
 * 2) Go to the Debugger tab.
 * 3) If the 'Mode Setup' options are not visible, click the 'Show Generator' button.
 * 4) In the Mode Setup|Reset Mode drop down ensure that
 *    'Software System Reset' is selected.
 */

#include "main.h"
#include "stdint.h"
#include "stdarg.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo includes */
#include "aws_demo_runner.h"
#include "aws_system_init.h"
#include "aws_logging_task.h"
#include "aws_wifi.h"
#include "aws_clientcredential.h"
#include "aws_dev_mode_key_provisioning.h"

/* Kage Demo includes */
#include "sil_demo.h"
#include "cycle_counter.h"

/* WiFi driver includes. */
#include "es_wifi.h"

/* Application version info. */
#include "aws_application_version.h"

/* Coremark include */
#include "coremark.h"

/* Declare the firmware version structure for all to see. */
const AppVersion32_t xAppFirmwareVersion =
{
    .u.x.ucMajor = APP_VERSION_MAJOR,
    .u.x.ucMinor = APP_VERSION_MINOR,
    .u.x.usBuild = APP_VERSION_BUILD,
};

/* The SPI driver polls at a high priority. The logging task's priority must also
 * be high to be not be starved of CPU time. */
#define mainLOGGING_TASK_PRIORITY                       ( configMAX_PRIORITIES - 1 )
#define mainLOGGING_TASK_STACK_SIZE                     ( configMINIMAL_STACK_SIZE * 5 )
#define mainLOGGING_MESSAGE_QUEUE_LENGTH                ( 15 )

/* Minimum required WiFi firmware version. */
#define mainREQUIRED_WIFI_FIRMWARE_WICED_MAJOR_VERSION  ( 3 )
#define mainREQUIRED_WIFI_FIRMWARE_WICED_MINOR_VERSION  ( 5 )
#define mainREQUIRED_WIFI_FIRMWARE_WICED_PATCH_VERSION  ( 2 )
#define mainREQUIRED_WIFI_FIRMWARE_INVENTEK_VERSION     ( 5 )
/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook( void );

/* Defined in es_wifi_io.c. */
extern void SPI_WIFI_ISR(void);
extern SPI_HandleTypeDef hspi;

#ifdef USE_OFFLOAD_SSL
    /* Defined in aws_wifi.c. */
    extern WIFIReturnCode_t WIFI_GetFirmwareVersion( uint8_t *pucBuffer );
#endif /* USE_OFFLOAD_SSL */

/**********************
* Global Variables
**********************/
RTC_HandleTypeDef xHrtc;
RNG_HandleTypeDef xHrng;

/* Private variables ---------------------------------------------------------*/
static UART_HandleTypeDef xConsoleUart PRIVILEGED_DATA;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config( void ) PRIVILEGED_FUNCTION;
static void Console_UART_Init( void ) PRIVILEGED_FUNCTION;
static void RTC_Init( void ) PRIVILEGED_FUNCTION;
static void prvWifiConnect( void );

/**
 * @brief Initializes the STM32L475 IoT node board.
 *
 * Initialization of clock, LEDs, RNG, RTC, and WIFI module.
 */
static void prvMiscInitialization( void ) PRIVILEGED_FUNCTION;

/**
 * @brief Initializes the FreeRTOS heap.
 *
 * Heap_5 is being used because the RAM is not contiguous, therefore the heap
 * needs to be initialized.  See http://www.freertos.org/a00111.html
 */
static void prvInitializeHeap( void ) PRIVILEGED_FUNCTION;

#ifdef USE_OFFLOAD_SSL

    /**
     * @brief Checks whether the Inventek module's firmware version needs to be
     * updated.
     *
     * Prints a message to inform the user to update the WiFi firmware.
     */
    static void prvCheckWiFiFirmwareVersion( void );

#endif /* USE_OFFLOAD_SSL */
/*-----------------------------------------------------------*/

/**
 * @brief Application runtime entry point.
 */
int main( void ) PRIVILEGED_FUNCTION
{
    /* Perform any hardware initialization that does not require the RTOS to be
     * running.  */
    prvMiscInitialization();

    /* Create tasks that are not dependent on the WiFi being initialized. */
    xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                            mainLOGGING_TASK_PRIORITY,
                            mainLOGGING_MESSAGE_QUEUE_LENGTH );

    /* Start the scheduler.  Initialization that requires the OS to be running,
     * including the WiFi initialization, is performed in the RTOS daemon task
     * startup hook. */
//    start_microbenchmark();
//    configPRINTF( ( "Test\r\n" ) );
    vTaskStartScheduler();

    return 0;
}
/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook( void )
{
#if 0
	printMPUConfig(0);
	printMPUConfig(1);
	printMPUConfig(2);
	printMPUConfig(3);
	printMPUConfig(4);
	printMPUConfig(5);
	printMPUConfig(6);
	printMPUConfig(7);
#endif
coremark_main();
#ifdef MICRO_BENCHMARK
extern uint32_t ulCycleSpill;
extern uint32_t ulCycleRestore;
#ifdef EXCEPTION_MICRO_BENCHMARK
	__asm volatile ( "	SVC	%0	\n" :: "i" (portSVC_PRINT_CYCLE) : "memory" ); // Call special SVC ID in SVC handler to measure exception sill and restore cycles
	configPRINTF( ( "Spill: %u Restore: %u \r\n", ulCycleSpill, ulCycleRestore ) );
#endif
#ifdef SECURE_API_MICRO_BENCHMARK
	configPRINTF( ( "MPU checks: %u cycles\r\n", ulCycleSpill ) );
#endif
#ifdef EXCEPTION_NEW_MICRO_BENCHMARK
	vMeasureException();
#endif
#endif
}
/*-----------------------------------------------------------*/

static void prvWifiConnect( void )
{
    WIFINetworkParams_t xNetworkParams;
    WIFIReturnCode_t xWifiStatus;
    uint8_t ucIPAddr[ 4 ];

    /* Setup WiFi parameters to connect to access point. */
    xNetworkParams.pcSSID = clientcredentialWIFI_SSID;
    xNetworkParams.ucSSIDLength = sizeof( clientcredentialWIFI_SSID );
    xNetworkParams.pcPassword = clientcredentialWIFI_PASSWORD;
    xNetworkParams.ucPasswordLength = sizeof( clientcredentialWIFI_PASSWORD );
    xNetworkParams.xSecurity = clientcredentialWIFI_SECURITY;
    xNetworkParams.cChannel = 0;

    /* Try connecting using provided wifi credentials. */
    xWifiStatus = WIFI_ConnectAP( &( xNetworkParams ) );

    if( xWifiStatus == eWiFiSuccess )
    {
        configPRINTF( ( "WiFi connected to AP %s.\r\n", xNetworkParams.pcSSID ) );

        /* Get IP address of the device. */
        WIFI_GetIP( &ucIPAddr[ 0 ] );

        configPRINTF( ( "IP Address acquired %d.%d.%d.%d\r\n",
                        ucIPAddr[ 0 ], ucIPAddr[ 1 ], ucIPAddr[ 2 ], ucIPAddr[ 3 ] ) );
    }
    else
    {
        /* Connection failed configure softAP to allow user to set wifi credentials. */
        configPRINTF( ( "WiFi failed to connect to AP %s.\r\n", xNetworkParams.pcSSID ) );

        xNetworkParams.pcSSID = wificonfigACCESS_POINT_SSID_PREFIX;
        xNetworkParams.pcPassword = wificonfigACCESS_POINT_PASSKEY;
        xNetworkParams.xSecurity = wificonfigACCESS_POINT_SECURITY;
        xNetworkParams.cChannel = wificonfigACCESS_POINT_CHANNEL;

        configPRINTF( ( "Connect to softAP %s using password %s. \r\n",
                        xNetworkParams.pcSSID, xNetworkParams.pcPassword ) );

        while( WIFI_ConfigureAP( &xNetworkParams ) != eWiFiSuccess )
        {
            configPRINTF( ( "Connect to softAP %s using password %s and configure WiFi. \r\n",
                            xNetworkParams.pcSSID, xNetworkParams.pcPassword ) );
        }

        configPRINTF( ( "WiFi configuration successful. \r\n", xNetworkParams.pcSSID ) );
    }
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    uint32_t * pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
 * function then they must be declared static - otherwise they will be allocated on
 * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    /* Silhouette: Increased stack size to 4096 due to shadow stack */
    static StackType_t uxIdleTaskStack[ 2 * STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    /* Silhouette: Increased stack size to 4096 due to shadow stack */
    *pulIdleTaskStackSize = STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetTimerTaskMemory() to provide the memory that is
 * used by the RTOS daemon/time task. */
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     uint32_t * pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
 * function then they must be declared static - otherwise they will be allocated on
 * the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    /* Silhouette: Size increased for shadow stack */
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH + STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
/*-----------------------------------------------------------*/

/**
 * @brief Publishes a character to the STM32L475 UART
 *
 * This is used to implement the tinyprintf created by Spare Time Labs
 * http://www.sparetimelabs.com/tinyprintf/tinyprintf.php
 *
 * @param pv    unused void pointer for compliance with tinyprintf
 * @param ch    character to be printed
 */
void vSTM32L475putc( void * pv,
                     char ch ) PRIVILEGED_FUNCTION
{
    while( HAL_OK != HAL_UART_Transmit( &xConsoleUart, ( uint8_t * ) &ch, 1, 30000 ) )
    {
    }
}
/*-----------------------------------------------------------*/

/**
 * @brief Initializes the board.
 */
static void prvMiscInitialization( void )
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock. */
    SystemClock_Config();

    /* Heap_5 is being used because the RAM is not contiguous in memory, so the
     * heap must be initialized. */
    prvInitializeHeap();

    BSP_LED_Init( LED_GREEN );
    BSP_PB_Init( BUTTON_USER, BUTTON_MODE_EXTI );

    /* RNG init function. */
    xHrng.Instance = RNG;

    if( HAL_RNG_Init( &xHrng ) != HAL_OK )
    {
        Error_Handler();
    }

    /* RTC init. */
    RTC_Init();

    /* UART console init. */
    Console_UART_Init();
}
/*-----------------------------------------------------------*/

/**
 * @brief Initializes the system clock.
 */
static void SystemClock_Config( void )
{
    RCC_OscInitTypeDef xRCC_OscInitStruct;
    RCC_ClkInitTypeDef xRCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef xPeriphClkInit;

    xRCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
    xRCC_OscInitStruct.LSEState = RCC_LSE_ON;
    xRCC_OscInitStruct.MSIState = RCC_MSI_ON;
    xRCC_OscInitStruct.MSICalibrationValue = 0;
    xRCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
    xRCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    xRCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    xRCC_OscInitStruct.PLL.PLLM = 6;
    xRCC_OscInitStruct.PLL.PLLN = 20;
    xRCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    xRCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    xRCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;

    if( HAL_RCC_OscConfig( &xRCC_OscInitStruct ) != HAL_OK )
    {
        Error_Handler();
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     * clocks dividers. */
    xRCC_ClkInitStruct.ClockType = ( RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
                                     | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 );
    xRCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    xRCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    xRCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    xRCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if( HAL_RCC_ClockConfig( &xRCC_ClkInitStruct, FLASH_LATENCY_4 ) != HAL_OK )
    {
        Error_Handler();
    }

    xPeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC
                                          | RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_USART3 | RCC_PERIPHCLK_I2C2
                                          | RCC_PERIPHCLK_RNG;
    xPeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    xPeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
    xPeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
    xPeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    xPeriphClkInit.RngClockSelection = RCC_RNGCLKSOURCE_MSI;

    if( HAL_RCCEx_PeriphCLKConfig( &xPeriphClkInit ) != HAL_OK )
    {
        Error_Handler();
    }

    __HAL_RCC_PWR_CLK_ENABLE();

    if( HAL_PWREx_ControlVoltageScaling( PWR_REGULATOR_VOLTAGE_SCALE1 ) != HAL_OK )
    {
        Error_Handler();
    }

    /* Enable MSI PLL mode. */
    HAL_RCCEx_EnableMSIPLLMode();
}
/*-----------------------------------------------------------*/

/**
 * @brief UART console initialization function.
 */
static void Console_UART_Init( void )
{
    xConsoleUart.Instance = USART1;
    xConsoleUart.Init.BaudRate = 115200;
    xConsoleUart.Init.WordLength = UART_WORDLENGTH_8B;
    xConsoleUart.Init.StopBits = UART_STOPBITS_1;
    xConsoleUart.Init.Parity = UART_PARITY_NONE;
    xConsoleUart.Init.Mode = UART_MODE_TX_RX;
    xConsoleUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    xConsoleUart.Init.OverSampling = UART_OVERSAMPLING_16;
    xConsoleUart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    xConsoleUart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    BSP_COM_Init( COM1, &xConsoleUart );
}
/*-----------------------------------------------------------*/

/**
 * @brief RTC init function.
 */
static void RTC_Init( void )
{
    RTC_TimeTypeDef xsTime;
    RTC_DateTypeDef xsDate;

    /* Initialize RTC Only. */
    xHrtc.Instance = RTC;
    xHrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    xHrtc.Init.AsynchPrediv = 127;
    xHrtc.Init.SynchPrediv = 255;
    xHrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    xHrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
    xHrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    xHrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if( HAL_RTC_Init( &xHrtc ) != HAL_OK )
    {
        Error_Handler();
    }

    /* Initialize RTC and set the Time and Date. */
    xsTime.Hours = 0x12;
    xsTime.Minutes = 0x0;
    xsTime.Seconds = 0x0;
    xsTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    xsTime.StoreOperation = RTC_STOREOPERATION_RESET;

    if( HAL_RTC_SetTime( &xHrtc, &xsTime, RTC_FORMAT_BCD ) != HAL_OK )
    {
        Error_Handler();
    }

    xsDate.WeekDay = RTC_WEEKDAY_FRIDAY;
    xsDate.Month = RTC_MONTH_JANUARY;
    xsDate.Date = 0x24;
    xsDate.Year = 0x17;

    if( HAL_RTC_SetDate( &xHrtc, &xsDate, RTC_FORMAT_BCD ) != HAL_OK )
    {
        Error_Handler();
    }
}
/*-----------------------------------------------------------*/

/**
 * @brief  This function is executed in case of error occurrence.
 */
void Error_Handler( void ) PRIVILEGED_FUNCTION
{
    while( 1 )
    {
        BSP_LED_Toggle( LED_GREEN );
        HAL_Delay( 200 );
    }
}
/*-----------------------------------------------------------*/

/**
 * @brief Warn user if pvPortMalloc fails.
 *
 * Called if a call to pvPortMalloc() fails because there is insufficient
 * free memory available in the FreeRTOS heap.  pvPortMalloc() is called
 * internally by FreeRTOS API functions that create tasks, queues, software
 * timers, and semaphores.  The size of the FreeRTOS heap is set by the
 * configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h.
 *
 */
void vApplicationMallocFailedHook()
{
//    taskDISABLE_INTERRUPTS();

	configASSERT(0);
    for( ;; );
}
/*-----------------------------------------------------------*/

/**
 * @brief Loop forever if stack overflow is detected.
 *
 * If configCHECK_FOR_STACK_OVERFLOW is set to 1,
 * this hook provides a location for applications to
 * define a response to a stack overflow.
 *
 * Use this hook to help identify that a stack overflow
 * has occurred.
 *
 */
void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    char * pcTaskName )
{
//    portDISABLE_INTERRUPTS();

	configASSERT(0);
    /* Loop forever */
    for( ; ; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
}
/*-----------------------------------------------------------*/

//void * malloc( size_t xSize )
//{
//    configASSERT( xSize == ~0 );
//
//    return NULL;
//}
/*-----------------------------------------------------------*/


void vOutputChar( const char cChar,
                  const TickType_t xTicksToWait )
{
    ( void ) cChar;
    ( void ) xTicksToWait;
}
/*-----------------------------------------------------------*/

void vMainUARTPrintString( char * pcString ) PRIVILEGED_FUNCTION
{
    const uint32_t ulTimeout = 3000UL;

    HAL_UART_Transmit( &xConsoleUart,
                       ( uint8_t * ) pcString,
                       strlen( pcString ),
                       ulTimeout );
}
/*-----------------------------------------------------------*/

void prvGetRegistersFromStack( uint32_t * pulFaultStackAddress ) __attribute__((used))
{
/* These are volatile to try and prevent the compiler/linker optimising them
 * away as the variables never actually get used.  If the debugger won't show the
 * values of the variables, make them global my moving their declaration outside
 * of this function. */
    volatile uint32_t r0;
    volatile uint32_t r1;
    volatile uint32_t r2;
    volatile uint32_t r3;
    volatile uint32_t r12;
    volatile uint32_t lr;  /* Link register. */
    volatile uint32_t pc;  /* Program counter. */
    volatile uint32_t psr; /* Program status register. */

    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];

    /* Remove compiler warnings about the variables not being used. */
    ( void ) r0;
    ( void ) r1;
    ( void ) r2;
    ( void ) r3;
    ( void ) r12;
    ( void ) lr;  /* Link register. */
    ( void ) pc;  /* Program counter. */
    ( void ) psr; /* Program status register. */

    /* When the following line is hit, the variables contain the register values. */
    for( ; ; )
    {
    }
}
/*-----------------------------------------------------------*/
void prvKageUsageHandler( void )
//void HardFault_Handler( void )
{
	/* As the microbenchmark triggers a divide-by-0 fault,
	 * we need to clear the status bit to clear the fault.
	 */
	__asm volatile
	(
		"ldr r0, =0xe000ed14			\n"
		"ldr r2, [r0]					\n"
		"bic r2, #16						\n"
		"str r2, [r0]					\n"
//		"ldr r0, =0xe000ed28			\n"
//		"ldr r2, =0x02000000			\n" // div-by-0 bit in UFSR
//		"str r2, [r0]					\n"
	);
}

/* The fault handler implementation calls a function called
 * prvGetRegistersFromStack(). */
void HardFault_Handler( void ) __attribute__((used)) PRIVILEGED_FUNCTION
{
	while (1) {

	}
//    __asm volatile
//    (
//        " tst lr, #4                                                \n"
//        " ite eq                                                    \n"
//        " mrseq r0, msp                                             \n"
//        " mrsne r0, psp                                             \n"
//        " ldr r1, [r0, #24]                                         \n"
//        " ldr r2, handler2_address_const                            \n"
//        " bx r2                                                     \n"
//        " handler2_address_const: .word prvGetRegistersFromStack    \n"
//    );
}
void HardFault_Handler_kage( void )
{
//    __asm volatile
//    (
//        " tst lr, #4                                                \n"
//        " ite eq                                                    \n"
//        " mrseq r0, msp                                             \n"
//        " mrsne r0, psp                                             \n"
//        " ldr r1, [r0, #24]                                         \n"
//        " ldr r2, handler2_address_const                            \n"
//        " bx r2                                                     \n"
//        " handler2_address_const: .word prvGetRegistersFromStack    \n"
//    );
		/* Assumes psp was in use. */
			__asm volatile
			(
					// r0: stack pointer that contains processor state
					// r1: original basepri value
					// r2: shadow stack of r0
					// r3: temp register to hold values

					// Set priority to highest to disable interrupt preemption
					"	mrs r1, basepri					\n"
					"	mov r0, %1						\n"
					"	msr basepri, r0					\n"
					"	isb								\n"
					"	dsb								\n"
				#ifndef USE_PROCESS_STACK	/* Code should not be required if a main() is using the process stack. */
					"	tst lr, #4						\n"
					"	ite eq							\n"
					"	mrseq r0, msp					\n"
					"	mrsne r0, psp					\n"
				#else
					"	mrs r0, psp						\n"
				#endif
					"	isb								\n"
					// Increment the nested untrusted exception counter
					// r2 and r3 temporarily used here to hold counter
					"	ldr r2, numNested				\n"
					"	ldr r3, [r2]					\n"
					"	add r3, r3, #1					\n"
					"	str r3, [r2]					\n"
#ifndef KAGE_INVERT
					// Copy hardware-saved processor state to r0 + offset
					"	add r2, r0, #4092		\n"
					"	ldr r3, [r0]					\n" // r0
					"	str r3, [r2]					\n"
					"	ldr r3, [r0, #4]				\n" // r1
					"	str r3, [r2, #4]				\n"
					"	ldr r3, [r0, #8]				\n" // r2
					"	str r3, [r2, #8]				\n"
					"	ldr r3, [r0, #12]				\n" // r3
					"	str r3, [r2, #12]				\n"
					"	ldr r3, [r0, #16]				\n" // r12
					"	str r3, [r2, #16]				\n"
					"	ldr r3, [r0, #20]				\n" // lr
					"	str r3, [r2, #20]				\n"
					"	ldr r3, [r0, #24]				\n" // return addr
					"	str r3, [r2, #24]				\n"
					"	ldr r3, [r0, #28]				\n" // xpsr
					"	str r3, [r2, #28]				\n"
					"	add r2, r2, #32					\n"
					"	add r3, r0, #32					\n"
					"	tst r14, #0x10					\n" /* Is the task using the FPU context?  If so, spill low vfp registers. */
					"	itttt eq						\n"
					"	vldmeq r3, {s0-s15}				\n" // s0-s15
					"	vstmeq r2, {s0-s15}				\n"
					"	ldreq r3, [r3, #64]				\n" // fpscr
					"	streq r3, [r2, #64]				\n"
#endif
#ifdef STRD_EXPERIMENT
					// Copy hardware-saved processor state to r0 + offset
					"	add r2, r0, #4092		\n"
					"	ldrd r3, r12, [r0]				\n" // r0, r1
					"	strd r3, r12, [r2]				\n"
					"	ldrd r3, r12, [r0, #8]			\n" // r2, r3
					"	strd r3, r12, [r2, #8]			\n"
					"	ldrd r3, r12, [r0, #16]			\n" // r12, lr
					"	strd r3, r12, [r2, #16]			\n"
					"	ldrd r3, r12, [r0, #24]			\n" // return addr
					"	strd r3, r12, [r2, #24]			\n"
					"	add r2, r2, #32					\n"
					"	add r3, r0, #32					\n"
					"	tst r14, #0x10					\n" /* Is the task using the FPU context?  If so, spill low vfp registers. */
					"	itttt eq						\n"
					"	vldmeq r3, {s0-s15}				\n" // s0-s15
					"	vstmeq r2, {s0-s15}				\n"
					"	ldreq r3, [r3, #64]				\n" // fpscr
					"	streq r3, [r2, #64]				\n"
#endif
//					"	push {r14}\n"
					// Spill other processor state to kernel shadow stack
					"	add sp, sp, #4092		\n" // point sp to shadow stack
					"									\n"
					"	tst r14, #0x10					\n" /* Is the task using the FPU context?  If so, push high vfp registers. */
					"	it eq							\n"
					"	vstmdbeq sp!, {s16-s31}			\n"
					"									\n"
					"	mrs r3, control					\n"
					"	stmdb sp!, {r0, r3-r11, r14}	\n" /* Save the remaining registers and current r0. */
					"	sub sp, sp, #4092		\n" // restore sp
					// Set task stacks and shadow stacks to privileged Read-only
					// (Currently assuming that task stacks is always the 5th region)
					"	ldr r0, =0xe000ed98				\n"
					"	mov r2, #4						\n"
					"	str r2, [r0]					\n"
					"	ldr r0, =0xe000eda0				\n"
					"	ldr r2, [r0]					\n"
					"	orr r2, #67108864				\n" // 0x04000000, privileged read-only bit in AP bits
					"	str r2, [r0]					\n"
					// Disable the MPU region of foreground task stack
					// (Currently assuming that it is always the 8th region)
					"	ldr r0, =0xe000ed98				\n"
					"	mov r2, #7						\n"
					"	str r2, [r0]					\n"
					"	ldr r0, =0xe000eda0				\n"
					"	ldr r2, [r0]					\n"
					"	bic r2, #1						\n"
					"	str r2, [r0]					\n"
					// Reset basepri
					"	msr basepri, r1					\n"
					"	isb								\n"
					"	dsb								\n"
					// Clear r0-r3 before calling C function
					"	mov r0, #0						\n"
					"	mov r1, #0						\n"
					"	mov r2, #0						\n"
					"	mov r3, #0						\n"
					// Call the C handler function
					"	bl %0							\n"
					// Set priority to highest to disable interrupt preemption
					"	mrs r1, basepri					\n"
					"	mov r0, %1						\n"
					"	msr basepri, r0					\n"
					"	isb								\n"
					"	dsb								\n"
					// Decrement the nested untrusted exception counter
					// r0 and r2 temporarily used here to hold counter
					"	ldr r0, numNested				\n"
					"	ldr r2, [r0]					\n"
					"	sub r2, r2, #1					\n"
					"	str r2, [r0]					\n"
					// If the exception is nested, don't
					// restore MPU
					"	teq r2, #0						\n"
					"	it ne							\n"
					"	bne skip_mpu_res				\n"
					// Set task stacks and shadow stacks back to privileged RW
					// (Currently assuming that task stacks is always the 5th region)
					"	ldr r0, =0xe000ed98				\n"
					"	mov r2, #4						\n"
					"	str r2, [r0]					\n"
					"	ldr r0, =0xe000eda0				\n"
					"	ldr r2, [r0]					\n"
					"	bic r2, #67108864				\n" // 0x04000000, privileged read-only bit in AP bits
					"	str r2, [r0]					\n"
					// Enable the region of foreground task stack
					"	ldr r0, =0xe000ed98				\n"
					"	mov r2, #7						\n"
					"	str r2, [r0]					\n"
					"	ldr r0, =0xe000eda0				\n"
					"	ldr r2, [r0]					\n"
					"	orr r2, #1						\n"
					"	str r2, [r0]					\n"
					// Restore other processor state from kernel shadow stack
					"	skip_mpu_res:					\n"
//					"	pop {r14}\n"
					"	add sp, sp, #4092		\n" // point sp to shadow stack
					"	ldmia sp!, {r0, r3-r11, r14}		\n" /* Pop r0 and the registers that are not automatically saved on exception entry. */
					"	msr control, r3					\n"
					"									\n"
					"	tst r14, #0x10					\n" /* Is the task using the FPU context?  If so, pop the high vfp registers too. */
					"	it eq							\n"
					"	vldmiaeq sp!, {s16-s31}			\n"
					"	sub sp, sp, #4092		\n" // restore sp
					"									\n"
#ifndef KAGE_INVERT
					// Restore hardware-saved processor state from r0 + offset
					"	add r2, r0, #4092		\n"
					"	ldr r3, [r2]					\n" // r0
					"	str r3, [r0]					\n"
					"	ldr r3, [r2, #4]				\n" // r1
					"	str r3, [r0, #4]				\n"
					"	ldr r3, [r2, #8]				\n" // r2
					"	str r3, [r0, #8]				\n"
					"	ldr r3, [r2, #12]				\n" // r3
					"	str r3, [r0, #12]				\n"
					"	ldr r3, [r2, #16]				\n" // r12
					"	str r3, [r0, #16]				\n"
					"	ldr r3, [r2, #20]				\n" // lr
					"	str r3, [r0, #20]				\n"
					"	ldr r3, [r2, #24]				\n" // return addr
					"	str r3, [r0, #24]				\n"
					"	ldr r3, [r2, #28]				\n" // xpsr
					"	str r3, [r0, #28]				\n"
					"	add r2, r2, #32					\n"
					"	add r3, r0, #32					\n"
					"	tst r14, #0x10					\n" /* Is the task using the FPU context?  If so, spill low vfp registers. */
					"	itttt eq						\n"
					"	vldmeq r2, {s0-s15}				\n" // s0-s15
					"	vstmeq r3, {s0-s15}				\n"
					"	ldreq r2, [r2, #64]				\n" // fpscr
					"	streq r2, [r3, #64]				\n"
#endif
#ifdef STRD_EXPERIMENT
					// Copy hardware-saved processor state to r0 + offset
					"	add r2, r0, #4092		\n"
					"	ldrd r3, r12, [r2]				\n" // r0, r1
					"	strd r3, r12, [r0]				\n"
					"	ldrd r3, r12, [r2, #8]			\n" // r2, r3
					"	strd r3, r12, [r0, #8]			\n"
					"	ldrd r3, r12, [r2, #16]			\n" // r12, lr
					"	strd r3, r12, [r0, #16]			\n"
					"	ldrd r3, r12, [r2, #24]			\n" // return addr
					"	strd r3, r12, [r0, #24]			\n"
					"	add r2, r2, #32					\n"
					"	add r3, r0, #32					\n"
					"	tst r14, #0x10					\n" /* Is the task using the FPU context?  If so, spill low vfp registers. */
					"	itttt eq						\n"
					"	vldmeq r2, {s0-s15}				\n" // s0-s15
					"	vstmeq r3, {s0-s15}				\n"
					"	ldreq r2, [r2, #64]				\n" // fpscr
					"	streq r2, [r3, #64]				\n"
#endif
					// Reset basepri
					"	msr basepri, r1					\n"
					"	isb								\n"
					"	dsb								\n"
					// Exception return
					"	bx r14							\n"
					// Reference of the nested handler counter
					"	numNested:  .word uxNestedUntrustedException\n"
					::"i"(prvKageUsageHandler), "i"(configMAX_SYSCALL_INTERRUPT_PRIORITY): "r0", "r1", "r2", "r3", "memory"
			);
}
/*-----------------------------------------------------------*/

/* The fault handler implementation calls a function called
 * prvGetRegistersFromStack(). */
void MemManage_Handler( void ) PRIVILEGED_FUNCTION
{
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler3_address_const                            \n"
        " bx r2                                                     \n"
        " handler3_address_const: .word prvGetRegistersFromStack    \n"
    );
}
/*-----------------------------------------------------------*/

///* The fault handler implementation calls a function called
// * prvGetRegistersFromStack(). */
//void ADC1_2_IRQHandler( void )
//{
//    __asm volatile
//    (
//        " tst lr, #4                                                \n"
//        " ite eq                                                    \n"
//        " mrseq r0, msp                                             \n"
//        " mrsne r0, psp                                             \n"
//        " ldr r1, [r0, #24]                                         \n"
//        " ldr r2, handler4_address_const                            \n"
//        " bx r2                                                     \n"
//        " handler4_address_const: .word prvGetRegistersFromStack    \n"
//    );
//}
///*-----------------------------------------------------------*/
//
///* The fault handler implementation calls a function called
// * prvGetRegistersFromStack(). */
//void WWDG_IRQHandler( void )
//{
//    __asm volatile
//    (
//        " tst lr, #4                                                \n"
//        " ite eq                                                    \n"
//        " mrseq r0, msp                                             \n"
//        " mrsne r0, psp                                             \n"
//        " ldr r1, [r0, #24]                                         \n"
//        " ldr r2, handler5_address_const                            \n"
//        " bx r2                                                     \n"
//        " handler5_address_const: .word prvGetRegistersFromStack    \n"
//    );
//}
/*-----------------------------------------------------------*/


/* Psuedo random number generator.  Just used by demos so does not need to be
 * secure.  Do not use the standard C library rand() function as it can cause
 * unexpected behaviour, such as calls to malloc(). */
int iMainRand32( void )
{
    static UBaseType_t uxlNextRand; /*_RB_ Not seeded. */
    const uint32_t ulMultiplier = 0x015a4e35UL, ulIncrement = 1UL;

    /* Utility function to generate a pseudo random number. */

    uxlNextRand = ( ulMultiplier * uxlNextRand ) + ulIncrement;

    return( ( int ) ( uxlNextRand >> 16UL ) & 0x7fffUL );
}
/*-----------------------------------------------------------*/

static void prvInitializeHeap( void )
{
    static uint8_t ucHeap1[ configTOTAL_HEAP_SIZE ] PRIVILEGED_DATA;
    // Silhouette: Reduced heap2 size from 27KB to 7KB due to size of RAM2
//    static uint8_t ucHeap2[ 15 * 1024 ] __attribute__( ( section( ".freertos_heap2" ) ) );

    HeapRegion_t xHeapRegions[] =
    {
//        { ( unsigned char * ) ucHeap2, sizeof( ucHeap2 ) },
        { ( unsigned char * ) ucHeap1, sizeof( ucHeap1 ) },
		{ NULL,                                        0 },
        { NULL,                                        0 }
    };

    vPortDefineHeapRegions( xHeapRegions );
}
/*-----------------------------------------------------------*/

#ifdef USE_OFFLOAD_SSL

    static void prvCheckWiFiFirmwareVersion( void )
    {
        int32_t lWicedMajorVersion = 0, lWicedMinorVersion = 0, lWicedPatchVersion = 0, lInventekVersion = 0, lParsedFields = 0;
        uint8_t ucFirmwareVersion[ ES_WIFI_FW_REV_SIZE ];
        BaseType_t xNeedsUpdate = pdFALSE;

        if( WIFI_GetFirmwareVersion( &( ucFirmwareVersion[ 0 ] ) ) == eWiFiSuccess )
        {
            configPRINTF( ( "WiFi firmware version is: %s\r\n", ( char * ) ucFirmwareVersion ) );

            /* Parse the firmware revision number. */
            lParsedFields = sscanf( ( char* ) ucFirmwareVersion,
                                    "C%ld.%ld.%ld.%ld.STM",
                                    &( lWicedMajorVersion ),
                                    &( lWicedMinorVersion ),
                                    &( lWicedPatchVersion ),
                                    &( lInventekVersion ) );

            /* Check if the firmware version needs to be updated. */
            if( lParsedFields > 0 )
            {
                if( lWicedMajorVersion < mainREQUIRED_WIFI_FIRMWARE_WICED_MAJOR_VERSION )
                {
                    xNeedsUpdate = pdTRUE;
                }
                else if ( ( lWicedMajorVersion == mainREQUIRED_WIFI_FIRMWARE_WICED_MAJOR_VERSION ) &&
                          ( lWicedMinorVersion < mainREQUIRED_WIFI_FIRMWARE_WICED_MINOR_VERSION ) )
                {
                    xNeedsUpdate = pdTRUE;
                }
                else if ( ( lWicedMajorVersion == mainREQUIRED_WIFI_FIRMWARE_WICED_MAJOR_VERSION ) &&
                          ( lWicedMinorVersion == mainREQUIRED_WIFI_FIRMWARE_WICED_MINOR_VERSION ) &&
                          ( lWicedPatchVersion < mainREQUIRED_WIFI_FIRMWARE_WICED_PATCH_VERSION ) )
                {
                    xNeedsUpdate = pdTRUE;
                }
                else if ( ( lWicedMajorVersion == mainREQUIRED_WIFI_FIRMWARE_WICED_MAJOR_VERSION ) &&
                          ( lWicedMinorVersion == mainREQUIRED_WIFI_FIRMWARE_WICED_MINOR_VERSION ) &&
                          ( lWicedPatchVersion == mainREQUIRED_WIFI_FIRMWARE_WICED_PATCH_VERSION ) &&
                          ( lInventekVersion < mainREQUIRED_WIFI_FIRMWARE_INVENTEK_VERSION ) )
                {
                    xNeedsUpdate = pdTRUE;
                }

                /* Print a warning for the user to inform that the WiFi Firmware
                * needs to be updated. */
                if( xNeedsUpdate == pdTRUE )
                {
                    configPRINTF( ( "[WARN] WiFi firmware needs to be updated.\r\n" ) );
                }
                else
                {
                    configPRINTF( ( "WiFi firmware is up-to-date.\r\n" ) );
                }
            }
            else
            {
                configPRINTF( ( "Failed to parse the WiFi firmware version.\r\n" ) );
            }
        }
        else
        {
            configPRINTF( ( "Failed to get WiFi firmware version.\r\n" ) );
        }
    }

#endif /* USE_OFFLOAD_SSL */
/*-----------------------------------------------------------*/

/**
 * @brief  EXTI line detection callback.
 *
 * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
 */
void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin )
{
    switch( GPIO_Pin )
    {
        /* Pin number 1 is connected to Inventek Module Cmd-Data
         * ready pin. */
        case( GPIO_PIN_1 ):
        {
            SPI_WIFI_ISR();
            break;
        }

        default:
        {
            break;
        }
    }
}
/*-----------------------------------------------------------*/

/**
 * @brief SPI Interrupt Handler.
 *
 * @note Inventek module is configured to use SPI3.
 */
void SPI3_IRQHandler( void ) PRIVILEGED_FUNCTION
{
    HAL_SPI_IRQHandler( &( hspi ) );
}
/*-----------------------------------------------------------*/

/**
 * @brief Period elapsed callback in non blocking mode
 *
 * @note This function is called  when TIM1 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 *
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if( htim->Instance == TIM6 )
    {
        HAL_IncTick();
    }
}
/*-----------------------------------------------------------*/
