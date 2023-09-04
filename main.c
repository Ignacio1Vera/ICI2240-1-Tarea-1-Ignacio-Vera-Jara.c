#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Reserva {
    char nombreEstudiante[21];
    struct Reserva* siguiente;
} Reserva;

typedef struct Libro {
    char titulo[51];
    char autor[51];
    char genero[51];
    char ISBN[11];
    char ubicacion[11];
    char estado[12]; // "Disponible", "Reservado" o "Prestado"
    char ultimoPrestado[21];
    struct Libro* siguiente;
    struct Reserva* reservas; // Campo para almacenar reservas
} Libro;

// Función para registrar un libro
void registrarLibro(Libro** biblioteca, const Libro* nuevoLibro) {
    Libro* nuevoNodo = (Libro*)malloc(sizeof(Libro));
    if (nuevoNodo == NULL) {
        printf("Error: no se pudo asignar memoria para el nuevo libro.\n");
        return;
    }
    
    *nuevoNodo = *nuevoLibro;
    nuevoNodo->siguiente = *biblioteca;
    *biblioteca = nuevoNodo;
    printf("Libro registrado exitosamente.\n");
}

// Función para agregar una reserva a un libro
void agregarReserva(Libro* libro, const char* nombreEstudiante) {
    Reserva* nuevaReserva = malloc(sizeof(Reserva));
    strcpy(nuevaReserva->nombreEstudiante, nombreEstudiante);

    nuevaReserva->siguiente = libro->reservas;
    libro->reservas = nuevaReserva;
}


// Función para mostrar datos de un libro por título y autor
void mostrarDatosLibro(const Libro* biblioteca, const char* titulo, const char* autor) {
    const Libro* actual = biblioteca;
    while (actual != NULL) {
        if (strcmp(actual->titulo, titulo) == 0 && strcmp(actual->autor, autor) == 0) {
            printf("\n------------------------------------------\n");
            printf("    Título: %s\n", actual->titulo);
            printf("    Autor: %s\n", actual->autor);
            printf("    Género: %s\n", actual->genero);
            printf("    ISBN: %s\n", actual->ISBN);
            printf("    Ubicación: %s\n", actual->ubicacion);
            printf("    Estado: %s\n", actual->estado);

            if (strcmp(actual->estado, "Reservado") == 0) {
                printf("    Reservado por:\n");
                Reserva* reserva = actual->reservas;
                while (reserva != NULL) {
                    printf("        %s\n", reserva->nombreEstudiante);
                    reserva = reserva->siguiente;
                }
                printf("\n");
            } else if (strcmp(actual->estado, "Prestado") == 0) {
                printf("    Prestado a: %s\n", actual->ultimoPrestado);
            }

            printf("\n------------------------------------------\n");
            return;
        }
        actual = actual->siguiente;
    }
    printf("El libro no existe en la biblioteca.\n");
}

// Función para reservar un libro por título y autor
void reservarLibro(Libro** biblioteca, const char* titulo, const char* autor, const char* estudiante) {
    Libro* actual = *biblioteca;
    while (actual != NULL) {
        if (strcmp(actual->titulo, titulo) == 0 && strcmp(actual->autor, autor) == 0) {
            // Cambiar el estado a "Reservado" y establecer el estudiante que lo reservó
            strcpy(actual->ultimoPrestado, estudiante);
            strcpy(actual->estado, "Reservado");
            printf("Libro \"%s\" reservado por %s.\n", actual->titulo, estudiante);
            return;
        } else {
            actual = actual->siguiente;
        }
    }
    printf("El libro no existe en la biblioteca o no está disponible para reservar.\n");
}


// Función para devolver un libro por título y autor
void devolverLibro(Libro* biblioteca, const char* titulo, const char* autor) {
    Libro* actual = biblioteca;
    while (actual != NULL) {
        if (strcmp(actual->titulo, titulo) == 0 && strcmp(actual->autor, autor) == 0) {
            if (strcmp(actual->estado, "Prestado") == 0) {
                // Cambiar el estado a "Disponible" y eliminar el estudiante que lo prestó
                strcpy(actual->estado, "Disponible");
                strcpy(actual->ultimoPrestado, "");
                printf("Libro \"%s\" devuelto con éxito.\n", actual->titulo);
                return;
            } else {
                printf("El libro no está prestado en este momento.\n");
                return;
            }
        }
        actual = actual->siguiente;
    }
    printf("El libro no existe en la biblioteca.\n");
}

