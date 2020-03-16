#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include "Files.h"


class FileSystem {
private:
    Directory* rootDirectory;
    Directory* workingDirectory;
public:
    FileSystem();// Constructor
    FileSystem(const FileSystem& other);//Copy Constructor
    FileSystem(FileSystem&& other);//Move Constructor
    ~FileSystem();// Destructor
    FileSystem& operator=(const FileSystem& other);//Copy assignment
    FileSystem& operator=(FileSystem&& other);//Move assignment

    Directory& getRootDirectory() const; // Return reference to the root directory
    Directory& getWorkingDirectory() const; // Return reference to the working directory
    void setWorkingDirectory(Directory *newWorkingDirectory); // Change the working directory of the file system


};


#endif
