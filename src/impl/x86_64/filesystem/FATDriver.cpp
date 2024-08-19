#include "FATDriver.hpp"

#include "utils.hpp"
#include "memory_manager.hpp"
#include "tokenizer.hpp"

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
    uint16_t rootEntryCount = *(uint16_t*) &sector[17];
    uint16_t totalSectors16 = *(uint16_t*) &sector[19];
    uint16_t sectorsPerFAT16 = *(uint16_t*) &sector[22];

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
                if(string_comp(entryName, path) == true) {
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
    io::my_cout << "File not Found\n";
    return false;           // File not Found
}

bool FATDriver::createEntryCommon(const char* name, uint16_t firstCluster, uint32_t size, uint8_t attributes, uint16_t directoryCluster) {
    uint8_t sector[512];
    uint32_t directorySector;
    if(directoryCluster != 0xFFFF)
        directorySector = clusterToLBA(directoryCluster);
    else
        directorySector = rootDirStart;
    // directorySector = rootDirStart;
    bool found = false;
    int freeEntryOffset = -1;

    // Search for a free Dir-Entry in the Root Dir
    for(uint32_t i = 0; i < sectorsPerCluster; i++) {
        if(!diskDriver.readSector(directorySector + i, sector))
            return false;       // Failed to read Sector

        for(int j = 0; j < 512; j += 32) {
            if(sector[j] == 0 || sector[j] == 0xE5) {
                // Found a free Entry
                freeEntryOffset = j;
                found = true;
                break;
            }
        }

        if(found)
            break;
    }
    
    if(!found)
        return false;       // No free Directory-Entry found

    // Create a dir-entry
    kmemset(&sector[freeEntryOffset], 0, 32);
    kmemcpy(name, &sector[freeEntryOffset], 11);
    sector[freeEntryOffset + 11] = attributes;        // File Attribute: Archive or Directory
    *(uint16_t*) &sector[freeEntryOffset + 26] = firstCluster;
    *(uint32_t*) &sector[freeEntryOffset + 28] = size;

    // Write updated dir-sector back to the disk
    if(!diskDriver.writeSector(directorySector + (freeEntryOffset / 512), sector))
        return false;       // Failed to write Sector
    
    return true;
}

bool FATDriver::createFile(const char* path, const uint8_t* data, uint32_t size, uint16_t directoryCluster) {
    // Find free clusters and write Data to them
    uint32_t remainingSize = size;
    uint16_t firstCluster = findFreeCluster();
    uint16_t currentCluster = firstCluster;

    if(firstCluster == 0xFFFF)
        return false;   // No Free Clusters available

    while(remainingSize > 0) {
        uint8_t dataSector[512] = {0};
        size_t bytesToCopy = (remainingSize > 512) ? 512 : remainingSize;
        kmemcpy(data, dataSector, bytesToCopy);

        uint32_t lba = clusterToLBA(currentCluster);
        if(!diskDriver.writeSector(lba, dataSector))
            return false;       // Failed to write Sector

        remainingSize -= bytesToCopy;
        data += bytesToCopy;

        if(remainingSize > 0) {
            uint16_t nextCluster = findFreeCluster();
            if(nextCluster == 0xFFFF)
                return false;   // No free Clusters available

            setNextCluster(currentCluster, nextCluster);
            currentCluster = nextCluster;
        }
    }

    setNextCluster(currentCluster, 0xFFFF);     // Mark end of cluster Chain

    return createEntryCommon(path, firstCluster, size, 0x20, directoryCluster);   // 0x20: Archive Attribute
}

bool FATDriver::createDirectory(const char* name, uint16_t parentCluster)  {
    // Allocate a Cluster for the Directory
    uint16_t firstCluster = findFreeCluster();
    if(firstCluster == 0xFFFF)
        return false;           // No free Clusters available

    // Init the directory Cluster
    uint8_t dirSector[512] = {0};
    uint32_t lba = clusterToLBA(firstCluster);
    if(!diskDriver.writeSector(lba, dirSector))
        return false;           // Failed to initialize directory Sector
    
    // Create the directory Entry in the specified parent directory
    return createEntryCommon(name, firstCluster, 0, 0x10, parentCluster);  // Size: 0; 0x10 = Directory Attribute
}

uint32_t FATDriver::clusterToLBA(uint32_t cluster) {
    return dataStart + (cluster - 2) * sectorsPerCluster;
}

