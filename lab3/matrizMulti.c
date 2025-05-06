#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
typedef long ssize_t;
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

float **load_matrix(const char *filename, int *rows, int *cols)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error opening file");
        return NULL;
    }

    float **matrix = NULL;
    *rows = 0;
    *cols = 0;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1)
    {
        char *token;
        int current_col = 0;

        matrix = realloc(matrix, (*rows + 1) * sizeof(float *));
        if (!matrix)
        {
            perror("Error reallocating matrix");
            fclose(file);
            free(line);
            return NULL;
        }

        matrix[*rows] = NULL;

        token = strtok(line, " \t\n");
        while (token)
        {
            matrix[*rows] = realloc(matrix[*rows], (current_col + 1) * sizeof(float));
            matrix[*rows][current_col] = strtof(token, NULL); // usar strtof en vez de atof
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
            for (int i = 0; i <= *rows; i++)
                free(matrix[i]);
            free(matrix);
            return NULL;
        }

        (*rows)++;
    }

    free(line);
    fclose(file);
    return matrix;
}

void dot_product(float **matrix1, float **matrix2, int proceses, int rows, int cols)
{
    int subset;
    int extra;

    subset = rows / proceses;
    extra = rows % proceses;

    for (int i = 0; i < proceses; i++)
    {
        int start_row = i * subset + (i < extra ? i : extra);
        int num_rows = subset + (i < extra ? 1 : 0);

        int pid = fork();

        if (pid < 0)
        {
            perror("Error al hacer fork");
            exit(1);
        }
        else if (pid == 0)
        {

            // algoritmo de multipliacion de matrix

            exit(0);
        }
    }

    for (int i = 0; i < proceses; i++)
    {
        wait(NULL);
    }
}

void print_matrix(float **matrix, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            printf("%6.2f ", matrix[i][j]);
        }
        printf("\n");
    }
}

// Example usage
int main()
{
    int rows1, cols1;
    float **matrix1 = load_matrix("matriz1.txt", &rows1, &cols1);

    if (matrix1)
    {
        printf("Matrix 1:\n");
        print_matrix(matrix1, rows1, cols1);
    }

    return 0;
}
