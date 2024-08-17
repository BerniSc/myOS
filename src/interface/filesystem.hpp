#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

class FileSystemObject {
public:
    virtual void open() = 0;
    virtual void close() = 0;
    virtual int read(char* buffer, int size) = 0;
    virtual int write(const char* buffer, int size) = 0;
    virtual ~FileSystemObject() {}
};

class File : public FileSystemObject {
    // Implementation specific to files
};

class Directory : public FileSystemObject {
    // Implementation specific to directories, might include:
    // - A list of FileSystemObject pointers representing the contents
public:
    void add(FileSystemObject* obj); // Add a file or directory
    void remove(FileSystemObject* obj); // Remove a file or directory
    // Override methods from FileSystemObject
};

#endif

