/**
 * @file       ble_click_services.c
 * @copyright  Copyright (C) 2020 ThuanLe. All rights reserved.
 * @license    This project is released under the ThuanLe License.
 * @version    2.0.0
 * @date       2020-06-02
 * @author     Thuan Le
 * @brief      CAS (Click Available Service)
 * @note       None
 */

/* Includes ----------------------------------------------------------- */
#include "ble_click_services.h"
#include "log.h"

#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"

/* Private Defines ---------------------------------------------------------- */
#define CAS_UUID_SERV                       (0xFFF0)
#define CAS_UUID_CHAR_CLICK_AVAILABLE       (0xFFF1)

#define CHAR_CLICK_AVAILABLE_VALUE_POS      (2)

/* Private Macros ----------------------------------------------------------- */
/* Private Defines ---------------------------------------------------------- */
// GATT Profile Service UUID
static CONST uint8 CAS_UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(CAS_UUID_SERV), HI_UINT16(CAS_UUID_SERV)
};

// Characteristic UUID
static CONST uint8 CAS_CHAR_CLICK_AVAILABLE_UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(CAS_UUID_CHAR_CLICK_AVAILABLE), HI_UINT16(CAS_UUID_CHAR_CLICK_AVAILABLE)
};

// Characterictic property
uint8_t CAS_CHAR_PROPS[] =
{
  0,
  GATT_PROP_READ | GATT_PROP_NOTIFY,
  GATT_PROP_READ | GATT_PROP_NOTIFY,
};

// Profile Service attribute
static CONST gattAttrType_t cas_service = { ATT_BT_UUID_SIZE, CAS_UUID };

// Profile struct
static struct
{
  struct
  {
    struct
    {
      uint8_t click_available[1]; // Charaterictic value
    }
    value;
  }
  chars;
}
m_cas;

// Profile atrribute
static gattAttribute_t cas_atrr_tbl[] =
{
  // Profile Service
  {
    {ATT_BT_UUID_SIZE, primaryServiceUUID}, /* type */
    GATT_PERMIT_READ,                       /* permissions */
    0,                                      /* handle */
    (uint8 *)&cas_service                   /* p_value */
  },

  // Characteristic 1 Declaration
  {
    {ATT_BT_UUID_SIZE, characterUUID},
    GATT_PERMIT_READ,
    0,
    &CAS_CHAR_PROPS[CAS_ID_CHAR_CLICK_AVAILBLE]
  },

  // Characteristic Value 1
  {
    {ATT_BT_UUID_SIZE, CAS_CHAR_CLICK_AVAILABLE_UUID},
    GATT_PERMIT_READ | GATT_PERMIT_WRITE,
    0,
    m_cas.chars.value.click_available
  },
};

/* Private function prototypes ---------------------------------------- */
static bStatus_t cas_read_attr_cb(uint16           conn_handle,
                                  gattAttribute_t *p_attr,
                                  uint8           *p_value, 
                                  uint8           *p_len, 
                                  uint16           offset, 
                                  uint8            max_len);

static bStatus_t cas_write_attr_cb(uint16          conn_handle, 
                                  gattAttribute_t *p_attr,
                                  uint8           *p_value, 
                                  uint8            len, 
                                  uint16           offset);

/*********************************************************************
 * PROFILE CALLBACKS
 */
static CONST gattServiceCBs_t cas_callbacks =
{
  cas_read_attr_cb,
  cas_write_attr_cb,
  NULL
};

/* Public function ----------------------------------------- */
bStatus_t cas_add_service(void)
{
  uint8 status = SUCCESS;

  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService(cas_atrr_tbl,
                                       GATT_NUM_ATTRS(cas_atrr_tbl),
                                       &cas_callbacks);

  LOG("cas_add_service:%x\n", status);

  return ( status );
}

bStatus_t cas_set_parameter(cas_id_t char_id, uint8 len, void *value)
{
  bStatus_t ret = SUCCESS;


  return (ret);
}

bStatus_t cas_get_parameter(cas_id_t char_id, void *value)
{
  bStatus_t ret = SUCCESS;


  return (ret);
}


bStatus_t cas_notify(cas_id_t char_id, uint16 conn_handle, attHandleValueNoti_t *p_noti)
{
  bStatus_t ret;

  // Set the handle
  p_noti->handle = cas_atrr_tbl[CHAR_CLICK_AVAILABLE_VALUE_POS].handle;
  
  // Send the notification
  ret = GATT_Notification(conn_handle, p_noti, FALSE);

  if (SUCCESS == ret)
  {
    LOG("cas_notify success\n");
  }
  
  return ret; 
}

/* Private Function definitions ----------------------------------------------- */
/**
 * @brief       Write an attribute.
 *
 * @param[in]   conn_handle     Connection message was received on
 *              p_attr          Pointer to attribute
 *              p_value         Pointer to data to be read
 *              p_len           Length of data to be read
 *              offset          Offset of the first octet to be read
 *
 * @return      Success or Failure
 */
static bStatus_t cas_write_attr_cb(uint16          conn_handle, 
                                  gattAttribute_t *p_attr,
                                  uint8           *p_value, 
                                  uint8            len, 
                                  uint16           offset)
{
  bStatus_t status = SUCCESS;
  LOG("Humidity write callback\n");
  return (status);
}

/**
 * @brief       Read an attribute.
 *
 * @param[in]   conn_handle     Connection message was received on
 *              p_attr          Pointer to attribute
 *              p_value         Pointer to data to be read
 *              p_len           Length of data to be read
 *              offset          Offset of the first octet to be read
 *              max_len         Maximum length of data to be read
 *
 * @return      Success or Failure
 */
static uint8 cas_read_attr_cb(uint16 conn_handle,
                              gattAttribute_t *p_attr,
                              uint8 *p_value,
                              uint8 *p_len,
                              uint16 offset,
                              uint8 max_len)
{
  bStatus_t status = SUCCESS;
  LOG("Humidity read callback\n");
  return (status);
}

/* End of file -------------------------------------------------------- */
