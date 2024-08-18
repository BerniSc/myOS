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
    outb(IO_BASE + 0x07, 0xE0 | ((lba >> 24) & 0x0F));
    outb(IO_BASE + 0x02, 1);                        // Sector Count
    outb(IO_BASE + 0x03, (uint8_t) lba);            // LBA Low Byte
    outb(IO_BASE + 0x04, (uint8_t) (lba >> 8));     // LBA Mid Byte
    outb(IO_BASE + 0x05, (uint8_t) (lba >> 16));    // LBA High Byte
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
    outb(IO_BASE + 0x07, 0xE0 | ((lba >> 24) & 0x0F));
    outb(IO_BASE + 0x02, 1);                        // Sector Count
    outb(IO_BASE + 0x03, (uint8_t) lba);            // LBA Low Byte
    outb(IO_BASE + 0x04, (uint8_t) (lba >> 8));     // LBA Mid Byte
    outb(IO_BASE + 0x05, (uint8_t) (lba >> 16));    // LBA High Byte
    sendCommand(0x30);                              // Write Command

    if(!waitForReady())
        return false;

    writeData(buffer, 512);                         // Write 512 Bytes (1 Sector)

    return true;
}

bool DiskDriver::waitForReady() {
    // Wait for the Disk to be Ready
    for(int i = 0; i < 10000; i++) {
        uint8_t status = inb(IO_BASE + 0x07);
        if(!(status & 0x80))
            return true;        // Check BSY Bit
    }
    return false;
}

void DiskDriver::sendCommand(uint8_t command) {
    outb(IO_BASE + 0x07, command);
}

void DiskDriver::readData(uint8_t* buffer, size_t size) {
    for(size_t i = 0; i < size / 2; i++) {
        uint16_t data = inw(IO_BASE);
        buffer[2 * i] = data & 0xFF;
        buffer[2 * i + 1] = (data >> 8) & 0xFF;
    }
}

void DiskDriver::writeData(const uint8_t* buffer, size_t size) {
    for(size_t i = 0; i < size / 2; i++) {
        uint16_t data = buffer[2 * i] | (buffer[2 * i + 1] << 8);
        outw(IO_BASE, data);
    }
}