bool FATDriver::formatDisk() {
    uint8_t sector[512] = {0};

    // Populate the boot sector with BPB
    sector[11] = 0x00;                  // Bytes per Sector
    sector[12] = 0x02;
    sector[13] = 0x01;                  // Sectors per Cluster
    *(uint16_t*) &sector[14] = 1;       // Reserved Sectors
    sector[16] = 2;                     // Number if FATs
    *(uint16_t*) &sector[17] = 512;     // Root Entry Count
    *(uint16_t*) &sector[19] = 2880;    // Total sectors for a 1.44MB floppy
    sector[21] = 0xF8;                  // Media Descriptor
    *(uint16_t*) &sector[22] = 9;       // Sectors per FAT
    *(uint16_t*) &sector[24] = 18;      // Sectors per Track
    *(uint16_t*) &sector[26] = 2;       // Number of Heads

    // Write OEM Name and File-System-Type
    const char* oemName = "MKDOSFS ";
    const char* fsType =  "FAT16   ";
    kmemcpy(oemName, &sector[3], 8);
    kmemcpy(fsType, &sector[54], 8);

    if(!diskDriver.writeSector(0, sector))
        return false;       // Failed to write Boot Sector
    
    // Initialize the FAT tables
    for(uint32_t i = 0; i < 2 * 9; ++i) {
        kmemset(sector, 0, 512);
        if(i == 0) {
            sector[0] = 0xF8;       // Media Descriptor
            sector[1] = 0xFF;
            sector[2] = 0xFF;
        }
        if(!diskDriver.writeSector(1 + i, sector))
            return false;   // Failed to initialize the FAT Tables
    }

    // Initialize the root Directory
    for(uint32_t i = 0; i < 14; ++i) {
        kmemset(sector, 0, 512);
        if(!diskDriver.writeSector(19 + i, sector))
            return false;   // Failed to initialize the rootDirectory

    }

    // Refresh internal state after Formatting
    return initialize();
}

uint16_t FATDriver::getNextCluster(uint16_t cluster) {
    uint8_t sector[512];
    uint32_t fatSector = fatStart + (cluster * 2 / 512);
    uint32_t fatOffset = (cluster * 2) % 512;

    if(!diskDriver.readSector(fatSector, sector))
        return 0xFFFF;      // Error in reading Sector

    return *(uint16_t*) &sector[fatOffset];
}

bool FATDriver::setNextCluster(uint16_t cluster, uint16_t value) {
    uint8_t sector[512];
    uint32_t fatSector = fatStart + (cluster * 2 / 512);
    uint32_t fatOffset = (cluster * 2) % 512;

    if(!diskDriver.readSector(fatSector, sector))
        return false;      // Error in reading Sector

    *(uint16_t*) &sector[fatOffset] = value;
    return diskDriver.writeSector(fatSector, sector);
}

uint16_t FATDriver::findFreeCluster() {
    uint8_t sector[512];

    const uint32_t TOTAL_SECTORS = 2880;
    const uint32_t BYTES_PER_SECTOR = 512;

    for(uint32_t i = 0; i < ((TOTAL_SECTORS * BYTES_PER_SECTOR) / 512); ++i) {
io::my_cout << "_1\n";
        if(!diskDriver.readSector(fatStart + i, sector))
            return 0xFFFF;  // Failed to read Sector
io::my_cout << "_2\n";
        
        for(uint32_t j = 0; j < 512; j += 2) {
            uint16_t cluster = *(uint16_t*) &sector[j];

            if(cluster == 0) {
                // Found free Cluster
                return (i * 256) + (j / 2);
            }
        }
    }
    io::my_cout << "NO FREE CLUSTERS\n";

    return 0xFFFF;  // No free Clusters found
}

// Split the Path and Traverse it to find a specific Cluster
uint16_t FATDriver::findDirectoryCluster(const char* path) {
    // TODO Fix
    // For now assue path is split unix-esque using "DIR1/DIR2"
    uint16_t currentCluster = rootDirStart;     // Start from the Root Directory. Thats not REALLY efficient. TODO Refactor later
    uint8_t sector[512];

    if(string_comp(path, "/"))
        return static_cast<uint16_t>(rootDirStart);

    Tokenizer tokenizer(path, '/');
    const char* token;

    while((token = tokenizer.nextToken()) != nullptr) {
        bool found = false;

        for(int i = 0; i < sectorsPerCluster; i++) {
            if(!diskDriver.readSector(clusterToLBA(currentCluster) + i, sector)) {
                io::my_cout(io::COLOUR_LIGHT_BLUE, io::COLOUR_LIGHT_GRAY) << "Failed to read Sector in findDirCluster\n";
                delete[] token;
                return 0xFFFF;      // Failed to read Sector
            }

            for(int j = 0; j < 512; j+= 32) {
                if(sector[j] != 0 && sector[j] != 0xE5 && (sector[j + 11] & 0x10)) {
                    // Compare the directoryName
                    if(string_comp((char*) &sector[j], token)) {
                        currentCluster = *(uint16_t*) &sector[j + 26];
                        found = true;
                        break;
                    }
                }
            }
            if(found)
                break;
        }
        delete[] token;

        if(!found)
            return 0xFFFF;  // Directory not found
    }   
    return currentCluster;
}