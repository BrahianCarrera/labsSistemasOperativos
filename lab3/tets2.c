#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

double **load_matrix(const char *filename, int *rows, int *cols)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error opening file");
        return NULL;
    }

    double **matrix = NULL;
    *rows = 0;
    *cols = 0;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1)
    {
        char *token;
        int current_col = 0;

        matrix = realloc(matrix, (*rows + 1) * sizeof(double *));
        matrix[*rows] = NULL;

        token = strtok(line, " \t\n");
        while (token)
        {
            matrix[*rows] = realloc(matrix[*rows], (current_col + 1) * sizeof(double));
            matrix[*rows][current_col] = strtod(token, NULL); // strtod es para double
            token = strtok(NULL, " \t\n");
            current_col++;
        }

        if (*cols == 0)
        {
            *cols = current_col;
        }
        else if (current_col != *cols)
        {
            fprintf(stderr, "Inconsistent number of columns in row %d\n", *rows);
            fclose(file);
            free(line);
            // Liberar la matriz ya asignada
            for (int i = 0; i <= *rows; i++)
            {
                free(matrix[i]);
            }
            free(matrix);
            return NULL;
        }

        (*rows)++;
    }

    free(line);
    fclose(file);
    return matrix;
}
void print_matrix(double **matrix, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            printf("%.10f ", matrix[i][j]);
            // esto está bien para double también
        }
        printf("\n");
    }
}

int main()
{
    int rows1, cols1;
    double **matrix1 = load_matrix("matriz1.txt", &rows1, &cols1);

    if (matrix1)
    {
        printf("Matrix 1:\n");
        print_matrix(matrix1, rows1, cols1);
    }

    return 0;
}