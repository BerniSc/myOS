#include "disk_driver.hpp"

#define ATA_PRIMARY_IO 0x1F0
#define ATA_CMD_READ_PIO 0x20
#define ATA_CMD_WRITE_PIO 0x30
#define ATA_SECTOR_SIZE 512

disk_driver::disk_driver() {
    // Initialization if needed
}

void disk_driver::read(uint32_t lba, uint8_t sector_count, uint16_t* buffer) {
    // Select drive (master) and LBA mode
    outb(ATA_PRIMARY_IO + 6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_IO + 2, sector_count);
    outb(ATA_PRIMARY_IO + 3, (uint8_t)lba);
    outb(ATA_PRIMARY_IO + 4, (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_IO + 5, (uint8_t)(lba >> 16));
    outb(ATA_PRIMARY_IO + 7, ATA_CMD_READ_PIO);

    for(uint8_t sector = 0; sector < sector_count; ++sector) {
        // Polling
        while(!(inb(ATA_PRIMARY_IO + 7) & 0x08));

        // Read sector
        for(int i = 0; i < ATA_SECTOR_SIZE / 2; ++i) {
            buffer[sector * (ATA_SECTOR_SIZE / 2) + i] = inw((uint16_t) ATA_PRIMARY_IO);
        }
    }
}

void disk_driver::write(uint32_t lba, uint8_t sector_count, uint16_t* buffer) {
    // Select drive (master) and LBA mode
    outb(ATA_PRIMARY_IO + 6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_IO + 2, sector_count);
    outb(ATA_PRIMARY_IO + 3, (uint8_t)lba);
    outb(ATA_PRIMARY_IO + 4, (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_IO + 5, (uint8_t)(lba >> 16));
    outb(ATA_PRIMARY_IO + 7, ATA_CMD_WRITE_PIO);

    for(uint8_t sector = 0; sector < sector_count; ++sector) {
        // Polling
        while(!(inb(ATA_PRIMARY_IO + 7) & 0x08));

        // Write sector
        for(int i = 0; i < ATA_SECTOR_SIZE / 2; ++i) {
            outw(ATA_PRIMARY_IO, buffer[sector * (ATA_SECTOR_SIZE / 2) + i]);
        }
    }
}