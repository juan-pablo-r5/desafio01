#include "funciones.h"
#include <ctime>
void mostrar_matriz(const unsigned char* tablero, int filas, int cols) {
    std::cout << "Tablero (" << filas << "x" << cols << "):\n";
    for (int f = 0; f < filas; ++f) {
        for (int c = 0; c < cols; ++c) {
            std::cout << (int)obtener_ficha(tablero, f, c, cols) << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}
int main()
{
    srand(time(NULL));

    int filas = 4;
    int cols = 4;
    size_t bytes_reservados = 0;

    unsigned char* tablero = crear_tablero(filas, cols, bytes_reservados);
    inicializar_tablero_aleatorio(tablero, filas, cols);

    std::cout << "Bytes reservados: " << bytes_reservados << "\n\n";

    // Mostrar el tablero leído bit a bit
    std::cout << "Tablero inicial:\n";
    for (int f = 0; f < filas; ++f) {
        for (int c = 0; c < cols; ++c) {
            int valor = obtener_ficha(tablero, f, c, cols);
            std::cout << valor << " ";
        }
        std::cout << "\n";
    }

    // Modificar una casilla específica (ejemplo: poner la ficha 5 en la fila 1, columna 2)
    fijar_ficha(tablero, 1, 2, cols, 5);

    std::cout << "\nValor modificado en (1, 2): " << (int)obtener_ficha(tablero, 1, 2, cols) << "\n";



    mostrar_matriz(tablero, filas, cols);
    imprimir_tira_binaria(tablero, bytes_reservados, filas, cols);

    // 1. Agregar una fila en la posición intermedia 1
    std::cout << "--- AGREGANDO FILA EN POSICION 1 ---\n";
    tablero = agregar_linea(tablero, filas, cols, bytes_reservados, 1, true);
    mostrar_matriz(tablero, filas, cols);
    imprimir_tira_binaria(tablero, bytes_reservados, filas, cols);

    // 2. Eliminar una fila para evaluar reasignación o retención de memoria
    std::cout << "--- ELIMINANDO FILA EN POSICION 1 ---\n";
    tablero = eliminar_linea(tablero, filas, cols, bytes_reservados, 1, true);
    mostrar_matriz(tablero, filas, cols);
    imprimir_tira_binaria(tablero, bytes_reservados, filas, cols);


    delete[] tablero;
    return 0;
}
