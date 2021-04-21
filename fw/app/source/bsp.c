/**
 * @file       bsp.c
 * @copyright  Copyright (C) 2020 Thuan Le. All rights reserved.
 * @license    This project is released under the Thuan Le License.
 * @version    1.0.0
 * @date       2021-04-21
 * @author     Thuan Le
 * @brief      Board Support Package (BSP)
 * @note       None
 */

/* Includes ----------------------------------------------------------- */
#include "bsp.h"

#include "gpio.h"

/* Private defines ---------------------------------------------------- */
/* Function definitions ----------------------------------------------- */
void bsp_init(void)
{
  hal_gpio_pin_init(LED_INDICATE, GPIO_OUTPUT);
  hal_gpio_pin_init(BUZZER, GPIO_OUTPUT);
  hal_gpio_pin_init(HALL_SENSOR_PWM, GPIO_OUTPUT);

  hal_gpio_pin_init(HALL_SENSOR_LOGIC, GPIO_INPUT);
  hal_gpio_pin_init(USER_BUTTON, GPIO_INPUT);

  hal_gpio_pull_set(LED_INDICATE, PULL_DOWN);
  hal_gpio_write(LED_INDICATE, 1);
}

/* End of file -------------------------------------------------------- */
