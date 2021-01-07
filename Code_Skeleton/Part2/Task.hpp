//
// Created by student on 12/31/20.
//

#ifndef TASK_HPP
#define TASK_HPP

#include "../Part1/Headers.hpp"
#include "../Part1/Semaphore.hpp"

class Task
{
public:
    int_mat *curr_matrix;
    int_mat *next_matrix;
    int first_row; //The first row the thread should deal with
    int last_row; //The last row the thread should deal with
    int max_height;
    int max_width;  //The width of the row
    uint phase;  //the phase 1 for death or life 2 for color
    Semaphore *sync_sem;
    pthread_mutex_t *tile_hist_lock;



    Task(int_mat *curr_matrix, int_mat *next_matrix, int first_row, int last_row, int max_height, int max_width, uint phase, Semaphore *sync_sem, pthread_mutex_t *tile_hist_lock):
    curr_matrix(curr_matrix), next_matrix(next_matrix), first_row(first_row), last_row(last_row), max_height(max_height), max_width(max_width), phase(phase), sync_sem(sync_sem), tile_hist_lock(tile_hist_lock){
      /*  this->curr_matrix = curr_matrix;
        this->next_matrix = next_matrix;
        this->first_row = first_row;
        this->last_row = last_row;
        this->max_height = max_height;
        this->max_width = max_width;
        this->phase = phase;
        this->sync_sem = sync_sem;
        this->tile_hist_lock = tile_hist_lock;*/

    }
    ~Task() = default;
};

#endif //TASK_HPP
