#include "Semaphore.hpp"

Semaphore::Semaphore() {
    this->blocker=0;
    pthread_mutex_init(this->mutex, nullptr);
    pthread_cond_init(this->cond, nullptr);
}

Semaphore::Semaphore(unsigned int val) {
    this->blocker=val;
    pthread_mutex_init(this->mutex, nullptr);
    pthread_cond_init(this->cond, nullptr);
}

void Semaphore::up() {
    pthread_mutex_lock(this->mutex);
    this->blocker++;
    pthread_cond_signal(this->cond);
    pthread_mutex_unlock(this->mutex);
}

void Semaphore::down() {
    pthread_mutex_lock(this->mutex);
    this->blocker--;
    while (this.blocker <= 0){
        pthread_cond_wait(this->cond,this->mutex);
    }
    pthread_mutex_unlock(this->mutex);
}

