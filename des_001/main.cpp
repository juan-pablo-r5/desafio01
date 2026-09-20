#include "funciones.h"
#include <ctime>
#include <iostream>

using namespace std;

int main() {
    srand(time(NULL));

    int filas = 4;
    int cols = 4;
    size_t bytes_reservados = 0;

    // Metricas del juego
    int puntaje = 0;
    int eliminaciones_usuario = 0;
    int total_fichas_destruidas = 0;
    int combinaciones = 0;
    int cascadas = 0;

    unsigned char* tablero = nullptr;

    int opcion_inicial = 0;
    cout << "=================================\n";
    cout << "     BIENVENIDO A SWEET CRUSH    \n";
    cout << "=================================\n";
    cout << "1. Cargar partida desde archivo TXT\n";
    cout << "2. Crear nuevo tablero aleatorio\n";
    cout << "Seleccione opcion: ";
    cin >> opcion_inicial;

    if (opcion_inicial == 1) {
        tablero = cargar_partida_txt("partida_guardada.txt", filas, cols, bytes_reservados, puntaje, eliminaciones_usuario, total_fichas_destruidas, combinaciones, cascadas);
        if (tablero != nullptr) {
            cout << "\n[OK] Partida cargada exitosamente desde 'partida_guardada.txt'\n\n";
        } else {
            cout << "\n[ERROR] No se pudo abrir 'partida_guardada.txt'. Se creara un nuevo tablero.\n\n";
        }
    }

    // Si seleccionó la opción 2 o si la carga desde TXT falló
    if (tablero == nullptr) {
        filas = 4;
        cols = 4;
        tablero = crear_tablero(filas, cols, bytes_reservados);
        inicializar_tablero_aleatorio(tablero, filas, cols);

        procesar_cascadas(tablero, filas, cols, puntaje, total_fichas_destruidas, combinaciones, cascadas);
        puntaje = 0; total_fichas_destruidas = 0; combinaciones = 0; cascadas = 0;

        // Guardado binario inicial del estado limpio
        registrar_estado_memoria(tablero, bytes_reservados, "historial_bits.bin");
    }

    int opcion = 0;
    while (opcion != 9) {
        mostrar_tablero(tablero, filas, cols);
        imprimir_tira_binaria(tablero, bytes_reservados, filas, cols);

        cout << "--- SWEET CRUSH MENU ---\n";
        cout << "1. Eliminar ficha (Fila, Columna)\n";
        cout << "2. Agregar fila\n";
        cout << "3. Eliminar fila \n";
        cout << "4. Agregar columna\n";
        cout << "5. Eliminar columna \n";
        cout << "6. Ver estadisticas\n";
        cout << "7. Guardar partida en TXT\n";
        cout << "8. Cargar partida desde TXT\n";
        cout << "9. Salir\n";
        cout << "Seleccione opcion: ";
        cin >> opcion;

        // Copia de respaldo previa para detectar mutación de datos en RAM
        size_t bytes_antes = bytes_reservados;
        unsigned char* copia_antes = new unsigned char[bytes_antes];
        for (size_t i = 0; i < bytes_antes; ++i) {
            copia_antes[i] = tablero[i];
        }

        switch (opcion) {
        case 1: {
            int f, c;
            cout << "Ingrese fila (0 a " << filas - 1 << ") y columna (0 a " << cols - 1 << "): ";
            cin >> f >> c;
            if (f >= 0 && f < filas && c >= 0 && c < cols) {
                eliminaciones_usuario++;
                eliminar_ficha_usuario(tablero, filas, cols, f, c, puntaje, total_fichas_destruidas, combinaciones, cascadas);
            } else {
                cout << "Coordenadas fuera de rango.\n";
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
            if (filas > 2) {
                int pos;
                cout << "Ingrese posicion de fila a eliminar (0 a " << filas - 1 << "): ";
                cin >> pos;
                if (pos >= 0 && pos < filas) {
                    tablero = eliminar_linea(tablero, filas, cols, bytes_reservados, pos, true);
                    procesar_cascadas(tablero, filas, cols, puntaje, total_fichas_destruidas, combinaciones, cascadas);
                }
            } else {
                cout << "No se puede reducir a menos de 2 filas.\n";
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
            if (cols > 2) {
                int pos;
                cout << "Ingrese posicion de columna a eliminar (0 a " << cols - 1 << "): ";
                cin >> pos;
                if (pos >= 0 && pos < cols) {
                    tablero = eliminar_linea(tablero, filas, cols, bytes_reservados, pos, false);
                    procesar_cascadas(tablero, filas, cols, puntaje, total_fichas_destruidas, combinaciones, cascadas);
                }
            } else {
                cout << "No se puede reducir a menos de 2 columnas.\n";
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
            break;
        }

        case 7: {
            if (guardar_partida_txt("partida_guardada.txt", tablero, filas, cols, bytes_reservados, puntaje, eliminaciones_usuario, total_fichas_destruidas, combinaciones, cascadas)) {
                cout << "\n[OK] Partida guardada exitosamente en 'partida_guardada.txt'\n\n";
            } else {
                cout << "\n[ERROR] No se pudo guardar la partida.\n\n";
            }
            break;
        }

        case 8: {
            unsigned char* cargado = cargar_partida_txt("partida_guardada.txt", filas, cols, bytes_reservados, puntaje, eliminaciones_usuario, total_fichas_destruidas, combinaciones, cascadas);
            if (cargado != nullptr) {
                delete[] tablero;
                tablero = cargado;
                cout << "\n[OK] Partida cargada exitosamente desde 'partida_guardada.txt'\n\n";
            } else {
                cout << "\n[ERROR] No se encontro el archivo 'partida_guardada.txt'\n\n";
            }
            break;
        }

        case 9: {
            cout << "Gracias por jugar!\n";
            break;
        }
        }

        // Verificación de mutacion de datos en memoria para el autoguardado de respaldo
        bool memoria_cambio = false;
        if (bytes_antes != bytes_reservados) {
            memoria_cambio = true;
        } else {
            for (size_t i = 0; i < bytes_reservados; ++i) {
                if (copia_antes[i] != tablero[i]) {
                    memoria_cambio = true;
                    break;
                }
            }
        }

        if (memoria_cambio) {
            registrar_estado_memoria(tablero, bytes_reservados, "partida_guardada.bin");
            exportar_reporte_bits(tablero, bytes_reservados, filas, cols, "reporte_bits.txt");
        }

        delete[] copia_antes;
    }

    delete[] tablero;
    return 0;
}
