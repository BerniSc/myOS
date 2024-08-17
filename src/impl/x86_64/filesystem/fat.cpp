#include "fat.hpp"
#include "utils.hpp"

void printf(char*);
void printfHex(uint8_t);
void printfHex16(uint16_t);

//Converts the file name stored in a FAT directory entry into a more human-sensible format
void convertFromFATFormat(char* input, char* output) {
	unsigned short counter = 0;

	//iterate through the 8 letter file name, adding a dot when the end is reached
	for(counter = 0; counter < 8; counter++) {
		if(input[counter] == 0x20) {
			output[counter] = '.';
			break;
		}

		output[counter] = input[counter];
	}

	//if the entire 8 letters of the file name were used, tack a dot onto the end
	if(counter == 8) {
		output[counter] = '.';
	}

	unsigned short counter2 = 8;

	//iterate through the three-letter extension, adding it on. (Note: if the input is a directory (which has no extension) it erases the dot put in previously)
	for(counter2 = 8; counter2 < 11; counter2++) {
		++counter;
		if(input[counter2] == 0x20 || input[counter2] == 0x20) {
			if(counter2 == 8) //there is no extension, the dot added earlier must be removed
				counter -= 2; //it's minus two because the for loop above iterates the loop as well
			break;
		}
		output[counter] = input[counter2];		
	}

	++counter;
	while(counter < 12) {
		output[counter] = ' ';
		++counter;
	}

	output[12] = '\0'; //ensures proper termination regardless of program operation previously
	return;
}

void trimName(char *str, size_t size) {
    if(!size)
        return;

    for(size_t i = size-1; ; i--) {
        if(str[i] == ' ')
            str[i] = '\0';
        else if(str[i])
            break;

        if(i == 0)
            break;
    }
}

void printtime(uint16_t date, uint16_t time) {
    //Hour
    io::my_cout << ((time & 0xFE00) >> 11) << ":" << \
    //Minute
    ((time & 0x1E0) >> 5) << ":" << \
    //Sec
    ((time & 0x1F) << 1) << " ; ";

    //Day
    io::my_cout << (date & 0x1F) << ":" << \
    //Month
    ((date & 0x1E0) >> 5) << ":" << \
    //Year
    (((date & 0xFE00) >> 9) + 1980);
}


filesystem::FatFileSystem::FatFileSystem() {
    
}

// Helper function to write zeros to a sector
void clearSector(disk_driver* diskDriver, uint32_t sector) {
    io::my_cout << "clear start\n";
    uint16_t zeroBuffer[256] = {0}; // Each sector is 512 bytes
    for (int i = 0; i < 2; ++i) { // Write 512 bytes of zeros
        diskDriver->write(sector, 1, zeroBuffer);
        sector++;
    }
    io::my_cout << "clear stop\n";
}

void filesystem::FatFileSystem::formatDiskToFAT(disk_driver* diskDriver) {
    io::my_cout << "Started Formatting the FAT Disk" << io::OSTREAM_APPEND::endl;
    BiosParameterBlock32 bpb = {0};
    // Populate the BPB with appropriate values for your disk
    // This is a simplified example. Adjust values according to your disk's geometry and requirements.
    bpb.bytesPerSector = 512;
    bpb.sectorsPerCluster = 8;
    bpb.reservedSectors = 32;
    bpb.fatCopies = 2;
    bpb.totalSectors = 2880; // Example for a floppy disk
    bpb.fatSectorCount = 9; // Example value
    bpb.rootCluster = 2; // The first data cluster
    // ... Set other BPB fields as needed

    // Write the BPB to the first sector of the disk
    diskDriver->write(0, 1, (uint16_t*)&bpb);

    // Clear FAT tables. For simplicity, we're only clearing a small part.
    // In a real scenario, you need to clear all sectors used by the FAT tables.
    for (uint32_t sector = bpb.reservedSectors; sector < bpb.reservedSectors + bpb.fatSectorCount * bpb.fatCopies; ++sector) {
        clearSector(diskDriver, sector);
    }

    // Initialize the root directory cluster with zeros
    uint32_t rootDirStartSector = bpb.reservedSectors + (bpb.fatSectorCount * bpb.fatCopies);
    clearSector(diskDriver, rootDirStartSector);

    io::my_cout << "Finished Formatting the FAT Disk" << io::OSTREAM_APPEND::endl;
}

