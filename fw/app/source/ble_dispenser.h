/**
 * @file       zigbee_webee.h
 * @copyright  Copyright (C) 2020 ThuanLe. All rights reserved.
 * @license    This project is released under the ThuanLe License.
 * @version    1.0.0
 * @date       2020-08-26
 * @author     Thuan Le
 * @brief      Driver support zigbee webee communication
 * @note       None
 * @example    None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __ZIGBEE_WEBEE_H
#define __ZIGBEE_WEBEE_H

/* Includes ----------------------------------------------------------- */
/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief <enum descriptiton>
 */
typedef enum 
{
  PUBLIC_ENUM_1, /**< Description of PUBLIC_ENUM_1 */
  PUBLIC_ENUM_2, /**< Description of PUBLIC_ENUM_2 */
  PUBLIC_ENUM_3  /**< Description of PUBLIC_ENUM_3 */
}
public_enum_type_t;

/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
/**
 * @brief  <function description>
 *
 * @param[in]     <param_name>  <param_despcription>
 * @param[out]    <param_name>  <param_despcription>
 * @param[inout]  <param_name>  <param_despcription>
 *
 * @attention  <API attention note>
 *
 * @return  
 *  - 0: Success
 *  - 1: Error
 */

#endif // __ZIGBEE_WEBEE_H

/* End of file -------------------------------------------------------- */



/**************************************************************************************************
THIS IS EMPTY HEADER
**************************************************************************************************/

/**************************************************************************************************
  Filename:       simpleBLEperipheral.h
  Revised:         
  Revision:        

  Description:    This file contains the Simple BLE Peripheral sample application
                  definitions and prototypes.

 
**************************************************************************************************/

#ifndef SIMPLEBLEPERIPHERAL_H
#define SIMPLEBLEPERIPHERAL_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "types.h"
/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

#define CHANGE_ADV_TYPE_TIME   30000  //30S���û�����Ӿͱ�ɲ������ӹ㲥
// Simple BLE Peripheral Task Events
#define SBP_START_DEVICE_EVT                           0x0001
#define SBP_ADD_RL_EVT                                 0x0002
#define SBP_NOTIFY_EVT                               0x0004
#define SBP_RESET_ADV_EVT                              0x0008
#define SBP_CONNECTED_EVT                              0x0010

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the BLE Application
 */
extern void SimpleBLEPeripheral_Init( uint8 task_id );

/*
 * Task Event Processor for the BLE Application
 */
extern uint16 gapConnHandle;
extern void simpleProfile_Set_Notify_Event(void); //notify cmd set rsp data
extern void SimpleBLEPeripheral_SetDevName(uint8*data,uint8 len);
extern void SimpleBLEPeripheral_SetBeaconUUID(uint8* data,uint8 len);
extern void SimpleBLEPeripheral_SetMajor(uint8* data,uint8 len);
extern void SimpleBLEPeripheral_SetMinor(uint8* data,uint8 len);
extern void SimpleBLEPeripheral_SetRSSI(uint8 data);

extern void ble_dispenser_init(uint8 task_id);
extern uint16 ble_dispenser_process_event(uint8 task_id, uint16 events);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEBLEPERIPHERAL_H */
