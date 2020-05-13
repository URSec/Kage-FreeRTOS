/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "support.h"

static void silHighMicroTask( void * pvParameters );
static void silLowMicroTask( void * pvParameters );
static void silBeebsTask( void * pvParameters );
static void silEmptyTask( void * pvParameters );
void vCallback( TimerHandle_t xTimer );

static StackType_t demoStackBuffer[STACK_SIZE * 2] TASK_STACK;
static StackType_t demoStackBufferT[STACK_SIZE * 2] TASK_STACK;

static TaskHandle_t highMicroTask;
static TimerHandle_t xTimer;

void start_microbenchmark()
{
	BaseType_t xReturn = pdFAIL;

	/* Structs for logging task
	 * */
	/// This is a dynamic task definition

//	static StackType_t logStackBuffer[STACK_SIZE * 2] __attribute__ ((aligned (STACK_SIZE * 2)));
	// Create an TaskParameters_t structure that defines the task to be created.
	TaskParameters_t microTaskParameters =
	{
		silHighMicroTask,		// pvTaskCode - the function that implements the task.
		"MicroBenchmark",	// pcName - just a text name for the task to assist debugging.
		STACK_SIZE,		// usStackDepth	- the stack size DEFINED IN WORDS.
		NULL,		// pvParameters - passed into the task function as the function parameters.
		( configMAX_PRIORITIES - 3 | portPRIVILEGE_BIT ),// uxPriority - task priority, set the portPRIVILEGE_BIT if the task should run in a privileged state.
		//( 1UL ),// uxPriority - task priority, set the portPRIVILEGE_BIT if the task should run in a privileged state.
		demoStackBuffer,// puxStackBuffer - the buffer to be used as the task stack.

		// xRegions - Allocate up to three separate memory regions for access by
		// the task, with appropriate access permissions.
		{
			// {Base address,	Length,	Parameters}
//			{ &logStackBuffer[STACK_SIZE],	STACK_SIZE_IN_BYTES, portMPU_REGION_PRIVILEGED_READ_WRITE }, // shadow stack.
			{ &demoStackBuffer[0],	STACK_SIZE_IN_BYTES, portMPU_REGION_READ_WRITE }, // the other two region left unused.
			{ 0,0,0 },
			{ 0,0,0 }
		}
	};
	if( xTaskCreateRestricted( &microTaskParameters, &highMicroTask ) == pdPASS )
//            if( xTaskCreate( prvLoggingTask, "Logging", usStackSize, NULL, uxPriority, NULL ) == pdPASS )
	{
		xReturn = pdPASS;
	}
	else
	{

	}

	xTimer = xTimerCreate("Micro Timer", // Timer name
						  100, 			 // Timer period in ticks
						  pdTRUE,		 // Auto reload?
						  (void *)0,	 // Timer ID
						  vCallback		 // Callback function
						  );
	xTimerStart(xTimer, 0);

	TaskParameters_t microTaskParametersT =
		{
			silLowMicroTask,		// pvTaskCode - the function that implements the task.
			"MicroBenchmark2",	// pcName - just a text name for the task to assist debugging.
			STACK_SIZE,		// usStackDepth	- the stack size DEFINED IN WORDS.
			NULL,		// pvParameters - passed into the task function as the function parameters.
			( configMAX_PRIORITIES - 4 | portPRIVILEGE_BIT ),// uxPriority - task priority, set the portPRIVILEGE_BIT if the task should run in a privileged state.
			//( 1UL ),// uxPriority - task priority, set the portPRIVILEGE_BIT if the task should run in a privileged state.
			demoStackBufferT,// puxStackBuffer - the buffer to be used as the task stack.

			// xRegions - Allocate up to three separate memory regions for access by
			// the task, with appropriate access permissions.
			{
				// {Base address,	Length,	Parameters}
	//			{ &logStackBuffer[STACK_SIZE],	STACK_SIZE_IN_BYTES, portMPU_REGION_PRIVILEGED_READ_WRITE }, // shadow stack.
				{ &demoStackBufferT[0],	STACK_SIZE_IN_BYTES, portMPU_REGION_READ_WRITE }, // the other two region left unused.
				{ 0,0,0 },
				{ 0,0,0 }
			}
		};
	xTaskCreateRestricted( &microTaskParametersT, NULL );
}

static void silHighMicroTask( void * pvParameters )
{
	configPRINTF( ( "Started Microbenchmark High Priority Task \r\n" ) );
	for(;;){
		vTaskSuspend(NULL);
//		configPRINTF( ( "Resumed\r\n" ) );
	}
}

static void silLowMicroTask( void * pvParameters )
{
	configPRINTF( ( "Started Microbenchmark Low Priority Task \r\n" ) );
	int iterations = 1000000;
	int i = 0;
	for (i = 0; i < iterations; i++)
	{
		portYIELD();
	}
	configPRINTF( ( "Finished %d yields\r\n", iterations ) );
	for(;;){

	}
}

