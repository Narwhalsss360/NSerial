#ifndef NSerialCom_h
#define NSerialCom_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include <NDefs.h>
#include <NFuncs.h>
#include "NSerialData.h"

#if !(NFUNCS_MAJOR_VERSION >= 1 && NFUNCS_MINOR_VERSION >= 1)
#error "Update NDefs library to atleast 1.1.0."
#endif

#ifndef NSD_LENGTH
#define NSD_LENGTH 32
#endif

#define SOH 0x01

#ifndef IN_STREAM_BUFFER_LENGTH
#define IN_STREAM_BUFFER_LENGTH 37
#endif

#define STREAM_MIN_BUFFER_SIZE 9
#define STREAM_BUFFER_SIZE_INDEX_START 1
#define STREAM_BUFFER_SIZE_INDEX_LENGTH 2
#define STREAM_BUFFER_ADDRESS_INDEX_START 3
#define STREAM_BUFFER_ADDRESS_INDEX_LENGTH 4
#define STREAM_BUFFER_DATA_INDEX_START 7
#define STREAM_BUFFER_DATA_UINT32_LENGTH 8
#define STREAM_WAIT_TIME 3

class NSerialCom
{
private:
    NSD data[NSD_LENGTH];
    uint8_t nextAdd;
    pNSD newData;
    uint8_t search(uint16_t);
    void storeData(NSD);
    uint8_t streamBuffer[IN_STREAM_BUFFER_LENGTH];
    void clearBuffer();
    void n2sl(char[], uint32_t, uint8_t);
public:
    NSerialCom();
    void serialEvent();
    NSD get(uint16_t);
    void send(rNSD);
    void (*onReceive)(rNSD, uint8_t*);
};

extern void serialEvent();
extern NSerialCom NSerial;
#endif