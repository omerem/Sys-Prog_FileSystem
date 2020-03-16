#include "../include/Files.h"
#include "../include/FileSystem.h"
#include "../include/Environment.h"
#include "../include/Commands.h"
#include <algorithm>
#include <typeinfo>
#include <iostream>
#include <sstream>
using namespace std;
Environment::Environment(): fs(), commandsHistory(){}


void Environment::start(){
    //forChecking();
    string  lineStr;
    cout<<fs.getWorkingDirectory().getAbsolutePath()<<">";
    getline(cin, lineStr);
    while (lineStr!="exit")
    {
        BaseCommand* cmd = createCommandObj (lineStr);
        cmd->execute(getFileSystem());
        addToHistory(cmd);
        cout<<fs.getWorkingDirectory().getAbsolutePath()<<">";
        getline(cin, lineStr);
        if (shouldEchoInput())
            cout << lineStr << endl;
    }
}

void Environment::forChecking() {

    Directory* a = new Directory("a",&fs.getRootDirectory());
    Directory* b = new Directory("b",&fs.getRootDirectory());
//    Directory* b1 = new Directory("b", a);
//    Directory* b2 = new Directory("b",&fs.getRootDirectory());
//    Directory* c = new Directory("c",nullptr);
//    Directory* d = new Directory("d",nullptr);
    fs.getRootDirectory().addFile(a);
    fs.getRootDirectory().addFile(b);
//    fs.getRootDirectory().addFile(b2);
//    a->addFile(b1);
    File* f1 = new File("f1",100);
//    File* f2 = new File("f1",2);
    a->addFile(f1);
//    d->addFile(f2);
}

Environment::Environment(const Environment& other): fs(other.fs), commandsHistory(other.commandsHistory){
    for (int i = 0; (unsigned) i < other.commandsHistory.size(); i++) {
        commandsHistory.push_back(&other.commandsHistory[i]->clone());
    }
    if(verbose==1 || verbose==3){
        cout<<"Environment::Environment(const Environment& other)"<<endl;
    }
}

Environment::Environment(Environment&& other): fs(other.fs), commandsHistory(other.commandsHistory){
    for (unsigned int i = 0; i < other.commandsHistory.size(); i++) {
        commandsHistory.push_back(other.commandsHistory[i]);
        other.commandsHistory[i]= nullptr;
    }
    if(verbose==1 || verbose==3){
        cout<<"Environment::Environment(Environment&& other)"<<endl;
    }
}

Environment::~Environment(){
    for (unsigned int i = 0; i < commandsHistory.size(); i++)
    {
        while(!commandsHistory.empty()){
            delete commandsHistory.back();
            commandsHistory.back()= nullptr;
            commandsHistory.pop_back();
        }
    }
    if(verbose==1 || verbose==3){
        cout<<"Environment::~Environment()"<<endl;
    }
}

Environment& Environment::operator=(Environment &&other) {
    for (unsigned int i = 0; i < commandsHistory.size(); i++) {
        delete commandsHistory[i];
    }
    commandsHistory.clear();
    for (unsigned int i = 0; i < other.commandsHistory.size(); i++) {
        commandsHistory.push_back(other.commandsHistory[i]);
        other.commandsHistory[i]= nullptr;
    }
    if(verbose==1 || verbose==3){
        cout<<"Environment& Environment::operator=(Environment &&other)"<<endl;
    }
    return *this;
}

Environment& Environment::operator=(const Environment &other) {
    for (unsigned int i = 0; i < commandsHistory.size(); i++) {
        delete commandsHistory[i];
    }
    commandsHistory.clear();
    for (unsigned int i = 0; i < other.commandsHistory.size(); i++) {
        commandsHistory.push_back(&other.commandsHistory[i]->clone());
    }
    if(verbose==1 || verbose==3){
        cout<<"Environment& Environment::operator=(const Environment &other)"<<endl;
    }
    return *this; // return problem
}


void Environment::addToHistory(BaseCommand *command) {
    commandsHistory.push_back(command);
}

bool Environment::shouldEchoInput() {
    return ((verbose==2)||(verbose==3));
}

bool Environment::sholudPrintRuleOf5() {
    return ((verbose==1)||(verbose==3));
}


string Environment::getCmdText (const string lineStr)
{
    unsigned int i=0;
    while (i<lineStr.size()&&lineStr[i]!=' '){
        i++;
    }
    string cmdStr;
    cmdStr= lineStr.substr(0,i);
    return cmdStr;
}

BaseCommand * Environment::createCommandObj (const string lineStr)
//(const string cmdStr, const string argsStr)
{

    string cmdStr, argsStr;
    cmdStr = getCmdText (lineStr);
    if(cmdStr.size()<lineStr.size())
        argsStr = lineStr.substr(cmdStr.size()+1, lineStr.size());

    BaseCommand * cmdObj = nullptr;

    if (cmdStr=="pwd") {
        cmdObj = new PwdCommand(argsStr);
        return cmdObj;
    }
    if (cmdStr=="cd") {
        cmdObj = new CdCommand(argsStr);
        return cmdObj;
    }
    if (cmdStr=="ls") {
        cmdObj = new LsCommand(argsStr);
        return cmdObj;
    }
    if (cmdStr=="mkdir") {
        cmdObj = new MkdirCommand(argsStr);
        return cmdObj;
    }
    if (cmdStr=="mkfile") {
        cmdObj = new MkfileCommand(argsStr);
        return cmdObj;
    }
    if (cmdStr=="rm") {
        cmdObj = new RmCommand(argsStr);
        return cmdObj;
    }
    if (cmdStr=="mv") {
        cmdObj = new MvCommand(argsStr);
        return cmdObj;
    }
    if (cmdStr=="history") {
        cmdObj = new HistoryCommand(argsStr, commandsHistory);
        return cmdObj;
    }
    if (cmdStr=="exec") {
        cmdObj = new ExecCommand(argsStr, commandsHistory);
        return cmdObj;
    }
    if (cmdStr=="verbose") {
        cmdObj = new VerboseCommand(argsStr);
        return cmdObj;
    }
    if (cmdStr=="rename") {
        cmdObj = new RenameCommand(argsStr);
        return cmdObj;
    }
    if (cmdStr=="cp") {
        cmdObj = new CpCommand(argsStr);
        return cmdObj;
    }
    cmdObj = new ErrorCommand(lineStr);
    return cmdObj;
}

FileSystem &Environment::getFileSystem(){
    return fs;
}

const vector<BaseCommand *> &Environment::getHistory() const {
    return commandsHistory;
}
