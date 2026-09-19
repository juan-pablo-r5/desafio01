#include "funciones.h"
#include <ctime>
#include <iostream>
using namespace std;


int main(){

    srand(time(NULL));

    int filas = 4;
    int cols = 4;
    size_t bytes_reservados = 0;

    unsigned char* tablero = crear_tablero(filas, cols, bytes_reservados);
    inicializar_tablero_aleatorio(tablero, filas, cols);

    cout << "Bytes reservados: " << bytes_reservados << "\n\n";

    // Mostrar el tablero leido bit a bit
    cout << "Tablero inicial:\n";
    for (int f = 0; f < filas; ++f) {
        for (int c = 0; c < cols; ++c) {
            int valor = obtener_ficha(tablero, f, c, cols);
            cout << valor << " ";
        }
        cout << "\n";
    }


    // Métricas del juego
    int puntaje = 0;
    int eliminaciones_usuario = 0;
    int total_fichas_destruidas = 0;
    int combinaciones = 0;
    int cascadas = 0;

    // Limpiar combinaciones iniciales para arrancar con el tablero estable
    procesar_cascadas(tablero, filas, cols, puntaje, total_fichas_destruidas, combinaciones, cascadas);
    puntaje = 0; total_fichas_destruidas = 0; combinaciones = 0; cascadas = 0;


    // Guardamos los bits del estado inicial del tablero
    registrar_estado_memoria(tablero, bytes_reservados, "historial_bits.txt");


    // Limpiar combinaciones iniciales para arrancar con el tablero estable
    procesar_cascadas(tablero, filas, cols, puntaje, total_fichas_destruidas, combinaciones, cascadas);
    puntaje = 0; total_fichas_destruidas = 0; combinaciones = 0; cascadas = 0;

    int opcion = 0;
    while (opcion != 7) {
        mostrar_tablero(tablero, filas, cols);
        imprimir_tira_binaria(tablero, bytes_reservados, filas, cols);

        cout << "--- SWEET CRUSH MENU ---\n";
        cout << "1. Eliminar ficha (Fila, Columna)\n";
        cout << "2. Agregar fila\n";
        cout << "3. Eliminar fila\n";
        cout << "4. Agregar columna\n";
        cout << "5. Eliminar columna\n";
        cout << "6. Ver estadisticas\n";
        cout << "7. Salir\n";
        cout << "Seleccione opcion: ";
        cin >> opcion;

        //Respaldo y guardado de byted ante cualquier cambio o jugada
        size_t bytes_antes = bytes_reservados;
        unsigned char* copia_antes = new unsigned char[bytes_antes];
        for (size_t i = 0; i < bytes_antes; ++i) {
            copia_antes[i] = tablero[i];
        }

        switch(opcion) {

            case 1: {
                int f, c;
                cout << "Ingrese fila (0 a " << filas - 1 << ") y columna (0 a " << cols - 1 << "): ";
                cin >> f >> c;
                if (f >= 0 && f < filas && c >= 0 && c < cols) {
                    eliminaciones_usuario++;
                    eliminar_ficha_usuario(tablero, filas, cols, f, c, puntaje, total_fichas_destruidas, combinaciones, cascadas);
                }
                break;
            }

            case 2: {
                int pos;
                cout << "Ingrese posicion para insertar fila (0 a " << filas << "): ";
                cin >> pos;
                if (pos >= 0 && pos <= filas) {
                    tablero = agregar_linea(tablero, filas, cols, bytes_reservados, pos, true);
                    procesar_cascadas(tablero, filas, cols, puntaje, total_fichas_destruidas, combinaciones, cascadas);
                }
                break;
            }

            case 3: {
                int pos;
                cout << "Ingrese posicion de fila a eliminar (0 a " << filas - 1 << "): ";
                cin >> pos;
                if (pos >= 0 && pos < filas) {
                    tablero = eliminar_linea(tablero, filas, cols, bytes_reservados, pos, true);
                    procesar_cascadas(tablero, filas, cols, puntaje, total_fichas_destruidas, combinaciones, cascadas);
                }
                break;
            }

            case 4: {
                int pos;
                cout << "Ingrese posicion para insertar columna (0 a " << cols << "): ";
                cin >> pos;
                if (pos >= 0 && pos <= cols) {
                    tablero = agregar_linea(tablero, filas, cols, bytes_reservados, pos, false);
                    procesar_cascadas(tablero, filas, cols, puntaje, total_fichas_destruidas, combinaciones, cascadas);
                }
                break;
            }

            case 5: {
                int pos;
                cout << "Ingrese posicion de columna a eliminar (0 a " << cols - 1 << "): ";
                cin >> pos;
                if (pos >= 0 && pos < cols) {
                    tablero = eliminar_linea(tablero, filas, cols, bytes_reservados, pos, false);
                    procesar_cascadas(tablero, filas, cols, puntaje, total_fichas_destruidas, combinaciones, cascadas);
                }
                break;
            }

            case 6: {
                cout << "\n=== ESTADISTICAS DEL JUEGO ===\n";
                cout << "Dimensiones actuales: " << filas << "x" << cols << "\n";
                cout << "Bytes fisicos reservados: " << bytes_reservados << "\n";
                cout << "Eliminaciones del usuario: " << eliminaciones_usuario << "\n";
                cout << "Total fichas destruidas: " << total_fichas_destruidas << "\n";
                cout << "Combinaciones detectadas: " << combinaciones << "\n";
                cout << "Cascadas producidas: " << cascadas << "\n";
                cout << "Puntuacion total: " << puntaje << "\n\n";
                cin.clear();
                cin.ignore(10000, '\n');
                break;
            }

            case 7: {
                cout << "Gracias por jugar!!!\n";
                break;
            }
        }

        bool memoria_cambio = false;

        // Compara las dimensiones físicas
        if (bytes_antes != bytes_reservados) {
            memoria_cambio = true;
        } else {
            // Compara byte por byte la secuencia en RAM
            for (size_t i = 0; i < bytes_reservados; ++i) {
                if (copia_antes[i] != tablero[i]) {
                    memoria_cambio = true;
                    break;
                }
            }
        }

        // Si los bits mutaron, la función clona los datos sobre el puntero sencillo de respaldo
        if (memoria_cambio) {
            // 1. Guardado binario real (para el programa)
            registrar_estado_memoria(tablero, bytes_reservados, "partida_guardada.bin");

            // 2. Guardado de texto visual (para ti y para el video)
            exportar_reporte_bits(tablero, bytes_reservados, filas, cols, "reporte_bits.txt");
        }

        delete[] copia_antes; // Libera el bloque temporal de este turno
    }
    delete[] tablero;
    return 0;
}
