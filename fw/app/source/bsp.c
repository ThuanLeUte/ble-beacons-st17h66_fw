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
#include "log.h"

/* Private defines ---------------------------------------------------- */
/* Function definitions ----------------------------------------------- */
extern void WaitMs(uint32_t wtTime);

void bsp_init(void)
{
  hal_gpio_pin_init(LED_INDICATE, GPIO_OUTPUT);
  // hal_gpio_pin_init(BUZZER, GPIO_OUTPUT);
  hal_gpio_pin_init(HALL_SENSOR_PWM, GPIO_OUTPUT);

  hal_gpio_pin_init(HALL_SENSOR_LOGIC, GPIO_INPUT);
  // hal_gpio_pin_init(USER_BUTTON, GPIO_INPUT);

  // hal_gpio_pull_set(LED_INDICATE, PULL_DOWN);
  // hal_gpio_pull_set(LED_INDICATE, GPIO_PULL_UP);
  // hal_gpio_write(LED_INDICATE, 1);
  hal_gpio_write(HALL_SENSOR_PWM, 1);
  while (1)
  {
    WaitMs(200);
    hal_gpio_write(HALL_SENSOR_PWM, 1);
    WaitMs(100);
    if (hal_gpio_read(HALL_SENSOR_LOGIC))
    {
      hal_gpio_write(LED_INDICATE, 0);
    }
    else
    {
      LOG("Hall pressed");
      hal_gpio_write(LED_INDICATE, 1);
      hal_gpio_write(HALL_SENSOR_PWM, 0);
    }
  }
}

/* End of file -------------------------------------------------------- */
