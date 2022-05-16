#include "NSerialCom.h"

NSerialData::NSerialData()
    : address(NULL), data(NULL), length(NULL)
{
}

NSerialData::NSerialData(uint16_t addr, void *pData, uint8_t len)
    : address(addr), data((uint8_t *)malloc(length)), length(len)
{
    memcpy(data, pData, length);
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
    free(data);
}

void NSerialData::get(uint32_t &out)
{
    out = reinterpret_c_style(uint32_t, data);
}

void NSerialData::get(String &out)
{
    out = (char *)data;
}

NSerialCom::NSerialCom()
    :data({ NSerialData() })
{
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
    uint8_t count = ZERO;
    bool end = false;

    while (Serial.available())
    {
        if (count < IN_STREAM_BUFFER_LENGTH || !end)
            inStreamBuffer[count] = (char)Serial.read();
        else
            Serial.read();
        if (inStreamBuffer[count] == EOT)
            end = true;
        count++;
    }

    if (inStreamBuffer[ZERO] == SOH)
    {
        if ((reinterpret_c_style(uint16_t, &inStreamBuffer[STREAM_BUFFER_ADDRESS_INDEX_START]) + STREAM_MIN_BUFFER_SIZE) != strlen((const char*)inStreamBuffer))
            onReceive(NSD(), (const char*)inStreamBuffer);
            return;

        storeData({inStreamBuffer[STREAM_BUFFER_SIZE_INDEX_START], &inStreamBuffer[STREAM_BUFFER_DATA_INDEX_START], reinterpret_c_style(uint16_t, &inStreamBuffer[STREAM_BUFFER_ADDRESS_INDEX_START])});
        if (onReceive != NULL)
            onReceive({inStreamBuffer[STREAM_BUFFER_SIZE_INDEX_START], &inStreamBuffer[STREAM_BUFFER_DATA_INDEX_START], reinterpret_c_style(uint16_t, &inStreamBuffer[STREAM_BUFFER_ADDRESS_INDEX_START])}, (const char*)inStreamBuffer);
    }
    else
    {
        onReceive(NSD(), (const char*)inStreamBuffer);
    }
    memset(inStreamBuffer, NULL, IN_STREAM_BUFFER_LENGTH);
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

    sendBuffer[bufferLength - 1] = EOT;
    Serial.print((const char*)sendBuffer);
}

NSerialCom NSerial = NSerialCom();

extern void serialEvent()
{
    NSerial.serialEvent();
}