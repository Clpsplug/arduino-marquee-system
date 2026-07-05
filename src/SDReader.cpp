#include "SDReader.h"

using namespace ECP::ArduinoMarquee;

SDReader::SDReader(int chipSelectPin) :
    _chipSelectPin(chipSelectPin)
    , _error(SDReaderError::NotInitialized)
{}

SDReader::~SDReader()
= default;

bool SDReader::init()
{
    if (!SD.begin(_chipSelectPin))
    {
        _error = SDReaderError::NoCard;
        return false;
    }
    _error = SDReaderError::FileNotOpen;
    return true;
}

void SDReader::open(const char* file_name)
{
    _file = SD.open(file_name);
    if (!_file)
    {
        _error = SDReaderError::FileMissing;
    }
    _error = SDReaderError::OK;
}

int SDReader::read(char* buf, size_t buf_size)
{
    return _file.read(buf, buf_size);
}

bool SDReader::isEOF()
{
    return !_file.available();
}

void SDReader::close()
{
    _file.close();
    _error = SDReaderError::FileNotOpen;
}

SDReaderError SDReader::getError() const
{
    return _error;
}
