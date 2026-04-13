#ifndef BSQ_H
# define BSQ_H

# define UNINITIALIZED -1

typedef struct {
	int map_i;	// vertical (y) map size
	int map_j;	// horizontal (x) map size
	char **arr2d;  // map data from file, w/o header, treated as const, solution bsq not written in it

	char empty;
	char obst;
	char full;

	int sol_a;	// square size (side a) of the solution square
	int sol_i;  // y coordinate, top-left corner as starting point of the solution square
	int sol_j;  // x coordinate, top-left corner as starting point of the solution square
} t_map;

#endif
