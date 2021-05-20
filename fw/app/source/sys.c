/**
 * @file       sys.c
 * @copyright  Copyright (C) 2020 Thuan Le. All rights reserved.
 * @license    This project is released under the Thuan Le License.
 * @version    1.0.0
 * @date       2021-04-21
 * @author     Thuan Le
 * @brief      System file
 * @note       None
 */

/* Includes ----------------------------------------------------------- */
#include "sys.h"
#include "ble_misc_services.h"
#include "bsp.h"
#include "damos_ram.h"
#include "ble_timer.h"
#include "ble_dispenser.h"
#include "sbpProfile_ota.h"

/* Private defines ---------------------------------------------------- */
/* Function definitions ----------------------------------------------- */
static void m_ble_notify_humi(void)
{
  // attHandleValueNoti_t humi_meas;
  // humi_meas.len = 1;
  // humi_meas.value[0] =  g_dispenser.click_count;
  // mcs_notify(MCS_ID_CHAR_CLICK_COUNT, g_gap_conn_handle, &humi_meas);
  // simpleProfile_Notify(g_gap_conn_handle,  &g_dispenser.click_count, 1);
  // simpleProfile_Set_Notify_Event();
}
void pin_event_handler(GPIO_Pin_e pin, IO_Wakeup_Pol_e type)
{
  // g_dispenser.click_count++;
  // m_ble_notify_humi();
  // LOG("Send notify\n");
}

void periodic_1s_callback(void)
{
  // if (hal_gpio_read(USER_BUTTON))
  // {
  //   hal_gpio_write(LED_INDICATE, 0);
  // }
  // else
  // {
  //   LOG("Button pressed");
  //   g_dispenser.click_count++;
  //   m_ble_notify_humi();
  //   LOG("Send notify");
  //   hal_gpio_write(LED_INDICATE, 1);
  // }
}


/* End of file -------------------------------------------------------- */
