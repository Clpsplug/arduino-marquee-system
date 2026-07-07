#include "MC24FC.h"
#include <Wire.h>

// FIXME: Could Arduino Uno R4 hold more than this?
constexpr uint16_t MAX_I2C_BUFFER_SIZE = 32;

MC24FC::MC24FC(std::uint8_t i2c_addr, std::uint32_t max_capacity_bits, std::uint32_t page_size) :
    _i2cAddr(i2c_addr & 0x7F) // Address is 7-bit
    , _maxCapacity(max_capacity_bits)
    , _pageSize(page_size)
    , _currentAddress(0xFFFF) // Keep the address out of bounds.
    , _error(MC24FCError::OK) {
}

MC24FC::~MC24FC() {
    Wire.end();
}

void MC24FC::init() {
    Wire.begin();
    // Quick ACK check
    Wire.beginTransmission(_i2cAddr); // control byte, write op
    const auto ret = Wire.endTransmission(); // immediately end to check ACK
    setError(ret);
}

bool MC24FC::readByte(std::uint16_t address, std::uint8_t *data) {
    _currentAddress = address;
    if (_currentAddress >= _maxCapacity / 8) {
        _error = MC24FCError::ADDRESS_OUT_OF_RANGE;
        return false;
    }
    // We must convert the address to big endian
    char address_bytes[2];
    address_bytes[0] = static_cast<char>(address >> 8);
    address_bytes[1] = static_cast<char>(address & 0xFF);

    // Address the EEPROM as "Write" to write the random access address
    Wire.beginTransmission(_i2cAddr);
    Wire.write(address_bytes[0]);
    Wire.write(address_bytes[1]);
    const auto ret = Wire.endTransmission(false); // Keep the line hot
    setError(ret);
    if (ret != 0) {
        return false;
    }
    // Address the EEPROM as "Read" to read from the written address above
    // NOTE: "Read" bit is handled internally.
    if (
        const auto size = Wire.requestFrom(_i2cAddr, 1); size != 1
        || !Wire.available()) {
        Serial.println("ERROR: Bad size");
        return false;
    }
    if (data != nullptr) {
        *data = Wire.read();
    }
    _currentAddress++;
    return true;
}

std::uint8_t MC24FC::readNext() {
    // Address the EEPROM as "Read" to read from the written address above
    Wire.requestFrom(_i2cAddr, 1);
    const std::uint8_t val = Wire.read();
    _currentAddress++;
    return val;
}

std::uint16_t MC24FC::readNextBytes(char *outbuf, std::uint16_t length) {
    if (outbuf == nullptr) {
        _error = MC24FCError::INVALID_PARAMETER;
        return 0;
    }

    std::uint16_t read_bytes = 0;
    Wire.requestFrom(_i2cAddr, length);
    while (Wire.available()) {
        outbuf[read_bytes] = static_cast<char>(Wire.read());
        read_bytes++;
        if (
            _currentAddress >= _maxCapacity / 8 // Probably impossible, but just in case.
            || _currentAddress == UINT16_MAX
        ) {
            _error = MC24FCError::ADDRESS_OUT_OF_RANGE;
            break;
        }
        _currentAddress++;
    }

    return read_bytes;
}


void MC24FC::writeAt(std::uint16_t offset, const char *buf, std::uint16_t length) {
    auto current_written_bytes = 0;
    auto remaining_length = length;
    auto buf_offset = 0;

    if (buf == nullptr) {
        _error = MC24FCError::INVALID_PARAMETER;
        return;
    }

    if (_pageSize < length) {
        _error = MC24FCError::WRITE_DATA_TOO_LARGE;
        return;
    }

    while (buf_offset < length) {
        // We must convert the address to big endian
        char address_bytes[2];
        address_bytes[0] = static_cast<char>(offset >> 8);
        address_bytes[1] = static_cast<char>(offset & 0xFF);

        // Address the EEPROM as "Write" to write the random access address
        Wire.beginTransmission(_i2cAddr);
        Wire.write(address_bytes, 2); // Make sure we use the 'non-string' overload. Don't omit the second arg.
        current_written_bytes = sizeof(_i2cAddr) + sizeof(address_bytes);

        const auto allowed_max_size = MAX_I2C_BUFFER_SIZE - current_written_bytes;
        const auto written_size = this->writeIntoPage(
            offset + buf_offset,
            buf + buf_offset,
            remaining_length,
            allowed_max_size
            );

        buf_offset += written_size;
        remaining_length -= written_size;
        {
            auto ret = Wire.endTransmission(); // Actually end the transmission so that we don't exceed 32 bytes
            setError(ret);
            // Non-ACK result at this point is an error.
            if (ret != 0) {
                break;
            }
        }

        // The EEPROM will stop ACK-ing for write command at this point
        // because it's busy writing the data. We need to poll the ACK.
        // To do so, we send a control byte of WRITE until we get an ACK.
        {
            int ret;
            while (true) {
                Wire.beginTransmission(_i2cAddr); // control byte, write op
                ret = Wire.endTransmission(); // immediately end to check ACK
                if (ret == 0) {
                    break;
                }
                if (ret == 4 || ret == 5) {
                    // Error 4 and 5 must NOT continue and report the error.
                    break;
                }
                delayMicroseconds(100); // don't spam.
            }
            setError(ret);
        }
    }
    _currentAddress += offset - remaining_length;
}

std::uint16_t MC24FC::writeIntoPage(std::uint16_t offset, const char *buf, std::uint16_t length,
    std::uint16_t i2c_limit) const {
    const std::uint16_t page_remaining = _pageSize - offset % _pageSize;
    const auto writable_size = std::min(length, std::min(i2c_limit, page_remaining));
    Wire.write(buf, writable_size);
    return writable_size;
}

void MC24FC::setError(const int wire_return_code) {
    switch (wire_return_code) {
        case 0:
            _error = MC24FCError::OK;
            break;
        case 1:
            _error = MC24FCError::I2C_BUFFER_OVERFLOW;
            break;
        case 2:
            _error = MC24FCError::WRONG_I2C_ADDRESS;
            break;
        case 3:
            _error = MC24FCError::I2C_AGENT_DOESNT_ACK;
            break;
        case 4:
            _error = MC24FCError::UNKNOWN_I2C_ERROR;
            break;
        case 5:
            _error = MC24FCError::I2C_FAIL;
            break;
        default:
            _error = MC24FCError::I2C_UNIMPLEMENTED_ERROR;
            break;
    }
}

MC24FCError MC24FC::getError() const {
    return _error;
}
