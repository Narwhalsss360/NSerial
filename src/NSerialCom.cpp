#include "NSerialCom.h"

#pragma region NSerialData
NSerialData::NSerialData()
    : address(NULL), data(NULL), length(NULL), str(false)
{
}

NSerialData::NSerialData(uint16_t addr, void *pData, uint8_t len)
    :address(addr), length(len), data(nullptr), str(false);
{
    data = (uint8_t*)malloc(len);
    if (data == nullptr)
    {
        length = 0;
        return;
    }
    memmove(data, pData, len);
    return;
}

NSerialData::NSerialData(uint16_t addr, String str)
    : NSerialData(addr, (void *)str.c_str(), str.length() + 1)
{
    str = true;
}

NSerialData::NSerialData(uint16_t addr, uint32_t num)
    : NSerialData(addr, (void *)&num, sizeof(num))
{
    str = false;
}

NSerialData::NSerialData(uint16_t addr, char *cstr)
    : NSerialData(addr, (void *)str, strlen(cstr) + 1)
{
    str = true;
}

NSerialData::~NSerialData()
{
    free(data);
    free(this);
}

void NSerialData::get(uint32_t &out)
{
    if (!str)
        out = x2i(data, length);
}

void NSerialData::get(String &out)
{
    if (str)
        out = (char *)data;
}
#pragma endregion

#pragma region NSerialCom
NSerialCom::NSerialCom()
    :data({ NSerialData() }), streamBuffer({ ZERO }), newData(NULL)
{
}

void NSerialCom::clearBuffer()
{
    for (uint8_t i = ZERO; i < IN_STREAM_BUFFER_LENGTH; i++)
    {
        streamBuffer[i] = ZERO;
    }
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
        char* sizeHex = streamBuffer + STREAM_BUFFER_SIZE_INDEX_START;
        char* addressHex = streamBuffer + STREAM_BUFFER_ADDRESS_INDEX_START;

        uint16_t address = x2i(addressHex, STREAM_BUFFER_ADDRESS_INDEX_LENGTH);
        uint8_t size = x2i(sizeHex, STREAM_BUFFER_SIZE_INDEX_LENGTH);
        
        if (newData != NULL)
            newData->~NSerialData();

        newData = new NSerialData(address, (void *)&streamBuffer[STREAM_BUFFER_DATA_INDEX_START], size * 2);
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
    if (!newData.length)
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
#pragma endregion NSerialCom

#pragma region NSerialEvents
NSerialCom NSerial = NSerialCom();

extern void serialEvent()
{
    NSerial.serialEvent();
}
#pragma endregion