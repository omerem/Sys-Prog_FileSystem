#include "../include/Files.h"
#include "../include/FileSystem.h"
#include "../include/Commands.h"
#include "../include/Environment.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

BaseCommand::BaseCommand(string args): args(args){
//    this->args=args;
}

BaseCommand::BaseCommand(const BaseCommand& other): args(other.args){}

string BaseCommand::getArgs() {
    return args;
}

int BaseCommand::getIntFromString(string str) {
    int num;

    stringstream strStream(getArgs());
    strStream >> num;
    return num;
}

vector<string> BaseCommand::split(string s, char c) {
    vector <string> result;
    string temp = "";
    for (unsigned int i = 0; i <s.length(); ++i) {
        if (s[i] == c) {
            result.push_back(temp);
            temp = "";
        } else if (i == s.length()-1) {
            temp += s[i];
            result.push_back(temp);
            temp = "";
        }
        else {
            temp += s[i];
        }
    }
    if (result.size()>0 && result[0]=="")
        result.erase(result.begin());
    return result;
}


BaseFile *BaseCommand::isValidPathToDirectory(string path, FileSystem &fs) {
    BaseFile *pos = isValidPath(path, fs);
    if (pos == nullptr)
        return nullptr;
    if (pos->isFile())
        return nullptr;
    else
        return pos;
}

BaseFile *BaseCommand::isValidPath(string path, FileSystem &fs) {
    if(path=="/")
        return &fs.getRootDirectory();
    vector<string> vec_path = split(path, '/');
    Directory* pos=&fs.getWorkingDirectory();
    bool found=false;
    if (path.at(0)=='/') // start from RD
        pos=&fs.getRootDirectory();
    for (string word: vec_path)
    {
        found = false;
        if (word=="..") {
            pos = pos->getParent();
            if (pos == nullptr)
                return nullptr;
            continue;
        }

        for (unsigned int i=0; i<(pos->getChildren()).size(); i++)
        {
            BaseFile *child = (pos->getChildren())[i];
            if (child->getName() == word) {
                if (child->isDirectory()) {
                    pos = dynamic_cast<Directory *>(child);
                    found = true;
                    break;
                }
                else {// pos is file
                    if (word == vec_path[vec_path.size()-1])
                        return child;
                    else
                        return nullptr;
                }
            }

        }
        if(!found)
            return nullptr;
    } // end for each word
    return pos;
}

bool BaseCommand::gotChildInThatName(Directory *dir, string name) {
    int numberOfChildren = dir->getChildren().size();
    for (int i=0; i<numberOfChildren; i++){
        if (dir->getChildren()[i]->getName() == name) {
            return true;
        }
    }
    return false;
}

Directory *BaseCommand::isValidPathToDirectory_returnDirectory(string path, FileSystem &fs) {
    BaseFile* baseFilePos = isValidPathToDirectory(path, fs);
    Directory* directoryPos = dynamic_cast<Directory*> (baseFilePos);
    return directoryPos;
}

Directory *BaseCommand::getDirectoryFromVector(vector<string> pathVec, FileSystem &fs, string srcName) {
    Directory* pos;
    if (srcName.at(0) == '/')
    {
        pos = &fs.getRootDirectory();
    }
    else
    {
        pos = &fs.getWorkingDirectory();
    }
    for (unsigned int i = 0; i < pathVec.size(); i++)
    {
        pos = getAChild(pos, pathVec[i]);
        if (pos == nullptr)
        {
            return nullptr;
        }
    }
    return pos;
}

Directory* BaseCommand::getAChild(Directory *pos, string childName)
{
    if (childName == "..")
    {
        return pos->getParent();
    }

    unsigned int numberOfChildren = pos->getChildren().size();
    for (unsigned int i=0; i<numberOfChildren; i++)
    {
        if (pos->getChildren()[i]->getName() == childName)
        {
            return dynamic_cast<Directory*>(pos->getChildren()[i]);
        }
    }
    return nullptr; //return problem
}

