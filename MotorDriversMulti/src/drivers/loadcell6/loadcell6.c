/**
 * @file    load6cell.c
 * @brief   Load Cell 6 Source code.
 * @version 1.0.0
 * @date    17.10.2025
 * @author  LisumLab
 */

/*!
 * @file loadcell6.c
 * @brief Load Cell 6 Click Driver for LPC1768.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "loadcell6.h"
#include "uart.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/**
 * @brief Dummy data.
 * @details Definition of dummy data.
 */
#define DUMMY 0x00

/**
 * @brief SPI bit masks.
 * @details Definition of bit masks data.
 */
#define SPI_REG_ADDR_BIT_MASK 0x1F
#define SPI_READ_BIT_MASK 0x01	// | set
#define SPI_WRITE_BIT_MASK 0xFE //~& reset
#define CMD_CONVERSION_MODE 0xBF
#define DATA_24_BIT_MASK 0x00FFFFFF

#define LOADCELL6_REG_CTRL1 0x01	// CTRL1 registar
#define LOADCELL6_CTRL1_CONTSC 0x40 // CONTSC bit (B6)
#define LOADCELL6_DR_1KSPS 0x09		// RATE[3:0] za 1 kSPS

#define LOADCELL6_CMD_REG_WRITE 0x40 // MODE=1 + START=1 + R/W=0 (primer)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/**
 * @brief Load Cell 6 measure delay function.
 * @details This function executes a measure delay of 1 microsecond.
 * @return Nothing.
 * @note Uses LPC1768 HAL utility timer-based delay function.
 */
static void dev_measure_delay(void);

/**
 * @brief Load Cell 6 reset delay function.
 * @details This function executes a reset delay of 100 milliseconds.
 * @return Nothing.
 * @note Uses LPC1768 HAL utility timer-based delay function.
 */
static void dev_reset_delay(void);

/**
 * @brief Load Cell 6 sync delay function.
 * @details This function executes a sync delay of 10 milliseconds.
 * @return Nothing.
 * @note Uses LPC1768 HAL utility timer-based delay function.
 */
static void dev_sync_delay(void);

/**
 * @brief Load Cell 6 config delay function.
 * @details This function executes a configuration delay of 10 milliseconds.
 * @return Nothing.
 * @note Uses LPC1768 HAL utility timer-based delay function.
 */
static void dev_config_delay(void);

/*******************************************************************************
 * API
 ******************************************************************************/

loadcell6_return_value_t loadcell6_init(loadcell6_t* ctx)
{
	lStatus_t status;
	spi_hal_cfg_t spi_cfg;
	gpio_hal_cfg_type_t gpio_cfg;

	if (!ctx) {
		return LOADCELL6_ERROR_INVALID_ARG;
	}

	// --- SPI configuration ---
	spi_cfg.databit = ctx->data_bits;
	spi_cfg.cpha = ctx->cpha;
	spi_cfg.cpol = ctx->cpol;
	spi_cfg.mode = ctx->mode;

	// SYN pin (input)
	gpio_cfg.pinDir = GPIO_HAL_OUTPUT;
	gpio_cfg.pinMode = GPIO_HAL_PINMODE_PULLUP;
	gpio_cfg.openDrain = GPIO_HAL_OPENDRAIN_NORMAL;
	status = HAL_GPIO_Init(ctx->syn_port, ctx->syn_pin, &gpio_cfg);
	if (status != lStatus_Success) {
		return LOADCELL6_ERROR_GPIO;
	}

	HAL_GPIO_SetPinValue(ctx->syn_port, ctx->syn_pin, GPIO_HAL_SET);

	// RDY pin
	gpio_cfg.pinDir = GPIO_HAL_INPUT;
	gpio_cfg.pinMode = GPIO_HAL_PINMODE_PULLUP;
	gpio_cfg.openDrain = GPIO_HAL_OPENDRAIN_NORMAL;
	status = HAL_GPIO_Init(ctx->rdy_port, ctx->rdy_pin, &gpio_cfg);
	if (status != lStatus_Success) {
		return LOADCELL6_ERROR_GPIO;
	}

	// RST pin (output)
	gpio_cfg.pinDir = GPIO_HAL_OUTPUT;
	gpio_cfg.pinMode = GPIO_HAL_PINMODE_TRISTATE;
	gpio_cfg.openDrain = GPIO_HAL_OPENDRAIN_NORMAL;
	status = HAL_GPIO_Init(ctx->rst_port, ctx->rst_pin, &gpio_cfg);
	if (status != lStatus_Success) {
		return LOADCELL6_ERROR_GPIO;
	}

	HAL_GPIO_SetPinValue(ctx->rst_port, ctx->rst_pin, GPIO_HAL_SET);

	// CS pin (output)
	gpio_cfg.pinDir = GPIO_HAL_OUTPUT;
	gpio_cfg.pinMode = GPIO_HAL_PINMODE_TRISTATE;
	gpio_cfg.openDrain = GPIO_HAL_OPENDRAIN_NORMAL;
	status = HAL_GPIO_Init(ctx->cs_port, ctx->cs_pin, &gpio_cfg);
	if (status != lStatus_Success) {
		return LOADCELL6_ERROR_GPIO;
	}

	// Set CS high (deselected)
	LOADCELL6_CS_DEASSERT(ctx);

	ctx->initialized = true;

	return LOADCELL6_OK;
}

