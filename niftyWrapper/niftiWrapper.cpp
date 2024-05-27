#include "nifti2_io.h"

#include "niftiWrapper.h"

niftiWrapper::niftiWrapper() : image(NULL) {}

niftiWrapper::niftiWrapper(niftiWrapper &&other) noexcept {
    std::swap(image, other.image);
}

niftiWrapper::niftiWrapper(const niftiWrapper &other) {
    this->image = other.image;
}

niftiWrapper &niftiWrapper::operator=(const niftiWrapper &other) {
    if (this != &other)
        this->image = other.image;
    return *this;
}


niftiWrapper &niftiWrapper::operator=(niftiWrapper &&other) noexcept {
    if (this != &other) {
        std::swap(image, other.image);
    }
    return *this;
}

niftiWrapper::~niftiWrapper() {
    if(image != NULL)
        nifti_image_free(image);
}

unsigned char niftiWrapper::normalize(void *value, int index) const {
    if(image == NULL)
        return 0;

    unsigned char result = 0;

    bool *boolData;
    unsigned char *ucharData;
    short *shortData;
    int *intData;
    float *floatData;
    double *doubleData;
    unsigned short *ushortData;
    unsigned int *uintData;
    char *charData;

    switch(image->datatype) {
        case 1: // DT_BINARY
            boolData = (bool*) value;
            result = boolData[index] ? 255 : 0;
            break;
        case 2: // DT_UNSIGNED_CHAR
            ucharData = (unsigned char*) value;
            result = ucharData[index];
            break;
        case 4: // DT_INT16
            shortData = (short*) value;
            result = (((int) shortData[index])) / 128;
            break;
        case 8: // DT_INT32
            intData = (int*) value;
            result = ((long long) intData[index] + 2147483647LL / 2) / 2147483647LL;
            break;
        case 16: // DT_FLOAT32
            floatData = (float*) value;
            if(floatData[index] > 255)
                result = 255;
            else
                result = ((floatData[index]));
            break;
        case 64: // DT_FLOAT64
            doubleData = (double*) value;
            result = (doubleData[index] + 1) * 128.0;
            break;
        case 256: // DT_INT8
            charData = (char*) value;
            result = ((int) charData[index] + 127);
            break;
        case 512: // DT_UINT16
            ushortData = (unsigned short*) value;
            result = ushortData[index] / 65535.0;
            break;
        case 768: // DT_UINT32
            uintData = (unsigned int*) value;
            result = uintData[index] / 4294967295.0;
            break;
        default:
            printf("Unsupported datatype: %d\n", image->datatype);
            break;
    }

    return result;
}

size_t niftiWrapper::sizeOfDatatype() const{
    if(!imageValid())
        return 0;
    size_t bytes = 0;

    switch (image->datatype) {
        case 2: // DT_UNSIGNED_CHAR
            bytes = sizeof(unsigned char);
            break;
        case 4: // DT_INT16
            bytes = sizeof(int16_t);
            break;
        case 8: // DT_INT32
            bytes = sizeof(int32_t);
            break;
        case 16: // DT_FLOAT32
            bytes = sizeof(int32_t);
            break;
        case 64: // DT_FLOAT64
            bytes = sizeof(double);
            break;
        case 256: // DT_INT8
            bytes = sizeof(int8_t);
            break;
        case 512: // DT_UINT16
            bytes = sizeof(uint16_t);
            break;
        case 768: // DT_UINT32
            bytes = sizeof(uint32_t);
            break;
        default:
            printf("Unsupported datatype: %d\n", image->datatype);
            break;
    }
    return bytes;
}

bool niftiWrapper::read(char *fname) {
    if(imageValid())
        nifti_image_free(image);
    image = NULL;
    image = nifti_image_read(fname, 1);
    return imageValid();
}

bool niftiWrapper::toImage(int sliceNum, SliceType type, sf::Image &retVal) {
    return toImage(sliceNum, 0, type, retVal);
}

bool niftiWrapper::timeVals(std::array<int, 3> dim, std::vector<unsigned char> &retVals){
    if(!imageValid())
        return false;
    int64_t dims[8] = { 0,  dim[0], dim[1], dim[2], -1, -1, -1, -1 };

    int numberOfTime = image->dim[4];

    void *data = NULL;

    int64_t collapsedImageBytes = nifti_read_collapsed_image(image, dims, &data);

    if(!collapsedImageBytes){
        if(data != NULL)
            free(data);
        return false;
    }

    retVals = std::vector<unsigned char>(numberOfTime);

    for (int i = 0; i < collapsedImageBytes / sizeOfDatatype(); ++i) {
        retVals[i] = normalize(data, i);
    }
    return true;
}

bool niftiWrapper::toImage(int sliceNum, int sliceTime, SliceType type, sf::Image &imageRet) {
    if(!imageValid())
        return false;
    int64_t dims[8] = { 0,  -1, -1, -1, sliceTime, -1, -1, -1 };
    long imageWidth, imageHeight;
    switch (type) {
        case front:
            imageWidth = image->dim[1];
            imageHeight = image->dim[2];
            dims[3] = sliceNum;
            break;
        case top:
            imageWidth = image->dim[1];
            imageHeight = image->dim[3];
            dims[2] = sliceNum;
            break;
        case side:
            imageWidth = image->dim[2];
            imageHeight = image->dim[3];
            dims[1] = sliceNum;
            break;
    }
    void *data = NULL;

    int64_t collapsedImageBytes = nifti_read_collapsed_image(image, dims, &data);

    if(!collapsedImageBytes){
        if(data != NULL)
            free(data);
        return false;
    }

    imageRet.create(imageWidth, imageHeight);

    unsigned char normalizedCollapsedImage[imageWidth * imageHeight];
    int i;
    for (i = 0; i < collapsedImageBytes / sizeOfDatatype(); ++i) {
        normalizedCollapsedImage[i] = normalize(data, i);
    }

    free(data);

    for (unsigned int x = 0; x < imageWidth; ++x){
        for (unsigned int y = 0; y < imageHeight; ++y) {
            unsigned char value = normalizedCollapsedImage[y * imageWidth + x];
            sf::Color color(value, value, value);
            imageRet.setPixel(x, y, color);
        }
    }
    return true;
}

bool niftiWrapper::getNumOfSlice(SliceType type, int &retVal){
    if(!imageValid())
        return false;

    switch (type) {
        case front:
            retVal = image->dim[3];
            break;
        case top:
            retVal = image->dim[2];
            break;
        case side:
            retVal = image->dim[1];
            break;
    }
    return true;
}

bool niftiWrapper::getSizeOfSlice(SliceType type, int &retWidth, int &retHeight){
    if(!imageValid())
        return false;

    switch (type) {
        case front:
            retWidth = image->dim[1];
            retHeight = image->dim[2];
            break;
        case top:
            retWidth = image->dim[1];
            retHeight = image->dim[3];
            break;
        case side:
            retWidth = image->dim[2];
            retHeight = image->dim[3];
            break;
    }
    return true;
}


bool niftiWrapper::imageValid() const{

    return image != NULL;
}

bool niftiWrapper::getTimeDim(int &retVal) {
    if(!imageValid())
        return false;
    retVal = image->dim[4];
    return true;
}








