#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include "Files.h"
#include "Commands.h"
#include <string>
#include <vector>


using namespace std;

class Environment {
private:
    FileSystem fs;
    vector<BaseCommand *> commandsHistory;

public:
    Environment(); //Consturctor
    Environment(const Environment &other); //Copy Consturctor
    ~Environment(); //Desturctor
    Environment &operator=(const Environment &other);//Copy assignment
    Environment &operator=(Environment &&other);//Move assignment
    Environment(Environment &&other); //Move Consturctor
    void start();
    FileSystem &getFileSystem(); // Get a reference to the file system
    void addToHistory(BaseCommand *command); // Add a new command to the history
    const vector<BaseCommand *> &getHistory() const;// Return a reference to the history of commands
    BaseCommand * createCommandObj (const string lineStr);
    string getCmdText (const string lineStr);
    bool shouldEchoInput();
    bool sholudPrintRuleOf5();
    void forChecking();
};
#endif