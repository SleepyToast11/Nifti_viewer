//
// Created by jsparnaay on 21/04/24.
//

#ifndef CMAKESFMLPROJECT_MRIWINDOW_H
#define CMAKESFMLPROJECT_MRIWINDOW_H

#include <SFML/Graphics.hpp>
#include <atomic>
#include <thread>
#include <memory>
#include "../IPC/IPC.h"
#include "../niftyWrapper/niftiWrapper.h"

class MRIWindow{
private:
    std::shared_ptr<niftiWrapper> niftiData;
    SliceType sliceType;
    std::shared_ptr <std::atomic_bool> kill = std::make_shared<std::atomic_bool>(false);
    std::thread windowWorker;
    IPC<sf::Image> imagePipe;
    void threadWorker(int width, int height, std::string windowName);
    std::mutex dimMutex;
    std::array<int, 4> currentDim = {0};
    std::atomic_int currentLoader = 0;
    IPC<std::array<int, 4>>::Writer dimPipe;
public:
    std::thread updateDim(std::array<int, 4>);
    std::thread loadImageCoor_t(int sliceNum, int timeIndex);
    void loadImage(const sf::Image& image);
    void loadImageCoorNoTime(int sliceNum);
    void loadImageCoor(int sliceNum, int timeIndex);
    MRIWindow(SliceType sliceType,  std::shared_ptr<niftiWrapper> niftiData, IPC<std::array<int, 4>>::Writer dimPipe);
    ~MRIWindow();
    void join();
    bool getKill();
};


#endif //CMAKESFMLPROJECT_MRIWINDOW_H
