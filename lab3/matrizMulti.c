#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 1024

int main() {
    FILE *file;
    char line[MAX_LINE];
    int filas = 0, columnas = 0;

    // Primer recorrido: contar filas y columnas
    file = fopen("matriz1_pequeña.txt", "r");
    if (!file) {
        perror("Error al abrir el archivo");
        return 1;
    }

    while (fgets(line, sizeof(line), file)) {
        if (filas == 0) {
            // Contar columnas en la primera línea
            char *token = strtok(line, " \n");
            while (token) {
                columnas++;
                token = strtok(NULL, " \n");
            }
        }
        filas++;
    }
    fclose(file);

    // Reservar memoria
    float **matriz = malloc(filas * sizeof(float *));
    for (int i = 0; i < filas; i++) {
        matriz[i] = malloc(columnas * sizeof(float));
    }

    return 0;
}