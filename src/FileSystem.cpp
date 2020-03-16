#include "../include/Files.h"
#include "../include/FileSystem.h"
#include <iostream>

FileSystem::FileSystem(): rootDirectory(new Directory("/", nullptr)), workingDirectory(rootDirectory){};
//FileSystem::FileSystem(): rootDirectory(), workingDirectory(){
//    rootDirectory = new Directory("/", nullptr);
//    workingDirectory = rootDirectory;
//};
FileSystem::FileSystem(const FileSystem& other): rootDirectory(new Directory(*other.rootDirectory)), workingDirectory(rootDirectory){
    if(verbose==1 || verbose==3){
        cout<<"FileSystem::FileSystem(const FileSystem& other)"<<endl;
    }
}
FileSystem::FileSystem(FileSystem&& other): rootDirectory(new Directory(std::move(*other.rootDirectory))), workingDirectory(rootDirectory){
    delete other.rootDirectory;
    other.rootDirectory= nullptr;
    other.workingDirectory= nullptr;
    if(verbose==1 || verbose==3){
        cout<<"FileSystem::FileSystem(FileSystem&& other)"<<endl;
    }
}

FileSystem::~FileSystem(){
    delete rootDirectory;
    if(verbose==1 || verbose==3){
        cout<<"FileSystem::~FileSystem()"<<endl;
    }
}

FileSystem& FileSystem::operator=(const FileSystem &other){
    delete rootDirectory;
    rootDirectory=new Directory(*other.rootDirectory);
    workingDirectory=rootDirectory;
    if(verbose==1 || verbose==3){
        cout<<"FileSystem& FileSystem::operator=(const FileSystem &other)"<<endl;
    }
    return *this; // changing unREACHABLE
}

FileSystem& FileSystem::operator=(FileSystem &&other){
    if(this==&other)
        return *this;
    rootDirectory=new Directory(std::move(*other.rootDirectory));
    workingDirectory=rootDirectory;
    if(verbose==1 || verbose==3){
        cout<<"FileSystem& FileSystem::operator=(FileSystem &&other)"<<endl;
    }
    return *this; // changing unREACHABLE
}

Directory &FileSystem::getRootDirectory() const {
    return *rootDirectory;
}

Directory& FileSystem::getWorkingDirectory() const {
    return *workingDirectory;
}

void FileSystem::setWorkingDirectory(Directory *newWorkingDirectory){
    workingDirectory=newWorkingDirectory;
}
