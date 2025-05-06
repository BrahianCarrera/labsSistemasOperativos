#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int contar_floats_primera_linea(const char *nombre_archivo)
{
    FILE *archivo = fopen(nombre_archivo, "r");
    if (!archivo)
    {
        perror("No se pudo abrir el archivo");
        return -1;
    }

    char *linea = NULL;
    size_t len = 0;
    size_t nread = getline(&linea, &len, archivo);

    int contador = 0;
    if (nread != -1)
    {
        char *ptr = linea;
        char *end;

        while (*ptr)
        {
            // Intentar convertir un float desde ptr
            float valor = strtof(ptr, &end);

            if (ptr != end)
            {
                contador++;
                ptr = end; // avanzar al siguiente token
            }
            else
            {
                ptr++; // avanzar si no se encontró número
            }
        }
    }

    free(linea);
    fclose(archivo);
    return contador;
}

int main()
{
    const char *ruta = "matriz1.txt";
    int cantidad = contar_floats_primera_linea(ruta);
    if (cantidad >= 0)
        printf("Cantidad de floats en la primera línea: %d\n", cantidad);
    return 0;
}
