#include <iostream>
#include "niftyWrapper/niftiWrapper.h"
#include "Controller.h"
#include "shell/sshell.h"
#include <X11/Xlib.h>


int main() {

    XInitThreads();

    /*
    std::shared_ptr<niftiWrapper> nif = std::make_shared<niftiWrapper>();
    char fname[] = "bp_mc.nii.gz";
    if(nif->read(fname))
        std::cout<<"yess";

    Controller controller(nif);
    */

    Shell shell;
    shell.startShell(); //blocking until exit is called

    return 0;
}
