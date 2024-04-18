#ifndef DISK_DRIVER_HPP
#define DISK_DRIVER_HPP

#include <stdint.h> // minimal inttypes.h
#include "assembly_inline.hpp"
#include "io.hpp" 

class disk_driver {
    public:
        disk_driver();
        // Needs to be uint16_t, as ATA PIO Relies on Data Segements to be 2 Bytes 
        // TODO Check Status Registers for Success/Failure and handle accordingly
        void read(uint32_t lba, uint8_t sector_count, uint16_t* buffer);
        void write(uint32_t lba, uint8_t sector_count, uint16_t* buffer);
};

#endif