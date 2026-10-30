/**
 *	@file     eth.c
 *  @brief    HAL ETH library.
 *  @details  v1.1
 *  @author   LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "eth.h"

#include "lpc17xx_emac.h"
#include "lpc17xx_pinsel.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void prvETH_InitPins(void);

/*******************************************************************************
 * Code
 ******************************************************************************/

void HAL_ETH_Init(eth_hal_phy_t *eth, const uint8_t *dev_mac)
{
	prvETH_InitPins();

	EMAC_CFG_Type emac_cfg;
	emac_cfg.Mode = EMAC_MODE_AUTO;
	emac_cfg.pbEMAC_Addr = dev_mac;
	EMAC_Init(&emac_cfg);

	eth->recv = HAL_ETH_Recv;
	eth->send = HAL_ETH_Send;
}

uint16_t HAL_ETH_Recv(void *phy, uint8_t *data, uint16_t size)
{
	uint16_t curr_size;

	/* check if there is packet to read */
	if (EMAC_CheckReceiveIndex() == FALSE)
		return 0;

#if HAL_ETH_DROP_CRC
	curr_size = (uint16_t)(EMAC_GetReceiveDataSize() - 3);
#else
	curr_size = (uint16_t)(EMAC_GetReceiveDataSize() + 1);
#endif

	/* drop if packet is too large */
	if (curr_size > size) {
		EMAC_UpdateRxConsumeIndex();
		return 0;
	}

	/* receive packet */
	EMAC_PACKETBUF_Type rx_packet;
	rx_packet.pbDataBuf = (uint32_t *)data;
	rx_packet.ulDataLen = curr_size;
	EMAC_ReadPacketBuffer(&rx_packet);

	/* update index */
	EMAC_UpdateRxConsumeIndex();

	return curr_size;
}

uint16_t HAL_ETH_Send(void *phy, const uint8_t *data, uint16_t size)
{

	/* check if hardware is ready to write */
	if (EMAC_CheckTransmitIndex() == FALSE)
		return 0;

	/* write packet */
	EMAC_PACKETBUF_Type tx_packet;
	tx_packet.ulDataLen = size;
	tx_packet.pbDataBuf = (uint32_t *)data;
	EMAC_WritePacketBuffer(&tx_packet);

	/* update index */
	EMAC_UpdateTxProduceIndex();

	return size;
}

/****************************** static functions ******************************/

/* Initialize Ethernet Pins */
static void prvETH_InitPins(void)
{
	PINSEL_CFG_Type pin_cfg;
	pin_cfg.Funcnum = PINSEL_FUNC_1;
	pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
	pin_cfg.Portnum = 1;
	pin_cfg.Pinnum = 0;
	PINSEL_ConfigPin(&pin_cfg);
	pin_cfg.Pinnum = 1;
	PINSEL_ConfigPin(&pin_cfg);
	pin_cfg.Pinnum = 4;
	PINSEL_ConfigPin(&pin_cfg);
	pin_cfg.Pinnum = 8;
	PINSEL_ConfigPin(&pin_cfg);
	pin_cfg.Pinnum = 9;
	PINSEL_ConfigPin(&pin_cfg);
	pin_cfg.Pinnum = 10;
	PINSEL_ConfigPin(&pin_cfg);
	pin_cfg.Pinnum = 14;
	PINSEL_ConfigPin(&pin_cfg);
	pin_cfg.Pinnum = 15;
	PINSEL_ConfigPin(&pin_cfg);
	pin_cfg.Pinnum = 16;
	PINSEL_ConfigPin(&pin_cfg);
	pin_cfg.Pinnum = 17;
	PINSEL_ConfigPin(&pin_cfg);
}

/* --------------------------------- End Of File -----------------------------*/