/*
 * Part of the solution for Assignment 1 (CS 464/564),
 * by Stefan Bruda.
 */

#include <stdio.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <string>
#include <filesystem>
#include <vector>
#include "sshell.h"
#include "../niftyWrapper/niftiWrapper.h"
#include "../Controller.h"

#define recv_nodata -1
/*
 * Global configuration variables.
 */

size_t str_tokenize(char* str, char** tokens, const size_t n) {
    size_t tok_size = 1;
    tokens[0] = str;

    size_t i = 0;
    while (i < n) {
        if (str[i] == ' ') {
            str[i] = '\0';
            i++;
            for (; i < n && str[i] == ' '; i++)
                /* NOP */;
            if (i < n) {
                tokens[tok_size] = str + i;
                tok_size++;
            }
        }
        else
            i++;
    }

    return tok_size;
}

const char* path[] = {"/bin","/usr/bin",0}; // path, null terminated (static)

std::string prompt(){
    return std::string(std::filesystem::current_path().append(" $:").string<char>().c_str());
}

/*
 * Typical reaper.
 */
void zombie_reaper (int signal) {
    int ignore;
    while (waitpid(-1, &ignore, WNOHANG) >= 0)
        /* NOP */;
}

int readline(const int fd, char* buf, const size_t max) {
    size_t i;
    int begin = 1;

    for (i = 0; i < max; i++) {
        char tmp;
        int what = read(fd,&tmp,1);
        if (what == -1)
            return -1;
        if (begin) {
            if (what == 0)
                return recv_nodata;
            begin = 0;
        }
        if (what == 0 || tmp == '\n') {
            buf[i] = '\0';
            return i;
        }
        buf[i] = tmp;
    }
    buf[i] = '\0';
    return i;
}

/*
 * Dummy reaper, set as signal handler in those cases when we want
 * really to wait for children.  Used by run_it().
 *
 * Note: we do need a function (that does nothing) for all of this, we
 * cannot just use SIG_IGN since this is not guaranteed to work
 * according to the POSIX standard on the matter.
 */
void block_zombie_reaper (int signal) {
    /* NOP */
}

/*
 * run_it(c, a, e, p) executes the command c with arguments a and
 * within environment p just like execve.  In addition, run_it also
 * awaits for the completion of c and returns a status integer (which
 * has the same structure as the integer returned by waitpid). If c
 * cannot be executed, then run_it attempts to prefix c successively
 * with the values in p (the path, null terminated) and execute the
 * result.  The function stops at the first match, and so it executes
 * at most one external command.
 */





int Shell::runCommand (const char* command, char* const argv[], char* const envp[], const char** path) {

    // we really want to wait for children so we inhibit the normal
    // handling of SIGCHLD
    signal(SIGCHLD, block_zombie_reaper);
    int childp = fork();
    int status = 0;

    if (childp == 0) { // child does execve

        execve(command, argv, envp);     // attempt to execute with no path prefix...
        for (size_t i = 0; path[i] != 0; i++) { // ... then try the path prefixes
            char* cp = new char [strlen(path[i]) + strlen(command) + 2];
            sprintf(cp, "%s/%s", path[i], command);
            execve(cp, argv, envp);
            delete [] cp;
        }

        // If we get here then all execve calls failed and errno is set
        char* message = new char [strlen(command)+10];
        sprintf(message, "exec %s", command);
        perror(message);
        delete [] message;
        exit(errno);   // crucial to exit so that the function does not
        // return twice!
    }

    else { // parent just waits for child completion
        waitpid(childp, &status, 0);
        // we restore the signal handler now that our baby answered
        signal(SIGCHLD, zombie_reaper);
        return status;
    }
}

void Shell::shell() {
    char command[129];   // buffer for commands
    command[128] = '\0';
    char* com_tok[129];  // buffer for the tokenized commands
    size_t num_tok;      // number of tokens

    printf("Welcome here are the commands : open [filename.nii/filename.nii.gz]\n\topen a nifti file\ncd [dir]\n\tChange directory\nAll other command a usual shell has\n");
    signal(SIGCHLD, zombie_reaper);

    // Command loop:
    while(1) {
        // Read input:
        printf("%s",std::filesystem::current_path().append(" $:").string<char>().c_str());
        fflush(stdin);
        if (fgets(command, 128, stdin) == 0) {
            // EOF, will quit
            printf("\nBye\n");
            return;
        }
        // fgets includes the newline in the buffer, get rid of it
        if(strlen(command) > 0 && command[strlen(command) - 1] == '\n')
            command[strlen(command) - 1] = '\0';

        // Tokenize input:
        char** real_com = com_tok;  // may need to skip the first
        // token, so we use a pointer to
        // access the tokenized command
        num_tok = str_tokenize(command, real_com, strlen(command));
        real_com[num_tok] = 0;      // null termination for execve

        int bg = 0;
        if (strcmp(com_tok[0], "&") == 0) { // background command coming
            bg = 1;
            // discard the first token now that we know that it
            // specifies a background process...
            real_com = com_tok + 1;
        }

        // ASSERT: num_tok > 0

        // Process input:
        if (strlen(real_com[0]) == 0) // no command, luser just pressed return
            continue;

        else if (strcmp(real_com[0], "exit") == 0) {
            printf("Bye\n");
            return;
        }

        else if (strcmp(real_com[0], "cd") == 0) {
            if (real_com[1] == 0){
                printf("cd: too few arguments\n");
                continue;
            }
            if(strncmp(real_com[1], "..", 2) == 0)
                printf("%d\n", chdir(std::filesystem::current_path().parent_path().c_str()));

            else
                // list all the files given in the command line arguments
                printf("%d\n", chdir(std::filesystem::current_path().append("/").append(real_com[1]).c_str()));
        }

        else if (strcmp(real_com[0], "open") == 0) {
            if (real_com[1] == 0){
                printf("open nifti: too few arguments\n");
                continue;
            }
            openMRIViewer(real_com[1]);
        }

        else { // external command
            if (bg) {  // background command, we fork a process that
                // awaits for its completion
                int bgp = fork();
                if (bgp == 0) {
                    // child executing the command
                    int r = runCommand(real_com[0], real_com, envp, path);
                    printf("& %s done (%d)\n", real_com[0], WEXITSTATUS(r));
                    if (r != 0) {
                        printf("& %s completed with a non-null exit code\n", real_com[0]);
                    }
                    return; //this should never happen
                }
                else  // parent goes ahead and accepts the next command
                continue;
            }
            else {  // foreground command, we execute it and wait for completion
                int r = runCommand(real_com[0], real_com, envp, path);
                if (r != 0) {
                    printf("%s completed with a non-null exit code (%d)\n", real_com[0], WEXITSTATUS(r));
                }
            }
        }
    }
}

void Shell::startShell(){
    shell();
}


void Shell::openMRIViewer(char* fname){

    std::shared_ptr<niftiWrapper> nif = std::make_shared<niftiWrapper>();
    if(!nif->read(fname)) {
        printf("%s", "Failed to open file\n");
        return;
    }
    printf("%s", "Successfully open the file, please close all the windows to return to the cmd prompt\n");
    Controller controller(nif);
    controller.join();
}


Shell::Shell() = default;