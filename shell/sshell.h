//
// Created by jerome on 3/20/24.
//

#ifndef MRIASSIGNMENT_SSHELL_H
#define MRIASSIGNMENT_SSHELL_H

#include <mutex>
#include <condition_variable>

/*
void taskRead(IPC<std::string>::Reader *reader);

void taskWriter(IPC<std::string>::Writer *writer);
int shell (char** envp, IPC<std::string>::Writer *writer);
*/
class Shell{
    char **envp = environ;
    std::mutex started;
    static int runCommand (const char* command, char* const argv[], char* const envp[], const char** path);
    void shell();
    void openMRIViewer(char* fname);
public:
    Shell();
    void startShell();
};
#endif //MRIASSIGNMENT_SSHELL_H
