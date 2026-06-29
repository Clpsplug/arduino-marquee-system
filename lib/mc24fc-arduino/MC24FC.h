#ifndef MC24FC_H
#define MC24FC_H

#include <cstdint>

class MC24FC
{
public:
    explicit MC24FC(std::uint8_t i2c_addr = 0x50, std::uint32_t max_capacity_bits = 256*1024);
    ~MC24FC();

    void init();

    /**
     * @brief Read a byte from an arbitrary address.
     * @param address
     * @return
     */
    bool readByte(std::uint16_t address, std::uint8_t* data);

    /**
     * Read a byte from the current EEPROM's address.
     * After the operation, the address is incremented.
     * @return
     */
    std::uint8_t readNext();

    /**
     * Read bytes from the current EEPROM's address.
     * After the operation, the address is incremented by the length.
     * @param outbuf It is the caller's responsibility to ensure that the buffer is large enough to hold the data
     * @param length
     * @return read bytes
     */
    std::uint16_t readNextBytes(char* outbuf, std::uint16_t length);

private:
    std::uint8_t _i2cAddr;
    std::uint32_t _maxCapacity;
    std::uint16_t _currentAddress;
};

#endif
