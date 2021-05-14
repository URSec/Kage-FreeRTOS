/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
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


	if( xTaskCreate( silHighMicroTask, "MicroBenchmark", 1024, NULL, ( configMAX_PRIORITIES - 3 | portPRIVILEGE_BIT ),  &highMicroTask ) == pdPASS )
	{
		xReturn = pdPASS;
	}
	else
	{

	}


	xTaskCreate( silLowMicroTask, "MicroBenchmark2", 1024, NULL, ( configMAX_PRIORITIES - 4 | portPRIVILEGE_BIT ),  NULL );
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


static void silEmptyTask( void *pvParameters ){
	while(1){

	}
}