loadcell6_return_value_t loadcell6_default_cfg(loadcell6_t* ctx)
{
	loadcell6_return_value_t error_flag = LOADCELL6_OK;

	/* Enable PGA and set x1 gain */
	error_flag =
		loadcell6_write_reg(ctx, LOADCELL6_REG_CTRL2,
							LOADCELL6_CTRL2_PGAGEN | LOADCELL6_CTRL2_PGAIN_x32);

	if (error_flag != LOADCELL6_OK)
		return error_flag;
	dev_config_delay();

	error_flag =
		loadcell6_write_reg(ctx, LOADCELL6_REG_CTRL3, LOADCELL6_CTRL3_RESERVED);

	if (error_flag != LOADCELL6_OK)
		return error_flag;
	dev_config_delay();

	error_flag =
		loadcell6_write_reg(ctx, LOADCELL6_REG_CTRL4, LOADCELL6_CTRL4_DIR1);

	if (error_flag != LOADCELL6_OK)
		return error_flag;
	dev_config_delay();

	error_flag =
		loadcell6_write_reg(ctx, LOADCELL6_REG_CTRL5,
							LOADCELL6_CTRL5_NOSYSG | LOADCELL6_CTRL5_NOSYSO |
								LOADCELL6_CTRL5_NOSCG | LOADCELL6_CTRL5_NOSCO);

	if (error_flag != LOADCELL6_OK)
		return error_flag;
	dev_reset_delay();

	// Set DATA RATE 2ksps
	error_flag = loadcell6_set_rate_speed(ctx, LOADCELL6_1KSPS);
	if (error_flag != LOADCELL6_OK)
		return error_flag;

	dev_config_delay();

	return error_flag;
}

loadcell6_return_value_t loadcell6_hw_reset(loadcell6_t* ctx)
{
	if (!ctx->initialized) {
		return LOADCELL6_ERROR_CONFIG;
	}

	// Reset pulse - active low
	HAL_GPIO_SetPinValue(ctx->rst_port, ctx->rst_pin, GPIO_HAL_RESET);
	dev_reset_delay();
	HAL_GPIO_SetPinValue(ctx->rst_port, ctx->rst_pin, GPIO_HAL_SET);
	dev_reset_delay();

	return LOADCELL6_OK;
}

uint8_t loadcell6_sync(loadcell6_t* ctx)
{
	return HAL_GPIO_GetPinValue(ctx->syn_port, ctx->syn_pin);
}

uint8_t loadcell6_check_data_ready(loadcell6_t* ctx)
{
	return HAL_GPIO_GetPinValue(ctx->rdy_port, ctx->rdy_pin);
}

