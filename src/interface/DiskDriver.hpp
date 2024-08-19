#ifndef DISKDRIVER_HPP
#define DISKDRIVER_HPP

#include <stdint.h>
#include <cstddef>

#define IO_BASE           0x1F0
#define STATUS_REG        (IO_BASE + 0x07)
#define COMMAND_REG       STATUS_REG
#define DATA_REG          IO_BASE
#define ERROR_REG         (IO_BASE + 0x01)
#define SECTOR_COUNT_REG  (IO_BASE + 0x02)
#define LBA_LOW_REG       (IO_BASE + 0x03)
#define LBA_MID_REG       (IO_BASE + 0x04)
#define LBA_HIGH_REG      (IO_BASE + 0x05)
#define DRIVE_SELECT_REG  (IO_BASE + 0x06)

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

};

#endif