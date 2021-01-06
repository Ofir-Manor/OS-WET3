#ifndef _QUEUEL_H
#define _QUEUEL_H
#include "Headers.hpp"
#include "Semaphore.hpp"
// Single Producer - Multiple Consumer queue
template <typename T>class PCQueue
{
private:

    Semaphore sem;
    queue<T> pcQueue;
    int consumers_inside, producers_inside, producers_waiting;
    pthread_cond_t consume_allowed;
    pthread_cond_t produce_allowed;
    pthread_mutex_t lock;

    void producer_lock();
    void producer_unlock();
    void consumer_lock();
    void consumer_unlock();

    // Add your class memebers here
public:

    PCQueue();
    ~PCQueue();
	// Blocks while queue is empty. When queue holds items, allows for a single
	// thread to enter and remove an item from the front of the queue and return it. 
	// Assumes multiple consumers.
	T pop(); 

	// Allows for producer to enter with *minimal delay* and push items to back of the queue.
	// Hint for *minimal delay* - Allow the consumers to delay the producer as little as possible.  
	// Assumes single producer 
	void push(const T& item); 
};


/*********************************
*   Implementation starts here   *
**********************************/

template<typename T>
PCQueue<T>::PCQueue() {
    this->sem = Semaphore();
    this->pcQueue = queue<T>();
    this->consumers_inside=0;
    this->producers_inside=0;
    this->producers_waiting=0;

    pthread_mutex_init(&this->lock, nullptr);
    pthread_cond_init(&this->consume_allowed, nullptr);
    pthread_cond_init(&this->produce_allowed, nullptr);
}

template<typename T>
PCQueue<T>::~PCQueue() {
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

    this->sem.down();

    this->consumer_lock();

    T item = this->pcQueue.front(); /* queue's pop() only pops with no return value. front() returns element. */
    this->pcQueue.pop();
    this->consumer_unlock();

    return item;
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
    pthread_mutex_unlock(&this->lock); /* is unlock needed here? */
}

template<typename T>
void PCQueue<T>::producer_unlock() {
    pthread_mutex_lock(&this->lock); /* is lock needed here? */
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
    pthread_mutex_unlock(&this->lock); /* is unlock needed here? */
}

template<typename T>
void PCQueue<T>::consumer_unlock() {
    pthread_mutex_lock(&this->lock); /* is lock needed here? */
    this->consumers_inside--;
    if(this->consumers_inside == 0){
        pthread_cond_signal(&this->produce_allowed);
    }
    pthread_mutex_unlock(&this->lock);

}
// Recommendation: Use the implementation of the std::queue for this exercise
#endif