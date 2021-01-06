 #include "Game.hpp"


static const char *colors[7] = {BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN};
/*--------------------------------------------------------------------------------

--------------------------------------------------------------------------------*/
Game::Game(game_params p) {
    //saves all relevant information from game params

    this->m_gen_num = p.n_gen;
    this->m_thread_num = p.n_thread;
    this->print_on = p.print_on;
    this->interactive_on = p.interactive_on;
    this->filename = p.filename;

    this->m_gen_hist =new vector<float>;
    this->m_tile_hist = new vector<float>;
    this->m_threadpool = new vector<Thread*>;

    this->task_queue = new PCQueue<Task>();
}

 void Game::run() {

     _init_game(); // Starts the threads and all other variables you need
     print_board("Initial Board");
     for (uint i = 0; i < m_gen_num; ++i) {
         auto gen_start = std::chrono::system_clock::now();
         _step(i); // Iterates a single generation
         auto gen_end = std::chrono::system_clock::now();
         m_gen_hist.push_back((double)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
         print_board(nullptr);
     } // generation loop
     print_board("Final Board");
     _destroy_game();
 }

void Game::_init_game() {

    //TODO maybe combine creation and start (only one for loop)

    // Create threads
    for (uint i = 0; i < this->m_thread_num; ++i) {
        this->m_threadpool.push_back(new Tasked_thread(i, this->task_queue, this->num_of_finished_tasks, m_tile_hist));
    }
    //Create Game Fields
    char delimiter = ' '; // the delimiter is space
    this->curr_matrix = new int_mat(utils::read_file(this->filename, delimiter));
    this->next_matrix = new int_mat(utils::read_file(this->filename, delimiter));

    //Updates fields after create game fields
    this->matrix_height = curr_matrix.size();
    this->matrix_width = curr_matrix[0].size();
    this->m_thread_num = std::min(this->m_thread_num, this->matrix_height);

	// Start the threads
    for (uint i = 0; i < this->m_thread_num; ++i) {
        this->m_threadpool[i]->start();
    }

    // create task queue
    this->task_queue = PCQueue<Task>();

    // create and initialize counter of finished tasks
    this->num_of_finished_tasks = new int;
    *this->num_of_finished_tasks = 0;

	// Testing of your implementation will presume all threads are started here

}

void Game::_step(uint curr_gen) {

    // Push jobs to queue: PCQ will hold tasks which will be functions to perform. Each function will get a class with all the needed information
    uint thread_portion = this->matrix_height/this->m_thread_num;

    //phase 1

    //reset finished tasks counter
    *this->num_of_finished_tasks = 0;

    //add the tasks to PCQueue (Threads should automatically receive tasks and start working)
    for (uint i = 0; i < this->matrix_height; i += thread_portion){

        //add the remainder rows to the last thread
        uint last_row = i+thread_portion;
        if (last_row + thread_portion > this->matrix_height)
        {
            last_row = this->matrix_height;
        }

        //create the new task
        Task t = Task(this->curr_matrix, this->next_matrix, i, last_row, this->matrix_height, this->matrix_width, 1);
        //put the task in the queue
        this->task_queue.push(t);
    }

	// Wait for the workers to finish calculating phase 1
    while(*this->num_of_finished_tasks < this->m_thread_num){};

    // Swap pointers between current and next field
    this->curr_matrix = this->next_matrix;

    //phase 2

    //reset finished tasks counter
    *this->num_of_finished_tasks = 0;

    //add the tasks to PCQueue (Threads should automatically receive tasks and start working)
    for (uint i = 0; i < this->matrix_height; i += thread_portion){

        //add the remainder rows to the last thread
        uint last_row = i+thread_portion;
        if (last_row + thread_portion > this->matrix_height)
        {
            last_row = this->matrix_height;
        }

        //create the new task
        Task t = Task(this->curr_matrix, this->next_matrix, i, last_row, this->matrix_height, this->matrix_width, 2);
        //put the task in the queue
        this->task_queue.push(t);
    }

    // Wait for the workers to finish calculating phase 2
    while(*this->num_of_finished_tasks < this->m_thread_num){};

    // Swap pointers between current and next field
    this->curr_matrix = this->next_matrix;

}

void Game::_destroy_game(){
	// Destroys board and frees all threads and resources 
	// Not implemented in the Game's destructor for testing purposes. 
	// Testing of your implementation will presume all threads are joined here
	delete this->curr_matrix;
	delete this->next_matrix;
	delete this->num_of_finished_tasks;

	//delete threads
}

//TODO Add all the other methods

const vector<float> Game::gen_hist() const {
    return this->m_gen_hist;
}

const vector<float> Game::tile_hist() const {
    return this->m_tile_hist;
}

uint Game::thread_num() const {
    return this->m_thread_num;
}
/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
 inline void Game::print_board(const char* header) {

     if(print_on){

         // Clear the screen, to create a running animation
         if(interactive_on)
             system("clear");

         // Print small header if needed
         if (header != nullptr)
             cout << "<------------" << header << "------------>" << endl;

         // TODO: Print the board

         // Display for GEN_SLEEP_USEC micro-seconds on screen
         if(interactive_on)
             usleep(GEN_SLEEP_USEC);
     }

 }


/* Function sketch to use for printing the board. You will need to decide its placement and how exactly
	to bring in the field's parameters.

		cout << u8"╔" << string(u8"═") * field_width << u8"╗" << endl;
		for (uint i = 0; i < field_height ++i) {
			cout << u8"║";
			for (uint j = 0; j < field_width; ++j) {
                if (field[i][j] > 0)
                    cout << colors[field[i][j] % 7] << u8"█" << RESET;
                else
                    cout << u8"░";
			}
			cout << u8"║" << endl;
		}
		cout << u8"╚" << string(u8"═") * field_width << u8"╝" << endl;
*/


