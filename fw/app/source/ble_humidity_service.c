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
#define IBEACON_SET_CMD_DATA_MAX_LEN		20
#define SIMPLEPROFILE_SERVICE               0x00000001


/* Private Macros ----------------------------------------------------------- */
/* Private Defines ---------------------------------------------------------- */
#define SIMPLEPROFILE_SERV_UUID               0xFFF3
    
// Key Pressed UUID
#define SIMPLEPROFILE_CHAR1_UUID            0xFFF4
#define SIMPLEPROFILE_CHAR2_UUID            0xFFF5

// Simple GATT Profile Service UUID: 0xFFF0
static CONST uint8 simpleProfileServUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_SERV_UUID), HI_UINT16(SIMPLEPROFILE_SERV_UUID)
};

// Characteristic 1 UUID: 0xFFF1
static CONST uint8 simpleProfilechar1UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR1_UUID), HI_UINT16(SIMPLEPROFILE_CHAR1_UUID)
};

// Characteristic 2 UUID: 0xFFF2
static CONST uint8 simpleProfilechar2UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR2_UUID), HI_UINT16(SIMPLEPROFILE_CHAR2_UUID)
};


// Simple Profile Service attribute
static CONST gattAttrType_t simpleProfileService	=	{ ATT_BT_UUID_SIZE, simpleProfileServUUID };

// Simple Profile Characteristic 1 Properties
static uint8 simpleProfileChar1Props 			=	GATT_PROP_WRITE;
// Characteristic 1 Value
static uint8 simpleProfileChar1[IBEACON_SET_CMD_DATA_MAX_LEN];//max is 20bytes.
// Simple Profile Characteristic 1 User Description
static uint8 simpleProfileChar1UserDesp[]		=	"Set param\0";


// Simple Profile Characteristic 2 Properties
static uint8 simpleProfileChar2Props			=	GATT_PROP_READ | GATT_PROP_NOTIFY;
// Characteristic 2 Value
static uint8 simpleProfileChar2				=	0;
// Simple Profile Characteristic 2 User Description
static uint8 simpleProfileChar2UserDesp[]		=	"notify\0";
static gattCharCfg_t simpleProfileChar2Config;//[GATT_MAX_NUM_CONN];

static gattAttribute_t simpleProfileAttrTbl[]/*[SERVAPP_NUM_ATTR_SUPPORTED]*/ = 
{
	/* type */								/* permissions */			/* handle */	/* pValue */
	// Simple Profile Service
	{{ ATT_BT_UUID_SIZE, primaryServiceUUID },			GATT_PERMIT_READ,		0,				(uint8 *)&simpleProfileService},

	// Characteristic 1 Declaration
	{{ ATT_BT_UUID_SIZE, characterUUID },				GATT_PERMIT_READ,		0,				&simpleProfileChar1Props},
	// Characteristic Value 1
	{{ ATT_BT_UUID_SIZE, simpleProfilechar1UUID },	GATT_PERMIT_READ | GATT_PERMIT_WRITE,	0,	&simpleProfileChar1[0]},

	// Characteristic 2 Declaration
	{{ ATT_BT_UUID_SIZE, characterUUID },				GATT_PERMIT_READ,		0,				&simpleProfileChar2Props},
	// Characteristic Value 2
	{{ ATT_BT_UUID_SIZE, simpleProfilechar2UUID },	GATT_PERMIT_READ | GATT_PERMIT_WRITE,	0,	(uint8 *)&simpleProfileChar2},
	// Characteristic 2 User Description
//	{{ ATT_BT_UUID_SIZE, charUserDescUUID },			GATT_PERMIT_READ,		0,				simpleProfileChar2UserDesp},           
	// Characteristic 2 configuration
	{{ ATT_BT_UUID_SIZE, clientCharCfgUUID },			GATT_PERMIT_READ | GATT_PERMIT_WRITE,	0,	(uint8 *)&simpleProfileChar2Config}, 

};


/*********************************************************************
 * Profile Attributes - variables
 */
// Humidity Service attribut

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
static void simpleProfile_HandleConnStatusCB( uint16 connHandle, uint8 changeType )
{ 
	// Make sure this is not loopback connection
	if ( connHandle != LOOPBACK_CONNHANDLE )
	{
		// Reset Client Char Config if connection has dropped
		if ( ( changeType == LINKDB_STATUS_UPDATE_REMOVED )      ||
		     ( ( changeType == LINKDB_STATUS_UPDATE_STATEFLAGS ) && 
		       ( !linkDB_Up( connHandle ) ) ) )
		{ 
			//   GATTServApp_InitCharCfg( connHandle, simpleProfileChar4Config );
		}
	}
}
/*********************************************************************
 * PROFILE CALLBACKS
 */
// Humidity Service Callbacks
static CONST gattServiceCBs_t simpleProfileCBs =
{
  bhs_read_attr_cb,  	// Read callback function pointer
  bhs_write_attr_cb, 	// Write callback function pointer
  NULL                       	// Authorization callback function pointer
};

/*********************************************************************


/* Public function ----------------------------------------- */

bStatus_t bhs_add_service(uint32 services)
{
  uint8 status = SUCCESS;

  VOID linkDB_Register( simpleProfile_HandleConnStatusCB );  


  // Register GATT attribute list and CBs with GATT Server App
  if ( services & SIMPLEPROFILE_SERVICE )
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( simpleProfileAttrTbl, 
                                          GATT_NUM_ATTRS( simpleProfileAttrTbl ),
                                          &simpleProfileCBs );
  }
	LOG("addservic ret=%x\n",status);

  return ( status );
}

bStatus_t bhs_set_parameter(bhs_id_t char_id, uint8 len, void *value)
{
  bStatus_t ret = SUCCESS;


  return (ret);
}

bStatus_t bhs_get_parameter(bhs_id_t char_id, void *value)
{
  bStatus_t ret = SUCCESS;


  return (ret);
}

bStatus_t bhs_notify_humidity(uint16 conn_handle, attHandleValueNoti_t *p_noti)
{
  bhs_evt_t evt;
  bStatus_t ret;


  
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


  return (status);
}

/* End of file -------------------------------------------------------- */
