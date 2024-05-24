//
// Created by jsparnaay on 21/04/24.
//

#include "MRIWindow.h"
#include <algorithm>
#include <array>
#include <utility>

void MRIWindow::threadWorker(int width, int height, std::string windowName) {

    int scrollBarHeight = 3;

    sf::RenderWindow window(sf::VideoMode(width, height + scrollBarHeight), windowName);
    IPC<sf::Image>::Reader reader = imagePipe.getReader();
    window.setFramerateLimit(144);

    // Create an image and fill it with a gradient
    sf::Image image;
    image.create(width, height);
    for (unsigned int x = 0; x < image.getSize().x; ++x)
    {
        for (unsigned int y = 0; y < image.getSize().y; ++y)
        {
            sf::Color color(x * 255 / image.getSize().x, y * 255 / image.getSize().y, 128); // RGB gradient
            image.setPixel(x, y, color);
        }
    }

    // Create a texture from the image
    sf::Texture texture;
    texture.loadFromImage(image);

    // Create a sprite
    sf::Sprite sprite(texture);

    sf::RectangleShape scrollBar;
    sf::RectangleShape scrollButton;

    sf::RectangleShape xIndicator;
    sf::RectangleShape yIndicator;

    xIndicator.setSize(sf::Vector2f(1, height));
    xIndicator.setFillColor(sf::Color::Blue);
    xIndicator.setPosition(0, 0);

    yIndicator.setSize(sf::Vector2f(width, 1));
    yIndicator.setFillColor(sf::Color::Blue);
    yIndicator.setPosition(0, 0);

    int sliceNum;
    if(!niftiData->getNumOfSlice(sliceType, sliceNum))
        goto closeWindow;

    scrollBar.setSize(sf::Vector2f(width, scrollBarHeight));
    scrollBar.setFillColor(sf::Color::Red);
    scrollBar.setPosition(0, height);

    scrollButton.setSize(sf::Vector2f(width/sliceNum, scrollBarHeight));
    scrollButton.setFillColor(sf::Color::Blue);
    scrollButton.setPosition(0, height);

    while (window.isOpen() && !getKill())
    {
        sf::Event event{};
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed)
                goto closeWindow;

            if (event.type == sf::Event::MouseButtonPressed){
                if (event.mouseButton.button == sf::Mouse::Left){
                    if (scrollBar.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))){

                        scrollButton.setPosition( ((float) event.mouseButton.x) / window.getSize().x * width, height);

                        int sliceNum;
                        if(!niftiData->getNumOfSlice(sliceType, sliceNum))
                            goto closeWindow;

                        std::array<int, 4> temp = {0};
                        dimMutex.lock();
                        for (int i = 0; i < 4; ++i) {
                            temp[i] = currentDim[i];
                        }
                        dimMutex.unlock();

                        int newIndex = ((float) event.mouseButton.x) / window.getSize().x * sliceNum;

                        switch (sliceType) {
                            case front:
                                dimPipe.enqueue({temp[0], temp[1], newIndex, temp[3]});
                                break;
                            case side:
                                dimPipe.enqueue({newIndex, temp[1], temp[2], temp[3]});
                                break;
                            case top:
                                dimPipe.enqueue({temp[0], newIndex, temp[2], temp[3]});
                                break;
                        }

                        //loadImageCoor_t(((float) event.mouseButton.x) / window.getSize().x * sliceNum, temp[3]);
                    }
                }
                /*
                else if (sprite.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))){

                    int xn, yn;
                    if(!niftiData->getSizeOfSlice(sliceType, xn, yn))
                        goto closeWindow;

                    int xIndex = ((float) event.mouseButton.x) / window.getSize().x * xn;
                    int yIndex = ((float) event.mouseButton.y) / window.getSize().y * yn;

                    std::array<int, 4> temp = {0};
                    dimMutex.lock();
                    for (int i = 0; i < 4; ++i) {
                        temp[i] = currentDim[i];
                    }
                    dimMutex.unlock();

                    switch (sliceType) {
                        case front:
                            dimPipe.enqueue({xIndex, yIndex, temp[2], temp[3]});
                            break;
                        case side:
                            dimPipe.enqueue({temp[0], yIndex, xIndex, temp[3]});
                            break;
                        case top:
                            dimPipe.enqueue({xIndex, temp[1], yIndex, temp[3]});
                            break;
                    }
                    */

            }
        }

        if(reader.getSize() > 0){
            if(!reader.dequeue(image))
                goto closeWindow;
            texture.loadFromImage(image);
            sprite = sf::Sprite(texture);
        }

        std::array<int, 4> temp = {0};
        dimMutex.lock();
        for (int i = 0; i < 4; ++i) {
            temp[i] = currentDim[i];
        }
        dimMutex.unlock();

        int xIndicatorPos;
        int yIndicatorPos;
        switch (sliceType) {
            case front:
                xIndicatorPos = temp[0];
                yIndicatorPos = temp[1];
                break;
            case side:
                xIndicatorPos = temp[2];
                yIndicatorPos = temp[1];
                break;
            case top:
                xIndicatorPos = temp[0];
                yIndicatorPos = temp[2];
                break;
        }

        int xn, yn;
        if(!niftiData->getSizeOfSlice(sliceType, xn, yn))
            goto closeWindow;


        if(sliceType == side){
            std::swap(xn, yn);
        }

        xIndicatorPos = ((float) xIndicatorPos) / xn * width;
        yIndicatorPos = ((float) yIndicatorPos) / yn * height;


        xIndicator.setPosition(xIndicatorPos, 0);
        yIndicator.setPosition(0, yIndicatorPos);

        window.clear();
        window.draw(scrollBar);
        window.draw(scrollButton);
        window.draw(sprite);
        window.draw(xIndicator);
        window.draw(yIndicator);
        window.display();
    }
    closeWindow:
        window.close();
        *kill = true;
}

