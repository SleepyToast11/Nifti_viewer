//
// Created by jsparnaay on 22/04/24.
//

#ifndef CMAKESFMLPROJECT_SCROLLWINDOW_H
#define CMAKESFMLPROJECT_SCROLLWINDOW_H

#include <SFML/Graphics.hpp>
#include <atomic>
#include <thread>
#include <memory>
#include "../IPC/IPC.h"
#include "../niftyWrapper/niftiWrapper.h"


class ScrollWindow {
private:
    IPC<std::array<int, 4>>::Writer timeChanger;
    std::array<int, 4> currentDim = {0};
    std::shared_ptr<niftiWrapper> niftiData;
    std::shared_ptr <std::atomic_bool> kill = std::make_shared<std::atomic_bool>(false);
    std::thread windowWorker;
    IPC<sf::Image> imagePipe;
    std::mutex dimMutex;
    void threadWorker(unsigned int numberOfTimeSlice);
public:
    std::thread updateDims(std::array<int, 4> dims);
    void loadImage(std::vector<unsigned char> colorAcrossTime);
    ScrollWindow(std::shared_ptr<niftiWrapper> niftiData, IPC<std::array<int, 4>>::Writer timeChanger);
    ~ScrollWindow();
    void join();
    bool getKill();

    void loadImageCoor(std::array<int, 4> dims);
};


#endif //CMAKESFMLPROJECT_SCROLLWINDOW_H
