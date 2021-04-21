/**
 * @file       bsp.h
 * @copyright  Copyright (C) 2020 Thuan Le. All rights reserved.
 * @license    This project is released under the Thuan Le License.
 * @version    1.0.0
 * @date       2021-04-21
 * @author     Thuan Le
 * @brief      Board Support Package (BSP)
 * @note       None
 */

/* Define to prevent recursive inclusion ----------------------------- */
#ifndef __BSP_H
#define __BSP_H

/* Includes ---------------------------------------------------------- */
#include "stdint.h"

/* Public defines ---------------------------------------------------- */
#define LED_INDICATE           (GPIO_P34)
#define BUZZER                 (GPIO_P15)
#define HALL_SENSOR_PWM        (GPIO_P11)
#define HALL_SENSOR_LOGIC      (GPIO_P07)
#define USER_BUTTON            (GPIO_P20)
#define BATTERY_ADC            (GPIO_P18)

/* Public function prototypes ---------------------------------------- */
/**
 * @brief           Board support package init
 *
 * @param[in]       None
 *  
 * @attention       None
 *
 * @return          None
 */
void bsp_init(void);

#endif // __BSP_H

/* End of file ------------------------------------------------------- */
