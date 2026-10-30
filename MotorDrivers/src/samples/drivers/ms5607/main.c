/**
 * @file    main.c
 * @brief   MS5607 Barometer Example
 * @version 1.0.0
 * @date    31.12.2024
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>
#include <math.h>

/* Peripherals */
#include "common.h"
#include "gpio.h"
#include "util.h"

/* External hardware drivers */
#include "ms5607_i2c.h"
#include "ms5607_spi.h"
#include "ms5607.h"

/* Lib */

/* Middleware */
#include "FreeRTOS.h"
#include "task.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

// #define MS5607_USE_SPI
#define MS5607_USE_I2C

#ifdef MS5607_USE_I2C
#define MS5607_I2C_INSTANCE I2C_HAL_INSTANCE_0
#endif

#ifdef MS5607_USE_SPI
#define MS5607_SPI_INSTANCE SPI_HAL_INSTANCE_0
#define MS5607_SPI_CS_PORT  GPIO_HAL_INSTANCE_0
#define MS5607_SPI_CS_PIN   16
#endif

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* FreeRTOS */
uint8_t ucHeap[configTOTAL_HEAP_SIZE];
volatile uint32_t ulIdleCycleCount = 0UL;

/* Task Blinky Variables */
uint32_t blinky_dt = 500; /* [ms] */

/* Task MS5607 Variables */
ms5607_t ms5607;
ms5607_i2c_interface_t ms5607_i2c;
ms5607_spi_interface_t ms5607_spi;
float press, temp;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void task_blinky(void *arg);

void task_ms5607(void *arg);

/*******************************************************************************
 * Code
 ******************************************************************************/
int main()
{
	/*--------------------------------- NVIC ---------------------------------*/
	HAL_NVIC_SetPriority(SysTick_IRQn, 31);
	HAL_NVIC_SetPriority(PendSV_IRQn, 30);

	/*------------------------------- Dev Time -------------------------------*/
	HAL_DevTimeInit(TIM_HAL_INSTANCE_0);

	/*--------------------------------- GPIO ---------------------------------*/
	gpio_hal_cfg_type_t gpio_cfg;
	gpio_cfg.openDrain = GPIO_HAL_OPENDRAIN_NORMAL;
	gpio_cfg.pinMode = GPIO_HAL_PINMODE_PULLUP;
	gpio_cfg.pinDir = GPIO_HAL_OUTPUT;

	HAL_GPIO_Init(GPIO_HAL_INSTANCE_3, 25, &gpio_cfg);
	HAL_GPIO_Init(GPIO_HAL_INSTANCE_3, 26, &gpio_cfg);

    #ifdef MS5607_USE_SPI
    /*--------------------------------- SPI ----------------------------------*/
	spi_hal_cfg_t spi_cfg;
  	HAL_SPI_ConfigStructInit(&spi_cfg);

	spi_cfg.clockRate = 10000000;
	spi_cfg.databit = SPI_HAL_DATABIT_8;

	HAL_SPI_Init(MS5607_SPI_INSTANCE, &spi_cfg);

    #endif

    #ifdef MS5607_USE_I2C
    /*--------------------------------- I2C ----------------------------------*/
    HAL_I2C_Init(MS5607_I2C_INSTANCE, 400000);
    #endif

	/*------------------------------- FreeRTOS -------------------------------*/
	xTaskCreate(task_blinky, "led_red", configMINIMAL_STACK_SIZE, &blinky_dt, 1,
				NULL);

    xTaskCreate(task_ms5607, "MS5607", 256, NULL, 2, NULL);

	vTaskStartScheduler();

	/* loop */
	while (1) {
	}
}

/* ================================== Tasks ================================= */

void task_blinky(void *arg)
{
	uint32_t *period_ms = (uint32_t *)arg;
	TickType_t last_wake = xTaskGetTickCount();
	uint8_t led_state = 0;

	for (;;) {
		vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(*period_ms));

		if (led_state)
			HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_3, 25, 0);
		else
			HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_3, 25, 1);

		led_state = !led_state;
	}
}

void task_ms5607(void *arg)
{
	TickType_t last_wake = xTaskGetTickCount();
    lStatus_t status = lStatus_Fail;
	uint32_t t;

    #ifdef MS5607_USE_I2C
    // I2C Physical layer
	ms5607_i2c_init(&ms5607_i2c, MS5607_I2C_INSTANCE, 0);
    #endif

    #ifdef MS5607_USE_SPI
	// SPI Physical layer
	spi_hal_slave_t baro_slave;
	spi_hal_slave_id_t baro_slave_id;

	baro_slave.portNum = MS5607_SPI_CS_PORT;
	baro_slave.pinNum = MS5607_SPI_CS_PIN;

	baro_slave_id = HAL_SPI_SlaveInit(MS5607_SPI_INSTANCE, &baro_slave);

	ms5607_spi_init(&ms5607_spi, MS5607_SPI_INSTANCE, baro_slave_id);
    #endif

    // Sensor Init
    #ifdef MS5607_USE_I2C
    portENTER_CRITICAL();
	ms5607_init(&ms5607, (ms5607_interface_t *)&ms5607_i2c);
    portEXIT_CRITICAL();
    #endif

    #ifdef MS5607_USE_SPI
    ms5607_init(&ms5607, (ms5607_interface_t *)&ms5607_spi);
    #endif

    /**
     *  @note Critical sections are added because of known issue with I2C HAL. 
     */
	for ( ;; ) {
		// Downrate measurements from 70 to 50 Hz
		vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(20));

		/*--------------------------- Temperature ----------------------------*/
		portENTER_CRITICAL();
		status = ms5607_measure(&ms5607, MS5607_MEAS_TEMPERATURE,
                                MS5607_OSR_2048, &t);
		portEXIT_CRITICAL();

		if (status != lStatus_Success)
			continue;

        vTaskDelay(pdMS_TO_TICKS((TickType_t)ceil(t/1000.0f)));

		portENTER_CRITICAL();
		status = ms5607_collect(&ms5607, MS5607_MEAS_TEMPERATURE, &temp);
		portEXIT_CRITICAL();

		if (status != lStatus_Success)
			continue;

		/*----------------------------- Pressure -----------------------------*/
		portENTER_CRITICAL();
		status = ms5607_measure(&ms5607, MS5607_MEAS_PRESSURE, MS5607_OSR_4096,
                                &t);
		portEXIT_CRITICAL();

		if (status != lStatus_Success)
			continue;

        vTaskDelay(pdMS_TO_TICKS((TickType_t)ceil(t/1000.0f)));

		portENTER_CRITICAL();
		status = ms5607_collect(&ms5607, MS5607_MEAS_PRESSURE, &press);
		portEXIT_CRITICAL();
	}
}

/* ============================= User Callbacks ============================= */

/* ============================ Private Functions =========================== */

/* ============================= FreeRTOS Hooks ============================= */

/* Increment a counter while Application is in Idle. */
void vApplicationIdleHook(void) { ulIdleCycleCount++; }

/* Callback to trap FreeRTOS Misconfiguration. */
void vAssertCalled(const char *pcFile, uint32_t ulLine)
{
	taskDISABLE_INTERRUPTS();
	for (;;)
		;
}

/* Callback to trap FreeRTOS stack overflow. */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	taskDISABLE_INTERRUPTS();
	for (;;)
		;
}

/* Callback to trap FreeRTOS insufficient heap memory. */
void vApplicationMallocFailedHook(void)
{
	taskDISABLE_INTERRUPTS();
	for (;;)
		;
}