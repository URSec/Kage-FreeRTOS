/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "support.h"
#ifdef MICRO_BENCHMARK // Silhouette benchmark
#include "cycle_counter.h"
#endif
#ifdef QUEUE_BENCHMARK
#include "queue.h"
#endif
#ifdef STREAM_BUFFER_BENCHMARK
#include "stream_buffer.h"
#endif

#ifdef UNPRIV_API_BENCHMARK
static void silHighMicroTask( void * pvParameters ) PRIVILEGED_FUNCTION;
#else
static void silHighMicroTask( void * pvParameters );
#endif
static void silLowMicroTask( void * pvParameters );
static void silBeebsTask( void * pvParameters );
static void silEmptyTask( void * pvParameters );
void vCallback( TimerHandle_t xTimer );

static StackType_t demoStackBuffer[STACK_SIZE * 2] TASK_STACK;
static StackType_t demoStackBufferT[STACK_SIZE * 2] TASK_STACK;

static TaskHandle_t highMicroTask;
static TimerHandle_t xTimer;
#ifdef QUEUE_BENCHMARK
static QueueHandle_t demoQueue;
#endif
#ifdef STREAM_BUFFER_BENCHMARK
static StreamBufferHandle_t demoBuffer;
#endif

// static float dummyFloat;

#ifdef UNPRIV_API_BENCHMARK
void start_microbenchmark() PRIVILEGED_FUNCTION
#else
void start_microbenchmark()
#endif
{
	BaseType_t xReturn = pdFAIL;

	/* Structs for logging task
	 * */
#ifdef QUEUE_BENCHMARK
	uint32_t cycles;
	KIN1_ResetCycleCounter();
	demoQueue = xQueueCreate(10, sizeof(uint32_t));
	cycles = KIN1_GetCycleCounter();
	configPRINTF( ( "Creating queue of 10 uint32: %u\r\n", cycles ) );
#endif

#ifdef STREAM_BUFFER_BENCHMARK
	uint32_t cycles;
	KIN1_ResetCycleCounter();
	demoBuffer = xStreamBufferCreate(sizeof(uint32_t), 1);
	cycles = KIN1_GetCycleCounter();
	configPRINTF( ( "Creating stream buffer of 1 uint32: %u\r\n", cycles ) );
#endif

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

//	xTimer = xTimerCreate("Micro Timer", // Timer name
//						  100, 			 // Timer period in ticks
//						  pdFALSE,		 // Auto reload?
//						  (void *)0,	 // Timer ID
//						  vCallback		 // Callback function
//						  );
//	xTimerStart(xTimer, 0);

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
#ifdef QUEUE_BENCHMARK
	uint32_t dummyUnsigned, cycles;
//	configPRINTF( ( "Starting queue benchmark\r\n" ) );
	KIN1_ResetCycleCounter();
	if( xQueueReceive( demoQueue, &dummyUnsigned, portMAX_DELAY ) == pdPASS )
	{
		cycles = KIN1_GetCycleCounter();
		configPRINTF( ( "Received unsigned %u from queue: %u\r\n", dummyUnsigned, cycles ) );
	}
#endif
#ifdef STREAM_BUFFER_BENCHMARK
	uint32_t dummyUnsigned, cycles;
//	configPRINTF( ( "Starting queue benchmark\r\n" ) );
	KIN1_ResetCycleCounter();
	if( xStreamBufferReceive( demoBuffer, &dummyUnsigned, sizeof(uint32_t), portMAX_DELAY ) )
	{
		cycles = KIN1_GetCycleCounter();
		configPRINTF( ( "Received unsigned %u from stream buffer: %u\r\n", dummyUnsigned, cycles ) );
	}
#endif
	configPRINTF( ( "Started Microbenchmark High Priority Task \r\n" ) );
	vTaskSuspend(NULL);
//	 dummyFloat = 1.2;
	for(;;){
//		 dummyFloat = dummyFloat * dummyFloat;
//		vTaskSuspend(NULL);
	}
}

static void silLowMicroTask( void * pvParameters )
{
#ifdef CONTEXT_SWITCH_MICRO_BENCHMARK
	extern uint32_t ulCycleRestore;
#endif
#ifdef QUEUE_BENCHMARK
	uint32_t dummy = 9;
	xQueueSend(demoQueue, &dummy, 0);
#endif
#ifdef STREAM_BUFFER_BENCHMARK
	uint32_t dummy = 9, result;
	result = xStreamBufferSend(demoBuffer, &dummy, sizeof(uint32_t), 0);
//	configPRINTF( ( "Sent buffer: result: %u\r\n", result ) );
#endif
	configPRINTF( ( "Started Microbenchmark Low Priority Task \r\n" ) );
#ifdef SECURE_API_MICRO_BENCHMARK
	uint32_t *dummyData = (uint32_t *) pvPortMallocUser(sizeof(uint32_t));
	vMeasureCycles(dummyData, sizeof(uint32_t));
#endif
//	int iterations = 1000000;
//	int i = 0;
//	for (i = 0; i < iterations*600; i++)
//	{
////		portYIELD();
//		dummyFloat = dummyFloat + 1;
//	}
//	configPRINTF( ( "Finished %d yields\r\n", iterations ) );
#ifdef CONTEXT_SWITCH_MICRO_BENCHMARK
	configPRINTF( ( "Context Switch cycle: %u cycles\r\n", ulCycleRestore ) );
#endif
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

