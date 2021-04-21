/**
 * @file       ble_services.h
 * @copyright  Copyright (C) 2020 ThuanLe. All rights reserved.
 * @license    This project is released under the ThuanLe License.
 * @version    2.0.0
 * @date       2020-06-02
 * @author     Thuan Le
 * @brief      BS (BLE Service)
 * @note       None
 */

/* Includes ----------------------------------------------------------- */
#include "ble_services.h"
#include "log.h"

#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"

/* Private Defines ---------------------------------------------------------- */
#define BS_UUID_SERV         (0xFFF3)
#define BS_UUID_CHAR_1       (0xFFF4)
#define BS_UUID_CHAR_2       (0xFFF5)

#define IBEACON_SET_CMD_DATA_MAX_LEN    (20)
#define CHAR_1_MEAS_VALUE_POS           (2)

/* Private Macros ----------------------------------------------------------- */
/* Private Defines ---------------------------------------------------------- */
// GATT Profile Service UUID: 0xFFF0
static CONST uint8 BS_UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(BS_UUID_SERV), HI_UINT16(BS_UUID_SERV)
};

// Characteristic 1 UUID: 0xFFF1
static CONST uint8 BS_CHAR_1_UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(BS_UUID_CHAR_1), HI_UINT16(BS_UUID_CHAR_1)
};

// Characteristic 2 UUID: 0xFFF2
static CONST uint8 BS_CHAR_2_UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(BS_UUID_CHAR_2), HI_UINT16(BS_UUID_CHAR_2)
};

// Characterictic property
uint8_t BS_CHAR_PROPS[] =
{
  0,
  GATT_PROP_READ | GATT_PROP_NOTIFY,
  GATT_PROP_READ | GATT_PROP_NOTIFY,
};

// Profile Service attribute
static CONST gattAttrType_t bs_service = { ATT_BT_UUID_SIZE, BS_UUID };

// Profile struct
static struct
{
  struct
  {
    struct
    {
      uint8_t char_1[1]; // Charaterictic 1 value;
    }
    value;
  }
  chars;
}
m_bs;

// Profile atrribute
static gattAttribute_t bs_atrr_tbl[] =
{
  // Profile Service
  {
    {ATT_BT_UUID_SIZE, primaryServiceUUID}, /* type */
    GATT_PERMIT_READ,                       /* permissions */
    0,                                      /* handle */
    (uint8 *)&bs_service                    /* p_value */
  },

  // Characteristic 1 Declaration
  {
    {ATT_BT_UUID_SIZE, characterUUID},
    GATT_PERMIT_READ,
    0,
    &BS_CHAR_PROPS[BS_ID_CHAR_1]
  },

  // Characteristic Value 1
  {
    {ATT_BT_UUID_SIZE, BS_CHAR_1_UUID},
    GATT_PERMIT_READ | GATT_PERMIT_WRITE,
    0,
    m_bs.chars.value.char_1
  },
};

/* Private function prototypes ---------------------------------------- */
static bStatus_t bs_read_attr_cb(uint16           conn_handle,
                                  gattAttribute_t *p_attr,
                                  uint8           *p_value, 
                                  uint8           *p_len, 
                                  uint16           offset, 
                                  uint8            max_len);

static bStatus_t bs_write_attr_cb(uint16          conn_handle, 
                                  gattAttribute_t *p_attr,
                                  uint8           *p_value, 
                                  uint8            len, 
                                  uint16           offset);

/*********************************************************************
 * PROFILE CALLBACKS
 */
static CONST gattServiceCBs_t bs_callbacks =
{
  bs_read_attr_cb,
  bs_write_attr_cb,
  NULL
};

/* Public function ----------------------------------------- */
bStatus_t bs_add_service(void)
{
  uint8 status = SUCCESS;

  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService(bs_atrr_tbl,
                                       GATT_NUM_ATTRS(bs_atrr_tbl),
                                       &bs_callbacks);

  LOG("bs_add_service:%x\n", status);

  return ( status );
}

bStatus_t bs_set_parameter(bs_id_t char_id, uint8 len, void *value)
{
  bStatus_t ret = SUCCESS;


  return (ret);
}

bStatus_t bs_get_parameter(bs_id_t char_id, void *value)
{
  bStatus_t ret = SUCCESS;


  return (ret);
}

bStatus_t bs_notify(bs_id_t char_id, uint16 conn_handle, attHandleValueNoti_t *p_noti)
{
  bStatus_t ret;

  // Set the handle
  p_noti->handle = bs_atrr_tbl[CHAR_1_MEAS_VALUE_POS].handle;
  
  // Send the notification
  ret = GATT_Notification(conn_handle, p_noti, FALSE);

  if (SUCCESS == ret)
  {
    LOG("bs_notify success\n");
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
static bStatus_t bs_write_attr_cb(uint16          conn_handle, 
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
static uint8 bs_read_attr_cb(uint16 conn_handle,
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
