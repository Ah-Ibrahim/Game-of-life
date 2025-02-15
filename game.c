#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define GRID_SIZE 20
#define NUM_THREADS 4
#define GENERATIONS 32

int grid[GRID_SIZE][GRID_SIZE];
int gridImg[GRID_SIZE][GRID_SIZE];
pthread_barrier_t barrier;
int current_generation_number = 1;

typedef struct
{
	int start_row;
	bool is_controller;
} thread_data;

typedef struct
{
	int x;
	int y;
} coordinates;

void print_grid()
{
	system("clear");
	for (int i = 0; i < GRID_SIZE; i++)
	{
		for (int j = 0; j < GRID_SIZE; j++)
		{
			if (grid[i][j] == 1)
			{
				printf("# ");
			}
			else
			{
				printf("  ");
			}
		}
		printf("\n");
	}

	printf("\033[32mCurrent Generation Number:\033[0m %d\n", current_generation_number);
	usleep(500000);
}

void update_gridImg()
{
	for (int i = 0; i < GRID_SIZE; i++)
	{
		for (int j = 0; j < GRID_SIZE; j++)
		{
			gridImg[i][j] = grid[i][j];
		}
	}
}

int no_of_live_neighbours(coordinates cell_coordinates)
{
	int counter = 0;
	coordinates left_corner_coordinates = {cell_coordinates.x - 1, cell_coordinates.y - 1};
	coordinates relative_coordinates = {0, 0}; // relative coordinates with respect to left corner

	for (int i = 0; i < 9; i++)
	{
		// do not count yourself
		if (i == 4)
		{
			continue;
		}

		coordinates neigbour_coordinates; // coordintates for neigbour

		relative_coordinates.x = i % 3;
		relative_coordinates.y = (int)(i / 3);

		neigbour_coordinates.x = left_corner_coordinates.x + relative_coordinates.x;
		neigbour_coordinates.y = left_corner_coordinates.y + relative_coordinates.y;

		// Check if neigbour coordinates is not inside grid
		// count as dead
		if (!(neigbour_coordinates.x >= 0 && neigbour_coordinates.x < GRID_SIZE && neigbour_coordinates.y >= 0 && neigbour_coordinates.y < GRID_SIZE))
		{
			continue;
		}

		counter += gridImg[neigbour_coordinates.x][neigbour_coordinates.y];
	}

	return counter;
}
// Function to compute next generation of Game of Life
void compute_next_gen(void *arg)
{
	thread_data *data = (thread_data *)arg;

	int start_row = data->start_row;
	int end_row = start_row + 4;

	// Compute
	for (int i = start_row; i <= end_row; i++)
	{
		for (int j = 0; j < GRID_SIZE; j++)
		{
			coordinates cell_coordinates = {i, j};
			int neighbours_count = no_of_live_neighbours(cell_coordinates);

			// Kill Rule
			if (gridImg[i][j] && (neighbours_count < 2 || neighbours_count > 3))
			{
				grid[i][j] = 0;
			} // Birth Rule
			else if (!gridImg[i][j] && neighbours_count == 3)
			{
				grid[i][j] = 1;
			}
		}
	}
}

void *run_thread(void *arg)
{
	thread_data *data = (thread_data *)arg;
	bool is_controller = data->is_controller;

	compute_next_gen(arg);

	// Wait for each other to compute
	pthread_barrier_wait(&barrier);

	if (is_controller)
	{
		print_grid();
		current_generation_number++;
		update_gridImg();
	}

	// Waiting again until controller print grid and update status
	pthread_barrier_wait(&barrier);

	while (current_generation_number <= GENERATIONS)
	{
		run_thread(arg);
	}

	return NULL;
}

// Number of live neighbours for a cell in 3 * 3 block
// Ex:
// 0 1 2
// 3 # 5
// 6 7 8

void initialize_grid(int grid[GRID_SIZE][GRID_SIZE])
{
	for (int i = 0; i < GRID_SIZE; i++)
	{
		for (int j = 0; j < GRID_SIZE; j++)
		{
			grid[i][j] = 0; // Set every cell to 0 (dead)
		}
	}
}
void initialize_patterns(int grid[GRID_SIZE][GRID_SIZE])
{

	initialize_grid(grid);

	// Initialize Still Life (Square) at top-left
	grid[1][1] = 1;
	grid[1][2] = 1;
	grid[2][1] = 1;
	grid[2][2] = 1;

	// Initialize Oscillator (Blinker) in the middle
	grid[5][6] = 1;
	grid[6][6] = 1;
	grid[7][6] = 1;

	// Initialize Spaceship (Glider) in the bottom-right
	grid[10][10] = 1;
	grid[11][11] = 1;
	grid[12][9] = 1;
	grid[12][10] = 1;
	grid[12][11] = 1;
}

int main()
{
	initialize_patterns(grid);
	update_gridImg();

	// Array containing threads ID
	pthread_t threads[NUM_THREADS];
	// Array containing required threads data
	thread_data *threads_data[NUM_THREADS];

	pthread_barrier_init(&barrier, NULL, NUM_THREADS);

	// Initialize each thread data, then create it
	for (int i = 0; i < NUM_THREADS; i++)
	{
		threads_data[i] = (thread_data *)malloc(sizeof(thread_data));

		threads_data[i]->start_row = i * 5;
		threads_data[i]->is_controller = false;

		// Making only first thread print is_controller
		if (!i)
			threads_data[i]->is_controller = true;

		// Creating thread
		pthread_create(&threads[i], NULL, run_thread, (void *)threads_data[i]);
	}

	// Waiting for all threads
	for (int i = 0; i < NUM_THREADS; i++)
	{
		pthread_join(threads[i], NULL);
	}

	pthread_barrier_destroy(&barrier);

	return 0;
}
