#include <malloc.h>
#include "bsq.h"

int is_valid_header(FILE *stream, t_map *m)
{
	if (fscanf(stream, "%d%c%c%c\n", &(m->map_i), &(m->empty), &(m->obst), &(m->full)) != 4)
		return 0;
	if (m->empty == m->obst || m->empty == m->full || m->obst == m->full || m->map_i < 1)
		return 0;

	// is good header, init the rest
	m->map_j = UNINITIALIZED;
	m->sol_a = UNINITIALIZED;
	m->sol_i = UNINITIALIZED;
	m->sol_j = UNINITIALIZED;
	m->arr2d = malloc(sizeof(char*) * m->map_i);
	if (!(m->arr2d))
	{
		fprintf(stdout, "Error: malloc fail");
		return 0;
	}
	return 1;
}

int square_fits(t_map *m, int pos_i, int pos_j, int a)
{
	if (pos_i + a > m->map_i || pos_j + a > m->map_j)
		return 0;

	for (int i = pos_i; i < pos_i + a; i++)
	{
		for (int j = pos_j; j < pos_j + a; j++)
		{
			if (m->arr2d[i][j] == m->obst)
				return 0;
		}
	}
	return 1;
}

void try_squares_for_coords(t_map *m, int pos_i, int pos_j)
{
	int a = 1;

	while (square_fits(m, pos_i, pos_j, a))
	{
		if (a > m->sol_a)
		{
			m->sol_a = a;
			m->sol_i = pos_i;
			m->sol_j = pos_j;
		}
		a++;
	}
}

void find_bsq(t_map *m)
{
	for (int i = 0; i < m->map_i; i++)
	{
		for (int j = 0; j < m->map_j; j++)
		{
			try_squares_for_coords(m, i, j);
		}
	}
}

int is_within_square(t_map *m, int i , int j)
{
	return (i >= m->sol_i && j >= m->sol_j && i < m->sol_i + m->sol_a && j < m->sol_j + m->sol_a);
}

void print_map(t_map *m)
{
	for (int i = 0; i < m->map_i; i++)
	{
		for (int j = 0; j < m->map_j; j++)
		{
			if (is_within_square(m, i, j))
				fprintf(stdout, "%c", m->full);
			else
				fprintf(stdout, "%c", m->arr2d[i][j]);
		}
		fprintf(stdout, "\n");
	}
}

void do_map(FILE *stream, t_map *m)
{
	char *line = NULL;
	size_t size = 0;

	// extract and validate map body: start
	for (int i = 0; i < m->map_i; i++) {
		int nread = getline(&line, &size, stream);
		if (m->map_j == UNINITIALIZED)
			m->map_j = nread - 1;  // (nread -1) bc '\n' retained
		else if (nread == -1 || (nread - 1) != m->map_j || line[nread - 1] != '\n')
		{
			fprintf(stdout, "Error: invalid map\n");
			return;
		}
		m->arr2d[i] = malloc(sizeof(char) * m->map_j);
		if (!(m->arr2d[i]))
		{
			fprintf(stdout, "Error: malloc fail\n");
			return;
		}
		for (int j = 0; j < m->map_j; j++)
		{
			if (!(line[j] == m->empty || line[j] == m->obst || 
				(j == m->map_j - 1 && line[j] == '\n')))
			{
				fprintf(stdout, "Error: invalid map\n");
				return;
			}
			m->arr2d[i][j] = line[j];
		}
		free(line);
		line = NULL;
		size = 0;
	}
	if (getline(&line, &size, stream) != -1)
	{
		fprintf(stdout, "Error: invalid map\n");
		return;
	}
	free(line);
	// extract and validate map body: end

	find_bsq(m);
	print_map(m);

	for (int i = 0; i < m->map_i; i++) {
		if(m->arr2d[i])
			free(m->arr2d[i]);
	}
	free(m->arr2d);
}

void do_file(FILE* stream)
{
	t_map m;

	if (is_valid_header(stream, &m))
		do_map(stream, &m);
	else
		fprintf(stdout, "Error: invalid map\n");
}


int main(int argc, char **argv)
{
	FILE *stream = NULL;

	if (argc > 2)
	{
		fprintf(stdout, "Error: too many arguments\n");
		return 1;
	}
	else if (argc == 2)
		stream = fopen(argv[1], "r");
	else
		stream = stdin;
	
	if (!stream)
		fprintf(stdout, "Error: fopen fail\n");
	else
		do_file(stream);
	fclose(stream); // mpeshko
}