void MRIWindow::loadImage(const sf::Image& image) {
    if(!getKill())
        imagePipe.getWriter().enqueue(image);
}

void MRIWindow::join() {
    if(windowWorker.joinable())
        windowWorker.join();
}

bool MRIWindow::getKill() {
    return *kill;
}



MRIWindow::MRIWindow(SliceType sliceType, std::shared_ptr<niftiWrapper> niftiData, IPC<std::array<int, 4>>::Writer dimPipe) :
        sliceType(sliceType), niftiData(niftiData), dimPipe(dimPipe){
    if(niftiData->imageValid()){
        int x, y;
        niftiData->getSizeOfSlice(sliceType, x, y);

        std::string windowName;
        switch (sliceType) {
            case top:
                windowName = std::string("Axial view");
                break;
            case side:
                windowName = std::string("Sagittal view");
                break;
            case front:
                windowName = std::string("Coronal view");
        }

        windowWorker = std::thread(&MRIWindow::threadWorker, this, x, y,  windowName);
        loadImageCoorNoTime(0);
    }
}

void MRIWindow::loadImageCoorNoTime(int sliceNum) {
    loadImageCoor(sliceNum, 0);
}

void MRIWindow::loadImageCoor(int sliceNum, int timeIndex) {
    currentLoader++;
    int sliceNumMax;
    if(!niftiData->getNumOfSlice(sliceType, sliceNumMax)){
        currentLoader--;
        return;
    }
    if(sliceNumMax < sliceNum) {
        currentLoader--;
        return;
    }
    sf::Image image;
    if(niftiData->toImage(sliceNum, timeIndex, sliceType, image))
        loadImage(image);
    currentLoader--;
}

std::thread MRIWindow::loadImageCoor_t(int sliceNum, int timeIndex) {
    return std::thread(&MRIWindow::loadImageCoor, this, sliceNum, timeIndex);
}

MRIWindow::~MRIWindow() {
    *kill = true;
    while(currentLoader > 0)
        sleep(1);
    if(windowWorker.joinable())
        windowWorker.join();
}

std::thread MRIWindow::updateDim(std::array<int, 4> newDim) {
    dimMutex.lock();
    for (int i = 0; i < 4; ++i) {
        currentDim[i] = newDim[i];
    }
    dimMutex.unlock();
    switch (sliceType) {
        case front:
            return loadImageCoor_t(newDim[2], newDim[3]);
            break;
        case side:
            return loadImageCoor_t(newDim[0], newDim[3]);
            break;
        case top:
            return loadImageCoor_t(newDim[1], newDim[3]);
            break;
    }
};