loadcell6_return_value_t loadcell6_generic_write(loadcell6_t* ctx, uint8_t reg,
												 uint8_t* data_in, uint8_t len)
{
	lStatus_t status;
	uint8_t tx_buf[257] = {0};

	if (!ctx->initialized) {
		return LOADCELL6_ERROR_CONFIG;
	}

	tx_buf[0] = reg;
	for (uint8_t cnt = 1; cnt <= len; cnt++) {
		tx_buf[cnt] = data_in[cnt - 1];
	}

	// Select device (CS low)
	LOADCELL6_CS_ASSERT(ctx);

	status = HAL_SPI_Transmit(ctx->spi_instance, tx_buf, len + 1, 1000);

	// Deselect device (CS high)
	LOADCELL6_CS_DEASSERT(ctx);

	return (status == lStatus_Success) ? LOADCELL6_OK : LOADCELL6_ERROR_SPI;
}

loadcell6_return_value_t loadcell6_generic_read(loadcell6_t* ctx, uint8_t reg,
												uint8_t* data_out, uint8_t len)
{
	lStatus_t status;

	if (!ctx->initialized) {
		return LOADCELL6_ERROR_CONFIG;
	}

	// Select device (CS low)
	LOADCELL6_CS_ASSERT(ctx);

	// Send register address
	status = HAL_SPI_Transmit(ctx->spi_instance, &reg, 1, 1000);
	if (status == lStatus_Success) {
		// Read data
		status = HAL_SPI_Receive(ctx->spi_instance, data_out, len, 1000);
	}

	// Deselect device (CS high)
	LOADCELL6_CS_DEASSERT(ctx);

	return (status == lStatus_Success) ? LOADCELL6_OK : LOADCELL6_ERROR_SPI;
}

loadcell6_return_value_t loadcell6_write_reg(loadcell6_t* ctx, uint8_t reg,
											 uint8_t data_in)
{
	lStatus_t status;
	uint8_t tx_buf[2];

	if (!ctx->initialized) {
		return LOADCELL6_ERROR_CONFIG;
	}

	reg &= SPI_REG_ADDR_BIT_MASK;

	tx_buf[0] = reg << 1;
	tx_buf[0] |= LOADCELL6_CMD_START | LOADCELL6_CMD_REG_ACCESS_MODE;
	tx_buf[0] &= SPI_WRITE_BIT_MASK;

	tx_buf[1] = data_in;

	// Select device (CS low)
	LOADCELL6_CS_ASSERT(ctx);

	status = HAL_SPI_Transmit(ctx->spi_instance, tx_buf, 2, 1000);

	// Deselect device (CS high)
	LOADCELL6_CS_DEASSERT(ctx);

	return (status == lStatus_Success) ? LOADCELL6_OK : LOADCELL6_ERROR_SPI;
}

loadcell6_return_value_t loadcell6_set_rate_speed(loadcell6_t* ctx,
												  uint8_t rate)
{
	lStatus_t status;
	uint8_t tx_buf;

	if (!ctx->initialized) {
		return LOADCELL6_ERROR_CONFIG;
	}

	tx_buf = 0x89;

	// Select device (CS low)
	LOADCELL6_CS_ASSERT(ctx);

	status = HAL_SPI_Transmit(ctx->spi_instance, &tx_buf, 1, 1000);

	// Deselect device (CS high)
	LOADCELL6_CS_DEASSERT(ctx);

	return (status == lStatus_Success) ? LOADCELL6_OK : LOADCELL6_ERROR_SPI;
}

loadcell6_return_value_t loadcell6_set_rate_contsc(loadcell6_t* ctx,
												   uint8_t rate)
{
	lStatus_t status;
	uint8_t tx_buf[2];

	if (!ctx->initialized) {
		return LOADCELL6_ERROR_CONFIG;
	}

	// Formiraj podatke: DATA = CONTSC bit + RATE
	uint8_t data = LOADCELL6_CTRL1_CONTSC | (rate & 0x0F);

	// Adresa CTRL1 registra + write
	tx_buf[0] = LOADCELL6_REG_CTRL1 << 1; // RS[4:0] u B5..B1, R/W=0
	tx_buf[1] = data;

	// Start SPI transaction
	LOADCELL6_CS_ASSERT(ctx);
	status = HAL_SPI_Transmit(ctx->spi_instance, tx_buf, 2, 1000);
	LOADCELL6_CS_DEASSERT(ctx);

	return (status == lStatus_Success) ? LOADCELL6_OK : LOADCELL6_ERROR_SPI;
}

