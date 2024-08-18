#ifndef DISKDRIVER_HPP
#define DISKDRIVER_HPP

#include <stdint.h>
#include <cstddef>

class DiskDriver {
    public:
        DiskDriver();
        ~DiskDriver();

        bool initialize();
        bool readSector(uint32_t lba, uint8_t* buffer);
        bool writeSector(uint32_t lba, const uint8_t* buffer);

    private:
        bool waitForReady();
        void sendCommand(uint8_t command);
        void readData(uint8_t* buffer, size_t size);
        void writeData(const uint8_t* buffer, size_t size);

        const uint16_t IO_BASE = 0x1F0;
};

#endif