cmake_minimum_required(VERSION 3.20)

project (lpc1768)

set(SOURCES
	drivers/lpc17xx_adc.c
	drivers/lpc17xx_can.c
	drivers/lpc17xx_clkpwr.c
	drivers/lpc17xx_dac.c
	drivers/lpc17xx_emac.c
	drivers/lpc17xx_exti.c
	drivers/lpc17xx_gpdma.c
	drivers/lpc17xx_gpio.c
	drivers/lpc17xx_i2c.c
	drivers/lpc17xx_i2s.c
	drivers/lpc17xx_iap.c
	drivers/lpc17xx_mcpwm.c
	drivers/lpc17xx_nvic.c
	drivers/lpc17xx_pinsel.c
	drivers/lpc17xx_pwm.c
	drivers/lpc17xx_qei.c
	drivers/lpc17xx_rit.c
	drivers/lpc17xx_rtc.c
	drivers/lpc17xx_spi.c
	drivers/lpc17xx_ssp.c
	drivers/lpc17xx_systick.c
	drivers/lpc17xx_timer.c
	drivers/lpc17xx_uart.c
	drivers/lpc17xx_wdt.c
	core/system_LPC17xx.c

	# insert hal libs
	HAL/adc.c
	HAL/can.c
	HAL/common.c
	HAL/eth.c
	HAL/gpio.c
	HAL/i2c.c
	HAL/iap.c
	HAL/pwm.c
	HAL/spi.c
	HAL/rit.c
	HAL/timer.c
	HAL/uart.c
	HAL/util.c
)

add_library(${PROJECT_NAME}
STATIC
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
SYSTEM PUBLIC
	core
	drivers
	HAL
)
