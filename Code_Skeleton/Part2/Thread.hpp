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
        uint m_thread_id; // A number from 0 -> Number of threads initialized, providing a simple numbering for you to use
        PCQueue<Task*> *task_queue;
        vector<double> *m_tile_hist;
        pthread_mutex_t *uni_thread_lock;

    private:
        static void * entry_func(void * thread) {
            ((Thread *)thread)->thread_workload();
            return NULL;
        }
        pthread_t m_thread;


    public:
        Thread(uint thread_id, PCQueue<Task*> *task_queue, vector<double> *m_tile_hist, pthread_mutex_t * uni_thread_lock)
        {
            this->m_thread_id = thread_id;
            this->task_queue = task_queue;
            this->m_tile_hist = m_tile_hist;
            this->uni_thread_lock = uni_thread_lock;
        }
        virtual ~Thread() {} // Does nothing

        /** Returns true if the thread was successfully started, false if there was an error starting the thread */
        //TODO: Find out how to transfer the PCQueue
        bool start()
        {
           return (pthread_create(&(this->m_thread), nullptr, (this->entry_func), (void*) this) == 0);
        }

        /** Will not return until the internal thread has exited. */
        void join()
        {
            pthread_cancel(this->m_thread);
        }

        /** Returns the thread_id **/
        uint thread_id()
        {
            return this->m_thread_id;
        }

};

class Tasked_thread : public Thread{

    int_mat *curr_matrix;
    int_mat *next_matrix;
    int first_row; //The first row the thread should deal with
    int last_row; //The last row the thread should deal with
    int max_height;
    int max_width;  //The width of the row
    uint phase;  //the phase 1 for death or life 2 for color
    Semaphore *sync_sem;
    pthread_mutex_t *tile_hist_lock;

public:

    Tasked_thread(uint thread_id, PCQueue<Task*> *task_queue, vector<double> *m_tile_hist, pthread_mutex_t *uni_thread_lock):
    Thread(thread_id, task_queue, m_tile_hist, uni_thread_lock)
    {
    }

    ~Tasked_thread()
    {
    }

    void thread_workload() override {
        //perform from init to end
        while (1) {
            //take task out of pcqueue (if non then pcqueue should stop you)

            pthread_mutex_lock(this->uni_thread_lock);
            Task *temp = (*this->task_queue).pop();
            this->curr_matrix = temp->curr_matrix;
            this->next_matrix = temp->next_matrix;
            this->first_row = temp->first_row;
            this->last_row = temp->last_row;
            this->max_height = temp->max_height;
            this->max_width = temp->max_width;
            this->phase = temp->phase;
            this->sync_sem = temp->sync_sem;
            this->tile_hist_lock = temp->tile_hist_lock;
            pthread_mutex_unlock(this->uni_thread_lock);


            auto thread_start = std::chrono::system_clock::now();

            //take out relevant information out of task
            uint count;
            uint sum;
            uint neighb_cells[8];

            //foreach cell in threads matrix
            for (int i = this->first_row; i < this->last_row; ++i) {
                for (int j = 0; j < this->max_width; ++j) {

                    //Zero variables for each cell
                    for (int space = 0; space < 8; ++space) {
                        neighb_cells[space] = 0;
                    }

                    count =0;
                    sum =0;

                    //check the surrounding cells
                    for (int k = -1; k < 2; ++k)
                    {
                        for (int l = -1; l < 2; ++l)
                        {
                            //check if inside bounds
                            if ((i + k) < 0 || (i + k) >= this->max_height || (j + l) < 0 || (j + l) >= this->max_width) {
                                continue;
                            }
                            //add the cell and count

                            if (this->curr_matrix->operator[](i+k).operator[](j+l) > (uint)0) {
                                count++;
                                sum += (*this->curr_matrix)[i + k][j + l];
                                neighb_cells[(*this->curr_matrix)[i + k][j + l]]++;
                            }
                        }
                    }

                            //actions for phase 1
                            if(this->phase == 1){
                                //the cell is dead and it has three neighbors
                                if ((*this->curr_matrix)[i][j] == (uint)0 && count == 3)
                                {
                                    uint max = 0;
                                    uint dominant = 0;
                                    //find dominant cell color
                                    for (uint m = 0; m < 8; ++m) {
                                        if(neighb_cells[m] > max){
                                            max = neighb_cells[m];
                                            dominant = m;
                                        }
                                    }
                                    (*this->next_matrix)[i][j] = dominant;
                                }

                                //if the cell is alive and has two or three neighbors
                                //because we counted the current cell we add to count 1 in order to keep alive
                                else if ((*this->curr_matrix)[i][j] > 0 && (count == 4 || count == 3)){
                                    (*this->next_matrix)[i][j] = (*this->curr_matrix)[i][j];
                                }

                                //if non of the above
                                else {
                                    (*this->next_matrix)[i][j] = (uint) 0;
                                }
                            }

                            //actions for phase 2
                            else if (this->phase == 2){
                                //if the cell is alive, amke is the average of surrounding living cells
                                if ((*this->curr_matrix)[i][j] > 0){
                                    double avg = sum/count;
                                    uint rnd_avg = round(avg);
                                    (*this->next_matrix)[i][j] = rnd_avg;
                                }

                                //if the cell is dead then keep it dead
                                else{
                                    (*this->next_matrix)[i][j] = 0;
                                }
                            }
                }

            }

            auto thread_end = std::chrono::system_clock::now();
            pthread_mutex_lock(this->tile_hist_lock);
            (*this->m_tile_hist).push_back((double)std::chrono::duration_cast<std::chrono::microseconds>(thread_end - thread_start).count());
            pthread_mutex_unlock(this->tile_hist_lock);
            this->sync_sem->up();
            delete temp;
        }
    }
};


#endif
