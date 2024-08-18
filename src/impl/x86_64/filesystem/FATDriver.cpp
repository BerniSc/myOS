#include "FATDriver.hpp"
#include "utils.hpp"
#include "memory_manager.hpp"

FATDriver::FATDriver(DiskDriver& diskDriver) : diskDriver(diskDriver) {

}

FATDriver::~FATDriver() {

}

bool FATDriver::initialize() {
    return parseBPB();
}

bool FATDriver::parseBPB() {
    uint8_t sector[512];

    if(!diskDriver.readSector(0, sector))
        return false;

    // Parse BIOS Parameter Block (BPB)
    sectorsPerCluster = sector[13];
    uint16_t reservedSectors = *(uint16_t*) &sector[14];
    uint8_t numFATs = sector[16];
    uint16_t rootEntryCount = *(uint16_t*) & sector[17];
    uint16_t totalSectors16 = *(uint16_t*) & sector[19];
    uint16_t sectorsPerFAT16 = *(uint16_t*) & sector[22];

    if(totalSectors16 == 0) {
        uint32_t totalSectors32 = *(uint32_t*) &sector[32];
        // TODO Handle totalSectors32 if needed
    }

    fatStart = reservedSectors;
    rootDirStart = fatStart + numFATs * sectorsPerFAT16;
    rootDirSectors = (rootEntryCount * 32 + 511) / 512;
    dataStart = rootDirStart + rootDirSectors;

    return true;
}

Vector<char*> FATDriver::listDirectory(const char* path) {
    Vector<char*> entries;
    uint8_t sector[512];

    // TODO For Simplicity now only list Root Dir
    for(uint32_t i = 0; i < rootDirSectors; ++i) {
        if(!diskDriver.readSector(rootDirStart + i, sector))
            continue;
        
        for(int j = 0; j < 512; j += 32) {
            if(sector[j] == 0)
                return entries;     // No More entries
            
            if(sector[j] != 0xE5 && (sector[j + 11] & 0x08) == 0) {     // Check if its a valid entry
                char* entryName = new char[12];
                kmemcpy(&sector[j], entryName, 11);
                entryName[11] = '\0';
                entries.pushBack(entryName);
            }
        }
    }

    return entries;
}

bool FATDriver::readFile(const char* path, Vector<uint8_t>& buffer) {
    uint8_t sector[512];

    // Search the File in the Root Dir
    for(uint32_t i = 0; i < rootDirSectors; ++i) {
        if(!diskDriver.readSector(rootDirStart + i, sector))
            continue;

        for(int j = 0; j < 512; j += 32) {
            if(sector[j] == 0)
                return false;           // No more entries
            
            if(sector[j] != 0xE5 && (sector[j + 11] & 0x08) == 0) {     // Check if it's a valid entry
                char entryName[12];
                kmemcpy(&sector[j], entryName, 11);
                entryName[11] = '\0';

                // Compare the Entry Name with the requested Field Name
                if(string_comp(entryName, path) == false) {
                    // File Found, Read its Contents
                    uint16_t firstCluster = *(uint16_t*) &sector[j + 26];
                    uint32_t fileSize = *(uint32_t*) &sector[j + 28];

                    uint32_t currentCluster = firstCluster;
                    uint32_t remainingSize = fileSize;

                    while(remainingSize > 0) {
                        uint8_t dataSector[512];
                        uint32_t lba = clusterToLBA(currentCluster);

                        if(!diskDriver.readSector(lba, dataSector))
                            return false;           // Failed to read the Sector
                        
                        size_t bytesToCopy = (remainingSize > 512) ? 512 : remainingSize;
                        for(size_t k = 0; k < bytesToCopy; ++k)
                            buffer.pushBack(dataSector[k]);

                        remainingSize -= bytesToCopy;

                        // Move to the next Cluster if neccesary
                        if(remainingSize > 0) {
                            // Read the next Cluster from the FAT
                            uint32_t fatSector = fatStart + (currentCluster * 2 / 512);
                            uint32_t fatOffset = (currentCluster * 2) % 512;

                            if(!diskDriver.readSector(fatSector, sector))
                                return false;                   // Failed to read FAT Sector

                            currentCluster = *(uint16_t*) &sector[fatOffset];
                            if(currentCluster >= 0xFFF8)
                                return false;                   // End of Cluster Chain
                        }
                    }
                    return true;
                }
            }
        }
    }
    return false;           // File not Found
}

uint32_t FATDriver::clusterToLBA(uint32_t cluster) {
    return dataStart + (cluster - 2) * sectorsPerCluster;
}

bool FATDriver::formatDisk() {
    uint8_t sector[512] = {0};
    
}