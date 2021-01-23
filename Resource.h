#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef enum {
	ICN_UNKNOWN = -1,
  ICN_BATT_FULL = 0,
  ICN_BATT_75,
  ICN_BATT_50,
  ICN_BATT_25,
  ICN_CHRG_FULL,
  ICN_CHRG_75,
  ICN_CHRG_50,
  ICN_CHRG_25,
} IconID;

const uint16_t* getIcon(IconID eID, uint16_t* pWidth, uint16_t* pHeight);

#ifdef __cplusplus
}
#endif

#endif //_RESOURCE_H_
