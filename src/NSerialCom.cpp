#include "NSerialCom.h"

NSerialData::NSerialData()
    : address(NULL), data(NULL), length(NULL)
{
}

NSerialData::NSerialData(uint16_t addr, void *pData, uint8_t len)
    :address(addr), length(len), data(nullptr)
{
    Serial.println("\nAllocating data...");
    data = (uint8_t*)malloc(len);
    Serial.println("Allocated data.");
    if (data == nullptr)
    {
        length = 0;
        return;
    }
    memmove(data, pData, len);
    Serial.println("Moved data");
    return;
}

NSerialData::NSerialData(uint16_t addr, String str)
    : NSerialData(addr, (void *)str.c_str(), str.length() + 1)
{
}

NSerialData::NSerialData(uint16_t addr, uint32_t num)
    : NSerialData(addr, (void *)&num, sizeof(num))
{
}

NSerialData::NSerialData(uint16_t addr, char *str)
    : NSerialData(addr, (void *)str, strlen(str) + 1)
{
}

NSerialData::~NSerialData()
{
    Serial.println("Destructor called");
    free(this);
}

void NSerialData::get(uint32_t &out)
{
    out = x2i(data, length);
}

void NSerialData::get(String &out)
{
    out = (char *)data;
}

NSerialCom::NSerialCom()
    :data({ NSerialData() }), inStreamBuffer({ ZERO }), newData(NULL)
{
}

void NSerialCom::clearBuffer()
{
    for (uint8_t i = ZERO; i < IN_STREAM_BUFFER_LENGTH; i++)
    {
        inStreamBuffer[i] = ZERO;
    }
}

uint8_t NSerialCom::search(uint16_t addr)
{
    for (uint8_t i = ZERO; i < NSD_LENGTH; i++)
    {
        if (data[i].address == i)
            return i;
    }
    return NSD_LENGTH;
}

void NSerialCom::storeData(NSD newData)
{
    uint8_t index = search(newData.address);
    if (index < NSD_LENGTH)
    {
        data[index] = newData;
    }
    else
    {
        if (nextAdd == NSD_LENGTH)
            nextAdd = ZERO;
        data[nextAdd] = newData;
        nextAdd++;
    }
}

void NSerialCom::serialEvent()
{
    clearBuffer();
    uint8_t count = ZERO;

    while (Serial.available())
    {
        inStreamBuffer[count] = (char)Serial.read();
        count++;
        delayMicroseconds(STREAM_WAIT_TIME);
    }

    if (inStreamBuffer[ZERO] == SOH && count >= STREAM_MIN_BUFFER_SIZE)
    {
        char* sizeHex = inStreamBuffer + STREAM_BUFFER_SIZE_INDEX_START;
        char* addressHex = inStreamBuffer + STREAM_BUFFER_ADDRESS_INDEX_START;
        
        uint16_t address = x2i(addressHex, STREAM_BUFFER_ADDRESS_INDEX_LENGTH);
        uint8_t size = x2i(sizeHex, STREAM_BUFFER_SIZE_INDEX_LENGTH);
        
        if (newData != NULL)
            newData->~NSerialData();

        Serial.println("Allocating struct...");
        newData = new NSD(address, &inStreamBuffer[STREAM_BUFFER_DATA_INDEX_START], size * 2);
        Serial.println("Allocated struct.");

        
        //storeData(newData);
        //onReceive(newData, inStreamBuffer);
        
    }
    clearBuffer();
}

NSD NSerialCom::get(uint16_t addr)
{
    uint8_t index = search(addr);
    if (index < NSD_LENGTH)
        return data[index];
    return NSD();
}

void NSerialCom::send(rNSD newData)
{
    if (!newData.length)
        return;
    const uint8_t bufferLength = newData.length + STREAM_MIN_BUFFER_SIZE - 1;
    uint8_t sendBuffer[bufferLength];
    memset(sendBuffer, '_', bufferLength);

    sendBuffer[ZERO] = SOH;
    sendBuffer[STREAM_BUFFER_SIZE_INDEX_START] = newData.length;
    uint8_t address[2] = { 122 , 122 };
    memcpy(&sendBuffer[STREAM_BUFFER_ADDRESS_INDEX_START], &newData.address, sizeof(uint16_t));

    for (uint8_t i = ZERO; i <= newData.length; i++)
    {
        sendBuffer[STREAM_BUFFER_DATA_INDEX_START + i] = newData.data[i];
    }

    Serial.print((const char*)sendBuffer);
}

NSerialCom NSerial = NSerialCom();

extern void serialEvent()
{
    NSerial.serialEvent();
}