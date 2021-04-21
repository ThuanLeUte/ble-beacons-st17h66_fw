/**
 * @file       ble_dispenser.c
 * @copyright  Copyright (C) 2020 ThuanLe. All rights reserved.
 * @license    This project is released under the ThuanLe License.
 * @version    1.0.0
 * @date       2021-04-17
 * @author     Thuan Le
 * @brief      BLE application
 * @note       None
 * @example    None
 */

/* Includes ----------------------------------------------------------- */
#include "bcomdef.h"
#include "rf_phy_driver.h"
#include "global_config.h"
#include "OSAL.h"
#include "OSAL_PwrMgr.h"
#include "gatt.h"
#include "hci.h"
#include "gapgattserver.h"
#include "gattservapp.h"
#include "devinfoservice.h"
#include "sbpProfile_ota.h"
#include "ota_app_service.h"
#include "peripheral.h"
#include "gapbondmgr.h"

#include "ble_dispenser.h"
#include "ll.h"
#include "ll_hw_drv.h"
#include "ll_def.h"
#include "hci_tl.h"
#include "fs.h"
#include "osal_snv.h"

#include "ble_humidity_service.h"

/* Private defines ---------------------------------------------------- */
#define DEVINFO_SYSTEM_ID_LEN   8
#define DEVINFO_SYSTEM_ID       0

#define DEFAULT_DISCOVERABLE_MODE GAP_ADTYPE_FLAGS_GENERAL

// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL 20

// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL 30

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY 0

// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT 1000

// Whether to enable automatic parameter update request when a connection is formed
#define DEFAULT_ENABLE_UPDATE_REQUEST TRUE

// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL 6

// Length of bd addr as a string
#define B_ADDR_STR_LEN 15

#define BEACON_STOREDATA_FS_ID      (0x88)
#define BEACON_STORE_FS_FLAG_ID     (0x89)
#define BEACON_STORE_FS_FLAG_DATA   (0x66)

// Index start from 0
#define BEACON_ADV_UUID_INDEX       (9)
#define BEACON_ADV_MAJOR_INDEX      (25)
#define BEACON_ADV_MINOR_INDEX      (27)
#define BEACON_ADV_RSSI_INDEX       (29)

/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
volatile uint8_t g_current_adv_type = LL_ADV_CONNECTABLE_UNDIRECTED_EVT;

/* Private variables -------------------------------------------------- */
static uint8 m_dispenser_task_id;   // Task ID for internal task/event processing
static gaprole_States_t m_gap_profile_state	=	GAPROLE_INIT;
static uint16 m_gap_conn_handle;

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8 m_scan_rsp_data[] =
{
          // Complete name
  0x09,   // Length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  'D', 'I', 'S', 'P', 'E', 'N', ' ', ' ',
};

// Advert data for iBeacon
static uint8 m_advert_data[] =
{
  0x02,                             // Length of this data
  GAP_ADTYPE_FLAGS,
  DEFAULT_DISCOVERABLE_MODE |
  GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
  0x1A,                             // Length of this data including the data type byte
  GAP_ADTYPE_MANUFACTURER_SPECIFIC, // Manufacturer specific adv data type
  0x4c,                             // Company ID - Fixed
  0x00,                             // Company ID - Fixed
  0x02,                             // Data Type - Fixed
  0x15,                             // Data Length - Fixed
  0x7d, 0xb8, 0x60, 0xed, 0xb6, 0x4d, 0x4b, 0xb1,  // uuid
  0x98, 0x75, 0x8f, 0x16, 0x35, 0x5a, 0x97, 0xd2,  // uuid

  0x00,0x07,                        // Major
  0x02,0x55,                        // Minor
  0xc5                              // Power - The 2's complement of the calibrated Tx Power
};

ibeacon_store_data_t m_beacon_default_data = {
  {'D', 'I', 'S', 'P', 'E', 'N', ' ', ' '},        // Device name

  {0x7d, 0xb8, 0x60, 0xed, 0xb6, 0x4d, 0x4b, 0xb1, // uuid
   0x98, 0x75, 0x8f, 0x16, 0x35, 0x5a, 0x97, 0xd2},
  {0x00,0x07},  // Major
  {0x02,0x55},  // Minor
  500,          // Adv default interval 800*0.625=500ms
  0xC5,         // RSSI
  0xff
};

