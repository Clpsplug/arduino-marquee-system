#ifndef MC24FC_H
#define MC24FC_H

#include <cstdint>

enum class MC24FCError: std::uint32_t
{
    /**
     * @brief No error.
     */
    OK,
    /**
     * @brief I2C buffer overflow, did you write too much data?
     */
    I2C_BUFFER_OVERFLOW,
    /**
     * @brief I2C comm was attempted, but nobody answered (NACK on addressing).
     */
    WRONG_I2C_ADDRESS,
    /**
     * @brief The agent (slave) responded with NACK (NACK on data.)
     */
    I2C_AGENT_DOESNT_ACK,
    /**
     * @brief Unknown I2C error. (Wire lib error 4).
     */
    UNKNOWN_I2C_ERROR,
    /**
     * @brief I2C operation itself failed - this probably is a hardware fault.
     */
    I2C_FAIL,
    /**
     * @brief Wire.endTransmission() returned a legitimately unknown error.
     */
    I2C_UNIMPLEMENTED_ERROR,
    /**
     * @brief Tried to read an out-of-range address. This isn't an I2C error.
     */
    ADDRESS_OUT_OF_RANGE,
    MAX,
};

class MC24FC
{
public:
    explicit MC24FC(std::uint8_t i2c_addr = 0x50, std::uint32_t max_capacity_bits = 256 * 1024, std::uint32_t page_size = 32);

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

    /**
     * Write bytes at the requested address.
     *
     * @param offset Offset at which write starts.
     * @param buf data buffer.
     * @param length buffer length.
     */
    void writeAt(std::uint16_t offset, const char* buf, std::uint16_t length);

    [[nodiscard]] MC24FCError getError() const;

private:
    std::uint16_t writeIntoPage(std::uint16_t offset, const char* buf, std::uint16_t length, std::uint16_t i2c_limit) const;

    void setError(int wire_return_code);

    std::uint8_t _i2cAddr;
    std::uint32_t _maxCapacity;
    std::uint32_t _pageSize;
    std::uint16_t _currentAddress;
    MC24FCError _error;
};

#endif
