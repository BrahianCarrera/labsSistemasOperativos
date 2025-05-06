#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>

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
            perror("realloc");
            fclose(file);
            free(line);
            return NULL;
        }

        matrix[*rows] = NULL;

        token = strtok(line, " \n");
        while (token)
        {
            matrix[*rows] = realloc(matrix[*rows], (current_col + 1) * sizeof(float));
            if (!matrix[*rows])
            {
                perror("realloc inner");
                fclose(file);
                free(line);
                return NULL;
            }
            matrix[*rows][current_col] = atof(token);
            token = strtok(NULL, " \n");
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
            return NULL;
        }

        (*rows)++;
    }

    free(line);
    fclose(file);
    return matrix;
}

float *dot_product(float **matrix1, float **matrix2, int processes,
                   int rows, int cols, int rows2, int cols2)
{
    int subset = rows / processes;
    int extra = rows % processes;

    float *result = mmap(NULL, sizeof(float) * rows * cols2,
                         PROT_READ | PROT_WRITE,
                         MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (result == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    for (int p = 0; p < processes; p++)
    {
        int start_row = p * subset + (p < extra ? p : extra);
        int num_rows = subset + (p < extra ? 1 : 0);

        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork");
            exit(1);
        }
        else if (pid == 0)
        {
            for (int i = start_row; i < start_row + num_rows; i++)
            {
                for (int j = 0; j < cols2; j++)
                {
                    result[i * cols2 + j] = 0;
                    for (int k = 0; k < cols; k++)
                    {
                        result[i * cols2 + j] += matrix1[i][k] * matrix2[k][j];
                    }
                }
            }

            munmap(result, sizeof(float) * rows * cols2);
            exit(0);
        }
    }

    for (int i = 0; i < processes; i++)
        wait(NULL);

    return result;
}

void print_matrix_flat(float *flat_matrix, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            printf("%.10f ", flat_matrix[i * cols + j]);
        }
        printf("\n");
    }
}

void free_matrix(float **matrix, int rows)
{
    for (int i = 0; i < rows; i++)
        free(matrix[i]);
    free(matrix);
}

int verify_sizea(int cols1, int rows2)
{
    if (cols1 != rows2)
    {
        fprintf(stderr, "Error: No se pueden multiplicar matrices (cols 1: %d) y (rows 2%d)\n", cols1, rows2);
        return 1;
    }
    else
        return 0;
}

int main()
{
    clock_t start_t, end_t;
    int rows1, cols1;
    int rows2, cols2;
    double total_t;

    // Solo el proceso principal debería ejecutar esto
    float **matrix1 = load_matrix("matriz1.txt", &rows1, &cols1);
    if (!matrix1)
    {
        fprintf(stderr, "Error loading matrix A.\n");
        return 1;
    }

    float **matrix2 = load_matrix("matriz2.txt", &rows2, &cols2);
    if (!matrix2)
    {
        fprintf(stderr, "Error loading matrix B.\n");
        free_matrix(matrix1, rows1);
        return 1;
    }

    if (cols1 != rows2)
    {
        fprintf(stderr, "Error: Incompatible dimensions (%d cols vs %d rows).\n", cols1, rows2);
        free_matrix(matrix1, rows1);
        free_matrix(matrix2, rows2);
        return 1;
    }

    start_t = clock();
    float *result = dot_product(matrix1, matrix2, 10, rows1, cols1, rows2, cols2);
    end_t = clock();

    if (result)
    {
        printf("Resultado:\n");
        // print_matrix_flat(result, rows1, cols2);
        munmap(result, sizeof(float) * rows1 * cols2);
    }

    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    printf("Time for the calculation: %.10f seconds\n", total_t);

    // Liberar memoria
    free_matrix(matrix1, rows1);
    free_matrix(matrix2, rows2);

    return 0;
}
