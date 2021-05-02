/**
 * @file       ble_mode_services.h
 * @copyright  Copyright (C) 2020 ThuanLe. All rights reserved.
 * @license    This project is released under the ThuanLe License.
 * @version    2.0.0
 * @date       2020-06-02
 * @author     Thuan Le
 * @brief      MSS (Mode Selection Service)
 * @note       None
 */

/* Includes ----------------------------------------------------------- */
#include "ble_mode_services.h"
#include "log.h"

#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"

/* Private Defines ---------------------------------------------------------- */
#define MSS_UUID_SERV                      (0xFFF4)
#define MSS_UUID_CHAR_MODE_SELECTION       (0xFFF5)

#define CHAR_MODE_SELECTION_VALUE_POS      (2)

/* Private Macros ----------------------------------------------------------- */
/* Private Defines ---------------------------------------------------------- */
// GATT Profile Service UUID
static CONST uint8 MSS_UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(MSS_UUID_SERV), HI_UINT16(MSS_UUID_SERV)
};

// Characteristic UUID:
static CONST uint8 MSS_CHAR_MODE_SELECTION_UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(MSS_UUID_CHAR_MODE_SELECTION), HI_UINT16(MSS_UUID_CHAR_MODE_SELECTION)
};

// Characterictic property
uint8_t MSS_CHAR_PROPS[] =
{
  0,
  GATT_PROP_READ | GATT_PROP_NOTIFY,
  GATT_PROP_READ | GATT_PROP_NOTIFY,
};

// Profile Service attribute
static CONST gattAttrType_t mss_service = { ATT_BT_UUID_SIZE, MSS_UUID };

// Profile struct
static struct
{
  struct
  {
    struct
    {
      uint8_t mode_selection[1]; // Charaterictic value
    }
    value;
  }
  chars;
}
m_mss;

// Profile atrribute
static gattAttribute_t mss_atrr_tbl[] =
{
  // Profile Service
  {
    {ATT_BT_UUID_SIZE, primaryServiceUUID}, /* type */
    GATT_PERMIT_READ,                       /* permissions */
    0,                                      /* handle */
    (uint8 *)&mss_service                   /* p_value */
  },

  // Characteristic 1 Declaration
  {
    {ATT_BT_UUID_SIZE, characterUUID},
    GATT_PERMIT_READ,
    0,
    &MSS_CHAR_PROPS[MSS_ID_CHAR_MODE_SELECTION]
  },

  // Characteristic Value 1
  {
    {ATT_BT_UUID_SIZE, MSS_CHAR_MODE_SELECTION_UUID},
    GATT_PERMIT_READ | GATT_PERMIT_WRITE,
    0,
    m_mss.chars.value.mode_selection
  },
};

/* Private function prototypes ---------------------------------------- */
static bStatus_t mss_read_attr_cb(uint16           conn_handle,
                                  gattAttribute_t *p_attr,
                                  uint8           *p_value, 
                                  uint8           *p_len, 
                                  uint16           offset, 
                                  uint8            max_len);

static bStatus_t mss_write_attr_cb(uint16          conn_handle, 
                                  gattAttribute_t *p_attr,
                                  uint8           *p_value, 
                                  uint8            len, 
                                  uint16           offset);

/*********************************************************************
 * PROFILE CALLBACKS
 */
static CONST gattServiceCBs_t mss_callbacks =
{
  mss_read_attr_cb,
  mss_write_attr_cb,
  NULL
};

/* Public function ----------------------------------------- */
bStatus_t mss_add_service(void)
{
  uint8 status = SUCCESS;

  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService(mss_atrr_tbl,
                                       GATT_NUM_ATTRS(mss_atrr_tbl),
                                       &mss_callbacks);

  LOG("mss_add_service:%x\n", status);

  return ( status );
}

bStatus_t mss_set_parameter(mss_id_t char_id, uint8 len, void *value)
{
  bStatus_t ret = SUCCESS;


  return (ret);
}

bStatus_t mss_get_parameter(mss_id_t char_id, void *value)
{
  bStatus_t ret = SUCCESS;


  return (ret);
}

bStatus_t mss_notify(mss_id_t char_id, uint16 conn_handle, attHandleValueNoti_t *p_noti)
{
  bStatus_t ret;

  // Set the handle
  p_noti->handle = mss_atrr_tbl[CHAR_MODE_SELECTION_VALUE_POS].handle;
  
  // Send the notification
  ret = GATT_Notification(conn_handle, p_noti, FALSE);

  if (SUCCESS == ret)
  {
    LOG("mss_notify success\n");
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
static bStatus_t mss_write_attr_cb(uint16          conn_handle, 
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
static uint8 mss_read_attr_cb(uint16 conn_handle,
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
