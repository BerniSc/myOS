#include "kernel_tests.hpp"

struct ntor_test {
    ntor_test() {
        io::my_cout << "I have been created...\n";
    }

    // This is used as it can be called quiet -> better for larger Testcases 
    ntor_test(int* counter) {
        (*counter)++;
    }

    ~ntor_test() {
        io::my_cout << "I have been destroyed...\n";
    }

    void foo() {
        io::my_cout << "I Fooed...\n";
    }
};

void test_disk_driver(disk_driver& diskDriver) {
    char* strDiskTest = "Hello, Disk!";
    uint16_t bufferDiskTest[256]; // Buffer one sector
    kmemset(bufferDiskTest, 0, 512);
    // Copy string (assuming string is shorter than 512 bytes)
    kmemcpy(strDiskTest, bufferDiskTest, strlen(strDiskTest) + 1);

    // Write string to first Sector of Disk
    diskDriver.write(0, 1, bufferDiskTest);
    io::my_cout << "Wrote to disk: " << strDiskTest << io::OSTREAM_APPEND::endl;

    // Clear buffer to read back the data
    kmemset(bufferDiskTest, 0, 512);

    // Read back first sector 
    diskDriver.read(0, 1, bufferDiskTest);

    // Assuming read data is null-terminated string
    char* readStr = reinterpret_cast<char*>(bufferDiskTest);

    io::my_cout << "Read from disk: " << readStr << io::OSTREAM_APPEND::endl;

}

// Function for testing the Memory Manager
// To shorten the function it uses a little bit nasty tricks to be able to use ternary operator
//      Requires second and third param have same retval -> third one is "overwriten" with "void()"
void test_memory_manager(memory_manager& mm) {
    io::my_cout << "Now for testing my Memory Manager..." << io::OSTREAM_APPEND::endl;
    
    io::my_cout << "Creating and destroing via raw KMalloc/KFree..." << io::OSTREAM_APPEND::endl;
    // Hm... thats a new one...: https://stackoverflow.com/questions/11320822/why-does-calling-method-through-null-pointer-work-in-c
    ntor_test* mem_ptr = reinterpret_cast<ntor_test*> (my_kmalloc(sizeof(ntor_test) * 8));
    (mem_ptr != nullptr) ? (mem_ptr->foo()) : (io::my_cout << "Nope... You dont get any Memory anymore..." << io::OSTREAM_APPEND::endl, void());
    my_kfree(mem_ptr);

    io::my_cout << "Creating and destroing via new/delete..." << io::OSTREAM_APPEND::endl;
    ntor_test* new_test = new ntor_test();
    (new_test) ? (new_test->foo()) : (io::my_cout << "Nope... You don't get any Memory anymore..." << io::OSTREAM_APPEND::endl, void());
    delete new_test;
    
    {
        // TODO Implement actuall Dummy here
        char bufferDummy[constants::INPUT_BUFFER_SIZE];
        io::my_cout << "Testing on a larger Scale with 5x new and delete. Press Enter to continue..." << io::OSTREAM_APPEND::endl;
        io::my_cin >> bufferDummy;
    }

    mm.print_size_chunk();

    ntor_test* array[5];
    int created = 0;

    for(int i = 0; i < 5; i++) {
        array[i] = new ntor_test(&created);
    }
    io::my_cout << "Created " << created << " Elements..." << io::OSTREAM_APPEND::endl;
    for(int i = 0; i < 5; i++) {
        delete array[i];
    }

    mm.print_size_chunk();

    io::my_cout << "Creating and destroing per static Stack allocation..." << io::OSTREAM_APPEND::endl;
    {
        ntor_test my_test;
        my_test.foo();
    }

    io::my_cout << "Finaly testing the Pseudosmart Pointer..." << io::OSTREAM_APPEND::endl;
    {
        ps_pointer<ntor_test> my_test(new ntor_test);
        my_test->foo();
    }

    {
        // TODO Implement actuall Dummy here
        char bufferDummy[constants::INPUT_BUFFER_SIZE];
        io::my_cout << "Press enter to finish..." << io::OSTREAM_APPEND::endl;
        io::my_cin >> bufferDummy;
        io::my_cout << io::OSTREAM_APPEND::clear;
    }

}