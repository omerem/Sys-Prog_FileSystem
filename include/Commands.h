#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <string>
#include "FileSystem.h"



class BaseCommand {
private:
    string args;

public:
    BaseCommand(string args);
    BaseCommand(const BaseCommand& other);
    virtual ~BaseCommand(){};
//    BaseCommand(BaseCommand&& other);
//    BaseCommand& operator=(const BaseCommand& other);
    string getArgs();
    virtual void execute(FileSystem & fs) = 0;
    virtual string toString() = 0;
    virtual BaseCommand& clone()=0;
    Directory* isValidPathToDirectory_returnDirectory (string path, FileSystem &fs);
    BaseFile* isValidPathToDirectory(string path, FileSystem &fs);
    BaseFile* isValidPath2(string path, FileSystem &fs);
    BaseFile* isValidPath(string path, FileSystem &fs);
    vector<string> split(string s, char c);
    virtual string getNameCommand()=0;
    int getIntFromString(string str);
    bool gotChildInThatName (Directory* dir, string name);
    Directory* getDirectoryFromVector (vector<string> pathVec, FileSystem & fs, string srcName);
    Directory* getAChild (Directory* pos, string childName);


};

class PwdCommand : public BaseCommand {
private:
public:
    PwdCommand(string args);
    virtual PwdCommand& clone();
    void execute(FileSystem & fs); // Every derived class should implement this function according to the document (pdf)
    string toString();
    virtual string getNameCommand();

};

class CdCommand : public BaseCommand {
private:
public:
    CdCommand(string args);
    virtual CdCommand& clone();
    void execute(FileSystem & fs);
    string toString();
    virtual string getNameCommand();
};

class LsCommand : public BaseCommand {
private:
public:
    LsCommand(string args);
    virtual LsCommand& clone();
    void execute(FileSystem & fs);
    string toString();
    virtual string getNameCommand();
};

class MkdirCommand : public BaseCommand {
private:
public:
    MkdirCommand(string args);
    virtual MkdirCommand& clone();
    void execute(FileSystem & fs);
    string toString();
    virtual string getNameCommand();
};

class MkfileCommand : public BaseCommand {
private:
public:
    MkfileCommand(string args);
    void execute(FileSystem & fs);
    virtual MkfileCommand& clone();
    string toString();
    virtual string getNameCommand();
};

class CpCommand : public BaseCommand {
private:
public:
    CpCommand(string args);
    virtual CpCommand& clone();
    void execute(FileSystem & fs);
    string toString();
    virtual string getNameCommand();
    BaseFile* copyRecursion(BaseFile *copyFromMe, Directory * parent);
    //void copyChildrenFromOneToAnother (Directory* one, Directory* another);
};

class MvCommand : public BaseCommand {
private:
public:
    MvCommand(string args);
    virtual MvCommand& clone();
    void execute(FileSystem & fs);
    string toString();
    virtual string getNameCommand();
};

class RenameCommand : public BaseCommand {
private:
public:
    RenameCommand(string args);
    void execute(FileSystem & fs);
    virtual RenameCommand& clone();
    string toString();
    virtual string getNameCommand();
};

class RmCommand : public BaseCommand {
private:
public:
    RmCommand(string args);
    virtual RmCommand& clone();
    void execute(FileSystem & fs);
    string toString();
    virtual string getNameCommand();
};

class HistoryCommand : public BaseCommand {
private:
    const vector<BaseCommand *> & history;
public:
    HistoryCommand(string args, const vector<BaseCommand *> & history);
    virtual HistoryCommand& clone();
    void execute(FileSystem & fs);
    string toString();
    virtual string getNameCommand();
};


class VerboseCommand : public BaseCommand {
private:
public:
    VerboseCommand(string args);
    void execute(FileSystem & fs);
    virtual VerboseCommand& clone();
    string toString();
    virtual string getNameCommand();
};

class ErrorCommand : public BaseCommand {
private:
public:
    ErrorCommand(string args);
    ErrorCommand& clone();
    void execute(FileSystem & fs);
    string toString();
    virtual string getNameCommand();
};

class ExecCommand : public BaseCommand {
private:
    const vector<BaseCommand *> & history;
public:
    ExecCommand(string args, const vector<BaseCommand *> & history);
    void execute(FileSystem & fs);
    virtual ExecCommand& clone();
    string toString();
    virtual string getNameCommand();
};


#endif