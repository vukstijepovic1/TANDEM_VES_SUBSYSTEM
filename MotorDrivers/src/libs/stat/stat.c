/**
 * @file    stat.c
 * @brief   Calculate mean, variance and covariance using data shifted Naïve
 *          algorithm.
 * @version	1.0.0
 * @date    07.04.2025
 * @author  LisumLab
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "stat.h"

#include <math.h>

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

int32_t stat_var_init(stat_var_t *stat, uint32_t num, uint32_t skip,
					  stat_type_t type)
{
	if (num == 0 || skip >= num)
		return 1;

	stat->num = num;
	stat->skip = skip;
	stat->type = type;

	stat->count = 0;
	stat->mean = 0.0f;
	stat->var = 0.0f;
	stat->k = 0.0f;
	stat->ek = 0.0f;
	stat->ek2 = 0.0f;

	stat->ready = 0;

	return 0;
}

int32_t stat_cov_init(stat_cov_t *stat, uint32_t num, uint32_t skip,
					  stat_type_t type)
{
	if (num == 0 || skip >= num)
		return 1;

	stat->num = num;
	stat->skip = skip;
	stat->type = type;

	stat->count = 0;
	stat->cov = 0.0f;
	stat->kx = 0.0f;
	stat->ky = 0.0f;
	stat->ex = 0.0f;
	stat->ey = 0.0f;
	stat->exy = 0.0f;

	stat->ready = 0;

	return 0;
}

void stat_var_update(stat_var_t *stat, float data)
{
	if (stat->ready)
		return;

	/* use data for calculations only if skip number of samples passed */
	if (stat->count >= stat->skip && stat->count < stat->skip + stat->num) {
		if (stat->count == stat->skip)
			stat->k = data;

		stat->mean += data;
		stat->ek += (data - stat->k);
		stat->ek2 += (data - stat->k) * (data - stat->k);
	}

	if (stat->count < stat->skip + stat->num)
		stat->count++;

	/* calculate mean and variance using Naïve algorithm with shifted data */
	if (stat->count == stat->skip + stat->num) {
		stat->mean = stat->mean / stat->num;
		stat->var = (stat->ek2 - (stat->ek * stat->ek) / stat->num);
		stat->var /= stat->num;

		/* Bassel's correction */
		if (stat->type == STAT_TYPE_SAMPLE)
			stat->var = stat->var * stat->num / (stat->num - 1);

		/* Protect agains float point precision causing negative variance */
		if (stat->var < 0)
			stat->var = 0.0f;

		stat->ready = 1;
	}
}

void stat_cov_update(stat_cov_t *stat, float data1, float data2)
{
	if (stat->ready)
		return;

	/* use data for calculations only if skip number of samples passed */
	if (stat->count >= stat->skip && stat->count < stat->skip + stat->num) {
		if (stat->count == stat->skip) {
			stat->kx = data1;
			stat->ky = data2;
		}

		stat->ex += (data1 - stat->kx);
		stat->ey += (data2 - stat->ky);
		stat->exy += ((data1 - stat->kx) * (data2 - stat->ky));
	}

	if (stat->count < stat->skip + stat->num)
		stat->count++;

	/* calculate mean and variance using Naïve algorithm with shifted data */
	if (stat->count == stat->skip + stat->num) {
		stat->cov = stat->exy - stat->ex * stat->ey / stat->num;
		stat->cov /= stat->num;

		/* Bassel's correction */
		if (stat->type == STAT_TYPE_SAMPLE)
			stat->cov = stat->cov * stat->num / (stat->num - 1);

		stat->ready = 1;
	}
}

uint8_t stat_var_check(stat_var_t *stat) { return stat->ready; }

uint8_t stat_cov_check(stat_cov_t *stat) { return stat->ready; }

int32_t stat_mean_get(stat_var_t *stat, float *mean)
{
	if (!stat_var_check(stat))
		return 1;

	*mean = stat->mean;

	return 0;
}

int32_t stat_var_get(stat_var_t *stat, float *var)
{
	if (!stat_var_check(stat))
		return 1;

	*var = stat->var;

	return 0;
}

int32_t stat_std_dev_get(stat_var_t *stat, float *std_dev)
{
	if (!stat_var_check(stat))
		return 1;

	*std_dev = sqrtf(stat->var);

	return 0;
}

int32_t stat_cov_get(stat_cov_t *stat, float *cov)
{
	if (!stat_cov_check(stat))
		return 1;

	*cov = stat->cov;

	return 0;
}

/****************************** static functions ******************************/

/********************************* End Of File ********************************/