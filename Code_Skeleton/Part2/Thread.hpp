#ifndef __THREAD_H
#define __THREAD_H

#include "../Part1/Headers.hpp"
#include "Task.hpp"
#include "../Part1/PCQueue.hpp"
class Thread
{
    protected:
        /** Implement this method in your subclass with the code you want your thread to run. */
        virtual void thread_workload() = 0;
        uint thread_id; // A number from 0 -> Number of threads initialized, providing a simple numbering for you to use
        PCQueue<Task> task_queue;
        int *num_of_finished_tasks;
        vector<float> m_tile_hist;

    private:
        static void * entry_func(void * thread) {
            ((Thread *)thread)->thread_workload();
            return NULL;
        }
        pthread_t m_thread;


    public:
        Thread(uint thread_id, PCQueue<Task> task_queue, int *num_of_finished_tasks, vector<float> m_tile_hist)
        {
            this->thread_id = thread_id;
            this->task_queue = task_queue;
            this->num_of_finished_tasks = num_of_finished_tasks;
            this->m_tile_hist = m_tile_hist;
        }
        virtual ~Thread() {} // Does nothing

        /** Returns true if the thread was successfully started, false if there was an error starting the thread */
        //TODO: Find out how to transfer the PCQueue
        bool start()
        {
           return (pthread_create(&this->m_thread, nullptr, entry_func, nullptr) == 0);
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

};

class Tasked_thread : public Thread{

public:

    Tasked_thread(uint thread_id, PCQueue<Task> task_queue, int *num_of_finished_tasks, vector<float> m_tile_hist):
    Thread(thread_id, task_queue, num_of_finished_tasks, m_tile_hist){
    }

    ~Tasked_thread(){
    }

    void thread_workload() override {
        //perform from init to end
        while (1) {
            //take task out of pcqueue (if non then pcqueue should stop you)
            Task t = this->task_queue.pop();


            auto thread_start = std::chrono::system_clock::now();

            //take out relevant information out of task
            uint count;
            uint sum;
            uint arr[8] = {0};

            //foreach cell in threads matrix
            for (int i = t.first_row; i < t.last_row; ++i) {
                for (int j = 0; j < t.max_width; ++j) {

                    count =0;
                    sum =0;
                    //check the surrounding cells
                    for (int k = -1; k < 2; ++k) {
                        for (int l = -1; l < 2; ++l) {

                            //check if inside bounds
                            if ( (i + k) < 0 || (i + k) >= t.max_height || (j + l) < 0 || (j+l) >= t.max_width){
                                continue;
                            }

                            //add the cell and count
                            if(t.curr_matrix[i+k][j+l] > 0) {
                                count++;
                                sum += t.curr_matrix[i + k][j + l];
                                arr[t.curr_matrix[i+k][j+l]] ++;
                            }

                            //actions for phase 1
                            if(t->phase == 1){
                                //the cell is dead and it has three neighbors
                                if (t.curr_matrix[i][j] == 0 && count == 3)
                                {
                                    uint max = 0;
                                    uint dominant = 0;
                                    //find dominant cell color
                                    for (int m = 0; m < 8; ++m) {
                                        if(arr[m] > max){
                                            max = arr[m];
                                            dominant = m;
                                        }
                                    }

                                    t.next_matrix[i][j] = dominant;
                                }

                                //if the cell is alive and has two or three neighbors
                                //because we counted the current cell we add to count 1 in order to keep alive
                                else if (t.curr_matrix[i][j] > 0 && (count == 4 || count == 3)){
                                    t.next_matrix[i][j] = t.curr_matrix[i][j];
                                }

                                //if non of the above
                                else {
                                    t.next_matrix[i][j] = 0;
                                }
                            }

                            //actions for phase 2
                            else if (t.phase == 2){

                                //if the cell is alive, amke is the average of surrounding living cells
                                if (t.curr_matrix[i][j] > 0){
                                    t.next_matrix[i][j] = sum/count;
                                }

                                //if the cell is dead then keep it dead
                                else{
                                    t.next_matrix[i][j] = 0;
                                }
                            }

                        }
                    }

                }

            }
            //task is completed
            *num_of_finished_tasks++;

            //TODO maybe its supposed to be (time for phase1 + time for phase2)?
            auto thread_end = std::chrono::system_clock::now();
            m_tile_hist.push_back((float)std::chrono::duration_cast<std::chrono::microseconds>(thread_end - thread_start).count());
        }
    }
};


#endif