BaseFile *BaseCommand::isValidPath2(string path, FileSystem &fs) {
    if(path=="/")
        return &fs.getRootDirectory();
    Directory *cur_dir = &fs.getWorkingDirectory();
    vector<string> vec_path = split(path, '/');
    if (path.at(0) == '/')
        cur_dir = &fs.getRootDirectory();
    for (unsigned int i = 0; i < vec_path.size(); i++) {
        if (vec_path[i] == "..") {
            cur_dir = cur_dir->getParent();
            if (cur_dir == nullptr)
                return nullptr;
            continue;
        }
        bool found = false;
        for (BaseFile *child: cur_dir->getChildren()) {
            if (child->getName() == vec_path[i]) {
                found = true;
                if (child->isFile()) {
                    if (i < vec_path.size() - 1)
                        return nullptr;
                    return child->clone();
                }
                cur_dir = dynamic_cast<Directory *>(child);
                if (i == (vec_path.size() - 1))
                {
                    return cur_dir;
                }
                else
                    break;
            }
        }
        if (found)
            return nullptr;
    }
    return nullptr; //return problem
}

PwdCommand::PwdCommand(string args):BaseCommand(args){}
void PwdCommand::execute(FileSystem &fs) {
    cout<<fs.getWorkingDirectory().getAbsolutePath()<<endl;
}

string PwdCommand::toString(){
    return this->getArgs();
}

PwdCommand &PwdCommand::clone() {
    return *(new PwdCommand(this->getArgs()));
}

string PwdCommand::getNameCommand() {
    return "pwd";
}

CdCommand::CdCommand(string args):BaseCommand(args){}
void CdCommand::execute(FileSystem &fs) {
    string s=getArgs();
    BaseFile * cur_dir=isValidPathToDirectory(s, fs);
    if (cur_dir== nullptr || cur_dir->isFile()) {
        cout << "The system cannot find the path specified"<<endl;
        return;
    }
    fs.setWorkingDirectory(dynamic_cast<Directory*>(cur_dir));
}

string CdCommand::toString(){
    return this->getArgs();
}

CdCommand &CdCommand::clone() {
    return *(new CdCommand(this->getArgs()));
}

string CdCommand::getNameCommand() {
    return "cd";
}

LsCommand::LsCommand(string args):BaseCommand(args){}


string LsCommand::toString(){
    return this->getArgs();
}

LsCommand &LsCommand::clone() {
    return *(new LsCommand(*this));
}

string LsCommand::getNameCommand() {
    return "ls";
}

void LsCommand::execute(FileSystem &fs) {
    string args = getArgs();
    vector<string> vecArgs = split(args, ' ');
    bool bySize = false;
    Directory *pos;
    /**
     * case 1: ""
     * case 2: "-s"
     * case 3: "-s <path>"
     * case 4: "<path>"
     */
    if (args == "") //case 1
    {
        pos = &fs.getWorkingDirectory();
    }
    else
    {
        if (vecArgs[0] == "-s") // case 2, 3
        {
            bySize = true;
            if (vecArgs.size() == 1) // case 2
            {
                pos = &fs.getWorkingDirectory();
            }
            else // case 3
            {
                pos = isValidPathToDirectory_returnDirectory(vecArgs[1], fs);
            }
        }
        else // case 4
        {
            bySize = false;
            pos = isValidPathToDirectory_returnDirectory(vecArgs[0], fs);
        }
    }
    if (pos == nullptr) {
        cout << "The system cannot find the path specified" << endl;
    }
    else //there's such a directory
    {
        if (bySize) // sort by size
        {
            pos->sortBySize();
        }
        else // sort by name
        {
            pos->sortByName();
        }
        vector<BaseFile *> children = pos->getChildren();
        if (!(pos->getChildren().empty())) {
            for (unsigned int i = 0; i < children.size(); i++) {
                if (children[i]->isFile())
                    cout << "FILE" << "\t" << children[i]->getName() << "\t" << children[i]->getSize() << endl;
                else
                    cout << "DIR" << "\t" << children[i]->getName() << "\t" << children[i]->getSize() << endl;
            }
        }
    }
}
MkdirCommand::MkdirCommand(string args):BaseCommand(args){}

string MkdirCommand::toString(){
    return this->getArgs();
}

