/**
 * @file       ble_misc_services.h
 * @copyright  Copyright (C) 2020 ThuanLe. All rights reserved.
 * @license    This project is released under the ThuanLe License.
 * @version    2.0.0
 * @date       2020-06-02
 * @author     Thuan Le
 * @brief      MCS (Miscellaneous Service)
 * @note       None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BLE_MCS_SERVICES_H
#define __BLE_MCS_SERVICES_H

/* Includes ----------------------------------------------------------- */
#include "att.h"

/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */
typedef enum 
{
  MCS_ID_SERVICE = 0x00,
  MCS_ID_CHAR_CLICK_AVAILBLE,
  MCS_ID_CHAR_IDENTIFICATON,
  MCS_ID_CHAR_MODE_SELECTION,
  MCS_ID_CHAR_BOTTLE_REPLACEMENT,
  MCS_ID_MAX
}
mcs_id_t;

/* Public function prototypes ----------------------------------------- */
/**
 * @brief      Initializes the BLE service by registering
 *             GATT attributes with the GATT server.
 *
 * @param[in]  None
 *
 * @return
 *  - 0: Success
 *  - 1: Failure
 */
bStatus_t mcs_add_service(void);

/**
 * @brief      Set characterictics value.
 *
 * @param[in]  char_id        Characteristic ID
 *             len            Length of `value`
 *             value          Pointer to data to write. This is dependent on
 *                            the parameter ID and WILL be cast to the appropriate 
 *                            data type (example: data type of uint16 will be cast to 
 *                            uint16 pointer).
 *
 * @return
 *  - SUCCESS
 *  - INVALIDPARAMETER
 */
bStatus_t mcs_set_parameter(mcs_id_t char_id, uint8 len, void *value);

/**
 * @brief      Get characterictics value.
 *
 * @param[in]  char_id        Characteristic ID
 *             value          Pointer to data to write. This is dependent on
 *                            the parameter ID and WILL be cast to the appropriate 
 *                            data type (example: data type of uint16 will be cast to 
 *                            uint16 pointer).
 *
 * @return
 *  - SUCCESS
 *  - INVALIDPARAMETER
 */
bStatus_t mcs_get_parameter(mcs_id_t char_id, void *value);

/**
 * @brief       Send a notification containing a humidity measurement.
 *
 * @param[in]  char_id        Characteristic ID
 *             conn_handle    Connection handle
 *             p_noti         Pointer to notification structure.
 *
 * @return     Success or Failure
 * 
 */
bStatus_t mcs_notify(mcs_id_t char_id, uint16 conn_handle, attHandleValueNoti_t *p_noti);

#endif // __BLE_MCS_SERVICES_H

/* End of file -------------------------------------------------------- */