// GAP GATT Attributes
static uint8 m_att_device_name[GAP_DEVICE_NAME_LEN] = "DISPEN  ";

/* Private function prototypes ---------------------------------------- */
static void m_ble_process_osal_msg(osal_event_hdr_t *m_msg);
static void m_ble_dispenser_state_notification_cb(gaprole_States_t new_state);
static void m_ble_change_cb(uint8 param_id);
static void m_ble_update_adv(void);

static void m_ble_notify_humi(void);

// GAP Role Callbacks
static gapRolesCBs_t m_ble_dispenser_cbs =
{
  m_ble_dispenser_state_notification_cb, // Profile State Change Callbacks
  NULL
};

// Simple GATT Profile Callbacks
static simpleProfileCBs_t simpleBLEPeripheral_SimpleProfileCBs =
{
  m_ble_change_cb    // Charactersitic value change callback
};
static void on_ble_humi_service_evt(bhs_evt_t *pev)
{ 
  switch (pev->evt_id)
  {
  case BHS_EVT_NOTIFY_SENT:
    break;
  
  default:
    break;
  }
}
/* Function definitions ----------------------------------------------- */
/**
 * @brief   Initialization function for the Simple BLE Peripheral App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 * 
 * @param[in]   task_id  The OSAL assigned task ID
 *
 * @attention   None
 *
 * @return      Events not processed
 */
