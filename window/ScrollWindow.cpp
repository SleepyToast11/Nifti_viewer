//
// Created by jsparnaay on 22/04/24.
//

#include "ScrollWindow.h"

#define scrollBarHeight 10

void ScrollWindow::threadWorker(unsigned int sliceNum) {
    
    int width = sliceNum;
    int height = scrollBarHeight / 5;

    sf::RenderWindow window(sf::VideoMode(width, scrollBarHeight), "Time window");
    IPC<sf::Image>::Reader reader = imagePipe.getReader();
    window.setFramerateLimit(144);

    sf::Image image;

    image.create(width, scrollBarHeight*4/5);
    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite(texture);

    sf::RectangleShape scrollBar;
    sf::RectangleShape scrollButton;

    scrollBar.setSize(sf::Vector2f(width, height));
    scrollBar.setFillColor(sf::Color::Red);
    scrollBar.setPosition(0, scrollBarHeight*4/5);

    scrollButton.setSize(sf::Vector2f(width/sliceNum, height));
    scrollButton.setFillColor(sf::Color::Blue);
    scrollButton.setPosition(0, scrollBarHeight*4/5);

    while (window.isOpen() && !getKill()){
        sf::Event event{};
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed)
                goto closeWindow;
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (scrollBar.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {

                        scrollButton.setPosition(((float) event.mouseButton.x) / window.getSize().x * width,
                                                 scrollBarHeight * 4 / 5);

                        int newIndex = ((float) event.mouseButton.x) / window.getSize().x * sliceNum;

                        std::array<int, 4> temp = {0};
                        dimMutex.lock();
                        for (int i = 0; i < 4; ++i) {
                            temp[i] = currentDim[i];
                        }
                        dimMutex.unlock();

                        timeChanger.enqueue({temp[0], temp[1], temp[2], newIndex});
                    }
                }
            }
        }

        if(reader.getSize() > 0){
            if(!reader.dequeue(image))
                goto closeWindow;
            texture.loadFromImage(image);
            sprite = sf::Sprite(texture);
        }

        sprite.setPosition(0, 0);

        window.clear();
        window.draw(sprite);
        window.draw(scrollBar);
        window.draw(scrollButton);
        window.display();
    }
    closeWindow:
    window.close();
    *kill = true;
}

void ScrollWindow::join() {
    if(windowWorker.joinable())
        windowWorker.join();
}

bool ScrollWindow::getKill() {
    return *kill;
}

void ScrollWindow::loadImage(std::vector<unsigned char> colorAcrossTime) {
    sf::Image image;
    image.create(colorAcrossTime.size(), scrollBarHeight*4/5);

    for (int i = 0; i < image.getSize().x; ++i) {
        for (int j = 0; j < image.getSize().y; ++j) {
            unsigned char col = colorAcrossTime[i];
            sf::Color color(col,col,col); // RGB gradient
            image.setPixel(i, j, color);
        }
    }
    imagePipe.getWriter().enqueue(image);
}

void ScrollWindow::loadImageCoor(std::array<int, 4> dims) { //no need to threaded as it is not a long func
    sf::Image image;
    std::vector<unsigned char> timeVals;
    if(!niftiData->timeVals({dims[0], dims[1], dims[2]}, timeVals))
        return;

    std::array<int, 4> temp = {0};
    dimMutex.lock();
    for (int i = 0; i < 4; ++i) {
        currentDim[i] = dims[i];
    }
    dimMutex.unlock();

    loadImage(timeVals);
}

std::thread ScrollWindow::updateDims(std::array<int, 4> dims){
    return std::thread(&ScrollWindow::loadImageCoor, this, dims);
}

ScrollWindow::ScrollWindow( std::shared_ptr<niftiWrapper> niftiData, IPC<std::array<int, 4>>::Writer timeChanger) :
niftiData(niftiData) ,
timeChanger(timeChanger){
    int val;
    niftiData->getTimeDim(val);
    windowWorker = std::thread(&ScrollWindow::threadWorker, this, val);
    loadImageCoor({0,0,0});
}



ScrollWindow::~ScrollWindow() {
    *kill = true;
    if(windowWorker.joinable())
        windowWorker.join();
}