// Función para cancelar la reserva de un libro por título y autor
void cancelarReservaLibro(Libro** biblioteca, const char* titulo, const char* autor) {
    Libro* actual = *biblioteca;
    while (actual != NULL) {
        if (strcmp(actual->titulo, titulo) == 0 && strcmp(actual->autor, autor) == 0) {
            if (strcmp(actual->estado, "Reservado") == 0) {
                // Cambiar el estado a "Disponible" y eliminar la reserva
                strcpy(actual->estado, "Disponible");
                actual->reservas = NULL;
                printf("Reserva del libro \"%s\" cancelada con éxito.\n", actual->titulo);
                return;
            } else {
                printf("El libro no está reservado en este momento.\n");
                return;
            }
        }
        actual = actual->siguiente;
    }
    printf("El libro no existe en la biblioteca.\n");
}

// Función para retirar un libro por título y autor
void retirarLibro(Libro* biblioteca, const char* titulo, const char* autor) {
    char nombreEstudiante[21];
    printf("Ingrese el nombre del estudiante que retira el libro (hasta 20 caracteres): ");
    fgets(nombreEstudiante, sizeof(nombreEstudiante), stdin);
    nombreEstudiante[strcspn(nombreEstudiante, "\n")] = '\0';

    Libro* actual = biblioteca;

    while (actual != NULL) {
        if (strcmp(actual->titulo, titulo) == 0 && strcmp(actual->autor, autor) == 0) {
            // Si el libro está prestado, no se puede retirar
            if (strcmp(actual->estado, "Prestado") == 0) {
                printf("No se puede retirar el libro \"%s\" porque está prestado.\n", actual->titulo);
                return;
            }

            // Almacenar el nombre del estudiante que retiró el libro
            strcpy(actual->ultimoPrestado, nombreEstudiante);

            // Cambiar el estado del libro a "Prestado"
            strcpy(actual->estado, "Prestado");
            printf("Libro \"%s\" retirado por %s con éxito.\n", titulo, nombreEstudiante);
            return;
        }

        actual = actual->siguiente;
    }

    printf("El libro no existe en la biblioteca.\n");
}






// Función para cargar libros desde un archivo CSV
void cargarLibrosDesdeCSV(const char* nombreArchivo, Libro** biblioteca) {
    FILE* archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        printf("Error al abrir el archivo.\n");
        return;
    }

    char linea[256]; // Suponemos que cada línea tiene como máximo 255 caracteres

    while (fgets(linea, sizeof(linea), archivo)) {
        // Parsear la línea y crear un nuevo libro
        Libro nuevoLibro;
        sscanf(linea, "%50[^,],%50[^,],%50[^,],%10[^,],%10[^,],%11[^,],%20[^,],",
            nuevoLibro.titulo, nuevoLibro.autor, nuevoLibro.genero,
            nuevoLibro.ISBN, nuevoLibro.ubicacion, nuevoLibro.estado,
            nuevoLibro.ultimoPrestado);

        // Llamar a la función para registrar el libro en la biblioteca
        registrarLibro(biblioteca, &nuevoLibro);
    }

    fclose(archivo);
}


// Función para exportar libros a un archivo CSV
void exportarLibrosACSV(const char* nombreArchivo, const Libro* biblioteca) {
    FILE* archivo = fopen(nombreArchivo, "w");
    if (archivo == NULL) {
        printf("Error al abrir el archivo para escribir.\n");
        return;
    }

    const Libro* actual = biblioteca;
    while (actual != NULL) {
        fprintf(archivo, "%s,%s,%s,%s,%s,%s,%s\n", actual->titulo, actual->autor,
                actual->genero, actual->ISBN, actual->ubicacion, actual->estado,
                actual->ultimoPrestado);
        actual = actual->siguiente;
    }

    fclose(archivo);
    printf("Libros exportados al archivo CSV correctamente.\n");
}




// Función para mostrar el menú
void mostrarMenu() {
    printf("\n------------------------------------------\n");
    printf("|                 *Menú*                 |\n");
    printf("------------------------------------------\n");
    printf("1. Registrar libro\n");
    printf("2. Mostrar datos de libro\n");
    printf("3. Mostrar todos los libros\n");
    printf("4. Reservar libro\n");
    printf("5. Cancelar reserva de libro\n");
    printf("6. Retirar libro\n");
    printf("7. Devolver libro\n");
    printf("8. Mostrar libros prestados\n");
    printf("9. Importar libros desde un archivo CSV\n");
    printf("10. Exportar libros a un archivo CSV\n");
    printf("0. Salir\n");
    printf("Selecciona una opción: ");
}

