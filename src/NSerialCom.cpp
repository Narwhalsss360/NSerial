#include "NSerialCom.h"

NSerialCom::NSerialCom()
    :data({ NSerialData() }), streamBuffer({ ZERO }), newData(NULL)
{
}

void NSerialCom::clearBuffer()
{
    for (uint8_t i = ZERO; i < IN_STREAM_BUFFER_LENGTH; i++) { streamBuffer[i] = ZERO; }
}

void NSerialCom::n2sl(char buf[], uint32_t num,uint8_t length)
{
    String temp = String(num, HEX);

    if (temp.length() < length)
        temp = '0' + temp;

    char *tempCStr = temp.c_str();

    for (uint8_t i = 0; i < length; i++)
        buf[i] = tempCStr[i]; 
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
        streamBuffer[count] = (char)Serial.read();
        count++;
        delayMicroseconds(STREAM_WAIT_TIME);
    }

    if (streamBuffer[ZERO] == SOH && count >= STREAM_MIN_BUFFER_SIZE)
    {
        streamBuffer[count + 1] = NULLTERMINATOR;
        
        uint8_t type = x2i((streamBuffer + STREAM_BUFFER_TYPE_INDEX), 1);
        uint16_t address = x2i((streamBuffer + STREAM_BUFFER_ADDRESS_INDEX), STREAM_BUFFER_ADDRESS_INDEX_LENGTH);
        uint8_t size = strlen(streamBuffer) - STREAM_MIN_BUFFER_SIZE - 1;

        if (newData != NULL)
            newData->~NSerialData();

        newData = new NSerialData(type, address, (streamBuffer + STREAM_BUFFER_DATA_INDEX), size);
        Serial.println((uint16_t)newData);

        //storeData(newData);
        onReceive(*newData, streamBuffer);
    }
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
    clearBuffer();
    if (INVALID_NSD(newData))
        return;
    const uint8_t dataLength = newData.length;

    streamBuffer[ZERO] = SOH;
    n2sl(&streamBuffer[STREAM_BUFFER_SIZE_INDEX_START], newData.length, STREAM_BUFFER_SIZE_INDEX_LENGTH);
    n2sl(&streamBuffer[STREAM_BUFFER_ADDRESS_INDEX_START], newData.address, STREAM_BUFFER_ADDRESS_INDEX_LENGTH);

    char *sendBuffer;

    if (newData.str)
    {
        memcpy(&streamBuffer[STREAM_BUFFER_DATA_INDEX_START], newData.data, newData.length);
        sendBuffer = new char[newData.length + STREAM_MIN_BUFFER_SIZE];
        sendBuffer[newData.length + STREAM_MIN_BUFFER_SIZE - 1] = '\0';
    }
    else
    {
        n2sl(&streamBuffer[STREAM_BUFFER_DATA_INDEX_START], *(uint32_t*)newData.data, STREAM_BUFFER_DATA_UINT32_LENGTH);
        sendBuffer = new char[STREAM_MIN_BUFFER_SIZE + 8];
        sendBuffer[newData.length + STREAM_MIN_BUFFER_SIZE - 1] = '\0';
    }
    Serial.print(sendBuffer);
}

NSerialCom NSerial = NSerialCom();

extern void serialEvent()
{
    NSerial.serialEvent();
}