loadcell6_return_value_t loadcell6_read_data(loadcell6_t* ctx, uint8_t reg,
											 uint8_t* data_out)
{
	lStatus_t status;

	if (!ctx->initialized) {
		return LOADCELL6_ERROR_CONFIG;
	}

	uint8_t rx_buf;
	uint8_t tx_buf;

	tx_buf = reg & SPI_REG_ADDR_BIT_MASK;
	tx_buf = reg << 1;
	tx_buf |= LOADCELL6_CMD_START | LOADCELL6_CMD_REG_ACCESS_MODE;
	tx_buf |= SPI_READ_BIT_MASK;

	// Select device (CS low)
	LOADCELL6_CS_ASSERT(ctx);

	// Send address of register
	status = HAL_SPI_Transmit(ctx->spi_instance, &tx_buf, 1, 1000);
	if (status != lStatus_Success) {
		LOADCELL6_CS_DEASSERT(ctx);
		return LOADCELL6_ERROR_SPI;
	}

	// Receive 1 byte
	status = HAL_SPI_Receive(ctx->spi_instance, &rx_buf, 1, 1000);
	if (status != lStatus_Success) {
		LOADCELL6_CS_DEASSERT(ctx);
		return LOADCELL6_ERROR_SPI;
	}

	// Select device (CS low)
	LOADCELL6_CS_DEASSERT(ctx);

	*data_out = rx_buf;

	return (status == lStatus_Success) ? LOADCELL6_OK : LOADCELL6_ERROR;
}

loadcell6_return_value_t loadcell6_write_reg_data(loadcell6_t* ctx, uint8_t reg,
												  uint32_t data_in)
{
	lStatus_t status;

	if (!ctx->initialized) {
		return LOADCELL6_ERROR_CONFIG;
	}
	uint8_t tx_buf[4];

	data_in &= DATA_24_BIT_MASK;

	reg &= SPI_REG_ADDR_BIT_MASK;
	reg <<= 1;
	reg |= LOADCELL6_CMD_START | LOADCELL6_CMD_REG_ACCESS_MODE;
	reg |= SPI_WRITE_BIT_MASK;

	tx_buf[0] = reg;
	tx_buf[1] = (uint8_t)(data_in >> 16);
	tx_buf[2] = (uint8_t)(data_in >> 8);
	tx_buf[3] = (uint8_t)data_in;

	// Select device (CS low)
	LOADCELL6_CS_ASSERT(ctx);

	status = HAL_SPI_Transmit(ctx->spi_instance, tx_buf, 4, 1000);

	// Deselect device (CS high)
	LOADCELL6_CS_DEASSERT(ctx);

	return (status == lStatus_Success) ? LOADCELL6_OK : LOADCELL6_ERROR_SPI;
}

loadcell6_return_value_t loadcell6_read_reg_data(loadcell6_t* ctx, uint8_t reg,
												 uint32_t* data_out)
{
	lStatus_t status;

	if (!ctx->initialized) {
		return LOADCELL6_ERROR_CONFIG;
	}

	uint8_t rx_buf[3];

	reg &= SPI_REG_ADDR_BIT_MASK;
	reg <<= 1;
	reg |= LOADCELL6_CMD_START | LOADCELL6_CMD_REG_ACCESS_MODE;
	reg |= SPI_READ_BIT_MASK;

	// Select device (CS low)
	LOADCELL6_CS_ASSERT(ctx);

	// Send adress of register
	status = HAL_SPI_Transmit(ctx->spi_instance, &reg, 1, 1000);
	if (status != lStatus_Success) {
		LOADCELL6_CS_DEASSERT(ctx);
		return LOADCELL6_ERROR_SPI;
	}

	// Receive 3 bytes
	status = HAL_SPI_Receive(ctx->spi_instance, rx_buf, 3, 1000);
	if (status != lStatus_Success) {
		LOADCELL6_CS_DEASSERT(ctx);
		return LOADCELL6_ERROR_SPI;
	}

	// Select device (CS low)
	LOADCELL6_CS_DEASSERT(ctx);

	*data_out = ((uint32_t)rx_buf[0] << 16) | ((uint32_t)rx_buf[1] << 8) |
				((uint32_t)rx_buf[2]);

	return (status == lStatus_Success) ? LOADCELL6_OK : LOADCELL6_ERROR;
}

