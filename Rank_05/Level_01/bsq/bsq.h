#ifndef BSQ_H
# define BSQ_H

typedef struct {
	
    int n_cols; // vertical (y)
	int n_rows; // horizontal (x)
    char **grid;
	
	char empty;
    char obst;
    char full;

} t_map;

#endif