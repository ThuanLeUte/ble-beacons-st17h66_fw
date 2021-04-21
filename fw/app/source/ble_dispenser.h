/**
 * @file       ble_dispenser.h
 * @copyright  Copyright (C) 2020 ThuanLe. All rights reserved.
 * @license    This project is released under the ThuanLe License.
 * @version    1.0.0
 * @date       2021-04-18
 * @author     Thuan Le
 * @brief      BLE application
 * @note       None
 * @example    None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BLE_DISPENSER_H
#define __BLE_DISPENSER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ----------------------------------------------------------- */
#include "types.h"

/* Public defines ----------------------------------------------------- */
// Simple BLE Peripheral Task Events
#define SBP_START_DEVICE_EVT                           (0x0001)
#define SBP_ADD_RL_EVT                                 (0x0002)
#define SBP_NOTIFY_EVT                                 (0x0004)
#define SBP_RESET_ADV_EVT                              (0x0008)
#define SBP_CONNECTED_EVT                              (0x0010)

/* Public enumerate/structure ----------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
extern void simpleProfile_Set_Notify_Event(void);
extern void SimpleBLEPeripheral_SetDevName(uint8*data,uint8 len);
extern void SimpleBLEPeripheral_SetBeaconUUID(uint8* data,uint8 len);
extern void SimpleBLEPeripheral_SetMajor(uint8* data,uint8 len);
extern void SimpleBLEPeripheral_SetMinor(uint8* data,uint8 len);
extern void SimpleBLEPeripheral_SetRSSI(uint8 data);

extern void ble_dispenser_init(uint8 task_id);
extern uint16 ble_dispenser_process_event(uint8 task_id, uint16 events);

#ifdef __cplusplus
}
#endif

#endif // __BLE_DISPENSER_H

/* End of file -------------------------------------------------------- */