loadcell6_return_value_t loadcell6_set_command(loadcell6_t* ctx, uint8_t cmd)
{
	lStatus_t status;

	cmd |= LOADCELL6_CMD_START;
	cmd &= CMD_CONVERSION_MODE;

	// Select device (CS low)
	LOADCELL6_CS_ASSERT(ctx);

	status = HAL_SPI_Transmit(ctx->spi_instance, &cmd, 1, 1000);

	// Select device (CS low)
	LOADCELL6_CS_DEASSERT(ctx);

	return (status == lStatus_Success) ? LOADCELL6_OK : LOADCELL6_ERROR;
}

loadcell6_return_value_t loadcell6_get_status(loadcell6_t* ctx,
											  uint16_t* status)
{
	uint8_t rx_buf[2];

	loadcell6_return_value_t error_flag = loadcell6_generic_read(
		ctx,
		LOADCELL6_CMD_START | LOADCELL6_CMD_REG_ACCESS_MODE |
			SPI_READ_BIT_MASK | LOADCELL6_REG_STAT,
		rx_buf, 2);

	*status = rx_buf[0];
	*status <<= 8;
	*status |= rx_buf[1];

	return error_flag;
}

loadcell6_return_value_t loadcell6_read_adc(loadcell6_t* ctx,
											uint32_t* data_out)
{
	uint16_t status = 0xFFFF;
	uint8_t cmd = 0;

	/* Speed of sampling !!!! */
	cmd |= LOADCELL6_CMD_START;
	cmd |= LOADCELL6_1KSPS; // Change data rate here !!!!!

	loadcell6_return_value_t error_flag = loadcell6_set_command(ctx, cmd);

	do {
		loadcell6_get_status(ctx, &status);
	} while ((status & LOADCELL6_STAT_RDY) != 1);
	while (loadcell6_check_data_ready(ctx) == 1)
		;
	error_flag |= loadcell6_read_reg_data(ctx, LOADCELL6_REG_DATA, data_out);

	return error_flag;
}

loadcell6_return_value_t loadcell6_tare(loadcell6_t* ctx,
										loadcell6_data_t* cell_data)
{
	uint32_t result;
	uint32_t sum_val;
	float average_val;
	loadcell6_return_value_t error_flag = LOADCELL6_OK;

	for (uint8_t n_cnt = 0; n_cnt < 5; n_cnt++) {
		error_flag |= loadcell6_read_adc(ctx, &result);
		dev_measure_delay();
	}

	sum_val = 0;

	for (uint8_t n_cnt = 0; n_cnt < 100; n_cnt++) {
		error_flag |= loadcell6_read_adc(ctx, &result);
		sum_val += result;
		dev_measure_delay();
	}

	average_val = (float)sum_val;
	average_val /= 100.0;

	cell_data->tare = average_val;
	cell_data->tare_ok = LOADCELL6_DATA_OK;
	cell_data->weight_data_100g_ok = LOADCELL6_DATA_NO_DATA;
	cell_data->weight_data_200g_ok = LOADCELL6_DATA_NO_DATA;
	cell_data->weight_data_500g_ok = LOADCELL6_DATA_NO_DATA;
	cell_data->weight_data_1000g_ok = LOADCELL6_DATA_NO_DATA;
	cell_data->weight_data_5000g_ok = LOADCELL6_DATA_NO_DATA;
	cell_data->weight_data_10000g_ok = LOADCELL6_DATA_NO_DATA;

	return error_flag;
}

