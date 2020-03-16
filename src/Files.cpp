#include "../include/Files.h"
#include "../include/FileSystem.h"
#include <algorithm>
#include <typeinfo>
#include <iostream>


BaseFile::BaseFile(string name): name(name) {};

string BaseFile:: getName() const{
    return name;
};

void BaseFile::setName(string newName) {
    name=newName;
}

BaseFile::~BaseFile(){};

File::File(string name, int size): BaseFile(name), size(size){};
int File::getSize() {
    return size;
}

File* File::clone(){
    return new File(this->getName(), this->getSize());
}


bool File::isFile() {
    return true;
}

bool File::isDirectory() {
    return false;
}


Directory::Directory(string name, Directory *parent): BaseFile(name),children(), parent(parent){
};

Directory::Directory(const Directory& other):BaseFile(other.getName()),children(other.children), parent(other.parent) {
    for (unsigned int i = 0; i < other.children.size(); i++) {
        BaseFile *newBaseFile = other.children.at(i)->clone();
        if (typeid(*newBaseFile) == typeid(Directory)) {
            ((Directory *) newBaseFile)->setParent(this);
        }
        children.push_back(newBaseFile);
    }
    if(verbose==1 || verbose==3){
        cout<<"Directory::Directory(const Directory& other)"<<endl;
    }
}

Directory& Directory::operator=(const Directory& other){
    for (unsigned int i=0; i<children.size(); i++) {
        delete children.at(i);
    }
    children.clear();

    for (unsigned int i = 0; i < other.children.size(); i++) {
        BaseFile *newBaseFile = other.children.at(i)->clone();
        if (typeid(*newBaseFile) == typeid(Directory)) {
            ((Directory *) newBaseFile)->setParent(this);
        }
        children.push_back(newBaseFile);
    }
    setName(other.getName());
    parent=other.parent;
    if(verbose==1 || verbose==3){
        cout<<"Directory& Directory::operator=(const Directory& other)"<<endl;
    }
    return *this;
}


Directory::Directory(Directory&& other):BaseFile(other.getName()), children(other.children), parent(other.parent){
    for(BaseFile*& child:other.children){
        child= nullptr;
    }

    for(BaseFile* child:children){
        if (typeid(*child) == typeid(Directory)) {
            ((Directory *) child)->setParent(this);
        }
    }
    other.parent= nullptr;
    other.setName("");
    if(verbose==1 || verbose==3){
        cout<<"Directory::Directory(Directory&& other)"<<endl;
    }
}

Directory& Directory::operator=(Directory &&other){
    if (this==&other)
        return *this;
    for (unsigned int i=0; i<children.size(); i++) {
        delete children.at(i);
    }
    children.clear();
    children=other.children;

    for(BaseFile*& child:other.children){
        child= nullptr;
    }
    for(BaseFile* child:children){
        if (typeid(*child) == typeid(Directory)) {
            ((Directory *) child)->setParent(this);
        }
    }
    setName(other.getName());
    parent=other.parent;
    other.parent= nullptr;
    other.setName("");
    if(verbose==1 || verbose==3){
        cout<<"Directory& Directory::operator=(Directory &&other)"<<endl;
    }
    return *this; // unreachable //changing
}

Directory::~Directory(){
    for (unsigned int i=0; i<children.size(); i++) {
        delete children.at(i);
    }
    if(verbose==1 || verbose==3){
        cout<<"Directory::~Directory()"<<endl;
    }
}

BaseFile * Directory::clone(){
    return new Directory(*this);
}

Directory *Directory::getParent() const {
    return parent;
}

void Directory::setParent(Directory *newParent) {
    parent= newParent;
}

void Directory:: addFile(BaseFile *file) {
    children.push_back(file);
    if (Directory* d= dynamic_cast<Directory*>(file)){
        d->setParent(this);
    }

}

void Directory::removeFile(string name) {
    unsigned int i=0;
    while (i<children.size())
    {
        if (children.at(i)->getName()==name) {
            delete(children.at(i));
            children.erase(children.begin()+i);
            i = children.size();
        }
        i++;
    }
}

void Directory::earseLinkAssistance(string name) {
    unsigned int i=0;
    while (i<children.size())
    {
        if (children.at(i)->getName()==name) {
            children.erase(children.begin()+i);
            i = children.size();
        }
        i++;
    }
}

void Directory::removeFile(BaseFile *file){
    removeFile(file->getName());
}

void Directory::earseLink(BaseFile *file){
    earseLinkAssistance(file->getName());
}

void Directory::sortByName() {
    sort(children.begin(), children.end(), [] (const BaseFile *a, const BaseFile *b)-> bool{return a->getName()<b->getName();
    });
}

void Directory::sortBySize() {
    sort(children.begin(), children.end(), [] (BaseFile *a, BaseFile *b)-> bool{return a->getSize()< b->getSize();
    });
}

vector<BaseFile*> Directory::getChildren(){
    return children;
};

int Directory::getSize() {
    int totalSize=0;
    for (unsigned int i=0; i<children.size(); i++) {
        totalSize+=(children.at(i)->getSize());
    }
    return totalSize;
}

string Directory::getAbsolutePath(){
    if(this->getParent()==nullptr)
        return "/";
    if (this->getParent()->getParent()== nullptr)
        return this->getParent()->getAbsolutePath() +this->getName();
    return this->getParent()->getAbsolutePath() + "/"+this->getName();
}

bool Directory::isFile() {
    return false;
}

bool Directory::isDirectory() {
    return true;
}

bool Directory::isSonOf(string s) {
    for (unsigned int i = 0; i < this->getChildren().size(); i++) {
        if (this->getChildren()[i]->getName()==s)
            return true;
    }
    return false;
}


void Directory::setChildrenVector(vector<BaseFile *> &newChildren) {
    children = newChildren;
}

bool Directory::isInTheHierarchyOf(Directory *srcDir) {
    if (this == srcDir)
    {
        return true;
    }
    if (this->getParent() == nullptr) // if I am the root directory
    {
        return false;
    }
    return this->getParent()->isInTheHierarchyOf(srcDir);
}

bool Directory::gotAChildNamed(string name)
{
    unsigned int numberOfChildren = children.size();
    for (unsigned int i=0; i < numberOfChildren; i++)
    {
        if (name == children[i]->getName())
        {
            return true;
        }
    }
    return false;
}
