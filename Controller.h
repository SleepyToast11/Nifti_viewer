//
// Created by jsparnaay on 22/04/24.
//

#ifndef FINALMRIASSIGNMENT_CONTROLLER_H
#define FINALMRIASSIGNMENT_CONTROLLER_H


#include "window/MRIWindow.h"
#include "window/ScrollWindow.h"
#include "IPC/IPC.h"
class Controller {
private:
    std::shared_ptr<niftiWrapper> niftiData;
    std::thread dimensionWorkerThread;
    IPC<std::array<int, 4>> dimPipe = IPC<std::array<int, 4>>();
    MRIWindow sideWindow;
    MRIWindow frontWindow;
    MRIWindow topWindow;
    ScrollWindow scrollWindow;
    void dimensionWorker();
public:
    Controller(const std::shared_ptr<niftiWrapper> &niftiData);
    ~Controller();
    void join();
};


#endif //FINALMRIASSIGNMENT_CONTROLLER_H
