/*
Copyright 2018 Embedded Microprocessor Benchmark Consortium (EEMBC)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Original Author: Shay Gal-on
*/

/* Topic: Description
        This file contains additional configuration constants required to execute on
   different platforms over and above the POSIX defaults
*/
#ifndef CORE_PORTME_POSIX_OVERRIDES_H
#define CORE_PORTME_POSIX_OVERRIDES_H

#define ee_printf vLoggingPrintf

/* Kage: Toggle to use Kage's system call or FreeRTOS's system call */
#define KAGE 1

/* Kage uses FreeRTOS_MPU Task creation*/
#if KAGE
#define USE_FREERTOS_TASK_MPU 1
#define USE_FREERTOS_TASK 0
#endif

/* Set the number of threads based on symbols */
#ifdef SINGLE_T
#define MULTITHREAD 1
#elif DOUBLE_T
#define MULTITHREAD 2
#elif TRIPLE_T
#define MULTITHREAD 3
#endif

#include "FreeRTOS.h"

#define ITERATIONS 2000

/* None by default */

#endif