loadcell6_return_value_t loadcell6_calibration(loadcell6_t* ctx,
											   uint16_t cal_val,
											   loadcell6_data_t* cell_data)
{
	uint32_t result;
	uint32_t sum_val;
	float average_val;
	float tare_val;
	float weight_val;
	loadcell6_return_value_t error_flag = LOADCELL6_OK;

	tare_val = cell_data->tare;

	sum_val = 0;

	for (uint8_t n_cnt = 0; n_cnt < 100; n_cnt++) {
		error_flag |= loadcell6_read_adc(ctx, &result);
		sum_val += result;
		dev_measure_delay();
	}

	average_val = (float)sum_val;
	average_val /= 100.0;

	weight_val = average_val - tare_val;

	switch (cal_val) {
	case LOADCELL6_WEIGHT_100G: {
		cell_data->weight_coeff_100g = 100.0 / weight_val;
		cell_data->weight_data_100g_ok = LOADCELL6_DATA_OK;
		break;
	}
	case LOADCELL6_WEIGHT_200G: {
		cell_data->weight_coeff_200g = 200.0 / weight_val;
		cell_data->weight_data_200g_ok = LOADCELL6_DATA_OK;
		break;
	}
	case LOADCELL6_WEIGHT_500G: {
		cell_data->weight_coeff_500g = 500.0 / weight_val;
		cell_data->weight_data_500g_ok = LOADCELL6_DATA_OK;
		break;
	}
	case LOADCELL6_WEIGHT_1000G: {
		cell_data->weight_coeff_1000g = 1000.0 / weight_val;
		cell_data->weight_data_1000g_ok = LOADCELL6_DATA_OK;
		break;
	}
	case LOADCELL6_WEIGHT_5000G: {
		cell_data->weight_coeff_5000g = 5000.0 / weight_val;
		cell_data->weight_data_5000g_ok = LOADCELL6_DATA_OK;
		break;
	}
	case LOADCELL6_WEIGHT_10000G: {
		cell_data->weight_coeff_10000g = 10000.0 / weight_val;
		cell_data->weight_data_10000g_ok = LOADCELL6_DATA_OK;
		break;
	}
	default: {
		error_flag = LOADCELL6_ERROR;
		cell_data->weight_data_100g_ok = LOADCELL6_DATA_NO_DATA;
		cell_data->weight_data_200g_ok = LOADCELL6_DATA_NO_DATA;
		cell_data->weight_data_500g_ok = LOADCELL6_DATA_NO_DATA;
		cell_data->weight_data_1000g_ok = LOADCELL6_DATA_NO_DATA;
		cell_data->weight_data_5000g_ok = LOADCELL6_DATA_NO_DATA;
		cell_data->weight_data_10000g_ok = LOADCELL6_DATA_NO_DATA;
		break;
	}
	}

	return error_flag;
}

loadcell6_return_value_t loadcell6_get_weight(loadcell6_t* ctx,
											  loadcell6_data_t* cell_data,
											  float* weight_g)
{
	uint32_t result;
	uint32_t sum_val;
	float average_val;
	float tare_val;
	float weight_val;
	loadcell6_return_value_t error_flag = LOADCELL6_OK;

	tare_val = cell_data->tare;

	sum_val = 0;

	for (uint8_t n_cnt = 0; n_cnt < 100; n_cnt++) {
		error_flag |= loadcell6_read_adc(ctx, &result);

		sum_val += result;

		dev_measure_delay();
	}

	average_val = (float)sum_val;
	average_val /= 100.0;

	*weight_g = average_val - tare_val;

	if (cell_data->weight_data_100g_ok == LOADCELL6_DATA_OK) {
		*weight_g *= cell_data->weight_coeff_100g;
	}

	if (cell_data->weight_data_200g_ok == LOADCELL6_DATA_OK) {
		*weight_g *= cell_data->weight_coeff_200g;
	}

	if (cell_data->weight_data_500g_ok == LOADCELL6_DATA_OK) {
		*weight_g *= cell_data->weight_coeff_500g;
	}

	if (cell_data->weight_data_1000g_ok == LOADCELL6_DATA_OK) {
		*weight_g *= cell_data->weight_coeff_1000g;
	}

	if (cell_data->weight_data_5000g_ok == LOADCELL6_DATA_OK) {
		*weight_g *= cell_data->weight_coeff_5000g;
	}

	if (cell_data->weight_data_10000g_ok == LOADCELL6_DATA_OK) {
		*weight_g *= cell_data->weight_coeff_10000g;
	}

	if (*weight_g < 0) {
		*weight_g = LOADCELL6_WEIGHT_ZERO;
	}

	return error_flag;
}

static void dev_measure_delay(void)
{
	HAL_DelayUS(1); // 1 microsecond delay
}

static void dev_reset_delay(void)
{
	HAL_DelayMS(500); // 500 millisecond delay
}

static void dev_sync_delay(void)
{
	HAL_DelayMS(10); // 10 millisecond delay
}

static void dev_config_delay(void)
{
	HAL_DelayMS(10); // 10 millisecond delay
}