#include "Semaphore.hpp"

Semaphore::Semaphore() {
    this->counter=0;
    pthread_mutex_init(&this->mutex, nullptr);
    pthread_cond_init(&this->cond, nullptr);
}

Semaphore::Semaphore(unsigned int val) {
    this->counter=val;
    pthread_mutex_init(&this->mutex, nullptr);
    pthread_cond_init(&this->cond, nullptr);
}

Semaphore::~Semaphore() {
    pthread_mutex_destroy(&this->mutex);
    pthread_cond_destroy(&this->cond);
}

void Semaphore::up() {
    pthread_mutex_lock(&this->mutex);
    this->counter++;
    pthread_cond_signal(&this->cond);
    pthread_mutex_unlock(&this->mutex);
}

void Semaphore::down() {
    pthread_mutex_lock(&this->mutex);
    while (this->counter == 0){
        pthread_cond_wait(&this->cond,&this->mutex);
    }
    this->counter--;
    pthread_mutex_unlock(&this->mutex);
}

