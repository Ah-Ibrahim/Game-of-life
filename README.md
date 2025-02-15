# Conway's Game of Life (Multithreaded Implementation)

## Overview

This project implements Conway's Game of Life using multithreading with POSIX threads (`pthreads`). The simulation runs on a 20x20 grid for 32 generations, utilizing 4 threads to compute the next state of the grid in parallel. A synchronization barrier ensures proper thread coordination.

## Features

- **Multithreaded Computation:** Uses 4 threads to update different sections of the grid concurrently.
- **Synchronization with Barriers:** Ensures correct updates using `pthread_barrier_t`.
- **Predefined Patterns:** Initializes the grid with Still Life, Oscillator, and Spaceship patterns.
- **Console Visualization:** Prints the grid in the terminal, refreshing every generation.

## Requirments

- POSIX-compliant system (Linux/macOS)
- GCC compiler
- `pthread` library
- Make (optional)

## Installation & Compilation

```sh
# Compile the shell
make   # If a Makefile is available
# Or use command
gcc -pthread -o game game.c

# Then run the shell
./shell
```

## Implementation Details

### Grid Representation

- The game board is represented by a `GRID_SIZE x GRID_SIZE` (20x20) integer array.
- `grid[i][j] = 1` represents a live cell, `grid[i][j] = 0` represents a dead cell.
- A temporary grid (`gridImg`) is used for computing the next state.

### Multithreading Logic

- Each thread computes the next state for a **5-row section** of the grid.
- `pthread_barrier_t` ensures that all threads complete their computation before updating `gridImg` and printing the new generation.
- The first thread (`is_controller = true`) prints the updated grid.

### Rules of Conway's Game of Life

For each cell:

1. **Survival:** A live cell with 2 or 3 live neighbors stays alive.
2. **Death:** A live cell with fewer than 2 or more than 3 live neighbors dies.
3. **Birth:** A dead cell with exactly 3 live neighbors becomes alive.

## Code Structure

- `initialize_grid()`: Initializes an empty grid.
- `initialize_patterns()`: Sets predefined patterns (Still Life, Oscillator, and Spaceship).
- `print_grid()`: Displays the grid with generation number.
- `update_gridImg()`: Copies `grid` to `gridImg`.
- `no_of_live_neighbours()`: Counts live neighbors for a given cell.
- `compute_next_gen()`: Computes the next state for a section of the grid.
- `run_thread()`: Thread function for computing and synchronizing updates.

## Sample Output

```
# #
# #

  #
  #
  #

  #
   #
 # # #

Current Generation Number: 1
```

(Updates every 0.5 seconds)

## Author

Developed by Ahmed Ibrahim. Contributions and feedback are welcome!
