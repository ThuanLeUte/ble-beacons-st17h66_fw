/**
 * @file       ble_humidity_service.h
 * @copyright  Copyright (C) 2020 ThuanLe. All rights reserved.
 * @license    This project is released under the ThuanLe License.
 * @version    2.0.0
 * @date       2020-06-02
 * @author     Thuan Le
 *             
 * @brief      BHS (BLE Humidity service)
 *             
 * @note          
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BLE_HUMIDITY_SERVICE_H
#define __BLE_HUMIDITY_SERVICE_H

/* Includes ----------------------------------------------------------- */
#include "att.h"

/* Public defines ----------------------------------------------------- */
#define BHS_UUID_SERV         (0x1234)
#define BHS_UUID_CHAR_HUMI    (0x1235)

/* Public enumerate/structure ----------------------------------------- */
typedef enum 
{
  BHS_ID_SERVICE = 0x00,
  BHS_ID_CHAR_HUMI,
  BHS_ID_MAX
}
bhs_id_t;

typedef enum 
{
  BHS_EVT_NOTIFY_SENT,
  BHS_EVT_2
}
bhs_evt_id_t;

typedef struct
{
  bhs_evt_id_t  evt_id;
  bhs_id_t      param;
  void         *data;
}
bhs_evt_t;

typedef void (*bhs_cb_t)(bhs_evt_t* pev);

/* Public function prototypes ----------------------------------------- */
/**
 * @brief      Initializes the Humidity service by registering
 *             GATT attributes with the GATT server.
 *
 * @param[in]  <cb>             Callback function
 *
 * @return  
 *  - 0: Success
 *  - 1: Failure
 */
bStatus_t bhs_add_service(uint32 services);

/**
 * @brief      Set humidity value.
 *
 * @param[in]  <char_id>        Characteristic ID
 *             <len>            Length of `value`
 *             <value>          Pointer to data to write. This is dependent on
 *                              the parameter ID and WILL be cast to the appropriate 
 *                              data type (example: data type of uint16 will be cast to 
 *                              uint16 pointer).
 *
 * @return  
 *  - SUCCESS
 *  - INVALIDPARAMETER
 */
bStatus_t bhs_set_parameter(bhs_id_t char_id, uint8 len, void *value);

/**
 * @brief      Get humidity value.
 *
 * @param[in]  <char_id>        Characteristic ID
 *             <value>          pointer to data to write. This is dependent on
 *                              the parameter ID and WILL be cast to the appropriate 
 *                              data type (example: data type of uint16 will be cast to 
 *                              uint16 pointer).
 *
 * @return  
 *  - SUCCESS
 *  - INVALIDPARAMETER
 */
bStatus_t bhs_get_parameter(bhs_id_t char_id, void *value);

/**
 * @brief       Send a notification containing a humidity measurement.
 *
 * @param[in]   <conn_handle>   Connection handle
 *              <p_noti>        Pointer to notification structure.
 *
 * @return      Success or Failure
 * @note        This API is used by Data Logger Module (DLM)
 * 
 */
bStatus_t bhs_notify_humidity(uint16 conn_handle, attHandleValueNoti_t *p_noti);

#endif // __BLE_HUMIDITY_SERVICE_H

/* End of file -------------------------------------------------------- */
