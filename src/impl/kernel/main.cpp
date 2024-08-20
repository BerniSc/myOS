#include "io.hpp"
#include "print.hpp"
#include "cursor.hpp"
#include "utils.hpp"
#include "keyboard.hpp"
#include "timer.hpp"

#include "common_config.hpp"

#include "isrs.hpp"

#include "interrupts.hpp"

#include "memory_manager.hpp"
#include "ps_pointer.hpp"

#include "../interface/kernel_tests.hpp"
#include "disk_driver.hpp"
#include "fat.hpp"

#include "DiskDriver.hpp"
#include "FATDriver.hpp"
#include "vector.hpp"


//https://stackoverflow.com/questions/329059/what-is-gxx-personality-v0-for
void* __gxx_personality_v0;
void* _Unwind_Resume;

/*************************************************************************************
 * 
 *         Definition of the Global Variables that are Used by the Kernel
 * 
 *************************************************************************************/
keyboard_driver my_keyboard_driver;
interrupt_controller my_interrupt_ctl;

extern "C" uint64_t* heap_start;
extern "C" uint64_t* heap_end;

extern "C" void kernel_main() {
    char input_buffer[constants::INPUT_BUFFER_SIZE];

    memory_manager my_mm(heap_start, 4096 * 4);
    // disk_driver* my_disk_driver = new disk_driver();
    // disk_driver myRefDiskDriver;


    Timer my_timer;
    my_timer.init_timer(100);

    
    io::my_cout(io::COLOUR_LIGHT_BLUE);

    // Initialize IDT for Adding the Interrupts
    my_interrupt_ctl.init_interrupt_data_table();
    // Load the first Exception Handler (Div/0 Exception) to test Exception-Handling 
    my_interrupt_ctl.load_idt_entry(0x00, (uint64_t) division_zero_handler_interrupt, 0x08, 0x8E);
    // Load the Keyboard Interrupt Handler with its ISR-Number, the "Pointer" to the ASM Handler, its Code Segment and its Flags into the IDT  
    my_interrupt_ctl.load_idt_entry(0x21, (uint64_t) keyboard_handler_interrupt, 0x08, 0x8E);

    my_interrupt_ctl.load_idt_entry(0x20, (uint64_t) timer_handler_interrupt, 0x08, 0x8E);

    // After Connecting the ISR to the Handler the Keyboard Interrupt can be Activated    
    my_keyboard_driver.keyboard_init();
    my_keyboard_driver.set_silent(true);

    my_interrupt_ctl.enable_interrupts();

    my_timer.unmask_timer();
    sleep(200);

    io::my_cout(io::COLOUR_LIGHT_BLUE, io::COLOUR_LIGHT_GRAY) << "Press enter to proceed...";
    io::my_cin >> input_buffer;

    my_keyboard_driver.set_silent(false);
    
    io::my_cout(io::COLOUR_GREEN, io::COLOUR_BLACK) << io::OSTREAM_APPEND::clear;

    io::my_cout << "Welcome to my 64-bit kernel!" << io::OSTREAM_APPEND::endl;

    io::my_cout << "Would you Like to have the Cursor enabled? yes/no" << io::OSTREAM_APPEND::endl;
    while(true) {
        io::my_cin >> input_buffer;
        if(string_comp(input_buffer, "yes")) {
            break;
        } else if(string_comp(input_buffer, "no")) {
            CursorController::disableCursor();
            break;
        } 
        io::my_cout << "What are you trying to Say? ";
    }

    test_memory_manager(my_mm);
    io::my_cout << "Now for testing the Disk IO Driver Features: Press enter to continue..." << io::OSTREAM_APPEND::endl;
    io::my_cin >> input_buffer;
    //test_disk_driver(my_disk_driver);


    DiskDriver disk;
    if(!disk.initialize()) {
        io::my_cout << "Failed to Init Disk\n";
    }
    FATDriver fat(disk);
    if(!fat.initialize()) {
        io::my_cout << "Failed to init FAT\n";
    }
    if(!fat.formatDisk()) {
        io::my_cout << "Failed to Format Disk\n";
    }

    io::my_cout << "Formatted Disk and set up\n";
    const char* testFile = "TESTFLE.TXT";
    const uint8_t fileData[] = "Hello, FAAT16! Lol, does this Work\nsss";
    uint32_t fileSize = sizeof(fileData) - 1;   // Exclude 0 Terminator

    if(!fat.createDirectory("TESTDIR", 0xFFFF))
        io::my_cout << "Failed to create Dir\n";
        
    if(!fat.createFile(testFile, fileData, fileSize, 0xFFFF))
        io::my_cout << "Failed to create File\n";

    if(!fat.createFile(testFile, fileData, fileSize, fat.findDirectoryCluster("TESTDIR")));
        io::my_cout << "Failed to create File\n";

    io::my_cout << "========\n";
    Vector<char*> dirEntries = fat.listDirectory("/");
    for(size_t i = 0; i < dirEntries.size(); ++i) {
        io::my_cout << dirEntries[i] << "\n";
        delete[] dirEntries[i];
    }
    io::my_cout << "========\n";
    dirEntries = fat.listDirectory("TESTDIR");
    for(size_t i = 0; i < dirEntries.size(); ++i) {
        io::my_cout << dirEntries[i] << "\n";
        delete[] dirEntries[i];
    }
    io::my_cout << "\n=)(=\n";
    Vector<uint8_t> fileBuffer;

    if(fat.readFile(testFile, fileBuffer)) {
        io::my_cout << "File Read : " << testFile << "\n";
        for(size_t i = 0; i < fileBuffer.size(); ++i)
            io::my_cout << static_cast<char>(fileBuffer[i]);
        io::my_cout << io::OSTREAM_APPEND::endl;
    }

    // filesystem::FatFileSystem my_fat_fs;
//    filesystem::FatFileSystem my_fat_fs;
//    my_fat_fs.formatDiskToFAT(my_disk_driver);
//    my_fat_fs.Initialize(my_disk_driver, 0x0);
//    my_fat_fs.ListRootDir();

    io::my_cin >> input_buffer;

    io::my_cout << "Okay, then please enter your Name: ";
    io::my_cin >> input_buffer;


    io::my_cout << "Well, hello " << input_buffer << io::OSTREAM_APPEND::endl;
    
    // For testing of the CursorController Interface
    //CursorController::disableCursor();
    //CursorController::updateCursorPosition(0,3);
    //CursorController::enableCursor(2,3);

    io::my_cout << "And now for the Input test: " << io::OSTREAM_APPEND::endl;

    char buffer[constants::INPUT_BUFFER_SIZE];
    io::my_cin.operator>>(buffer);
    io::my_cout << "You have entered " << buffer << '\n'; 
    io::my_cin >> buffer;
    io::my_cout << "Entered: " << buffer << '\n';
    for(int i = 0; i < 10; i++) {
        char buffer_2[constants::INPUT_BUFFER_SIZE];
        io::my_cin >> buffer_2;
        io::my_cout << "Entered: " << buffer_2 << '\n';
    }

    while(true) __asm__("hlt\n\t");
}