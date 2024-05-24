//
// Created by jsparnaay on 22/04/24.
//

#include "Controller.h"



Controller::Controller(const std::shared_ptr<niftiWrapper> &niftiData) :
        niftiData(niftiData),
        frontWindow(front, niftiData, dimPipe.getWriter()),
        sideWindow(side, niftiData, dimPipe.getWriter()),
        topWindow(top, niftiData, dimPipe.getWriter()),
        scrollWindow(niftiData, dimPipe.getWriter()) {
    dimensionWorkerThread = std::thread(&Controller::dimensionWorker, this);
}

Controller::~Controller() {
    dimPipe.killAll();
    if(dimensionWorkerThread.joinable())
        dimensionWorkerThread.join();
}

void Controller::dimensionWorker() {
    std::array<int, 4> value = {0};
    IPC<std::array<int, 4>>::Reader reader = dimPipe.getReader();
    while(reader.dequeue(value)){
        std::thread frontWindowThread = frontWindow.updateDim(value);
        std::thread sideWindowThread = sideWindow.updateDim(value);
        std::thread topWindowThread = topWindow.updateDim(value);
        std::thread timeWindowThread = scrollWindow.updateDims(value);

        if(frontWindowThread.joinable())
            frontWindowThread.join();
        if(sideWindowThread.joinable())
            sideWindowThread.join();
        if(topWindowThread.joinable())
            topWindowThread.join();
        if(timeWindowThread.joinable())
            timeWindowThread.join();
    }
}

void Controller::join() {
    frontWindow.join();
    sideWindow.join();
    topWindow.join();
    scrollWindow.join();
}
