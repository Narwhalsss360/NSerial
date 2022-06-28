#ifndef NSerialData_h
#define NSerialData_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include <NDefs.h>

#define INVALID_NSD(nsd) (nsd.length == ZERO || nsd.data == NULL) ? true : false

enum NSDTYPES
{
    NSD_UINT8 = 1,
    NSD_INT8,
    NSD_UINT16,
    NSD_INT16,
    NSD_UINT32,
    NSD_INT32,
    NSD_FLOAT,
    NSD_DOUBLE,
    NSD_STRING
};

typedef struct NSerialData
{
    uint8_t type;
    uint16_t address;
    uint8_t length;
    byte *data;
    NSerialData();
    NSerialData(uint8_t, uint16_t, void *, uint8_t);
    NSerialData(uint16_t, uint32_t);
    NSerialData(uint16_t, int32_t);
    NSerialData(uint16_t, double);
    NSerialData(uint16_t, float);
    NSerialData(uint16_t, String);
} NSD_t;

typedef NSD_t NSD;
typedef NSD *pNSD;
typedef NSD &rNSD;

#endif