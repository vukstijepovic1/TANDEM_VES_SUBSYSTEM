/**
 * @file    ubx_common.c
 * @brief   UBX common types and definitions.
 * @version	1.0.0
 * @date    02.04.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "ubx_common.h"
#include "ubx_def.h"

#include <string.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void ubx_ack_ack_decode(ubx_t *inst, ubx_ack_ack_t *ack)
{
	memcpy(ack, inst->arr, sizeof(ubx_ack_ack_t));
}

void ubx_ack_nak_decode(ubx_t *inst, ubx_ack_nak_t *nak)
{
	memcpy(nak, inst->arr, sizeof(ubx_ack_nak_t));
}

void ubx_nav_hpposllh_decode(ubx_t *inst, ubx_nav_hpposllh_t *hpposllh)
{
	memcpy(hpposllh, inst->arr, sizeof(ubx_nav_hpposllh_t));
}

void ubx_nav_posecef_decode(ubx_t *inst, ubx_nav_posecef_t *posecef)
{
	memcpy(posecef, inst->arr, sizeof(ubx_nav_posecef_t));
}

void ubx_nav_posllh_decode(ubx_t *inst, ubx_nav_posllh_t *posllh)
{
	memcpy(posllh, inst->arr, sizeof(ubx_nav_posllh_t));
}

void ubx_nav_pvt_decode(ubx_t *inst, ubx_nav_pvt_t *pvt)
{
	memcpy(pvt, inst->arr, sizeof(ubx_nav_pvt_t));
}

void ubx_nav_relposned_decode(ubx_t *inst, ubx_nav_relposned_t *relposned)
{
	memcpy(relposned, inst->arr, sizeof(ubx_nav_relposned_t));
}

void ubx_nav_timeutc_decode(ubx_t *inst, ubx_nav_timeutc_t *timeutc)
{
	memcpy(timeutc, inst->arr, sizeof(ubx_nav_timeutc_t));
}

/****************************** static functions ******************************/

/********************************* End Of File ********************************/