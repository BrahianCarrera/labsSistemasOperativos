#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/shm.h>

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
                   int rows1, int cols1, int rows2, int cols2)
{
    if (cols1 != rows2)
    {
        fprintf(stderr, "Error: Incompatible dimensions for multiplication\n");
        return NULL;
    }

    int subset = rows1 / processes;
    int extra = rows1 % processes;

    int shmid = shmget(IPC_PRIVATE, sizeof(float) * rows1 * cols2, IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget");
        return NULL;
    }

    float *result = (float *)shmat(shmid, NULL, 0);
    if (result == (float *)-1)
    {
        perror("shmat");
        shmctl(shmid, IPC_RMID, NULL); // Clean up
        return NULL;
    }

    for (int p = 0; p < processes; p++)
    {
        int start_row = p * subset + (p < extra ? p : extra);
        int num_rows = subset + (p < extra ? 1 : 0);

        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork");
            shmdt(result);
            shmctl(shmid, IPC_RMID, NULL);
            exit(1);
        }
        else if (pid == 0)
        {
            for (int i = start_row; i < start_row + num_rows; i++)
            {
                for (int j = 0; j < cols2; j++)
                {
                    result[i * cols2 + j] = 0;
                    for (int k = 0; k < cols1; k++)
                    {
                        result[i * cols2 + j] += matrix1[i][k] * matrix2[k][j];
                    }
                }
            }

            shmdt(result); // Cada hijo solo se desconecta, no borra
            exit(0);       // Asegúrate de que el hijo no continúe
        }
    }

    // Espera a todos los hijos antes de usar los resultados
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
            printf("%.2f ", flat_matrix[i * cols + j]);
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

float *dot_product_sequential(float **matrix1, float **matrix2,
                              int rows1, int cols1, int rows2, int cols2)
{
    if (cols1 != rows2)
    {
        fprintf(stderr, "Error: Incompatible dimensions for multiplication\n");
        return NULL;
    }

    float *result = malloc(sizeof(float) * rows1 * cols2);
    if (!result)
    {
        perror("malloc");
        return NULL;
    }

    for (int i = 0; i < rows1; i++)
    {
        for (int j = 0; j < cols2; j++)
        {
            result[i * cols2 + j] = 0;
            for (int k = 0; k < cols1; k++)
            {
                result[i * cols2 + j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
    return result;
}

int main()
{
    clock_t start_t, end_t, start_t2, end_t2;
    int rows1, cols1;
    int rows2, cols2;
    double sequential_t, concurrent_t;

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
    float *seq_result = dot_product_sequential(matrix1, matrix2, rows1, cols1, rows2, cols2);
    end_t = clock();

    if (!seq_result)
    {
        fprintf(stderr, "Error trying to execute sequential dot product\n");
        free_matrix(matrix1, rows1);
        free_matrix(matrix2, rows2);
        return 1;
    }

    sequential_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;

    // Liberar mmemoria del procedimiento sequencial :P
    free(seq_result);

    int processes;
    printf("Please enter the number of processes to create ");
    if (scanf("%d", &processes) == 1 && processes > 0)
    {

        start_t2 = clock();
        float *result = dot_product(matrix1, matrix2, processes, rows1, cols1, rows2, cols2);
        end_t2 = clock();

        if (result)
        {

            concurrent_t = (double)(end_t2 - start_t2) / CLOCKS_PER_SEC;

            shmdt(result); // Detach
            // Get the shmid again for cleanup (you could also return shmid from dot_product)
            int shmid = shmget(IPC_PRIVATE, 0, 0666);
            shmctl(shmid, IPC_RMID, NULL); // Remove
        }
        else
        {
            fprintf(stderr, "Error while applying parallel dot product\n");
            free_matrix(matrix1, rows1);
            free_matrix(matrix2, rows2);
            return 1;
        }

        // Liberar memoria
        free_matrix(matrix1, rows1);
        free_matrix(matrix2, rows2);

        printf("Sequential time: %.10f \n", sequential_t);
        printf("Parallel time  (%d processes): %.10f \n", processes, concurrent_t);
        printf("Speedup: %.6fX\n", (sequential_t / concurrent_t));

        return 0;
    }
    else
    {
        printf("Error: please enter a correct processes value\n");
        free_matrix(matrix1, rows1);
        free_matrix(matrix2, rows2);
        return 1;
    }
}