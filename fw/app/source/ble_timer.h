/**
 * @file       ble_timer.h
 * @copyright  Copyright (C) 2020 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2020-06-11
 * @author     Thuan Le
 *             
 * @brief      Timer for smart gadget.
 *             
 * @note          
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BLE_TIMER_H
#define __BLE_TIMER_H

/* Includes ----------------------------------------------------------- */
#include "stdint.h"

/* Public defines ---------------------------------------------------- */
#define TIMER_1000_MS_EVT   (0x0001)
#define TIMER_50_MS_EVT     (0x0004)

/* Public Callbacks -------------------------------------------------- */
/**
 * @brief           Function is called in timer 1s to execute all the functions.
 * 
 * @param           None
 * 
 * @attention       None
 * 
 * @return          None
 */
void periodic_1s_callback(void); 

/* Public function prototypes ----------------------------------------- */
/**
 * @brief           Timer 1s init
 *
 * @param[in]       <task_id>  Task ID.
 *  
 * @attention       None
 *
 * @return          None
 */
void ble_timer_1s_init(uint8_t task_id);

/**
 * @brief           Timer stop
 *
 * @param[in]       <task_id>   Task ID.
 *                  <event_id>  Event ID.
 *  
 * @attention       None
 *
 * @return          None
 */
void ble_timer_stop(uint8_t task_id, uint16_t event_id);

/**
 * @brief           Timer process event 
 *
 * @param[in]       <task_id>   Task ID.
 *                  <events>    Task Event
 *  
 * @attention       None
 *
 * @return          None
 */
uint16_t ble_timer_process_event(uint8_t task_id, uint16_t events);

#endif // __BLE_TIMER_H

/* End of file -------------------------------------------------------- */
