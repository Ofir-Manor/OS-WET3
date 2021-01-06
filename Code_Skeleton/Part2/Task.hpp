//
// Created by student on 12/31/20.
//

#ifndef TASK_HPP
#define TASK_HPP

#include "../Part1/Headers.hpp"

class Task
{
public:
    int_mat *curr_matrix;
    int_mat *next_matrix;
    uint first_row; //The first row the thread should deal with
    uint last_row; //The last row the thread should deal with
    uint max_height;
    uint max_width;  //The width of the row
    uint phase;  //the phase 1 for death or life 2 for color
    pthread_mutex_t *mutex;


    Task(int_mat *curr_matrix, int_mat *next_matrix, uint first_row, uint last_row, uint max_height, uint max_width, uint phase, pthread_mutex_t *mutex){
        this->curr_matrix = curr_matrix;
        this->next_matrix = next_matrix;
        this->first_row = first_row;
        this->last_row = last_row;
        this->max_height = max_height;
        this->max_width = max_width;
        this->phase = phase;
        this->mutex = mutex;
    }
    ~Task() = default;
};

#endif //TASK_HPP