void MkdirCommand::execute(FileSystem &fs) {
    string s=getArgs();
    vector<string>vec=split(s,'/');
    Directory* cur_dir=&fs.getWorkingDirectory();
    bool found;
    bool case2 = false;
    if (getArgs().at(0)=='/') {
        cur_dir = &fs.getRootDirectory();
    }
    unsigned int numberOfWords = vec.size();
    for (unsigned int wordNumber = 0; wordNumber < numberOfWords; wordNumber++)/// ******************FOR 1
    {
        found = false;
        string word = vec[wordNumber];
        if (word=="..")
        {
            cur_dir = cur_dir->getParent();
            if (cur_dir == nullptr)
                return;
            continue;
        }

        for (unsigned int i=0; i<cur_dir->getChildren().size(); i++) /// ************** FOR 2 /// עבור כל ילד
        {
            /**
             * For each phrase in the path:
             * case 1: the word is     a child, this IS NOT the last phrase - we continue
             * case 2: the word IS     a child, this IS     the last phrase - error output
             * case 3: the word IS NOT a child - make a directory
             */
            BaseFile* child = cur_dir->getChildren()[i];
            if (child->getName() == word) /// CASES 1 & 2 - the word is a child
            {
                found = true;
                if (child->isFile()) // Case 2_a
                {
                    cout << "The directory already exists" << endl;
                    case2=true;
                    break;
                }
                else
                {
                    if (wordNumber == numberOfWords-1)/// case 2_b - reached to end
                    {
                        cout << "The directory already exists" << endl;
                        case2=true;
                    }
                    else // case 1
                    {
                        cur_dir = dynamic_cast<Directory*> (child);
                        break;
                    }
                }
            }
        } // end for each child
        if (case2 == true)
        {
            break;
        }
        if (!found) /// CASE 3
        {
            Directory *dir = new Directory(word, cur_dir);
            cur_dir->addFile(dir);
            cur_dir = dir;
        }
    } //end for each word
}

MkdirCommand &MkdirCommand::clone() {
    return *(new MkdirCommand(this->getArgs()));
}

string MkdirCommand::getNameCommand() {
    return "mkdir";
}

MkfileCommand::MkfileCommand(string args):BaseCommand(args){}
MkfileCommand &MkfileCommand::clone() {
    return *(new MkfileCommand(this->getArgs()));
}

void MkfileCommand::execute(FileSystem &fs) {
    vector<string>vec1=split(this->getArgs(), ' ');
    string name;
    if(vec1[0].find('/') != string::npos)
        name= (vec1[0].substr(vec1[0].find_last_of('/')+1, vec1[0].size()-1));
    else
        name= vec1[0];
    BaseFile *dir;
    if(vec1[0].find('/') != string::npos){
        string path = (vec1[0].substr(0, vec1[0].find_last_of('/')));
        dir=isValidPathToDirectory(path, fs);
    }
    else
        dir=&fs.getWorkingDirectory();
    if (dir== nullptr || dir->isFile())
        cout<<"The system cannot find the path specified"<<endl;
    else if(((Directory*)dir)->isSonOf(name)){
        cout<<"File already exists"<<endl;
    }
    else {
        File *newFile = new File(name, std::stoi(vec1[1]));
        ((Directory *) dir)->addFile(newFile);
    }
}

string MkfileCommand::toString(){
    return this->getArgs();
}

string MkfileCommand::getNameCommand() {
    return "mkfile";
}

MvCommand::MvCommand(string args):BaseCommand(args){}

string MvCommand::toString(){
    return this->getArgs();
}

MvCommand &MvCommand::clone() {
    return *(new MvCommand(this->getArgs()));
}

void MvCommand::execute(FileSystem &fs) {
    vector<string> vecArgs = split(this->getArgs(), ' ');
    string srcName = vecArgs[0];
    string destName = vecArgs[1];

    Directory *destDir = isValidPathToDirectory_returnDirectory(destName, fs);
    BaseFile *srcBaseFile = isValidPath(srcName, fs);

    if ((destDir == nullptr) || (srcBaseFile == nullptr)) {
        cout << "No such file or directory" << endl;
        return;
    }
    else
    {
        vector <string> pathVec = split (srcName, '/');
        string fileName=pathVec[pathVec.size()-1];
        pathVec.pop_back();

        if (srcBaseFile->isFile()) //source is a file
        {
            if (!(destDir->isSonOf(fileName)))
            {
                Directory *fatherOfSrcFile;
                fatherOfSrcFile = getDirectoryFromVector(pathVec, fs, srcName);
                fatherOfSrcFile->earseLinkAssistance(fileName);
                destDir->addFile(srcBaseFile);
            }
            else
            {
                cout << "Can't move directory" << endl;
            }
        }
        else // source is a directory
        {
            Directory *srcDir = dynamic_cast<Directory *> (srcBaseFile);
            if (fs.getWorkingDirectory().isInTheHierarchyOf(srcDir) || destDir->isSonOf(fileName)) // the source is a WD or its parents.
            {
                cout << "Can't move directory" << endl;
            }
            else // the move is legal
            {
                srcDir->getParent()->earseLink(srcDir);
                srcDir->setParent(destDir);
                destDir->addFile(srcDir);


//                Directory* newdir = new Directory(*srcDir);
//                srcDir->getParent()->removeFile(srcDir);
//                //srcDir->setParent(destDir);
//                destDir->addFile(newdir);
//                newdir->setParent(destDir);
            }
        }
    }
}

