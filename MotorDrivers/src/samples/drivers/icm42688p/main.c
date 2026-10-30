/**
 * @file    main.c
 * @brief   ICM42688P Example.
 * @version 1.0.0
 * @date    24.01.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

/* Peripherals */
#include "common.h"
#include "gpio.h"
#include "spi.h"
#include "util.h"

/* External hardware drivers */
#include "icm42688p.h"
#include "icm42688p_spi.h"

/* Lib */

/* Middleware */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define ICM42688P_INT_PORT GPIO_HAL_INSTANCE_0
#define ICM42688P_INT_PIN 4

#define ICM42688P_SPI_INSTANCE SPI_HAL_INSTANCE_0
#define ICM42688P_SPI_CLK_RATE 20000000
#define ICM42688P_SPI_CS_PORT GPIO_HAL_INSTANCE_0
#define ICM42688P_SPI_CS_PIN 16

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* FreeRTOS */
uint8_t ucHeap[configTOTAL_HEAP_SIZE];
volatile uint32_t ulIdleCycleCount = 0UL;

// Semaphore Handles
SemaphoreHandle_t semphr_icm42688p;

// Queue Handle
QueueHandle_t mailbox_sync;

/* Task Blinky Variables */
uint32_t blinky_dt = 500; /* [ms] */

/* Task ICM42688P */
uint32_t err_cnt = 0;
float accel[3];
float gyro[3];
float temp;
icm42688p_int_status_t int_status;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void task_blinky(void *arg);

void task_icm42688p(void *arg);

void gpio_cb(uint16_t pinNum);

/*******************************************************************************
 * Code
 ******************************************************************************/
int main()
{
	/*------------------------------- FreeRTOS -------------------------------*/
	semphr_icm42688p = xSemaphoreCreateBinary();
	mailbox_sync = xQueueCreate(1, sizeof(uint8_t));

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

	HAL_GPIO_ConfigureCallback(GPIO_HAL_INSTANCE_0, gpio_cb);

	/*---------------------------------- SPI ---------------------------------*/
	spi_hal_cfg_t spi_cfg = {.cpha = SPI_HAL_CPHA_FIRST,
							 .cpol = SPI_HAL_CPOL_HI,
							 .clockRate = ICM42688P_SPI_CLK_RATE,
							 .dataOrder = SPI_HAL_DATA_MSB_FIRST,
							 .databit = SPI_HAL_DATABIT_8,
							 .mode = SPI_HAL_MASTER_MODE};

	HAL_SPI_Init(ICM42688P_SPI_INSTANCE, &spi_cfg);

	/*------------------------------- FreeRTOS -------------------------------*/
	xTaskCreate(task_blinky, "led_red", configMINIMAL_STACK_SIZE, &blinky_dt, 1,
				NULL);

	xTaskCreate(task_icm42688p, "icm42688p", configMINIMAL_STACK_SIZE, NULL, 1,
				NULL);

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
		xQueueOverwrite(mailbox_sync, &led_state);
	}
}

void task_icm42688p(void *arg)
{
	lStatus_t status = lStatus_Fail;
	icm42688p_t imu;

	/*----------------------------- SPI Interface ----------------------------*/
	icm42688p_spi_interface_t spi_if;
	spi_hal_slave_t imu_slave;
	spi_hal_slave_id_t imu_slave_id;

	imu_slave.portNum = 0;
	imu_slave.pinNum = 16;

	imu_slave_id = HAL_SPI_SlaveInit(ICM42688P_SPI_INSTANCE, &imu_slave);
	icm42688p_spi_init(&spi_if, ICM42688P_SPI_INSTANCE, imu_slave_id);

	/*------------------------------ ICM42688P -------------------------------*/
	icm42688p_cfg_t cfg = {
		.spi_mode = ICM42688P_SPI_MODE_0_3,
		.gyro_fs = ICM42688P_GYRO_FS_SEL_2000,
		.gyro_odr = ICM42688P_GYRO_ODR_200,
		.accel_fs = ICM42688P_ACCEL_FS_SEL_4,
		.accel_odr = ICM42688P_ACCEL_ODR_200,
	};

	status = icm42688p_init(&imu, (icm42688p_interface_t *)&spi_if, &cfg);
	if (status != lStatus_Success) {
		vTaskDelete(NULL);
	}

	// Configure ICM42688P Interrupts
	icm42688p_int_cfg_t int1_cfg = {
		.int_mode = ICM42688P_INT_MODE_PULSED,
		.int_drive = ICM42688P_INT_DRIVE_CIRCUIT_OPEN_DRAIN,
		.int_polarity = ICM42688P_INT_POLARITY_ACTIVE_HIGH,
		.int_type = ICM42688P_INT_TYPE_DATA_RDY};

	icm42688p_int_cfg_t int2_cfg = {
		.int_mode = ICM42688P_INT_MODE_PULSED,
		.int_drive = ICM42688P_INT_DRIVE_CIRCUIT_OPEN_DRAIN,
		.int_polarity = ICM42688P_INT_POLARITY_ACTIVE_HIGH,
		.int_type = ICM42688P_INT_TYPE_DISABLE};

	status = icm42688p_int_cfg(&imu, &int1_cfg, &int2_cfg);
	if (status != lStatus_Success) {
		vTaskDelete(NULL);
	}

	// Configure MCU Interrupt pin
	gpio_hal_cfg_type_t gpio_cfg;

	gpio_cfg.pinMode = GPIO_HAL_PINMODE_PULLUP;
	gpio_cfg.pinDir = GPIO_HAL_INPUT;
	HAL_GPIO_Init(ICM42688P_INT_PORT, ICM42688P_INT_PIN, &gpio_cfg);

	HAL_GPIO_EnableInterrupt(ICM42688P_INT_PORT, ICM42688P_INT_PIN,
							 GPIO_HAL_EDGESTATE_RISING);

	for (;;) {
		// Wait for Task Notification
		xSemaphoreTake(semphr_icm42688p, portMAX_DELAY);

		status = icm42688p_int_status(&imu, &int_status);
		if (status != lStatus_Success) {
			err_cnt++;
			continue;
		}

		status = icm42688p_temp_get(&imu, &temp);
		if (status != lStatus_Success) {
			err_cnt++;
			continue;
		}

		status = icm42688p_gyro_get(&imu, &gyro[0], &gyro[1], &gyro[2]);
		if (status != lStatus_Success) {
			err_cnt++;
			continue;
		}

		status = icm42688p_accel_get(&imu, &accel[0], &accel[1], &accel[2]);
		if (status != lStatus_Success) {
			err_cnt++;
			continue;
		}

		uint8_t led_state = 0;

		if (pdPASS == xQueuePeek(mailbox_sync, &led_state, 0))
			HAL_GPIO_SetPinValue(GPIO_HAL_INSTANCE_3, 26, led_state);
	}
}

/* ============================= User Callbacks ============================= */

void gpio_cb(uint16_t pinNum)
{
	BaseType_t taskWoken = pdFALSE;

	if (HAL_GPIO_GetIntStatus(ICM42688P_INT_PORT, ICM42688P_INT_PIN,
							  GPIO_HAL_EDGESTATE_RISING)) {
		xSemaphoreGiveFromISR(semphr_icm42688p, &taskWoken);
	}

	portYIELD_FROM_ISR(taskWoken);
}

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