void vCallback( TimerHandle_t xTimer )
{
	vTaskResume(highMicroTask);
}

void start_beebsbenchmark()
{
	BaseType_t xReturn = pdFAIL;

	/* Structs for logging task
	 * */
//	static StackType_t logStackBuffer[STACK_SIZE * 2] __attribute__ ((aligned (STACK_SIZE * 2)));
	// Create an TaskParameters_t structure that defines the task to be created.
	TaskParameters_t beebsTaskParameters =
	{
		silBeebsTask,		// pvTaskCode - the function that implements the task.
		"BEEBSBenchmark",	// pcName - just a text name for the task to assist debugging.
		STACK_SIZE,		// usStackDepth	- the stack size DEFINED IN WORDS.
		NULL,		// pvParameters - passed into the task function as the function parameters.
		( configMAX_PRIORITIES - 3 | portPRIVILEGE_BIT ),// uxPriority - task priority, set the portPRIVILEGE_BIT if the task should run in a privileged state.
		//( 1UL ),// uxPriority - task priority, set the portPRIVILEGE_BIT if the task should run in a privileged state.
		demoStackBuffer,// puxStackBuffer - the buffer to be used as the task stack.

		// xRegions - Allocate up to three separate memory regions for access by
		// the task, with appropriate access permissions.
		{
			// {Base address,	Length,	Parameters}
//			{ &logStackBuffer[STACK_SIZE],	STACK_SIZE_IN_BYTES, portMPU_REGION_PRIVILEGED_READ_WRITE }, // shadow stack.
			{ &demoStackBuffer[0],	STACK_SIZE_IN_BYTES, portMPU_REGION_READ_WRITE }, // the other two region left unused.
			{ 0,0,0 },
			{ 0,0,0 }
		}
	};
	if( xTaskCreateRestricted( &beebsTaskParameters, NULL ) == pdPASS )
//            if( xTaskCreate( prvLoggingTask, "Logging", usStackSize, NULL, uxPriority, NULL ) == pdPASS )
	{
		xReturn = pdPASS;
	}
	else
	{

	}


	/* Structs for logging task
	 * */
//	static StackType_t logStackBuffer[STACK_SIZE * 2] __attribute__ ((aligned (STACK_SIZE * 2)));
	// Create an TaskParameters_t structure that defines the task to be created.
	TaskParameters_t emptyTaskParameters =
	{
		silEmptyTask,		// pvTaskCode - the function that implements the task.
		"BEEBSBenchmark",	// pcName - just a text name for the task to assist debugging.
		STACK_SIZE,		// usStackDepth	- the stack size DEFINED IN WORDS.
		NULL,		// pvParameters - passed into the task function as the function parameters.
		( configMAX_PRIORITIES - 3 | portPRIVILEGE_BIT ),// uxPriority - task priority, set the portPRIVILEGE_BIT if the task should run in a privileged state.
		//( 1UL ),// uxPriority - task priority, set the portPRIVILEGE_BIT if the task should run in a privileged state.
		demoStackBufferT,// puxStackBuffer - the buffer to be used as the task stack.

		// xRegions - Allocate up to three separate memory regions for access by
		// the task, with appropriate access permissions.
		{
			// {Base address,	Length,	Parameters}
//			{ &logStackBuffer[STACK_SIZE],	STACK_SIZE_IN_BYTES, portMPU_REGION_PRIVILEGED_READ_WRITE }, // shadow stack.
			{ &demoStackBufferT[0],	STACK_SIZE_IN_BYTES, portMPU_REGION_READ_WRITE }, // the other two region left unused.
			{ 0,0,0 },
			{ 0,0,0 }
		}
	};
	if( xTaskCreateRestricted( &emptyTaskParameters, NULL ) == pdPASS )
//            if( xTaskCreate( prvLoggingTask, "Logging", usStackSize, NULL, uxPriority, NULL ) == pdPASS )
	{
		xReturn = pdPASS;
	}
	else
	{

	}
}

static void silBeebsTask( void * pvParameters ){
	int i;
	volatile int result = 0;
	int correct;

	uint32_t t_start, t;

	configPRINTF( ( "Starting BEEBS benchmark\r\n" ) );

	t_start = xTaskGetTickCount();

	for (i = 0; i < REPEAT_FACTOR; i++) {
		initialise_benchmark();
		asm volatile ("" :: "r" (result) : "memory");
		result = benchmark();
		asm volatile ("" :: "r" (result) : "memory");
	}

	t = xTaskGetTickCount() - t_start;

	switch (verify_benchmark(result)) {
	case 1:
		configPRINTF( ( "Finished successfully: in %u ms.\r\n", t ) );
		break;
	case -1:
		configPRINTF( ( "Finished in %u ms, but no verify_benchmark() run.\r\n", t ) );
		break;
	default:
		configPRINTF( ( "Finished in %u ms, but verify_benchmark() found errors.\r\n", t ) );
		break;
	}

	for(;;){

	}
}

static void silEmptyTask( void *pvParameters ){
	while(1){

	}
}

