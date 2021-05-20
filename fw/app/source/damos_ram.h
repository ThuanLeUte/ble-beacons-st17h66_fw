/**
 * @file       damos_ram.h
 * @copyright  Copyright (C) 2020 Thuan Le. All rights reserved.
 * @license    This project is released under the Thuan Le License.
 * @version    1.0.0
 * @date       2020-06-11
 * @author     Thuan Le
 * @brief      Damos RAM
 * @note       None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __DAMOS_RAM_H
#define __DAMOS_RAM_H

/* Includes ----------------------------------------------------------- */
#include "stdint.h"

/* Public defines ---------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */
typedef struct
{
  uint8_t mode_selected;
  uint8_t click_count;
}
device_t;

/* Public variables --------------------------------------------------- */
extern device_t g_dispenser;

/* Public Callbacks -------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

#endif // __DAMOS_RAM_H

/* End of file -------------------------------------------------------- */
