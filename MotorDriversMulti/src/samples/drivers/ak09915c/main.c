/**
 * @file    main.h
 * @brief   AK09915C Magnetometer Example
 * @version 1.0.0
 * @date    31.12.2024
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

/* Peripherals */
#include "common.h"
#include "gpio.h"
#include "util.h"

/* External hardware drivers */
#include "ak09915c.h"
#include "ak09915c_i2c.h"
#include "ak09915c_spi.h"

/* Lib */

/* Middleware */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

#define AK09915C_USE_SPI
// #define AK09915C_USE_I2C

#define AK09915C_INT_PORT GPIO_HAL_INSTANCE_0
#define AK09915C_INT_PIN 4

#define AK09915C_RST_PORT GPIO_HAL_INSTANCE_0
#define AK09915C_RST_PIN 5

#ifdef AK09915C_USE_SPI
#define AK09915C_SPI_INSTANCE SPI_HAL_INSTANCE_0
#define AK09915C_SPI_CS_PORT GPIO_HAL_INSTANCE_0
#define AK09915C_SPI_CS_PIN 16
#endif

#ifdef AK09915C_USE_I2C
#define AK09915C_I2C_INSTANCE I2C_HAL_INSTANCE_0
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

SemaphoreHandle_t semphr_mag;

/* Task Blinky Variables */
uint32_t blinky_dt = 500; /* [ms] */

/* Task AK09915C Variables */
ak09915c_t ak09915c;
ak09915c_i2c_interface_t ak09915c_i2c;
ak09915c_spi_interface_t ak09915c_spi;
float magx, magy, magz;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void task_blinky(void *arg);

void task_ak09915c(void *arg);

void gpio_cb(uint16_t pinNum);

/*******************************************************************************
 * Code
 ******************************************************************************/
int main()
{
	/*------------------------------- FreeRTOS -------------------------------*/
	semphr_mag = xSemaphoreCreateBinary();

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

	// AK09915C INT pin.
	gpio_cfg.pinMode = GPIO_HAL_PINMODE_PULLUP;
	gpio_cfg.pinDir = GPIO_HAL_INPUT;
	HAL_GPIO_Init(AK09915C_INT_PORT, AK09915C_INT_PIN, &gpio_cfg);

	// AK09915C RST pin.
	gpio_cfg.pinMode = GPIO_HAL_PINMODE_PULLUP;
	gpio_cfg.pinDir = GPIO_HAL_OUTPUT;
	HAL_GPIO_Init(AK09915C_RST_PORT, AK09915C_RST_PIN, &gpio_cfg);

	HAL_GPIO_ConfigureCallback(GPIO_HAL_INSTANCE_0, gpio_cb);

	HAL_GPIO_EnableInterrupt(AK09915C_INT_PORT, AK09915C_INT_PIN,
							 GPIO_HAL_EDGESTATE_RISING);

#ifdef AK09915C_USE_I2C
	/*---------------------------------- I2C ---------------------------------*/
	HAL_I2C_Init(AK09915C_I2C_INSTANCE, 100000);
#endif

#ifdef AK09915C_USE_SPI
	/*--------------------------------- SPI ----------------------------------*/
	spi_hal_cfg_t spi_cfg;
	HAL_SPI_ConfigStructInit(&spi_cfg);

	spi_cfg.clockRate = 10000000;
	spi_cfg.databit = SPI_HAL_DATABIT_8;

	HAL_SPI_Init(AK09915C_SPI_INSTANCE, &spi_cfg);
#endif

	/*------------------------------- FreeRTOS -------------------------------*/
	xTaskCreate(task_blinky, "led_red", configMINIMAL_STACK_SIZE, &blinky_dt, 1,
				NULL);

	xTaskCreate(task_ak09915c, "AK09915C", 256, NULL, 2, NULL);

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

void task_ak09915c(void *arg)
{
	lStatus_t status = lStatus_Success;

	HAL_GPIO_SetPinValue(AK09915C_RST_PORT, AK09915C_RST_PIN, 1);

	// Wait for device to boot.
	vTaskDelay(pdMS_TO_TICKS(100));

#ifdef AK09915C_USE_I2C
	// I2C Physical layer
	ak09915c_i2c_init(&ak09915c_i2c, AK09915C_I2C_INSTANCE, 0, 0);
#endif

#ifdef AK09915C_USE_SPI
	// SPI Physical layer
	spi_hal_slave_t imu_slave;
	spi_hal_slave_id_t imu_slave_id;

	imu_slave.portNum = AK09915C_SPI_CS_PORT;
	imu_slave.pinNum = AK09915C_SPI_CS_PIN;

	imu_slave_id = HAL_SPI_SlaveInit(AK09915C_SPI_INSTANCE, &imu_slave);

	ak09915c_spi_init(&ak09915c_spi, AK09915C_SPI_INSTANCE, imu_slave_id);
#endif

	// Sensor Init
	ak09915c_cfg_t dev_cfg = {.noise_filt = lFunctionalState_Enable,
							  .op_mode = AK09915C_OP_MODE_CONTINUOUS3,
							  .sdr = AK09915C_SDR_LOW_NOISE};

#ifdef AK09915C_USE_I2C
	portENTER_CRITICAL();
	status = ak09915c_init(&ak09915c, (ak09915c_interface_t *)&ak09915c_i2c,
						   &dev_cfg);
	portEXIT_CRITICAL();
#endif

#ifdef AK09915C_USE_SPI
	status = ak09915c_init(&ak09915c, (ak09915c_interface_t *)&ak09915c_spi,
						   &dev_cfg);
#endif

	if (status != lStatus_Success)
		vTaskDelete(NULL);

	/**
	 *  @note Critical sections are added because of known issue with I2C HAL.
	 */
	portENTER_CRITICAL();
	ak09915c_sample_get(&ak09915c, &magx, &magy, &magz);
	portEXIT_CRITICAL();

	for (;;) {
		xSemaphoreTake(semphr_mag, portMAX_DELAY);

		portENTER_CRITICAL();
		ak09915c_sample_get(&ak09915c, &magx, &magy, &magz);
		portEXIT_CRITICAL();
	}
}

/* ============================= User Callbacks ============================= */

void gpio_cb(uint16_t pinNum)
{
	BaseType_t taskWoken = pdFALSE;

	if (HAL_GPIO_GetIntStatus(AK09915C_INT_PORT, AK09915C_INT_PIN,
							  GPIO_HAL_EDGESTATE_RISING)) {
		xSemaphoreGiveFromISR(semphr_mag, &taskWoken);
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