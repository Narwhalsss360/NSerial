#ifndef NSerialCom_h
#define NSerialCom_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <NDefs.h>

#ifndef NSD_LENGTH
#define NSD_LENGTH 32
#endif

#define SOH 0x01
#define EOT 0x04

#ifndef IN_STREAM_BUFFER_LENGTH
#define IN_STREAM_BUFFER_LENGTH 38
#endif

#define STREAM_MIN_BUFFER_SIZE 6
#define STREAM_BUFFER_SIZE_INDEX_START 1
#define STREAM_BUFFER_ADDRESS_INDEX_START 2
#define STREAM_BUFFER_ADDRESS_INDEX_END 3
#define STREAM_BUFFER_DATA_INDEX_START 4

#define INVALID_NSD(nsd) (nsd.length == ZERO) ? true : false

struct NSerialData
{
    uint16_t address;
    uint8_t *data;
    uint8_t length;
    NSerialData();
    NSerialData(uint16_t, void *, uint8_t);
    NSerialData(uint16_t, String);
    NSerialData(uint16_t, uint32_t);
    NSerialData(uint16_t, char *);
    ~NSerialData();
    void get(uint32_t &);
    void get(String &);
};

typedef NSerialData NSD;
typedef NSD *pNSD;
typedef NSD &rNSD;

class NSerialComClass
{
private:
    NSD data[NSD_LENGTH];
    uint8_t nextAdd;
    uint16_t length;
    uint8_t search(uint16_t);
    void storeData(NSD);
    uint8_t inStreamBuffer[IN_STREAM_BUFFER_LENGTH] = { ZERO };
public:
    NSerialComClass();
    void serialEvent();
    NSD get(uint16_t);
    void send(NSD &);
    void (*onReceive)(NSD, const char*);
};

extern void serialEvent();
extern NSerialComClass NSerial;
#endif