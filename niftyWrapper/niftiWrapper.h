//
// Created by jsparnaay on 22/04/24.
//

#ifndef NIFTYCLASSTEST_NIFTIWRAPPER_H
#define NIFTYCLASSTEST_NIFTIWRAPPER_H

#include "nifti2_io.h"
#include "atomic"
#include <SFML/Graphics.hpp>
#include <memory>

enum SliceType{
    top, side, front
};

class niftiWrapper {
    nifti_image * image = NULL;
    unsigned char normalize(void *value, int index) const;
    size_t sizeOfDatatype() const;

public:
    niftiWrapper();
    niftiWrapper(niftiWrapper const &other);
    niftiWrapper(niftiWrapper &&other) noexcept ;

    niftiWrapper& operator=(niftiWrapper const &other);
    niftiWrapper& operator=(niftiWrapper &&other) noexcept;
    ~niftiWrapper();
    bool read(char *fname);
    bool imageValid() const;
    bool getNumOfSlice(SliceType type, int &retVal);
    bool toImage(int sliceNum, SliceType type, sf::Image &retVal);
    bool toImage(int sliceNum, int sliceTime, SliceType type, sf::Image &retVal);
    bool getSizeOfSlice(SliceType type, int &retWidth, int &retHeight);
    bool timeVals(std::array<int, 3> dim, std::vector<unsigned char> &retVals);
    bool getTimeDim(int &retVal);
};


#endif //NIFTYCLASSTEST_NIFTIWRAPPER_H