/*
void filesystem::FatFileSystem::Initialize(disk_driver* diskDriver, uint32_t partitionOffset) {
    this->diskDriver = diskDriver;

    if (this->diskDriver) {
        io::my_cout << "Driver null'd\n";
        return;
    }

    io::my_cout << "Initializing FAT File-System" << io::OSTREAM_APPEND::endl;
    io::my_cout << "TEST\n";

    BiosParameterBlock32 bpb;
    bpb.bytesPerSector = 0;
    bpb.sectorsPerCluster = 0;
    bpb.reservedSectors = 0;
    bpb.fatCopies = 0;
    bpb.totalSectors = 0;
    bpb.fatSectorCount = 0;
    bpb.rootCluster = 0;
    bpb.volumeId = 0;
    bpb.tableSize = 0;

    io::my_cout << "ASUDH\n";
    diskDriver->test();
    sleep(10000);
    diskDriver->read(partitionOffset, sizeof(BiosParameterBlock32), (uint16_t*) &bpb);
    
    sleep(1000);


    io::my_cout << "sectors per cluster: " << \
    bpb.sectorsPerCluster << io::OSTREAM_APPEND::endl;

    io::my_cout << "bytes per sector: " << \
    bpb.bytesPerSector << io::OSTREAM_APPEND::endl;

    io::my_cout << "cluster size: " << \
    bpb.sectorsPerCluster * bpb.bytesPerSector << io::OSTREAM_APPEND::endl;

    io::my_cout << "Number of fats: " << \
    bpb.fatCopies << io::OSTREAM_APPEND::endl;

    io::my_cout << "Label: " << \
    (char*)bpb.volumeLabel << io::OSTREAM_APPEND::endl;
    
    io::my_cout << "Volume ID: " << \
    bpb.volumeId << io::OSTREAM_APPEND::endl;
    
    fatStart = partitionOffset + bpb.reservedSectors;
    fatSize = bpb.tableSize;
    io::my_cout << ("Size of 1 fat: ") << \ 
    (bpb.tableSize) << io::OSTREAM_APPEND::endl;

    dataStart = fatStart + fatSize*bpb.fatCopies;

    rootStart = dataStart + bpb.sectorsPerCluster*(bpb.rootCluster - 2);
    sectorsPerCluster = bpb.sectorsPerCluster;
} */

void filesystem::FatFileSystem::Initialize(disk_driver& diskDriver, uint32_t partitionOffset) {


    io::my_cout << "Initializing FAT File-System" << io::OSTREAM_APPEND::endl;
    io::my_cout << "TEST\n";

    BiosParameterBlock32 bpb;
    bpb.bytesPerSector = 0;
    bpb.sectorsPerCluster = 0;
    bpb.reservedSectors = 0;
    bpb.fatCopies = 0;
    bpb.totalSectors = 0;
    bpb.fatSectorCount = 0;
    bpb.rootCluster = 0;
    bpb.volumeId = 0;
    bpb.tableSize = 0;

    io::my_cout << "ASUDH\n";
    diskDriver.test();
    sleep(10000);
    diskDriver.read(partitionOffset, sizeof(BiosParameterBlock32), (uint16_t*) &bpb);
    
    sleep(1000);


    io::my_cout << "sectors per cluster: " << \
    bpb.sectorsPerCluster << io::OSTREAM_APPEND::endl;

    io::my_cout << "bytes per sector: " << \
    bpb.bytesPerSector << io::OSTREAM_APPEND::endl;

    io::my_cout << "cluster size: " << \
    bpb.sectorsPerCluster * bpb.bytesPerSector << io::OSTREAM_APPEND::endl;

    io::my_cout << "Number of fats: " << \
    bpb.fatCopies << io::OSTREAM_APPEND::endl;

    io::my_cout << "Label: " << \
    (char*)bpb.volumeLabel << io::OSTREAM_APPEND::endl;
    
    io::my_cout << "Volume ID: " << \
    bpb.volumeId << io::OSTREAM_APPEND::endl;
    
    fatStart = partitionOffset + bpb.reservedSectors;
    fatSize = bpb.tableSize;
    io::my_cout << ("Size of 1 fat: ") << \ 
    (bpb.tableSize) << io::OSTREAM_APPEND::endl;

    dataStart = fatStart + fatSize*bpb.fatCopies;

    rootStart = dataStart + bpb.sectorsPerCluster*(bpb.rootCluster - 2);
    sectorsPerCluster = bpb.sectorsPerCluster;
}

