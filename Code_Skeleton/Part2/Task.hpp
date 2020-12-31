//
// Created by student on 12/31/20.
//

#ifndef TASK_HPP
#define TASK_HPP

class Task
{
    uint first_row; //The first row the thread should deal with
    uint last_row; //The last row the thread should deal with
    uint width;  //The width of the row
    uint phase;  //the phase 1 for death or life 2 for color

public:
    Task(uint first_row, uint last_row, uint width, uint phase){
        this->first_row = first_row;
        this->last_row = last_row;
        this->width = width;
        this->phase = phase;
    }
    ~Task() = default;

    uint get_first_row(){
        return this->first_row;
    }

    uint get_last_row(){
        return this->last_row;
    }

    uint get_phase(){
        return this->phase;
    }

    uint get_width(){
        return this->width;
    }
};

#endif //TASK_HPP