string MvCommand::getNameCommand() {
    return "mv";
}

RmCommand::RmCommand(string args):BaseCommand(args){}
RmCommand &RmCommand::clone() {
    return *(new RmCommand(this->getArgs()));
}
string RmCommand::toString(){
    return this->getArgs();
}

void RmCommand::execute(FileSystem &fs) {// DEBUGGING
    string args = getArgs();
    BaseFile *removeMeBF = isValidPath(args, fs);
    if (removeMeBF == nullptr)
    {
        cout << "No such file or directory" <<endl;

        return;
    }
    if (removeMeBF->isFile()) // removeMeBF is a file
    {
        vector <string> pathVec = split (args, '/');
        string fileName=pathVec[pathVec.size()-1];
        pathVec.pop_back();
        Directory *fatherOfSrcFile = getDirectoryFromVector(pathVec, fs, args);
        fatherOfSrcFile->removeFile(fileName);

        return;
    }
    /// removeMeBF is a directory
    Directory *dir = dynamic_cast<Directory*> (removeMeBF);
    if (dir== nullptr || dir==&fs.getRootDirectory())
    {
        cout << "Can't remove directory" << endl;
    }
    else
    {
        if (fs.getWorkingDirectory().isInTheHierarchyOf(dir)) {
            cout << "Can't remove directory" << endl;
        }
        else
        {
            string parentPath = (args.substr(0, args.find_last_of('/')));
            if (parentPath == args)
            {
                fs.getWorkingDirectory().removeFile(args);
                return;
            }
            else
            {
                BaseFile *parent = isValidPathToDirectory(parentPath, fs);
                ((Directory *) parent)->removeFile((args.substr(args.find_last_of('/') + 1, args.size() - 1)));
            }
        }
    }
}

string RmCommand::getNameCommand() {
    return "rm";
}


HistoryCommand::HistoryCommand(string args, const vector<BaseCommand *> &history):BaseCommand(args), history(history) {}

string HistoryCommand::toString(){
    return this->getArgs();
}

HistoryCommand &HistoryCommand::clone() {
    return *(new HistoryCommand(this->getArgs(), this->history));
}

void HistoryCommand::execute(FileSystem &fs) {
    for (unsigned int i=0; i<history.size(); i++){
        cout << i;
        string commandName = history[i]->getNameCommand();
        if (commandName != "error")
            cout << "\t" + commandName;
        string argsName = history[i]->getArgs();
        if (argsName != "")
            cout << "\t" + argsName;
        cout << "" << endl;
    }
}

string HistoryCommand::getNameCommand() {
    return "history";
}


ErrorCommand::ErrorCommand(string args):BaseCommand(args){}
ErrorCommand &ErrorCommand::clone() {
    return *(new ErrorCommand(this->getArgs()));
}
string ErrorCommand::toString(){
    return this->getArgs();
}

void ErrorCommand::execute(FileSystem &fs) {
    vector<string> vec = split(getArgs(), ' ');
    cout<<vec[0]+": Unknown command"<<endl;
}

string ErrorCommand::getNameCommand() {
    return "error";
}




ExecCommand::ExecCommand(string args, const vector<BaseCommand *> & history):BaseCommand(args), history(history){}
string ExecCommand::toString(){
    return this->getArgs();
}

ExecCommand &ExecCommand::clone() {
    return *(new ExecCommand(this->getArgs(), history));
}

string ExecCommand::getNameCommand() {
    return "exec";
}


void ExecCommand::execute(FileSystem &fs) {
    int commandNumber=getIntFromString(getArgs());
    unsigned int unsignedCommandNumber = (unsigned int) commandNumber;
    if ((commandNumber<0) || (history.size()<=unsignedCommandNumber))
        cout << "Command not found" << endl;
    else
        history[commandNumber]->execute(fs);
}



VerboseCommand::VerboseCommand(string args):BaseCommand(args){}

string VerboseCommand::toString(){
    return this->getArgs();
}

VerboseCommand &VerboseCommand::clone() {
    return *(new VerboseCommand(this->getArgs()));
}

string VerboseCommand::getNameCommand() {
    return "verbose";
}

