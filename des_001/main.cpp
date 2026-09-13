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

    delete[] tablero;
    return 0;
}
