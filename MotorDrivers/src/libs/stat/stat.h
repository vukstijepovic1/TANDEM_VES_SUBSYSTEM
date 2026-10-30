/**
 * @file    stat.h
 * @brief   Calculate mean, variance and covariance using data shifted Naïve
 *          algorithm.
 * @version	1.0.0
 * @date    07.04.2025
 * @author  LisumLab
 */

#ifndef STAT_H
#define STAT_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Typedefs
 ******************************************************************************/

/*! @brief Data sample type */
typedef enum stat_type_t {
	STAT_TYPE_SAMPLE = 0,	 //!< Calculation using data samples.
	STAT_TYPE_POPULATION = 1 //!< Calculation using true population data.
} stat_type_t;

/*! @brief Stat structure */
typedef struct stat_var_t {
	uint32_t num;	//!< Number of samples.
	uint32_t skip;	//!< Number of samples to skip in beginning.
	uint32_t count; //!< Current sample counter.
	float mean;		//!< Mean.
	float var;		//!< Variance.

	float k;   //!< Internal variable used for calculation.
	float ek;  //!< Internal variable used for calculation.
	float ek2; //!< Internal variable used for calculation.

	stat_type_t type; //!< Is stat calc done using samples or true population.
	uint8_t ready;	  //!< Is calculation done.
} stat_var_t;

typedef struct stat_cov_t {
	uint32_t num;	//!< Number of samples.
	uint32_t skip;	//!< Number of samples to skip in beginning.
	uint32_t count; //!< Current sample counter.
	float cov;		//!< Covariance.

	float kx;  //!< Internal variable used for calculation.
	float ky;  //!< Internal variable used for calculation.
	float ex;  //!< Internal variable used for calculation.
	float ey;  //!< Internal variable used for calculation.
	float exy; //!< Internal variable used for calculation.

	stat_type_t type; //!< Is stat calc done using samples or true population.
	uint8_t ready;	  //!< Is calculation done.
} stat_cov_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initialize mean and variance calculation.
 *
 * @param[in] stat  Stat structure.
 * @param[in] num   Number of data samples to be used.
 * @param[in] skip  Number of data samples to be skipped in beginning.
 * @param[in] type  Are statistical calculation done using data samples or
 *                  data from true population (does Bassel's correction need to
 *                  be used).
 * @retval status
 */
int32_t stat_var_init(stat_var_t *stat, uint32_t num, uint32_t skip,
					  stat_type_t type);

/**
 * @brief Initialize covariance calculation.
 *
 * @param[in] stat  Stat structure.
 * @param[in] num   Number of data samples to be used.
 * @param[in] skip  Number of data samples to be skipped in beginning.
 * @param[in] type  Are statistical calculation done using data samples or
 *                  data from true population (does Bassel's correction need to
 *                  be used)
 * @retval status
 */
int32_t stat_cov_init(stat_cov_t *stat, uint32_t num, uint32_t skip,
					  stat_type_t type);

/**
 * @brief Update mean and variance calculation with new measurement.
 *
 * @param[in] stat  Stat structure.
 * @param[in] data  New data.
 * @retval None
 */
void stat_var_update(stat_var_t *stat, float data);

/**
 * @brief Update covariance calculation with new measurement.
 *
 * @param[in] stat  Stat structure.
 * @param[in] data1 New data 1.
 * @param[in] data2 New data 2.
 * @retval None
 */
void stat_cov_update(stat_cov_t *stat, float data1, float data2);

/**
 * @brief Check if mean and variance calculation is completed.
 *
 * @param[in] stat Stat structure.
 * @retval status
 */
uint8_t stat_var_check(stat_var_t *stat);

/**
 * @brief Check if covariance calculation is completed.
 *
 * @param[in] stat Stat structure.
 * @retval status
 */
uint8_t stat_cov_check(stat_cov_t *stat);

/**
 * @brief Get calculated mean.
 *
 * @param[in] stat  Stat structure.
 * @param[out] mean Mean.
 * @retval status
 */
int32_t stat_mean_get(stat_var_t *stat, float *mean);

/**
 * @brief Get calculated variance.
 *
 * @param[in] stat  Stat structure.
 * @param[out] var  Variance.
 * @retval status
 */
int32_t stat_var_get(stat_var_t *stat, float *var);

/**
 * @brief Get calculated variance.
 *
 * @param[in] stat      Stat structure.
 * @param[out] std_dev  Standard deviation.
 * @retval status
 */
int32_t stat_std_dev_get(stat_var_t *stat, float *std_dev);

/**
 * @brief Get calculated covariance.
 *
 * @param[in] stat  Stat structure.
 * @param[out] cov  Covariance.
 * @retval status
 */
int32_t stat_cov_get(stat_cov_t *stat, float *cov);

#ifdef __cplusplus
}
#endif

#endif /* STAT_H */