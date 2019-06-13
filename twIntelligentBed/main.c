/* ------------------------------------------
 * Copyright (c) 2015, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * \version 2015.05
 * \date 2014-12-17
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */

/**
 * \defgroup	EMBARC_APP_FREERTOS_DEMO	embARC FreeRTOS Demo Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_BOARD_EMSK
 * \ingroup	EMBARC_APPS_OS_FREERTOS
 * \brief	embARC Example for testing freertos task switch and interrupt/exception handling
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *
 * ### Design Concept
 *     This example is designed to show the functionality of freertos.
 *
 * ### Usage Manual
 *     Test case for show how FreeRTOS is working by task switching and interrupt/exception processing.
 *     ![ScreenShot of freertos-demo under freertos](pic/images/example/emsk/emsk_freertos_demo.jpg)
 *
 * ### Extra Comments
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_FREERTOS_DEMO
 * \brief	main source file of the freertos demo
 */

/**
 * \addtogroup	EMBARC_APP_FREERTOS_DEMO
 * @{
 */

#include "embARC.h"
#include "embARC_debug.h"
#include "emsk_adc.h"

#define BOARD_TEMP_I2C_SLAVE_ADDRESS	TEMP_I2C_SLAVE_ADDRESS
#define BOARD_ADC_I2C_SLAVE_ADDRESS		EMSK_ADC_I2C_ADDR


static void task1(void * par);
static void task2(void * par);
static void trap_exception(void *p_excinf);

/**
 * \var		task1_handle
 * \brief	handle of task1
 * \details	If task handle is not used, set Null.
 */
static TaskHandle_t task1_handle = NULL;
/**
 * \var		task2_handle
 * \brief	handle of task2
 * \details	If task handle is not used, set Null.
 */
static TaskHandle_t task2_handle = NULL;

static unsigned int start = 0;


/**
 * \brief  call FreeRTOS API, creat and start tasks
 */
int main(void)
{
	int32_t ercd;

	cpu_lock();
	board_init(); /*!< board init */
	EMBARC_PRINTF("Initial000\r\n");
	ercd = temp_sensor_init(BOARD_TEMP_I2C_SLAVE_ADDRESS);
	if (ercd != E_OK)
		EMBARC_PRINTF("Temperature sensor open failed\r\n");

	ercd = emsk_adc_init(BOARD_ADC_I2C_SLAVE_ADDRESS);
	if (ercd != E_OK)
		EMBARC_PRINTF("ADC sensor open failed\r\n");

	os_hal_exc_init();	/*!< exception initialization for OS to be called before any interrupt or exception */


	xTaskCreate(task1, "task1", 256, (void *)1, configMAX_PRIORITIES, &task1_handle);	/*!< FreeRTOS xTaskCreate() API function */
	xTaskCreate(task2, "task2", 256, (void *)2, configMAX_PRIORITIES, &task2_handle);	/*!< FreeRTOS xTaskCreate() API function */
	vTaskStartScheduler(); /*!< start the scheduler to execute tasks */

	return 0;
}


/**
 * \brief  task1 in FreeRTOS
 * \details Call vTaskDelayUntil() to execute taks1 with a fixed period 1 second.
 * \param[in] *par
 */
static void task1(void * par)
{
	unsigned int t;
	int32_t adc_val;
	TickType_t xLastExecutionTime;
	xLastExecutionTime = xTaskGetTickCount();	/*!< initialize current tick */
	while(1) {
		if (emsk_adc_read(&adc_val) == E_OK) {
			EMBARC_PRINTF("Current ADC: %d \r\n", (int)(adc_val));
		} else {
			EMBARC_PRINTF("Unable to read temperature sensor value, please check it!\r\n");
		}
		vTaskDelayUntil( &xLastExecutionTime, 1000);	/*!< This task should execute exactly every 1 second. */
	}

}

/**
 * \brief  task2 in FreeRTOS
 * \details Print information in task2 and suspend task2.
 * \param[in] *par
 */
static void task2(void * par)
{
	unsigned int t;
	int32_t temp_val;
	TickType_t xLastExecutionTime;
	xLastExecutionTime = xTaskGetTickCount();	/*!< initialize current tick */

	while(1) {
		if (temp_sensor_read(&temp_val) == E_OK)
			EMBARC_PRINTF("Current temperature: %d.%dC\r\n", (int)(temp_val/10), (int)(temp_val%10));
		else
			EMBARC_PRINTF("Unable to read temperature sensor value, please check it!\r\n");
		//EMBARC_PRINTF("int->task2:%d cycles\r\n", t);
		vTaskDelayUntil( &xLastExecutionTime, 5000);	/*!< This task should execute exactly every 5 second. */
	}
}

/**
 * \brief  trap exception
 * \details Call xTaskResumeFromISR() to resume task2 that can be called from within ISR.
 * If resuming the task2 should result in a context switch, call vPortYieldFromIsr() to generate task switch requrest.
 * \param[in] *p_excinf
 */
static void trap_exception(void *p_excinf)
{
	// show exception frame
	unsigned int t;
	t = perf_end();
	EMBARC_PRINTF("int->nest int:%d cycles\r\n", t);
	perf_start();
	if(xTaskResumeFromISR(task2_handle) == pdTRUE)	/*!< resume the suspended task2 */
		vPortYieldFromIsr();
}

/** @} */