// Función para liberar la memoria de la lista enlazada
void liberarBiblioteca(Libro* biblioteca) {
    while (biblioteca != NULL) {
        Libro* temp = biblioteca;
        biblioteca = biblioteca->siguiente;
        free(temp);
    }
}

int main() {
    Libro* biblioteca = NULL; // Inicializar la lista enlazada vacía
    int totalLibros = 0;
    
    
    int opcion;

    do {
        mostrarMenu();
        scanf("%d", &opcion);

        //Importante para que los fgets de abajo funcionen bien 
        while (getchar() != '\n');
        
        switch (opcion) {
            case 1:
            {
                Libro nuevoLibro;
                
                // Solicitar al usuario los datos del nuevo libro
                
                printf("Ingrese el título del libro (hasta 50 caracteres): ");
                fgets(nuevoLibro.titulo, sizeof(nuevoLibro.titulo), stdin);
                nuevoLibro.titulo[strcspn(nuevoLibro.titulo, "\n")] = '\0'; 

                printf("Ingrese el autor del libro (hasta 50 caracteres): ");
                fgets(nuevoLibro.autor, sizeof(nuevoLibro.autor), stdin);
                nuevoLibro.autor[strcspn(nuevoLibro.autor, "\n")] = '\0'; 

                printf("Ingrese el género del libro (hasta 50 caracteres): ");
                fgets(nuevoLibro.genero, sizeof(nuevoLibro.genero), stdin);
                nuevoLibro.genero[strcspn(nuevoLibro.genero, "\n")] = '\0'; 

                printf("Ingrese el ISBN del libro (hasta 10 caracteres): ");
                fgets(nuevoLibro.ISBN, sizeof(nuevoLibro.ISBN), stdin);
                nuevoLibro.ISBN[strcspn(nuevoLibro.ISBN, "\n")] = '\0'; 

                printf("Ingrese la ubicación del libro (hasta 10 caracteres): ");
                fgets(nuevoLibro.ubicacion, sizeof(nuevoLibro.ubicacion), stdin);
                nuevoLibro.ubicacion[strcspn(nuevoLibro.ubicacion, "\n")] = '\0'; 

                // Establecer el estado inicial (como es nuevo se le coloca "Disponible")
                strcpy(nuevoLibro.estado, "Disponible");
                
                // Inicializar el campo de último prestado (como es nuevo solo se coloca"")
                strcpy(nuevoLibro.ultimoPrestado, "");

                // Registrar el libro en la biblioteca
                registrarLibro(&biblioteca, &nuevoLibro);
                break;
            }
            case 2: 
            {
                char titulo[51];
                char autor[51];
                
                // Solicitar al usuario el título del libro
                printf("Ingrese el título del libro a mostrar (hasta 50 caracteres): ");
                fgets(titulo, sizeof(titulo), stdin);
                titulo[strcspn(titulo, "\n")] = '\0'; 

                // Solicitar al usuario el autor del libro
                printf("Ingrese el autor del libro a mostrar (hasta 50 caracteres): ");
                fgets(autor, sizeof(autor), stdin);
                autor[strcspn(autor, "\n")] = '\0';

                // Mostrar los datos del libro
                mostrarDatosLibro(biblioteca, titulo, autor);
                break;
            }
            case 3: 
            {
                // Mostrar todos los libros
                Libro* actual = biblioteca;
                while (actual != NULL) {
                    mostrarDatosLibro(actual, actual->titulo, actual->autor); // Llamar a mostrarDatosLibro para cada libro
                    actual = actual->siguiente;
                }
                break;
            }
            case 4: 
            {
                
                char titulo[51];
                char autor[51];
                char estudiante[21];
            
                // Solicitar al usuario el título del libro
                printf("Ingrese el título del libro a reservar (hasta 50 caracteres): ");
                fgets(titulo, sizeof(titulo), stdin);
                titulo[strcspn(titulo, "\n")] = '\0';  // Elimina el carácter '\n'
            
                // Solicitar al usuario el autor del libro
                printf("Ingrese el autor del libro a reservar (hasta 50 caracteres): ");
                fgets(autor, sizeof(autor), stdin);
                autor[strcspn(autor, "\n")] = '\0';
            
                // Solicitar al usuario su nombre para la reserva
                printf("Ingrese su nombre para la reserva (hasta 20 caracteres): ");
                fgets(estudiante, sizeof(estudiante), stdin);
                estudiante[strcspn(estudiante, "\n")] = '\0';
            
                // Llamar a la función para reservar el libro
                reservarLibro(&biblioteca, titulo, autor, estudiante);
                break;
            
            }
            
            case 5: 
            {
                char titulo[51];
                char autor[51];
                
                // Solicitar al usuario el título del libro para cancelar la reserva
                printf("Ingrese el título del libro para cancelar la reserva (hasta 50 caracteres): ");
                fgets(titulo, sizeof(titulo), stdin);
                titulo[strcspn(titulo, "\n")] = '\0'; 
            
                // Solicitar al usuario el autor del libro para cancelar la reserva
                printf("Ingrese el autor del libro para cancelar la reserva (hasta 50 caracteres): ");
                fgets(autor, sizeof(autor), stdin);
                autor[strcspn(autor, "\n")] = '\0';
            
                // Llamar a la función para cancelar la reserva del libro
                cancelarReservaLibro(&biblioteca, titulo, autor);
                break;
            }
            case 6: 
            {
                char titulo[51];
                char autor[51];
                
                // Solicitar al usuario el título del libro a retirar
                printf("Ingrese el título del libro a retirar (hasta 50 caracteres): ");
                fgets(titulo, sizeof(titulo), stdin);
                titulo[strcspn(titulo, "\n")] = '\0'; 
            
                // Solicitar al usuario el autor del libro a retirar
                printf("Ingrese el autor del libro a retirar (hasta 50 caracteres): ");
                fgets(autor, sizeof(autor), stdin);
                autor[strcspn(autor, "\n")] = '\0';
            
                // Llamar a la función para retirar el libro
                retirarLibro(biblioteca, titulo, autor);
            
                printf("Libro retirado exitosamente.\n");
                break;
            }
            
            case 7: 
            {
                char titulo[51];
                char autor[51];
                
                // Solicitar al usuario el título del libro a devolver
                printf("Ingrese el título del libro a devolver (hasta 50 caracteres): ");
                fgets(titulo, sizeof(titulo), stdin);
                titulo[strcspn(titulo, "\n")] = '\0'; 
            
                // Solicitar al usuario el autor del libro a devolver
                printf("Ingrese el autor del libro a devolver (hasta 50 caracteres): ");
                fgets(autor, sizeof(autor), stdin);
                autor[strcspn(autor, "\n")] = '\0';
            
                // Llamar a la función para devolver el libro
                devolverLibro(biblioteca, titulo, autor);
                break;
            }
            case 8: 
            {
                Libro* actual = biblioteca;
                int librosPrestados = 0;
                printf("Libros Prestados:\n");
                printf("\n------------------------------------------\n");
            
                while (actual != NULL) 
                {
                    if (strcmp(actual->estado, "Prestado") == 0) 
                    {
                        printf("    Título: %s\n", actual->titulo);
                        printf("    Autor: %s\n", actual->autor);
                        printf("    Prestado a: %s\n", actual->ultimoPrestado);
                        printf("------------------------------------------\n");
                        librosPrestados++;
                    }
                    actual = actual->siguiente;
                }
            
                if (librosPrestados == 0) {
                    printf("No hay libros prestados en este momento.\n");
                }
                break;
            }
            case 9: 
            {
                char nombreArchivo[256];
                printf("Ingrese el nombre del archivo CSV para importar libros: ");
                fgets(nombreArchivo, sizeof(nombreArchivo), stdin);
                nombreArchivo[strcspn(nombreArchivo, "\n")] = '\0';  // Elimina el carácter '\n'
            
                // Llamar a la función para importar libros desde el archivo CSV
                cargarLibrosDesdeCSV(nombreArchivo, &biblioteca);
                printf("Libros importados desde el archivo CSV.\n");
                break;
            }
            case 10: 
            {
                char nombreArchivo[256];
                printf("Ingrese el nombre del archivo CSV para exportar los libros: ");
                fgets(nombreArchivo, sizeof(nombreArchivo), stdin);
                nombreArchivo[strcspn(nombreArchivo, "\n")] = '\0';  // Elimina el carácter '\n'
            
                // Llamar a la función para exportar libros al archivo CSV
                exportarLibrosACSV(nombreArchivo, biblioteca);
                break;
            }
            case 0:
                printf("Saliendo de la aplicación.\n");
                break;
            default:
                printf("Opción no válida. Introduce un número válido.\n");
        }
    } while (opcion != 0);

    // Liberar la memoria de la lista enlazada antes de salir
    liberarBiblioteca(biblioteca);

    return 0;
}

//PROBLEMA RESERVA
