#include "IPC.h"
#include <mutex>

/*
template<typename T>
IPC<T>::IPC() : kill(false), reader(this), writer(this) {}

template<typename T>
void IPC<T>::killAll() {
    kill = true;
    cond.notify_all();
}

template<typename T>
IPC<T>::Reader::Reader(IPC* parent) : parent(parent) {}

template<typename T>
IPC<T>::Reader::Reader(const Reader& other) : parent(other.parent) {}

template<typename T>

IPC<T>::Reader::Reader(Reader&& other) noexcept : parent(other.parent) {
    other.parent = nullptr;
}

template<typename T>

void IPC<T>::Reader::killAll() {
    if (!parent->kill)
        parent->killAll();
}

template<typename T>
T IPC<T>::Reader::dequeue() {
    std::unique_lock<std::mutex> lock(parent->mutex);
    parent->cond.wait(lock, [this] { return parent->data.size() > 0 || parent->kill; });
    if (parent->kill) {
        return T();
    }
    T ret = parent->data.back();
    parent->data.pop_back();
    return ret;
}

template<typename T>

IPC<T>::Reader& IPC<T>::Reader::operator=(const Reader& other) {
    if (this != &other) {
        this->parent = other.parent;
    }
    return *this;
}

template<typename T>

typename IPC<T>::Reader& IPC<T>::Reader::operator=(Reader&& other) noexcept {
    if (this != &other) {
        this->parent = other.parent;
        other.parent = nullptr;
    }
    return *this;
}

template<typename T>

T IPC<T>::Reader::check() {
    std::unique_lock<std::mutex> lock(parent->mutex);
    parent->cond.wait(lock, [this] { return parent->data.size() > 0 || parent->kill; });
    if (parent->kill) {
        return T();
    }
    return parent->data.back();
}

template<typename T>

size_t IPC<T>::Reader::getSize() {
    std::unique_lock<std::mutex> lock(parent->mutex);
    return parent->data.size();
}

template<typename T>

IPC<T>::Writer::Writer(IPC* parent) : parent(parent) {}

template<typename T>

IPC<T>::Writer::Writer(const Writer& other) : parent(other.parent) {}

template<typename T>

void IPC<T>::Writer::killAll() {
    if (!parent->kill)
        parent->killAll();
}

template<typename T>

IPC<T>::Writer::Writer(Writer&& other) noexcept : parent(other.parent) {
    other.parent = nullptr;
}

template<typename T>

typename IPC<T>::Writer& IPC<T>::Writer::operator=(const Writer& other) {
    if (this != &other) { // self-assignment check
        this->parent = other.parent;
    }
    return *this;
}

template<typename T>

typename IPC<T>::Writer& IPC<T>::Writer::operator=(Writer&& other) noexcept {
    if (this != &other) { // self-assignment check
        this->parent = other.parent;
        other.parent = nullptr;
    }
    return *this;
}

template<typename T>

void IPC<T>::Writer::enqueue(T val) {
    std::unique_lock<std::mutex> lock(parent->mutex);
    parent->data.push_front(val);
    parent->cond.notify_one();
}

template<typename T>

size_t IPC<T>::Writer::getSize() {
    std::unique_lock<std::mutex> lock(parent->mutex);
    return parent->data.size();
}


template<typename T>
typename IPC<T>::Writer IPC<T>::getWriter() {
    return writer;
}

template<typename T>
typename IPC<T>::Reader IPC<T>::getReader() {
    return reader;
}


*/