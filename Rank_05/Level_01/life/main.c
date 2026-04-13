#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int	main(int argc, char *argv[]) {

	if(argc != 4)
		return 1;

	int width = atoi(argv[1]);
	int height = atoi(argv[2]);
	int iterations = atoi(argv[3]);

	char grid[height][width];
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++)
			grid[i][j] = 0;
	}

	char buf;
	ssize_t bytes_read;
	int x = 0, y = 0, pen = 0;
	// Nested Condition
	while((bytes_read = read(0, &buf, 1)) != 0) {
		if (bytes_read == -1) {
			return 1; }
		if (buf == 'w') { 
			if (y > 0) y--;
		} else if (buf == 's') {
			if (y < height - 1) y++;
		} else if (buf == 'a') {
			if (x > 0) x--;
		} else if (buf == 'd') {
			if (x < width - 1) x++;
		} else if (buf == 'x') {
			pen = !pen;
		}
		if(pen)
			grid[y][x] = 1;
	}

	// void Update
	for (int iter = 0; iter < iterations; iter++) {
		char grid_new[height][width];
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++)
			grid_new[i][j] = 0;
		}
		
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				int live_nei = 0;
				if (i > 0 && grid[i - 1][j] == 1)
					live_nei++;
				if (j > 0 && grid[i][j - 1] == 1)
					live_nei++;
				if (i > 0 && j > 0 && grid[i - 1][j - 1] == 1)
					live_nei++;
				if (i < height -1 && grid[i + 1][j] == 1)
					live_nei++;
				if (j < width -1 && grid[i][j + 1] == 1)
					live_nei++;
				if (i < height -1 && j < width - 1 && grid[i + 1][j + 1] == 1)
					live_nei++;
				if (i < height -1 && j > 0 && grid[i + 1][j - 1] == 1)
					live_nei++;
				if (i > 0 && j < width -1 && grid[i - 1][j + 1] == 1)
					live_nei++;
				// Any dead cell with three live neighbours becomes a live cell.
				if (grid[i][j] == 0 && live_nei == 3)
					grid_new[i][j] = 1;
				// Any live cell with two or three live neighbours survives.
				else if (grid[i][j] == 1 && (live_nei == 3 || live_nei == 2))
					grid_new[i][j] = 1;
				else 
					grid_new[i][j] = 0;
			}
		}
		
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++)
			grid[i][j] = grid_new[i][j];
		}
	}

	// print out
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (grid[i][j] == 0)
				putchar(' ');
			else if (grid[i][j] == 1)
				putchar('O');
		}	
		putchar('\n');
	}
	return 0;
}
