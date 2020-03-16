#ifndef FILES_H_
#define FILES_H_

#include <string>
#include <vector>
#include "../include/GlobalVariables.h"

using namespace std;

class BaseFile {
private:
    string name;

public:
    BaseFile(string name);
    virtual ~BaseFile()=0;
    string getName() const;
    void setName(string newName);
    virtual int getSize() = 0;
    virtual BaseFile * clone()=0;
    virtual bool isFile()=0;
    virtual bool isDirectory()=0;

};

class File:public BaseFile {
private:
    int size;

public:
    File(string name, int size); // Constructor
    int getSize(); // Return the size of the file
    virtual File * clone();
    bool isFile();
    bool isDirectory();

};

class Directory : public BaseFile {
private:
    vector<BaseFile*> children;
    Directory *parent;

public:

    Directory(string name, Directory *parent); // Constructor
    Directory(const Directory& other); // Copy Constructor
    Directory& operator= (const Directory& other); // Copy Assignment
    Directory(Directory&& other);//Move Constructor
    Directory& operator= (Directory&& other); // Move Assignment
    ~Directory(); // Destructor
    virtual BaseFile * clone();

    Directory *getParent() const; // Return a pointer to the parent of this directory
    void setParent(Directory *newParent); // Change the parent of this directory
    void addFile(BaseFile* file); // Add the file to children
    void removeFile(string name); // Remove the file with the specified name from children
    void earseLinkAssistance(string name);
    void removeFile(BaseFile* file); // Remove the file from children
    void earseLink(BaseFile* file);
    void sortByName(); // Sort children by name alphabetically (not recursively)
    void sortBySize(); // Sort children by size (not recursively)
    vector<BaseFile*> getChildren(); // Return children
    int getSize(); // Return the size of the directory (recursively)
    string getAbsolutePath();  //Return the path from the root to this
    bool isFile();
    bool isDirectory();
    bool isSonOf(string s);
    void setChildrenVector (vector<BaseFile*> &newChildren);
    bool isInTheHierarchyOf (Directory* pos);
    bool gotAChildNamed (string name);
};
#endif