void ble_dispenser_init(uint8 task_id)
{
  uint16 fs_read_len  = 0;
  int    snv_ret      = 0;
  uint16 adv_intvl    = 0;
  m_dispenser_task_id = task_id;
  uint8 fs_flag;

  // Read configure parameters
  {
    osal_snv_read(BEACON_STORE_FS_FLAG_ID, 1, &fs_flag);

    if (fs_flag != BEACON_STORE_FS_FLAG_DATA)
    {
      fs_flag = BEACON_STORE_FS_FLAG_DATA;
      osal_snv_write(BEACON_STORE_FS_FLAG_ID, 1, &fs_flag);
      osal_snv_write(BEACON_STOREDATA_FS_ID, sizeof(ibeacon_store_data_t) / sizeof(uint8), &m_beacon_default_data);
    }
    osal_snv_read(BEACON_STOREDATA_FS_ID, sizeof(ibeacon_store_data_t) / sizeof(uint8), &Ibeacon_store_data);
    osal_memcpy(m_att_device_name, Ibeacon_store_data.beacon_dev_name, IBEACON_DEV_NAME_MAX_LEN);
    osal_memcpy(&m_scan_rsp_data[2], Ibeacon_store_data.beacon_dev_name, IBEACON_DEV_NAME_MAX_LEN);
    osal_memcpy(&m_advert_data[BEACON_ADV_UUID_INDEX], Ibeacon_store_data.uuid, IBEACON_UUID_LEN);
    osal_memcpy(&m_advert_data[BEACON_ADV_MAJOR_INDEX], Ibeacon_store_data.major, IBEACON_VERSION_LEN);
    osal_memcpy(&m_advert_data[BEACON_ADV_MINOR_INDEX], Ibeacon_store_data.minor, IBEACON_VERSION_LEN);
    osal_memcpy(&m_advert_data[BEACON_ADV_RSSI_INDEX], &Ibeacon_store_data.RSSI, 1);
    LOG("Ibeacon_store_data.adv_intvl=%d\n", Ibeacon_store_data.advIntvl);
    adv_intvl = Ibeacon_store_data.advIntvl * 1000 / 625;
    LOG("intvl=%d\n", adv_intvl);
  }

  // Setup the GAP
  VOID GAP_SetParamValue(TGAP_CONN_PAUSE_PERIPHERAL, DEFAULT_CONN_PAUSE_PERIPHERAL);

  // Setup the GAP Peripheral Role Profile
  {
    // Device starts advertising upon initialization
    uint8 initial_advertising_enable = TRUE;

    uint8 enable_update_request = DEFAULT_ENABLE_UPDATE_REQUEST;
    uint8 adv_chn_map = GAP_ADVCHAN_37 | GAP_ADVCHAN_38 | GAP_ADVCHAN_39;

    // By setting this to zero, the device will go into the waiting state after
    // being discoverable for 30.72 second, and will not being advertising again
    // until the enabler is set back to TRUE
    uint16 gap_role_advert_offtime = 0;

    uint16 desired_min_interval  = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16 desired_max_interval  = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16 desired_slave_latency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16 desired_conn_timeout  = DEFAULT_DESIRED_CONN_TIMEOUT;

    uint8 peer_public_addr[] = 
    {
      0x01,
      0x02,
      0x03,
      0x04,
      0x05,
      0x06
    };

    GAPRole_SetParameter(GAPROLE_ADV_DIRECT_ADDR, sizeof(peer_public_addr), peer_public_addr);

    // Set adv channel map
    GAPRole_SetParameter(GAPROLE_ADV_CHANNEL_MAP, sizeof(uint8), &adv_chn_map);

    // Set the GAP Role Parameters
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8), &initial_advertising_enable);
    GAPRole_SetParameter(GAPROLE_ADVERT_OFF_TIME, sizeof(uint16), &gap_role_advert_offtime);

    GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(m_scan_rsp_data), m_scan_rsp_data);
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(m_advert_data), m_advert_data);

    GAPRole_SetParameter(GAPROLE_PARAM_UPDATE_ENABLE, sizeof(uint8), &enable_update_request);
    GAPRole_SetParameter(GAPROLE_MIN_CONN_INTERVAL, sizeof(uint16), &desired_min_interval);
    GAPRole_SetParameter(GAPROLE_MAX_CONN_INTERVAL, sizeof(uint16), &desired_max_interval);
    GAPRole_SetParameter(GAPROLE_SLAVE_LATENCY, sizeof(uint16), &desired_slave_latency);
    GAPRole_SetParameter(GAPROLE_TIMEOUT_MULTIPLIER, sizeof(uint16), &desired_conn_timeout);
  }

  // Set the GAP Characteristics
  GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, m_att_device_name);

  // Set advertising interval
  {
    uint16 adv_int = 3200;   // Actual time = adv_int * 625us
    adv_int = adv_intvl;
    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MIN, adv_int);
    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MAX, adv_int);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, adv_int);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, adv_int);
  }

  // Initialize GATT attributes
  GGS_AddService(GATT_ALL_SERVICES);           // GAP
  GATTServApp_AddService(GATT_ALL_SERVICES);   // GATT attributes
  DevInfo_AddService();                        // Device Information Service
  // SimpleProfile_AddService(GATT_ALL_SERVICES); // Simple GATT Profile
  bhs_add_service(GATT_ALL_SERVICES); // Humidity Service


  // Setup a delayed profile startup
  osal_set_event(m_dispenser_task_id, SBP_START_DEVICE_EVT);

  LOG("======================ble_dispenser_init done====================\n");
}

/**
 * @brief       This function is called to process all events for the task.
 *              Events include timers, messages and any other user defined events.
 *
 * @param[in]   task_id  The OSAL assigned task ID
 * @param[in]   events   Events to process
 *
 * @attention   None
 *
 * @return      Events not processed
 */