void filesystem::FatFileSystem::ListRootDir() {
    io::my_cout << "#-------------------#" << io::OSTREAM_APPEND::endl;
    DirectoryEntryFat32 dirent[16];
    diskDriver->read(rootStart, 16*sizeof(DirectoryEntryFat32), (uint16_t*) &dirent[0]);
    
    for(int i = 0; i < 16; i++) {
        if(dirent[i].name[0] == 0x00)
            break;
        
        if((dirent[i].attributes & 0x0F) == 0x0F)
            continue;

        char* foo = "          ";
        convertFromFATFormat((char*)dirent[i].name, foo);
        
        io::my_cout << \
        foo << \
        "     Size: " << \
        dirent[i].size << " ";
        printtime(dirent[i].cDate, dirent[i].cTime);
        io::my_cout << io::OSTREAM_APPEND::endl;
        
        if((dirent[i].attributes & 0x10) == 0x10) // directory
            continue;
    }
    io::my_cout << "#-------------------#" << io::OSTREAM_APPEND::endl;
}


int strcmp2(const char *s1, const char *s2)
{
    while ((*s1 == *s2) && *s1) { ++s1; ++s2; }
    return (((int) (unsigned char) *s1) - ((int) (unsigned char) *s2) == 0) ? 1 : 0;
}

//Only works in root dir right now
void filesystem::FatFileSystem::GetFile(char* name, uint8_t* buffer) {
    DirectoryEntryFat32 dirent[16];
    diskDriver->read(rootStart, 16*sizeof(DirectoryEntryFat32), (uint16_t*) &dirent[0]);
    
    for(int i = 0; i < 16; i++) {
        if(dirent[i].name[0] == 0x00)
            break;
        
        if((dirent[i].attributes & 0x0F) == 0x0F)
            continue;

        char* realname;
        convertFromFATFormat((char*)dirent[i].name, realname);

        trimName(realname, 11);

        if(strcmp2(realname, name) == 1) {
            uint32_t firstFileCluster = ((uint32_t)dirent[i].firstClusterHi) << 16 | ((uint32_t)dirent[i].firstClusterLow);               
                                
            int32_t SIZE = dirent[i].size;
            int32_t nextFileCluster = firstFileCluster;
            uint8_t fatbuffer[513];
            uint8_t tempbuffer[513];
            int32_t bytesRead = 0;
                                  
            while(SIZE > 0) {
                uint32_t fileSector = dataStart + sectorsPerCluster * (nextFileCluster-2);
                int sectorOffset = 0;

                io::my_cout << ("Reading Sector: ") << \
                fileSector << \
                " Cluster: " << \
                (nextFileCluster - 2) << io::OSTREAM_APPEND::endl; 

                for(; SIZE > 0; SIZE -= 512) {
                    diskDriver->read(fileSector+sectorOffset, 512, (uint16_t*) tempbuffer);
                    
                    tempbuffer[SIZE > 512 ? 512 : SIZE] = '\0';

                    for(int i = 0; i < 512; i++) {
                        buffer[bytesRead + i] = tempbuffer[i];
                    }

                    io::my_cout << (char*)tempbuffer;

                    bytesRead += (SIZE > 512 ? 512 : SIZE);
                    
                    if(++sectorOffset > sectorsPerCluster)
                        break;
                }                
                uint32_t fatSectorForCurrentCluster = nextFileCluster / (512/sizeof(uint32_t));
                diskDriver->read(fatStart + fatSectorForCurrentCluster, 512, (uint16_t*) fatbuffer);
                uint32_t fatOffsetInSectorForCurrentCluster = nextFileCluster % (512/sizeof(uint32_t));
                nextFileCluster = ((uint32_t*)&fatbuffer)[fatOffsetInSectorForCurrentCluster] & 0x0FFFFFFF;
            }
            break;
        }
    }
}