void VerboseCommand::execute(FileSystem &fs) {
    int num=getIntFromString(getArgs());
    if ((num==0)||(num==1)||(num==2)||(num==3))
        verbose=num;
    else
        cout << "wrong verbose input" << endl;
}


RenameCommand::RenameCommand(string args):BaseCommand(args){}

string RenameCommand::toString(){
    return this->getArgs();
}

RenameCommand &RenameCommand::clone() {
    return *(new RenameCommand(this->getArgs()));
}

string RenameCommand::getNameCommand() {
    return "rename";
}

void RenameCommand::execute(FileSystem &fs) {
    string args = getArgs();
    vector<string> vec = split(args, ' ');
    string path = vec[0];
    string newName = vec[1];

    BaseFile* pos = isValidPath(path, fs);
    if (pos == nullptr)
    {
        cout << "No such file or directory" << endl;
    }
    else
    {
        if (pos == &fs.getWorkingDirectory())
        {
            cout << "Can't rename the working directory" << endl;
        }
        else
        {
            if (pos == &fs.getRootDirectory()) {
                cout << "Can't rename the root directory" << endl;
            }
            else {
                vector<string> pathVec = split(args, '/');
                string baseFileName = pathVec[pathVec.size() - 1];
                pathVec.pop_back();
                Directory *fatherOfPos;
                fatherOfPos = getDirectoryFromVector(pathVec, fs, path);
                if (fatherOfPos->isSonOf(newName)) {
//                    cout << "Can't rename the directory or the file" << endl;
                }
                else
                {
                    pos->setName(newName);
                }
            }

        }
    }


}




CpCommand::CpCommand(string args):BaseCommand(args){}

string CpCommand::toString(){
    return this->getArgs();
}

CpCommand &CpCommand::clone() {
    return *(new CpCommand(this->getArgs()));
}

string CpCommand::getNameCommand() {
    return "cp";
}

void CpCommand::execute(FileSystem &fs) {

    string args = getArgs();
    vector<string> vec = split(args, ' ');
    string srcPath = vec[0];
    string destPath = vec[1];
    vector<string> vecDest = split(destPath, '/');
    string destFileName = vecDest[vecDest.size()-1];
    BaseFile *destBaseFile = isValidPathToDirectory(destPath, fs);
    BaseFile *srcBaseFile = isValidPath(srcPath, fs);
    if ((destBaseFile == nullptr) || (srcBaseFile == nullptr))
    {
        cout << "No such file or directory" << endl;
    }
    else {
        if (srcBaseFile == &fs.getRootDirectory())
        {
            cout << "Cannot copying root directory" << endl;
        }
        else {
            Directory *destDir = dynamic_cast<Directory *>(destBaseFile);
            if ((gotChildInThatName(destDir, destFileName))) {// there's allready a child in that name
                cout << "Name is allready exist" << endl;
            }
            else
            {
                string srcName = srcBaseFile->getName();
                if (destDir->gotAChildNamed(srcName)) //there's a child in this name
                {
//                    cout << "Ilegal copying" << endl;
                }
                else // legal case
                {
                    if (srcBaseFile->isFile())
                    {

                        File *temp = new File(srcName ,srcBaseFile->getSize());//copy to a new file object
                        destDir->addFile(temp);
                    }
                    else //the src is a dir
                    {
                        Directory *srcDir = dynamic_cast<Directory *>(srcBaseFile);
                        Directory *temp = new Directory(srcDir->getName(), destDir);
                        copyRecursion(srcBaseFile, temp);
                        destDir->addFile(temp);
                    }
                }
            }
        }
    }
}

BaseFile *CpCommand::copyRecursion(BaseFile *A, Directory *temp) {

    if (A->isDirectory())//1
    {
        Directory* dirA = dynamic_cast<Directory*>(A);
        int numberOfChildren = dirA->getChildren().size();
        for (int i=0; i<numberOfChildren; i++)//2
        {
            BaseFile* newChild;//3
            if (dirA->getChildren()[i]->isDirectory())//
            {
                newChild = new Directory (dirA->getChildren()[i]->getName(), temp);//3
                Directory* newChildDir = dynamic_cast<Directory*>(newChild);
                newChild = copyRecursion (dirA->getChildren()[i], newChildDir);//4
            }
            else // the child is a file
            {
                newChild = new File(dirA->getChildren()[i]->getName(), dirA->getChildren()[i]->getSize());
            }
            temp->addFile(newChild);
        }
    }
    return temp;
}



