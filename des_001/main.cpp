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


int main(){

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
    mostrar_tablero(tablero, filas, cols);


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



    // Métricas del juego
    int puntaje = 0;
    int eliminaciones_usuario = 0;
    int total_fichas_destruidas = 0;
    int combinaciones = 0;
    int cascadas = 0;

    //Configurar el estado de los registros de bits
    int turno_actual = 0;
    unsigned char** historial_estados = new unsigned char*[100]; // Soporta hasta 100 jugadas
    for (int i = 0; i < 100; ++i) historial_estados[i] = nullptr;

    // Guardar el estado inicial
    registrar_estado_memoria(historial_estados, turno_actual, tablero, bytes_reservados);


    // Limpiar combinaciones iniciales para arrancar con el tablero estable
    procesar_cascadas(tablero, filas, cols, puntaje, total_fichas_destruidas, combinaciones, cascadas);
    puntaje = 0; total_fichas_destruidas = 0; combinaciones = 0; cascadas = 0;

    int opcion = 0;
    while (opcion != 7) {
        mostrar_tablero(tablero, filas, cols);
        imprimir_tira_binaria(tablero, bytes_reservados, filas, cols);

        std::cout << "--- SWEET CRUSH MENU ---\n";
        std::cout << "1. Eliminar ficha (Fila, Columna)\n";
        std::cout << "2. Agregar fila\n";
        std::cout << "3. Eliminar fila\n";
        std::cout << "4. Agregar columna\n";
        std::cout << "5. Eliminar columna\n";
        std::cout << "6. Ver estadisticas\n";
        std::cout << "7. Salir\n";
        std::cout << "Seleccione opcion: ";
        std::cin >> opcion;

        //Respaldo y guardado de byted ante cualquier cambio o jugada
        size_t bytes_antes = bytes_reservados;
        unsigned char* copia_antes = new unsigned char[bytes_antes];
        for (size_t i = 0; i < bytes_antes; ++i) {
            copia_antes[i] = tablero[i];
        }

        switch(opcion) {

            case 1: {
                int f, c;
                std::cout << "Ingrese fila (0 a " << filas - 1 << ") y columna (0 a " << cols - 1 << "): ";
                std::cin >> f >> c;
                if (f >= 0 && f < filas && c >= 0 && c < cols) {
                    eliminaciones_usuario++;
                    eliminar_ficha_usuario(tablero, filas, cols, f, c, puntaje, total_fichas_destruidas, combinaciones, cascadas);
                }
                break;
            }

            case 2: {
                int pos;
                std::cout << "Ingrese posicion para insertar fila (0 a " << filas << "): ";
                std::cin >> pos;
                if (pos >= 0 && pos <= filas) {
                    tablero = agregar_linea(tablero, filas, cols, bytes_reservados, pos, true);
                    procesar_cascadas(tablero, filas, cols, puntaje, total_fichas_destruidas, combinaciones, cascadas);
                }
                break;
            }

            case 3: {
                int pos;
                std::cout << "Ingrese posicion de fila a eliminar (0 a " << filas - 1 << "): ";
                std::cin >> pos;
                if (pos >= 0 && pos < filas) {
                    tablero = eliminar_linea(tablero, filas, cols, bytes_reservados, pos, true);
                    procesar_cascadas(tablero, filas, cols, puntaje, total_fichas_destruidas, combinaciones, cascadas);
                }
                break;
            }

            case 4: {
                int pos;
                std::cout << "Ingrese posicion para insertar columna (0 a " << cols << "): ";
                std::cin >> pos;
                if (pos >= 0 && pos <= cols) {
                    tablero = agregar_linea(tablero, filas, cols, bytes_reservados, pos, false);
                    procesar_cascadas(tablero, filas, cols, puntaje, total_fichas_destruidas, combinaciones, cascadas);
                }
                break;
            }

            case 5: {
                int pos;
                std::cout << "Ingrese posicion de columna a eliminar (0 a " << cols - 1 << "): ";
                std::cin >> pos;
                if (pos >= 0 && pos < cols) {
                    tablero = eliminar_linea(tablero, filas, cols, bytes_reservados, pos, false);
                    procesar_cascadas(tablero, filas, cols, puntaje, total_fichas_destruidas, combinaciones, cascadas);
                }
                break;
            }

            case 6: {
                std::cout << "\n=== ESTADISTICAS DEL JUEGO ===\n";
                std::cout << "Dimensiones actuales: " << filas << "x" << cols << "\n";
                std::cout << "Bytes fisicos reservados: " << bytes_reservados << "\n";
                std::cout << "Eliminaciones del usuario: " << eliminaciones_usuario << "\n";
                std::cout << "Total fichas destruidas: " << total_fichas_destruidas << "\n";
                std::cout << "Combinaciones detectadas: " << combinaciones << "\n";
                std::cout << "Cascadas producidas: " << cascadas << "\n";
                std::cout << "Puntuacion total: " << puntaje << "\n\n";
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                break;
            }

            case 7: {
                std::cout << "Gracias por jugar!!!\n";
                break;
            }

        }

         bool memoria_cambio = false;

        //verifica si el tblero cambio
        if (bytes_antes != bytes_reservados) {
            memoria_cambio = true;
        }

        else {
                // Si mantienen el tamaño, comparamos la RAM byte por byte
            for (size_t i = 0; i < bytes_reservados; ++i) {
                if (copia_antes[i] != tablero[i]) {
                    memoria_cambio = true;
                    break;
                }
            }
        }

            // Si se detecta un cambio real en los bits, se avanza el turno y se registra de forma automática
        if (memoria_cambio && turno_actual < 99) {
            turno_actual++;
            registrar_estado_memoria(historial_estados, turno_actual, tablero, bytes_reservados);
        }

        delete[] copia_antes;
    }

    // Liberar de memoria dinanica del historial
    for (int i = 0; i <= turno_actual; ++i) {
        if (historial_estados[i] != nullptr) {
            delete[] historial_estados[i];
        }
    }
    delete[] historial_estados;

    delete[] tablero;
    return 0;
}
