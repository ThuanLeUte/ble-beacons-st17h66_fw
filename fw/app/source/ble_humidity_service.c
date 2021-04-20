/**
 * @file       ble_humidity_service.c
 * @copyright  Copyright (C) 2020 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    02.00.00
 * @date       2020-06-02
 * @author     Thuan Le 
 *             
 * @brief      BHS (BLE Humidity service)
 *             
 * @note          
 */

/* Includes ----------------------------------------------------------- */
#include "ble_humidity_service.h"
#include "log.h"

#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"

/* Private Defines ---------------------------------------------------------- */
// Position of humidity measurement value in attribute array
#define HUMI_MEAS_VALUE_POS    (2)

/* Private Macros ----------------------------------------------------------- */
#define  BASE_UUID(uuid)    0x41, 0xee, 0x68, 0x3a, 0x99, 0x0f, \
                           0x0e, 0x72, 0x85, 0x49, 0x8d, 0xb3, \
                           LO_UINT16(uuid), HI_UINT16(uuid), 0x00, 0x00

/* Private Defines ---------------------------------------------------------- */
const uint8_t BHS_UUID[][ATT_UUID_SIZE] =
{
  { BASE_UUID(BHS_UUID_SERV) },
  { BASE_UUID(BHS_UUID_CHAR_HUMI) }
};

#undef BASE_UUID

uint8_t BHS_CHAR_PROPS[] =
{
  0,
  GATT_PROP_READ | GATT_PROP_NOTIFY | GATT_PROP_EXTENDED 
};

static struct
{
  bhs_cb_t app_cb;
  struct
  {
    struct
    {
      uint8_t humi[4]; // Humidity value;
    }
    value;
  }
  chars;
}
m_bhs;

/*********************************************************************
 * Profile Attributes - variables
 */
// Humidity Service attribute
static CONST gattAttrType_t bhs_service = { ATT_UUID_SIZE, BHS_UUID[BHS_ID_SERVICE] };

/* Private function prototypes ---------------------------------------- */
static bStatus_t bhs_read_attr_cb(uint16           conn_handle,
                                  gattAttribute_t *p_attr,
                                  uint8           *p_value, 
                                  uint8           *p_len, 
                                  uint16           offset, 
                                  uint8            max_len);

static bStatus_t bhs_write_attr_cb(uint16          conn_handle, 
                                  gattAttribute_t *p_attr,
                                  uint8           *p_value, 
                                  uint8            len, 
                                  uint16           offset);

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Humidity Service Callbacks
CONST gattServiceCBs_t bhs_callbacks =
{
  bhs_read_attr_cb,  // Read callback function pointer
  bhs_write_attr_cb, // Write callback function pointer
  NULL               // Authorization callback function pointer
};

/*********************************************************************
 * Profile Attributes - Table
 */
static gattAttribute_t bhs_atrr_tbl[] =
{
  // Humidity Profile Service
  {
    {ATT_BT_UUID_SIZE, primaryServiceUUID}, /* type */
    GATT_PERMIT_READ,                       /* permissions */
    0,                                      /* handle */
    (uint8 *)&bhs_service                   /* p_value */
  },

  // Characteristic hunidity Declaration
  {
    {ATT_BT_UUID_SIZE, characterUUID},
    GATT_PERMIT_READ,
    0,
    &BHS_CHAR_PROPS[BHS_ID_CHAR_HUMI]
  },

  // Characteristic Value hunidity
  {
    {ATT_UUID_SIZE, BHS_UUID[BHS_ID_CHAR_HUMI]},
    GATT_PERMIT_READ,
    0,
    m_bhs.chars.value.humi
  }
};

/* Public function ----------------------------------------- */

bStatus_t bhs_add_service(bhs_cb_t cb)
{
  uint8 status = SUCCESS;

  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService(bhs_atrr_tbl,
                                       GATT_NUM_ATTRS(bhs_atrr_tbl),
                                       &bhs_callbacks);
  LOG("Add humidity service\n");
  if (status == SUCCESS)
  {
    m_bhs.app_cb = cb;
  }
  else
  {
    LOG("Add rawpass service failed!\n");
  }

  return (status);
}