uint16 ble_dispenser_process_event(uint8 task_id, uint16 events)
{
  VOID task_id; // OSAL required parameter that isn't used in this function

  LOG("ble_dispenser_process_event: 0x%x\n", events);

  if (events & SYS_EVENT_MSG)
  {
    uint8 *m_msg;
    if ((m_msg = osal_msg_receive(m_dispenser_task_id)) != NULL)
    {
      m_ble_process_osal_msg((osal_event_hdr_t *)m_msg);

      // Release the OSAL message
      VOID osal_msg_deallocate(m_msg);
    }
    return (events ^ SYS_EVENT_MSG);
  }

  if (events & SBP_START_DEVICE_EVT)
  {
    // Start the Device
    VOID GAPRole_StartDevice(&m_ble_dispenser_cbs);
    HCI_LE_ReadResolvingListSizeCmd();

    LOG("Devive start\n");

    return (events ^ SBP_START_DEVICE_EVT);
  }

  if (events & SBP_CONNECTED_EVT)
  {
    LOG("Device connected");
    return (events ^ SBP_CONNECTED_EVT);
  }

  if (events & SBP_NOTIFY_EVT)
  {
    LOG("Device notify data");
    return (events ^ SBP_NOTIFY_EVT);
  }

  return 0;
}

/* Private Function definitions --------------------------------------- */
/**
 * @brief       Process an incoming task message.
 *
 * @param[in]   m_msg     Message to process
 *
 * @attention   None
 *
 * @return      None
 */
static void m_ble_process_osal_msg(osal_event_hdr_t *m_msg)
{
  hciEvt_CmdComplete_t *p_hci_msg;
  switch (m_msg->event)
  {
  case HCI_GAP_EVENT_EVENT:
  {
    switch (m_msg->status)
    {
    case HCI_COMMAND_COMPLETE_EVENT_CODE:
      p_hci_msg = (hciEvt_CmdComplete_t *)m_msg;

      LOG("==> HCI_COMMAND_COMPLETE_EVENT_CODE: %x\n", p_hci_msg->cmdOpcode);
      break;
    default:
      break;
    }
  }
  }
}

/**
 * @brief       Notification from the profile of a state change
 *
 * @param[in]   new_state  New state
 *
 * @attention   None
 *
 * @return      None
 */
static void m_ble_dispenser_state_notification_cb(gaprole_States_t new_state)
{
  switch (new_state)
 {
  case GAPROLE_STARTED:
  {
    uint8 own_address[B_ADDR_LEN];
    uint8 str_addr[14]               = {0};
    uint8 initial_advertising_enable = TRUE;

    GAPRole_GetParameter(GAPROLE_BD_ADDR, own_address);

    // Set the GAP Characteristics
    GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, m_att_device_name);
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8), &initial_advertising_enable);
  }
  break;

  case GAPROLE_ADVERTISING:
    break;

  case GAPROLE_CONNECTED:
    HCI_PPLUS_ConnEventDoneNoticeCmd(m_dispenser_task_id, NULL);
    GAPRole_GetParameter(GAPROLE_CONNHANDLE, &m_gap_conn_handle);
    osal_set_event(m_dispenser_task_id, SBP_CONNECTED_EVT);
    break;

  case GAPROLE_CONNECTED_ADV:
    break;
  case GAPROLE_WAITING:
    break;

  case GAPROLE_WAITING_AFTER_TIMEOUT:
    break;

  case GAPROLE_ERROR:
    break;

  default:
    break;
  }
  m_gap_profile_state = new_state;

  LOG("[GAP ROLE %d]\n", new_state);

  VOID m_gap_profile_state;
}

/**
 * @brief       Callback from SimpleBLEProfile indicating a value change
 *
 * @param[in]   param_id  Parameter ID of the value that was changed.
 *
 * @attention   None
 *
 * @return      None
 */
static void m_ble_change_cb(uint8 param_id)
{
  switch (param_id)
  {
  default:
    break;
  }
}

/**
 * @brief       Update adv data and change the adv type
 *
 * @param[in]   None
 *
 * @attention   None
 *
 * @return      None
 */
static void m_ble_update_adv(void)
{
  uint8 new_uuid[IBEACON_UUID_LEN];
  uint16 major;
  uint16 minor;
  uint8 power;
  uint8 adv_type;
  uint8 initial_advertising_enable = FALSE;

  GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8), &initial_advertising_enable);
  LOG("Disable advertising\n");

  // Change advertising type
  g_current_adv_type = LL_ADV_NONCONNECTABLE_UNDIRECTED_EVT;
  adv_type           = g_current_adv_type;

  GAPRole_SetParameter(GAPROLE_ADV_EVENT_TYPE, sizeof(uint8), &adv_type);

  // Reset advertisement event, note that GAP/LL will process close adv event in advance
  osal_start_timerEx(m_dispenser_task_id, SBP_ADD_RL_EVT, 500);
}

