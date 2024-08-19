#include "DiskDriver.hpp"
#include "assembly_inline.hpp"
#include "io.hpp"

DiskDriver::DiskDriver() {

}

DiskDriver::~DiskDriver() {

}

bool DiskDriver::initialize() {
    // Initialize the Disk Driver
    return waitForReady();
}

bool DiskDriver::readSector(uint32_t lba, uint8_t* buffer) {
    if(!waitForReady())
        return false;

    // Send the Command to read a Sector
    outb(DRIVE_SELECT_REG, 0xE0 | ((lba >> 24) & 0x0F));
    outb(SECTOR_COUNT_REG, 1);                      // Sector Count
    outb(LBA_LOW_REG, (uint8_t) lba);               // LBA Low Byte
    outb(LBA_MID_REG, (uint8_t) (lba >> 8));        // LBA Mid Byte
    outb(LBA_HIGH_REG, (uint8_t) (lba >> 16));      // LBA High Byte
    sendCommand(0x20);                              // Read Command

    if(!waitForReady())
        return false;
    
    readData(buffer, 512);                          // Read 512 Bytes (1 Sector)
    
    return true;
}

bool DiskDriver::writeSector(uint32_t lba, const uint8_t* buffer) {
    if(!waitForReady())
        return false;

    // Send the Command to write a Sector
    outb(DRIVE_SELECT_REG, 0xE0 | ((lba >> 24) & 0x0F));
    
    // Dummy Reads to create delay --> https://wiki.osdev.org/ATA_PIO_Mode#400ns_delays TODO IMPLEMENT
    //for(int i = 0; i < 15; i++)
        //inb(STATUS_REG);

    outb(SECTOR_COUNT_REG, 1);                      // Sector Count
    outb(LBA_LOW_REG, (uint8_t) lba);               // LBA Low Byte
    outb(LBA_MID_REG, (uint8_t) (lba >> 8));        // LBA Mid Byte
    outb(LBA_HIGH_REG, (uint8_t) (lba >> 16));      // LBA High Byte
    sendCommand(0x30);                              // Write Command

    if(!waitForReady())
        return false;

    writeData(buffer, 512);                         // Write 512 Bytes (1 Sector)

    return true;
}

bool DiskDriver::waitForReady() {
    // Wait for the Disk to be Ready
    for(int i = 0; i < 10000; i++) {
        uint8_t status = inb(STATUS_REG);
        // Check BSY (BUSY) Bit 
        // TODO Check DRQ Bit as well (if DRQ (Data Request) bit is set), for now - once it poll it as well - it DOES NOT WORK ANYMORE
        // https://wiki.osdev.org/ATA_PIO_Mode
        if((!(status & 0x80)))
            return true;        
        if(status & 0x01) {
            uint8_t error = inb(ERROR_REG);
            io::my_cout << "DiskPolling Error: " << error << "\n";
            return false;
        }
    }
    return false;
}

void DiskDriver::sendCommand(uint8_t command) {
    outb(COMMAND_REG, command);
}

void DiskDriver::readData(uint8_t* buffer, size_t size) {
    for(size_t i = 0; i < size / 2; i++) {
        uint16_t data = inw(DATA_REG);
        buffer[2 * i] = data & 0xFF;
        buffer[2 * i + 1] = (data >> 8) & 0xFF;
    }
}

void DiskDriver::writeData(const uint8_t* buffer, size_t size) {
    for(size_t i = 0; i < size / 2; i++) {
        uint16_t data = buffer[2 * i] | (buffer[2 * i + 1] << 8);
        outw(DATA_REG, data);
    }
}