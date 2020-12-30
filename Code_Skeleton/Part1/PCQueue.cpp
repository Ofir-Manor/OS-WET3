#include "PCQueue.hpp"
#include "Semaphore.hpp"
#include "Headers.hpp"

template<typename T>
PCQueue<T>::PCQueue() {
    this->sem = new Semaphore();
    this->pcQueue = new queue<T>;
    this->consumers_inside=0;
    this->producers_inside=0;
    this->producers_waiting=0;

    pthread_mutex_init(&this->lock, nullptr);
    pthread_cond_init(&this->consume_allowed, nullptr);
    pthread_cond_init(&this->produce_allowed, nullptr);
}

template<typename T>
PCQueue<T>::~PCQueue() {
    delete this->sem;
    delete this->pcQueue;
    pthread_mutex_destroy(&this->lock);
    pthread_cond_destroy(&this->consume_allowed);
    pthread_cond_destroy(&this->produce_allowed);
}

template<typename T>
void PCQueue<T>::push(const T &item) {
    this->producer_lock();
    this->pcQueue.push(item);
    this->producer_unlock();
    this->sem.up();
}

template<typename T>
T PCQueue<T>::pop(){
    this->sem.down()
    this->consumer_lock();
    T item = this->pcQueue.pop();
    this->consumer_unlock();
    return T;
}

template<typename T>
void PCQueue<T>::producer_lock() {
    pthread_mutex_lock(&this->lock);
    this->producers_waiting++;
    while (producers_inside + consumers_inside >0){
        pthread_cond_wait(&this->produce_allowed, &this->lock);
    }
    this->producers_waiting--;
    this->producers_inside++;
    pthread_mutex_unlock(&this->lock);
}

template<typename T>
void PCQueue<T>::producer_unlock() {
    pthread_mutex_lock(&this->lock);
    this->producers_inside--;
    pthread_cond_signal(&produce_allowed);
    pthread_cond_broadcast(&consume_allowed);
    pthread_mutex_unlock(&this->lock);
}

template<typename T>
void PCQueue<T>::consumer_lock() {
    pthread_mutex_lock(&this->lock);
    while (this->producers_inside >0 || this->producers_waiting >0){
        pthread_cond_wait(&this->consume_allowed, &this->lock);
    }
    this->consumers_inside++;
    pthread_mutex_unlock(&this->lock);
}

template<typename T>
void PCQueue<T>::consumer_unlock() {
    pthread_mutex_lock(&this->lock);
    this->consumers_inside--;
    if(this->consumers_inside == 0){
        pthread_cond_signal(&this->produce_allowed);
    }
    pthread_mutex_unlock(&this->produce_allowed);

}