#include "funciones.h"
#include <ctime>

using namespace std;

int main()
{
    srand(time(NULL));

    int filas = 4;
    int cols = 4;
    size_t bytes_reservados = 0;

    unsigned char* tablero = crear_tablero(filas, cols, bytes_reservados);
    inicializar_tablero_aleatorio(tablero, filas, cols);

    cout << "Bytes reservados: " << bytes_reservados << "\n\n";

    // Mostrar el tablero leído bit a bit
    cout << "Tablero inicial:\n";
    for (int f = 0; f < filas; ++f) {
        for (int c = 0; c < cols; ++c) {
            int valor = obtener_ficha(tablero, f, c, cols);
            cout << valor << " ";
        }
        cout << "\n";
    }

    // Modificar una casilla específica (ejemplo: poner la ficha 5 en la fila 1, columna 2)
    fijar_ficha(tablero, 1, 2, cols, 5);

    cout << "\nValor modificado en (1, 2): " << (int)obtener_ficha(tablero, 1, 2, cols) << "\n";

//Agrego codigo de caida de las fichas y reemplazo de los espacios disponibles

    fijar_ficha(tablero, 1, 1, cols, 7);
    fijar_ficha(tablero, 2, 1, cols, 7);

    cout << "\nTablero tras romper fichas (f1,c1 y f2,c1 quedan con valor 7):\n";
    for (int f = 0; f < filas; ++f) {
        for (int c = 0; c < cols; ++c) {
            cout << (int)obtener_ficha(tablero, f, c, cols) << " ";
        }
        cout << "\n";
    }

//Aplicamos el algoritmo de gravedad y de relleno

    aplicar_gravedad(tablero, filas, cols);

    cout << "\nTablero tras aplicar gravedad:\n";
    for (int f = 0; f < filas; ++f) {
        for (int c = 0; c < cols; ++c) {
            cout << (int)obtener_ficha(tablero, f, c, cols) << " ";
        }
        cout << "\n";
    }

    rellenar_fichas_superiores(tablero, filas, cols);

    cout << "\nTablero final completamente rellenado:\n";
    for (int f = 0; f < filas; ++f) {
        for (int c = 0; c < cols; ++c) {
            cout << (int)obtener_ficha(tablero, f, c, cols) << " ";
        }
        cout << "\n";
    }

    delete[] tablero;
    return 0;
}
