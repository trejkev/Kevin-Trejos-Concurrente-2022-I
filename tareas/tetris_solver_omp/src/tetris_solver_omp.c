// Copyright 2022 Kevin Trejos Vargas <kevin.trejosvargas@ucr.ac.cr>
// Reading script based in Jose Andres Mena <jose.menaarias@ucr.ac.cr> code

#include <errno.h>
#include <pthread.h>
#include <omp.h>
#include <unistd.h>
#include <math.h>
#include "omp_solver_methods.h"


////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** arg) {
    // -- Set the amount of threads to be used
    size_t thread_qty = sysconf(_SC_NPROCESSORS_ONLN);
    if (argc == 2) {
        if (sscanf(arg[1], "%zu", &thread_qty) != 1 || errno) {
            fprintf(stderr, "Invalid number of threads.\n");
            return EXIT_FAILURE;
        }
    }
    printf("DEBUG: Thread count is %zu\n", thread_qty);

    // -- Read the basegame file
    FILE* fptr = stdin;
    fptr = fopen("test/Input_Tetris.txt", "r");  // Open the file in read mode
    // -- Check if  file opened correctly
    if (!fptr) {
        fprintf(stderr, "Invalid file \n");
        return EXIT_FAILURE;
    } else {
        printf("DEBUG: File test/Input_Tetris.txt opened \n");
    }
    game_t *matrix = read_matrix(fptr);  // Saves the document into record
    // -- Check if matrix creation was successful
    if (!matrix) {
        fprintf(stderr, "Invalid file content.\n");
        return EXIT_FAILURE;
    }

    // -- Redefine max threads if greater than maximum needed threads
    if (thread_qty > (size_t)matrix->gamezone_num_cols) {
        thread_qty = matrix->gamezone_num_cols;
        printf("DEBUG: Thread reduced to its max value: %zu\n", thread_qty);
    }

    printf("\n\n");  // Prints slenderization

    // -- Shared data keeps best games for each thread
    game_t* best_game[thread_qty*(matrix->depth + 1) + matrix->depth];
    for (size_t i = 0;
        i < (thread_qty*(matrix->depth + 1) + matrix->depth); i++) {
        best_game[i] = (game_t*)calloc(1, sizeof(game_t));
        best_game[i]->gamezone =
            (char**)create_matrix_value(matrix->gamezone_num_rows,
            matrix->gamezone_num_cols + 1, sizeof(char));
        best_game[i]->figures =
            (char*)calloc(matrix->num_figures, sizeof(char));
    }
    shared_data_t* shared_data =
        (shared_data_t*)calloc(1, sizeof(shared_data_t));
    shared_data->bg_matrix = best_game;

    // -- Private data for each thread out of parallel section
    private_data_t* all_private_data = (private_data_t*)calloc(
        thread_qty, sizeof(private_data_t));

    double start_time = omp_get_wtime();

    // -- Concurrence begins here
    #pragma omp parallel num_threads(thread_qty) \
        shared(shared_data, matrix, thread_qty, all_private_data) \
        default(none)
        {
        private_data_t* private_data;
        private_data = (private_data_t*)calloc(1, sizeof(private_data_t));
        private_data->basegame = game_cloner(matrix);
        private_data->best_score = matrix->gamezone_num_rows;
        private_data->num_threads = thread_qty;
        private_data->thread_num = omp_get_thread_num();
        private_data->shared_data = shared_data;
        private_data->save_best_game =
            (bool*)calloc(matrix->depth + 1, sizeof(bool));

        run_threads(&private_data[0]);

        // Not critical because it is thread safe
        all_private_data[omp_get_thread_num()].best_score =
            private_data->best_score;
        all_private_data[omp_get_thread_num()].thread_num =
            private_data->thread_num;

        // -- Destroy private data
        destroy_matrix(private_data->basegame,
            matrix->gamezone_num_rows);
        free(private_data->save_best_game);
        free(private_data);
        printf("DEBUG: Destroyed private data for thread %i\n",
            omp_get_thread_num());
    }

    double finish_time = omp_get_wtime();

    double elapsed = finish_time - start_time;

    printf("DEBUG: Elapsed time is: %f s\n", elapsed);

    // -- Save times from this scenario
    FILE *fptr2;
    fptr2 = fopen("test/times_depth.csv", "a");
    fprintf(fptr2, "%zu;%i;%.9lf;\n", thread_qty, matrix->depth, elapsed);
    fclose(fptr2);

    // -- Get which thread has the best score
    size_t thread_with_bs = 0;  // Option to take will be always first col
    for (size_t thread = 0; thread < thread_qty; thread++) {
        int best_score = all_private_data[thread_with_bs].best_score;
        if (all_private_data[thread].best_score < best_score) {
            thread_with_bs = thread;
        }
    }
    printf("DEBUG: Thread with best score is %zu\n", thread_with_bs);
    printf("DEBUG: Best score is %i\n",
        all_private_data[thread_with_bs].best_score);

    // -- Print the basegame
    printf("\n\nDEBUG: Basegame\n");
    for (int row = 0; row < 10; row++) {
        printf("DEBUG: %i  %s\n", row, matrix->gamezone[row]);
    }
    for (int row = 10; row < matrix->gamezone_num_rows; row++) {
        printf("DEBUG: %i %s\n", row, matrix->gamezone[row]);
    }
    printf("\n\n");

    // -- Print the best games
    for (int depth = 0; depth <= matrix->depth; depth++) {
        printf("DEBUG: Best game level %i\n", depth);
        for (int row = 0; row < 10; row++) {
            printf("DEBUG: %i  %s\n", row, shared_data->
                bg_matrix[thread_with_bs*(matrix->depth+1) + depth]->
                gamezone[row]);
        }
        for (int row = 10; row < matrix->gamezone_num_rows; row++) {
            printf("DEBUG: %i %s\n", row, shared_data->
                bg_matrix[thread_with_bs*(matrix->depth+1) + depth]->
                gamezone[row]);
        }
        printf("\n\n");
    }

    // -- Saves the best games into files
    for (int level = 0; level <= matrix->depth; level++) {
        char game_result_path[24];
        snprintf(game_result_path, sizeof(game_result_path), "%s%d%s",
            "test/tetris_play_", level, ".txt");
        FILE *fptr3;
        fptr3 = fopen(game_result_path, "w");
        // Check if  file opened correctly
        if (!fptr3) {
            fprintf(stderr, "DEBUG: Invalid file \n");
            return EXIT_FAILURE;
        } else {
            printf("DEBUG: File %s opened\n", game_result_path);
        }
        printf("DEBUG: Saving game %i\n", level);
        int offset = thread_with_bs*(matrix->depth + 1);
        write_bestgame(fptr3, shared_data->bg_matrix[offset + level]);
        fclose(fptr3);
    }


    // -- Destroy shared data
    for (size_t i = 0;
        i < (thread_qty*(matrix->depth + 1) + matrix->depth); i++) {
        destroy_matrix(best_game[i], matrix->gamezone_num_rows);
    }
    free(shared_data);
    printf("\n\nDEBUG: Destroyed shared data\n");

    // -- Destroy private data out of parallel section
    free(all_private_data);
    printf("DEBUG: Destroyed private data out of parallel section\n");

    // -- Destroy initial game
    destroy_matrix(matrix, matrix->gamezone_num_rows);
    printf("DEBUG: Destroyed initial game state\n");

    return EXIT_SUCCESS;
}
