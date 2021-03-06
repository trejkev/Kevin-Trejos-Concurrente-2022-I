# Tetris concurrent solver using pthreads

Description: These scripts are intended to solve the issue of finding the best possible game when playing tetris. It takes an initial state,
a certain amount of figures, and places one by one,  when reached the amount of figures desired to place, a metric is computed, which basically
counts the amount of empty cells, and computes the metric as score = max_height - y_position_first_non-empty_cell.

The whole description of the problem to solve can be found on these repositories, the first for serial solution, and the second for concurrent solution:
1. Serial solution: https://github.com/jocan3/CI0117-2022-S1/tree/main/enunciados/tareas/01
2. Concurrent solution: https://github.com/jocan3/CI0117-2022-S1/tree/main/enunciados/tareas/02

## Running the code:

  1. Make sure you are using a computer with the following libraries up and working:
        a. stdbool.h
        b. stdio.h
        c. stdlib.h
        d. stdint.h
        e. string.h
  2. Open a terminal pointing to where you'd like to have this project. Then clone the repo there (git clone command).
  3. Execute make command
  4. Run ./bin/tetris_solver_serial
  5. Wait for the program to end.
  6. Check files into /test for the txt files of the best game.

## Blocks mapping scheme

With blocks mapping scheme, if there is a maximum of 10 threads (because there is a maximum of 10 columns to distribute the threads), the following distribution will happen.

| **Threads** | **Distribution of blocks (columns)**                                                                                                                                                                                                                                        |
|:-----------:|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|      1      | Thread 0: init 0, end 10                                                                                                                                                                                                                                                    |
|      2      | Thread 0: init 0, end 5<br>Thread 1: init 5, end 10                                                                                                                                                                                                                         |
|      3      | Thread 0: init 0, end 4<br>Thread 1: init 4, end 7<br>Thread 2: init 7, end 10                                                                                                                                                                                              |
|      4      | Thread 0: init 0, end 3<br>Thread 1: init 3, end 6<br>Thread 2: init 6, end 8<br>Thread 3: init 8, end 10                                                                                                                                                                   |
|      5      | Thread 0: init 0, end 2<br>Thread 1: init 2, end 4<br>Thread 2: init 4, end 6<br>Thread 3: init 6, end 8<br>Thread 4: init 8, end 10                                                                                                                                        |
|      6      | Thread 0: init 0, end 2<br>Thread 1: init 2, end 4<br>Thread 2: init 4, end 6<br>Thread 3: init 6, end 8<br>Thread 4: init 8, end 9<br>Thread 5: init 9, end 10                                                                                                             |
|      7      | Thread 0: init 0, end 2<br>Thread 1: init 2, end 4<br>Thread 2: init 4, end 6<br>Thread 3: init 6, end 7<br>Thread 4: init 7, end 8<br>Thread 5: init 8, end 9<br>Thread 6: init 9, end 10                                                                                  |
|      8      | Thread 0: init 0, end 2<br>Thread 1: init 2, end 4<br>Thread 2: init 4, end 5<br>Thread 3: init 5, end 6<br>Thread 4: init 6, end 7<br>Thread 5: init 7, end 8<br>Thread 6: init 8, end 9<br>Thread 7: init 9, end 10                                                       |
|      9      | Thread 0: init 0, end 2<br>Thread 1: init 2, end 3<br>Thread 2: init 3, end 4<br>Thread 3: init 4, end 5<br>Thread 4: init 5, end 6<br>Thread 5: init 6, end 7<br>Thread 6: init 7, end 8<br>Thread 7: init 8, end 9<br>Thread 8: init 9, end 10                            |
|      10     | Thread 0: init 0, end 1<br>Thread 1: init 1, end 2<br>Thread 2: init 2, end 3<br>Thread 3: init 3, end 4<br>Thread 4: init 4, end 5<br>Thread 5: init 5, end 6<br>Thread 6: init 6, end 7<br>Thread 7: init 7, end 8<br>Thread 8: init 8, end 9<br>Thread 9: init 9, end 10 |

## Results:

A total of 60 trials were executed, using the first 6 levels of depth, and computing the time spent when the amount of threads is varied from 1 (serial case) to 10 (maximum threads that can be used by the script), with only Visual Studio running (see file test/times_depth.csv for individual results). The results obtained are marvelous and will be explained below.

With the increased amount of threads NOT necessarily the elapsed time enhances. In this steady-state was because from 5 to 9, using block mapping, there is always at least one thread executing 2 columns, instead of 1 as the rest of the threads, reason why this thread leads the time that the program will late to complete all the tasks, this can be seen in the following figure, which shows the results for depth 5 (to make more evident the behavior), but varying the number of threads.

<p align="center">
<img width="600" src="https://user-images.githubusercontent.com/18760154/170848742-89b63d5a-1d57-4448-b6b1-be17762f7e5e.png">
</p>
<p align="center">Figure 1. Elapsed time versus threads for depth 5.</p>
  
To show that this behavior keeps up for all the different depths, see the figure 2. Here the effect is shadowed (and not evident for depth 0 because of its low time spent by each thread) by the evidence that every depth gives an offset to the elapsed time, where basically the higher the depth, the higher the elapsed time.

<p align="center">
<img width="600" src="https://user-images.githubusercontent.com/18760154/170848764-ccff2d09-98f0-407d-9366-df86c43b3b11.png">
</p>
<p align="center">Figure 2. Elapsed time versus threads keeping a fixed depth.</p>

In regards to the depth effect over the elapsed time, see the figure 3, where it can be seen that no matter the number of threads, the elapsed time will increase if the depth increases.

<p align="center">
<img width="600" src="https://user-images.githubusercontent.com/18760154/170848828-58ef807f-cbbf-4e1a-9bd6-429dc7deaedd.png">
</p>
<p align="center">Figure 3. Elapsed time versus depth keeping fixed threads amount.</p>
