#ifndef FATDRIVER_HPP
#define FATDRIVER_HPP

#include "DiskDriver.hpp"
#include "vector.hpp"

#include <cstdint>

class FATDriver {
    public:
        FATDriver(DiskDriver& disk);
        ~FATDriver();

        bool initialize();
        bool formatDisk();
        Vector<char*> listDirectory(const char* path);
        bool readFile(const char* path, Vector<uint8_t>& buffer);

        bool createFile(const char* path, const uint8_t* data, uint32_t size);
        bool createDirectory(const char* name, uint16_t parentCluster);
    
    private:
        bool readSector(uint32_t lba, uint8_t* buffer);
        bool writeSector(uint32_t lba, const uint8_t* buffer);
        uint32_t clusterToLBA(uint32_t cluster);
        bool parseBPB();

        uint16_t getNextCluster(uint16_t cluster);
        bool setNextCluster(uint16_t cluster, uint16_t value);
        uint16_t findFreeCluster();

        DiskDriver& diskDriver;
        uint32_t fatStart;
        uint32_t dataStart;
        uint32_t rootDirStart;
        uint32_t sectorsPerCluster;
        uint32_t rootDirSectors;
};

#endif