bStatus_t bhs_set_parameter(bhs_id_t char_id, uint8 len, void *value)
{
  bStatus_t ret = SUCCESS;

  switch (char_id)
  {
  case BHS_ID_CHAR_HUMI:
    osal_memcpy(m_bhs.chars.value.humi, value, len);
    break;
  default:
    ret = INVALIDPARAMETER;
    break;
  }

  return (ret);
}

bStatus_t bhs_get_parameter(bhs_id_t char_id, void *value)
{
  bStatus_t ret = SUCCESS;

  switch (char_id)
  {
  case BHS_ID_CHAR_HUMI:
    osal_memcpy(value, m_bhs.chars.value.humi, sizeof(m_bhs.chars.value.humi));
    break;
  default:
    ret = INVALIDPARAMETER;
    break;
  }

  return (ret);
}

bStatus_t bhs_notify_humidity(uint16 conn_handle, attHandleValueNoti_t *p_noti)
{
  bhs_evt_t evt;
  bStatus_t ret;

  // Set the handle
  p_noti->handle = bhs_atrr_tbl[HUMI_MEAS_VALUE_POS].handle;
  
  // Send the notification
  ret = GATT_Notification(conn_handle, p_noti, FALSE);

  if (SUCCESS == ret)
  {
    evt.evt_id = BHS_EVT_NOTIFY_SENT;
    m_bhs.app_cb(&evt);
  }
  
  return ret; 
}

/* Private Function definitions ----------------------------------------------- */
/**
 * @brief       Write an attribute.
 *
 * @param[in]   <conn_handle>     Connection message was received on
 *              <p_attr>          Pointer to attribute
 *              <p_value>         Pointer to data to be read
 *              <p_len>           Length of data to be read
 *              <offset>          Offset of the first octet to be read
 *
 * @return      Success or Failure
 */
static bStatus_t bhs_write_attr_cb(uint16          conn_handle, 
                                  gattAttribute_t *p_attr,
                                  uint8           *p_value, 
                                  uint8            len, 
                                  uint16           offset)
{
  bStatus_t status = SUCCESS;
  LOG("Humidity write callback\n");

  // If attribute permissions require authorization to write, return error
  if (gattPermitAuthorWrite(p_attr->permissions))
    return (ATT_ERR_INSUFFICIENT_AUTHOR);  // Insufficient authorization

  if (p_attr->type.len == ATT_BT_UUID_SIZE)
  {
    // 16-bit UUID
  }
  else
  {
    // 128-bit UUID
    if (osal_memcmp(p_attr->type.uuid, BHS_UUID[BHS_ID_CHAR_HUMI], 16))
      osal_memcpy(m_bhs.chars.value.humi, p_value, 4);
  }
  
  return (status);
}

/**
 * @brief       Read an attribute.
 *
 * @param[in]   <conn_handle>     Connection message was received on
 *              <p_attr>          Pointer to attribute
 *              <p_value>         Pointer to data to be read
 *              <p_len>           Length of data to be read
 *              <offset>          Offset of the first octet to be read
 *              <max_len>         Maximum length of data to be read
 *
 * @return      Success or Failure
 */
static uint8 bhs_read_attr_cb(uint16 conn_handle,
                              gattAttribute_t *p_attr,
                              uint8 *p_value,
                              uint8 *p_len,
                              uint16 offset,
                              uint8 max_len)
{
  bStatus_t status = SUCCESS;
  LOG("Humidity read callback\n");

  // If attribute permissions require authorization to read, return error
  if (gattPermitAuthorRead(p_attr->permissions))
    return (ATT_ERR_INSUFFICIENT_AUTHOR); // Insufficient authorization

  if (p_attr->type.len == ATT_BT_UUID_SIZE)
  {
    // 16-bit UUID
  }
  else
  {
    // 128-bit UUID
    if (osal_memcmp(p_attr->type.uuid, BHS_UUID[BHS_ID_CHAR_HUMI], 16))
    {
      *p_len = 4;
      osal_memcpy(p_value, m_bhs.chars.value.humi, 4);
    }
    else
    {
      // Do nothing
    }
  }
  
  return (status);
}

/* End of file -------------------------------------------------------- */
