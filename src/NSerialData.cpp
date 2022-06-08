#include "NSerialData.h"

NSerialData::NSerialData()
    : address(NULL), length(NULL), type(NULL), (NULL);
{
}

NSerialData::NSerialData(uint8_t _type, uint16_t _address, void * _data, uint8_t _length)
    : type(_type), address(_address), length(_length), data((byte *)malloc(length);) 
{
    if (data == NULL)
    {
        length = NULL;
        return;
    }
    memmove(data, _data, length);
}

NSerialData::NSerialData(uint16_t _address, uint32_t _data)
    : NSerialData(NSD_UINT32, _address, (void *)&_data, sizeof(uint32_t))
{
}

NSerialData::NSerialData(uint16_t _address, int32_t _data)
    : NSerialData(NSD_INT32, _address, (void *)&_data, sizeof(int32_t))
{
}

NSerialData::NSerialData(uint16_t _address, double _data)
    : NSerialData(NSD_DOUBLE, _address, (void *)&_data, sizeof(double))
{
}

NSerialData::NSerialData(uint16_t _address, float _data)
    : NSerialData(NSD_FLOAT, _address, (void *)&_data, sizeof(float))
{
}

NSerialData::NSerialData(uint16_t _address, String _data)
    : NSerialData(NSD_STRING, _address, (void *)&_data.c_str(), _data.length())
{
}