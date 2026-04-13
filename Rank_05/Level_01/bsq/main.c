#include <stdio.h>
#include <stdlib.h>
#include "bsq.h"

void	parse_map(FILE *stream, t_map *m)
{
	m->grid = malloc(sizeof(char*) * m->n_cols);
	if (!(m->grid)) {
		fprintf(stdout, "Error: malloc fail");
		return ;
	}

	char *line = NULL;
	size_t size = 0;

	// extract and validate map body: start
	for (int i = 0; i < m->n_cols; i++) {
		int nread = getline(&line, &size, stream);
		if (m->n_rows == -1)
			m->n_rows = nread - 1;  // (nread -1) bc '\n' retained
		else if (nread == -1 || (nread - 1) != m->n_rows || line[nread - 1] != '\n')
		{
			fprintf(stdout, "Error: invalid map\n");
			// cleanup
			return;
		}
		m->grid[i] = malloc(sizeof(char) * m->n_rows);
		if (!(m->grid[i])) {
			fprintf(stdout, "Error: malloc fail\n");
			// cleanup
			return;
		}
		// check fo valid char set in map in a row
		for (int j = 0; j < m->n_rows; j++)
		{
			if (!(line[j] == m->empty || line[j] == m->obst || 
				(j == m->n_rows - 1 && line[j] == '\n'))) {
				fprintf(stdout, "Error: invalid map\n");
				// cleanup
				return;
			}
			m->grid[i][j] = line[j];
		}
	}
	// If we can still read another line, that's an error!
	// getline() returns -1 when it reaches end of file
	if (getline(&line, &size, stream) != -1) {
		fprintf(stdout, "Error: invalid map\n");
		free(line);
		// cleanup
		return;
	}
	free(line);

	// extract and validate map body: end

	/* find_bsq(m);
	print_map(m);
	// cleanup
	for (int i = 0; i < m->map_i; i++) {
		if(m->arr2d[i])
			free(m->arr2d[i]);
	}
	free(m->arr2d);
	*/
}

int is_valid_header(FILE *stream, t_map *m)
{
	if (fscanf(stream, "%d%c%c%c\n", &(m->n_cols), &(m->empty), 
		&(m->obst), &(m->full)) != 4)
		return 0;
	if (m->empty == m->obst || m->empty == m->full || 
			m->obst == m->full || m->n_cols < 1)
		return 0;

	// is good header, init the rest
	m->n_rows = -1;

	/* m->sol_a = -1;
	m->sol_i = -1;
	m->sol_j = -1; */
	
	return 1;
}

void	bsq(FILE *stream) {
	
	t_map m;

	if (is_valid_header(stream, &m)) {
		parse_map(stream, &m);
		// find_bsq
		// print map
	}
	else
		fprintf(stdout, "Error: invalid map\n");

}

int main(int argc, char **argv)
{
	FILE *stream = NULL;
	int should_close = 0; 

	if (argc > 2)
	{
		fprintf(stdout, "Error: too many arguments\n");
		return 1;
	}
	else if (argc == 2) {
		stream = fopen(argv[1], "r");
		should_close = 1;  // We opened it, we should close it
	} else {
		stream = stdin;
		should_close = 0;
	}
	if (!stream)
		fprintf(stdout, "Error: fopen fail\n");
	else
		bsq(stream);
	if (should_close)
		fclose(stream);
	return 0;
}
