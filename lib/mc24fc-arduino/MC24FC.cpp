#include "MC24FC.h"
#include <Wire.h>

MC24FC::MC24FC(std::uint8_t i2c_addr, std::uint32_t max_capacity_bits) :
    _i2cAddr(i2c_addr & 0x7F) // Address is 7-bit
    , _maxCapacity(max_capacity_bits)
    , _currentAddress(0xFFFF) // Keep the address out of bounds.
{
}
MC24FC::~MC24FC()
{
    Wire.end();
}

void MC24FC::init()
{
    Wire.begin();
}

bool MC24FC::readByte(std::uint16_t address, std::uint8_t* data)
{
    _currentAddress = address;
    if (_currentAddress >= _maxCapacity / 8)
    {
        // Out of range!
        Serial.println("Address out of range");
        return false;
    }
    // We must convert the address to big endian
    char address_bytes[2];
    address_bytes[0] = static_cast<char>(address >> 8);
    address_bytes[1] = static_cast<char>(address & 0xFF);

    // Address the EEPROM as "Write" to write the random access address
    Wire.beginTransmission(_i2cAddr);
    Wire.write(address_bytes);
    auto ret = Wire.endTransmission(false); // Keep the line hot
    if (ret != 0)
    {
        Serial.println(ret);
    }
    if (ret == 5)
    {
        Serial.println("ERROR: Timed out");
        return false;
    }
    if (ret == 2)
    {
        Serial.println("ERROR: Wrong address");
        return false;
    }
    // Address the EEPROM as "Read" to read from the written address above
    // NOTE: "Read" bit is handled internally.
    auto size = Wire.requestFrom(_i2cAddr, 1);
    if (size != 1)
    {
        Serial.println("ERROR: Bad size");
        return false;
    }
    *data = Wire.read();
    _currentAddress++;
    return true;
}

std::uint8_t MC24FC::readNext()
{
    // Address the EEPROM as "Read" to read from the written address above
    Wire.requestFrom(_i2cAddr, 1);
    std::uint8_t val = Wire.read();
    _currentAddress++;
    Wire.endTransmission();
    return val;
}

std::uint16_t MC24FC::readNextBytes(char* outbuf, std::uint16_t length)
{
    std::uint16_t read_bytes = 0;
    Wire.requestFrom(_i2cAddr, length);
    while (Wire.available())
    {
        outbuf[read_bytes] = static_cast<char>(Wire.read());
        read_bytes++;
        _currentAddress++;
    }

    return read_bytes;
}
