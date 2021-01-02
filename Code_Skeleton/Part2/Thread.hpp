#ifndef __THREAD_H
#define __THREAD_H

#include "../Part1/Headers.hpp"
#include "Task.hpp"
#include "../Part1/PCQueue.hpp"
class Thread
{
public:
	Thread(uint thread_id) 
	{
		this->thread_id = thread_id;
	} 
	virtual ~Thread() {} // Does nothing 

	/** Returns true if the thread was successfully started, false if there was an error starting the thread */
	bool start()
	{
	   return (pthread_create(&this->m_thread, nullptr, entry_func) == 0);
	}

	/** Will not return until the internal thread has exited. */
	void join()
	{
	    pthread_join(this->m_thread, nullptr);
	}

	/** Returns the thread_id **/
	uint thread_id()
	{
        return this->thread_id;
	}
protected:
	/** Implement this method in your subclass with the code you want your thread to run. */
	virtual void thread_workload() = 0;
	uint thread_id; // A number from 0 -> Number of threads initialized, providing a simple numbering for you to use

private:
	static void * entry_func(void * thread) { ((Thread *)thread)->thread_workload(); return NULL; }
	pthread_t m_thread;
};

class Tasked_thread : public Thread{

    Tasked_thread::Tasked_thread(uint thread_id): Thread(uint thread_id){
    }

    Tasked_thread::~Tasked_thread)(){
    }

    void thread_workload(PCQueue<Task> pcq) override {
        //perform from init to end
        while (1) {
            //take task out of pcqueue (if non then pcqueue should stop you
            Task t = pcq.pop();

            //take out relevant infromation out of task
            uint first_row = t.get_first_row();
            uint last_row = t.get_last_row();
            uint phase = t.get_phase();

            //perform the phase
            for (int i = first_row; i < last_row; ++i) {
                for (int j = 0; j < t.get_width(); ++j) {
                    if (phase == 1) {
                        //do what needs to be done in phase one from curr_matrix[i][j] to next_matrix[i][j]
                    }
                    if (phase == 2) {
                        //do what needs to be done in phase two from curr_matrix[i][j] to next_matrix[i][j]
                    }

                }

            }
        }
    }
};


#endif