void periodic_1s_callback(void)
{
  // m_ble_notify_humi();
  LOG("Send notify");
}

static void m_ble_notify_humi(void)
{
  attHandleValueNoti_t humi_meas;
  humi_meas.len = 1;
  humi_meas.value[0] = 100;
  bhs_notify_humidity(m_gap_conn_handle, &humi_meas);
}

/* Publish Function definitions --------------------------------------- */
void SimpleBLEPeripheral_SetDevName(uint8*data,uint8 len)
{
  uint8 dev_name[IBEACON_DEV_NAME_MAX_LEN] = {0};
  osal_memcpy(dev_name, data, len);
  osal_memcpy(Ibeacon_store_data.beacon_dev_name, dev_name, IBEACON_DEV_NAME_MAX_LEN);
  osal_memcpy(&m_scan_rsp_data[2], dev_name, IBEACON_DEV_NAME_MAX_LEN);
  osal_snv_write(BEACON_STOREDATA_FS_ID, sizeof(ibeacon_store_data_t) / sizeof(uint8), &Ibeacon_store_data);
}

void SimpleBLEPeripheral_SetBeaconUUID(uint8 *data, uint8 len)
{
  osal_memcpy(Ibeacon_store_data.uuid, data, len);
  osal_memcpy(&m_advert_data[BEACON_ADV_UUID_INDEX], data, len);
  GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(m_advert_data), m_advert_data);
  osal_snv_write(BEACON_STOREDATA_FS_ID, sizeof(ibeacon_store_data_t) / sizeof(uint8), &Ibeacon_store_data);
}

void SimpleBLEPeripheral_SetMajor(uint8 *data, uint8 len)
{
  osal_memcpy(&Ibeacon_store_data.major, data, len);
  osal_memcpy(&m_advert_data[BEACON_ADV_MAJOR_INDEX], data, len);
  GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(m_advert_data), m_advert_data);
  osal_snv_write(BEACON_STOREDATA_FS_ID, sizeof(ibeacon_store_data_t) / sizeof(uint8), &Ibeacon_store_data);
}
void SimpleBLEPeripheral_SetMinor(uint8 *data, uint8 len)
{
  osal_memcpy(&Ibeacon_store_data.minor, data, len);
  osal_memcpy(&m_advert_data[BEACON_ADV_MINOR_INDEX], data, len);
  GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(m_advert_data), m_advert_data);
  osal_snv_write(BEACON_STOREDATA_FS_ID, sizeof(ibeacon_store_data_t) / sizeof(uint8), &Ibeacon_store_data);
}

void SimpleBLEPeripheral_SetRSSI(uint8 data)
{
  uint8 temp;
  Ibeacon_store_data.RSSI = data;
  m_advert_data[BEACON_ADV_RSSI_INDEX] = data;
  GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(m_advert_data), m_advert_data);
  osal_snv_write(BEACON_STOREDATA_FS_ID, sizeof(ibeacon_store_data_t) / sizeof(uint8), &Ibeacon_store_data);
}

void SimpleBLEPeripheral_SetAdvIntvlTime(uint16 data)
{
  uint32 tmp = 0;
  uint16 adv_int = 800; // Actual time = adv_int * 625us
  tmp = (data * 1000) / 625;
  adv_int = (uint16)tmp;
  Ibeacon_store_data.advIntvl = data;
  GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MIN, adv_int);
  GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MAX, adv_int);
  GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, adv_int);
  GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, adv_int);

  osal_snv_write(BEACON_STOREDATA_FS_ID, sizeof(ibeacon_store_data_t) / sizeof(uint8), &Ibeacon_store_data);
}

void simpleProfile_Set_Notify_Event(void)
{
  osal_set_event(m_dispenser_task_id, SBP_NOTIFY_EVT);
}

/* End of file -------------------------------------------------------- */
