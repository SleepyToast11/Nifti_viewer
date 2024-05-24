//
// Created by jerome on 3/20/24.
//

#ifndef MRIASSIGNMENT_IPC_H
#define MRIASSIGNMENT_IPC_H


/*
#include "Pipe.h"
#include <list>
#include <mutex>
#include <condition_variable>



template<typename T>
class IPC{
    std::atomic<bool> kill;
public:
    IPC();
    void killAll(){
        kill = true;
        cond.notify_all();
    }
    template<typename Y>
    class Reader : public AbsReader<Y>{
    private:
        IPC<Y> *parent;
    public:

        explicit Reader(IPC *parent) : parent(parent){}
        Reader(const Reader& other) : parent(other.parent){}
        Reader(Reader&& other) noexcept : parent(other.parent) {
            other.parent = nullptr;
        }
        void killAll(){
            if(!parent->kill)
                parent->killAll();
        }
        Y dequeue()  {
            std::unique_lock lock(parent->mutex);
            parent->cond.wait(lock, [this]{return parent->data.size() > 0 || parent->kill;});
            if(parent->kill){
                return Y();
            }
            T ret = parent->data.back();
            parent->data.pop_back();
            return ret;
        }

        Reader& operator=(const Reader& other) {
            if (this != &other) {
                this->parent = other.parent;
            }
            return *this;
        }

        Reader& operator=(Reader&& other) noexcept {
            if (this != &other) {
                this->parent = other.parent;
                other.parent = nullptr;
            }
            return *this;
        }

        T check()  {
            std::unique_lock lock(parent->mutex);
            parent->cond.wait(lock, [this]{return parent->data.size() > 0 || parent->kill;});
            if(parent->kill){
                return Y();
            }
            return parent->data.back();
        }

        size_t getSize() {
            std::unique_lock lock(parent->mutex);
            return parent->data.size();
        }
    };

    template<typename Y>
    class Writer : public AbsWriter<Y>{
    private:
        IPC<Y> *parent;
    public:
        explicit Writer(IPC *parent) : parent(parent){}
        Writer(const Writer& other) : parent(other.parent) {}
        void killAll(){
            if(!parent->kill)
                parent->killAll();
        }
        Writer(Writer&& other) noexcept : parent(other.parent) {
            other.parent = nullptr;
        }
        Writer& operator=(const Writer& other) {
            if (this != &other) { // self-assignment check
                this->parent = other.parent;
            }
            return *this;
        }

        Writer& operator=(Writer&& other) noexcept {
            if (this != &other) { // self-assignment check
                this->parent = other.parent;
                other.parent = nullptr;
            }
            return *this;
        }


        void enqueue(T val)  {
            std::unique_lock lock(parent->mutex);
            parent->data.push_front(val);
            parent->cond.notify_one();
        }
        size_t getSize()  {
            std::unique_lock lock(parent->mutex);
            return parent->data.size();
        }
    };

    Writer<T> getWriter();
    Reader<T> getReader();
private:
    std::list<T> data;
    std::mutex mutex;
    std::condition_variable cond;
    Reader<T> reader;
    Writer<T> writer;
};
*/

#include <atomic>
#include <list>
#include <mutex>
#include <condition_variable>

template<typename T>
class IPC {
public:
    IPC() = default;

    void killAll(){
        kill = true;
        cond.notify_all();
    }

    class Reader{
    private:
        IPC<T>* parent;
    public:
        explicit Reader(IPC* parent) : parent(parent) {}

        Reader(const Reader& other) : parent(other.parent) {}

        Reader(Reader&& other) noexcept : parent(other.parent) {
            other.parent = nullptr;
        }

        Reader& operator=(const Reader& other) {
            if (this != &other) {
                this->parent = other.parent;
            }
            return *this;
        }

        Reader& operator=(Reader&& other) noexcept{
            if (this != &other) {
                this->parent = other.parent;
                other.parent = nullptr;
            }
            return *this;
        }

        void killAll() {
            if (!parent->kill)
                parent->killAll();
        }

        int dequeue(T& retVal) {
            std::unique_lock<std::mutex> lock(parent->mutex);
            parent->cond.wait(lock, [this] { return parent->data.size() > 0 || parent->kill; });
            if (parent->kill) {
                return 0;
            }
            retVal = parent->data.back();
            parent->data.pop_back();
            return 1;
        }

        int check(T& retVal) {
            std::unique_lock<std::mutex> lock(parent->mutex);
            parent->cond.wait(lock, [this] { return parent->data.size() > 0 || parent->kill; });
            if (parent->kill) {
                return 0;
            }
            retVal = parent->data.back();
            return 1;
        }

        size_t getSize() {
            std::unique_lock<std::mutex> lock(parent->mutex);
            return parent->data.size();
        }
    };

    class Writer{
    private:
        IPC<T>* parent;
    public:
        explicit Writer(IPC* parent) : parent(parent) {}

        Writer(const Writer& other) : parent(other.parent) {}

        void killAll(){
            if (!parent->kill)
                parent->killAll();
        }

        Writer(Writer&& other) noexcept{
            other.parent = nullptr;
        }

        Writer& operator=(const Writer& other){
            if (this != &other) { // self-assignment check
                this->parent = other.parent;
            }
            return *this;
        }

        Writer& operator=(Writer&& other) noexcept{
            if (this != &other) { // self-assignment check
                this->parent = other.parent;
                other.parent = nullptr;
            }
            return *this;
        }

        int enqueue(T val) {
            std::unique_lock<std::mutex> lock(parent->mutex);
            if(parent->kill)
                return 0;
            parent->data.push_front(val);
            parent->cond.notify_one();
            if(parent->kill)
                return 0;
            return 1;
        }

        size_t getSize() {
            std::unique_lock<std::mutex> lock(parent->mutex);
            return parent->data.size();}
    };
    Writer getWriter() { return Writer(this);}
    Reader getReader(){   return Reader(this);}
private:
    std::atomic<bool> kill = false;
    std::list<T> data;
    std::mutex mutex;
    std::condition_variable cond;
};


#endif //MRIASSIGNMENT_IPC_H
