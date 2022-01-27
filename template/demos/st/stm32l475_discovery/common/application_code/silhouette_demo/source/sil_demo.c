/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

static void silMicroTask1( void * pvParameters );
static void silMicroTask2( void * pvParameters );

static StackType_t demoStackBuffer[STACK_SIZE * 2] TASK_STACK;
static StackType_t demoStackBufferT[STACK_SIZE * 2] TASK_STACK;

static TaskHandle_t highMicroTask;

void start_demo_runner()
{
	BaseType_t xReturn = pdFAIL;

	// Create an TaskParameters_t structure that defines the task to be created.
	TaskParameters_t microTaskParameters =
	{
		silMicroTask1,		// pvTaskCode - the function that implements the task.
		"Demo task 1",	// pcName - just a text name for the task to assist debugging.
		STACK_SIZE,		// usStackDepth	- the stack size DEFINED IN WORDS.
		NULL,		// pvParameters - passed into the task function as the function parameters.
		( configMAX_PRIORITIES - 3 | portPRIVILEGE_BIT ),// uxPriority - task priority, set the portPRIVILEGE_BIT if the task should run in a privileged state.
		demoStackBuffer,// puxStackBuffer - the buffer to be used as the task stack.
		// xRegions - Allocate up to one separate memory regions for access by
		// the task, with appropriate access permissions.
		{
			{ 0,0,0 }
		}
	};
	if( xTaskCreateRestricted( &microTaskParameters, &highMicroTask ) == pdPASS )
	{
		xReturn = pdPASS;
	}
	else
	{

	}

	TaskParameters_t microTaskParametersT =
		{
			silMicroTask2,		// pvTaskCode - the function that implements the task.
			"Demo task 2",	// pcName - just a text name for the task to assist debugging.
			STACK_SIZE,		// usStackDepth	- the stack size DEFINED IN WORDS.
			NULL,		// pvParameters - passed into the task function as the function parameters.
			( configMAX_PRIORITIES - 3 | portPRIVILEGE_BIT ),// uxPriority - task priority, set the portPRIVILEGE_BIT if the task should run in a privileged state.
			demoStackBufferT,// puxStackBuffer - the buffer to be used as the task stack.
			// xRegions - Allocate up to one separate memory regions for access by
			// the task, with appropriate access permissions.
			{
				{ 0,0,0 }
			}
		};
	xTaskCreateRestricted( &microTaskParametersT, NULL );
}

static void silMicroTask1( void * pvParameters )
{
	configPRINTF( ( "Started Demo Task 1 \r\n" ) );
	for(;;){

	}
}

static void silMicroTask2( void * pvParameters )
{
	configPRINTF( ( "Started Demo Task 2 \r\n" ) );
	for(;;){

	}
}
