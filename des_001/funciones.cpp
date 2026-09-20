#include "funciones.h"
#include <cstdlib>
#include <fstream>
#include <iostream>

unsigned char obtener_ficha(const unsigned char* tablero, int fila, int col, int cols) {
    int indice = (fila * cols) + col;
    int bit_inicio = indice * 3;
    int byte_idx = bit_inicio / 8;
    int offset = bit_inicio % 8;

    if (offset <= 5) {
        return (tablero[byte_idx] >> offset) & 0x07;
    } else {
        int bits_b1 = 8 - offset;
        int bits_b2 = 3 - bits_b1;
        unsigned char parte1 = (tablero[byte_idx] >> offset) & ((1 << bits_b1) - 1);
        unsigned char parte2 = tablero[byte_idx + 1] & ((1 << bits_b2) - 1);
        return parte1 | (parte2 << bits_b1);
    }
}

void fijar_ficha(unsigned char* tablero, int fila, int col, int cols, unsigned char valor) {
    valor &= 0x07;
    int indice = (fila * cols) + col;
    int bit_inicio = indice * 3;
    int byte_idx = bit_inicio / 8;
    int offset = bit_inicio % 8;

    if (offset <= 5) {
        tablero[byte_idx] &= ~(0x07 << offset);
        tablero[byte_idx] |= (valor << offset);
    } else {
        int bits_b1 = 8 - offset;
        int bits_b2 = 3 - bits_b1;

        unsigned char mascara_b1 = ((1 << bits_b1) - 1) << offset;
        tablero[byte_idx] &= ~mascara_b1;
        tablero[byte_idx] |= ((valor & ((1 << bits_b1) - 1)) << offset);

        unsigned char mascara_b2 = (1 << bits_b2) - 1;
        tablero[byte_idx + 1] &= ~mascara_b2;
        tablero[byte_idx + 1] |= ((valor >> bits_b1) & mascara_b2);
    }
}

unsigned char* crear_tablero(int filas, int cols, size_t& bytes_reservados) {
    size_t bits_totales = filas * cols * 3;
    bytes_reservados = (bits_totales + 7) / 8;
    return new unsigned char[bytes_reservados]();
}

void inicializar_tablero_aleatorio(unsigned char* tablero, int filas, int cols) {
    for (int f = 0; f < filas; ++f) {
        for (int c = 0; c < cols; ++c) {
            fijar_ficha(tablero, f, c, cols, rand() % 6);
        }
    }
}

unsigned char* eliminar_linea(unsigned char* tablero, int& filas, int& cols, size_t& bytes_reservados, int pos, bool es_fila) {
    int nuevas_filas = es_fila ? filas - 1 : filas;
    int nuevas_cols = es_fila ? cols : cols - 1;

    size_t bits_nuevos = nuevas_filas * nuevas_cols * 3;
    size_t bytes_nuevos = (bits_nuevos + 7) / 8;

    unsigned char* temp = new unsigned char[bytes_nuevos]();

    for (int f = 0; f < nuevas_filas; ++f) {
        for (int c = 0; c < nuevas_cols; ++c) {
            int orig_f = (es_fila && f >= pos) ? f + 1 : f;
            int orig_c = (!es_fila && c >= pos) ? c + 1 : c;
            unsigned char ficha = obtener_ficha(tablero, orig_f, orig_c, cols);
            fijar_ficha(temp, f, c, nuevas_cols, ficha);
        }
    }

    double porcentaje_uso = ((double)bytes_nuevos / bytes_reservados) * 100.0;
    unsigned char* resultado = tablero;

    if (porcentaje_uso < 65.0) {
        delete[] tablero;
        resultado = temp;
        bytes_reservados = bytes_nuevos;
    } else {
        for (size_t i = 0; i < bytes_reservados; ++i) {
            tablero[i] = (i < bytes_nuevos) ? temp[i] : 0;
        }
        delete[] temp;
    }

    filas = nuevas_filas;
    cols = nuevas_cols;
    return resultado;
}

unsigned char* agregar_linea(unsigned char* tablero, int& filas, int& cols, size_t& bytes_reservados, int pos, bool es_fila) {
    int nuevas_filas = es_fila ? filas + 1 : filas;
    int nuevas_cols = es_fila ? cols : cols + 1;

    size_t bits_nuevos = nuevas_filas * nuevas_cols * 3;
    size_t bytes_nuevos = (bits_nuevos + 7) / 8;

    // Se asigna un nuevo bloque de memoria para acomodar el incremento
    unsigned char* nuevo_tablero = new unsigned char[bytes_nuevos]();

    for (int f = 0; f < nuevas_filas; ++f) {
        for (int c = 0; c < nuevas_cols; ++c) {
            bool es_nueva_linea = (es_fila && f == pos) || (!es_fila && c == pos);

            if (es_nueva_linea) {
                // Asignar ficha aleatoria uniforme (000 a 101)
                fijar_ficha(nuevo_tablero, f, c, nuevas_cols, rand() % 6);
            } else {
                // Recuperar ficha existente ajustando índices
                int orig_f = (es_fila && f > pos) ? f - 1 : f;
                int orig_c = (!es_fila && c > pos) ? c - 1 : c;
                unsigned char ficha = obtener_ficha(tablero, orig_f, orig_c, cols);
                fijar_ficha(nuevo_tablero, f, c, nuevas_cols, ficha);
            }
        }
    }

    delete[] tablero;
    bytes_reservados = bytes_nuevos;
    filas = nuevas_filas;
    cols = nuevas_cols;

    return nuevo_tablero;
}


void imprimir_tira_binaria(const unsigned char* tablero, size_t bytes_reservados, int filas, int cols) {
    size_t bits_utilizados = filas * cols * 3;
    cout << "Secuencia empaquetada en memoria (" << bytes_reservados << " bytes):\n";
    for (size_t i = 0; i < bytes_reservados; ++i) {
        cout << "Byte " << i << ": [";
        for (int bit = 7; bit >= 0; --bit) {
            size_t bit_global = (i * 8) + bit;
            if (bit_global < bits_utilizados) {
                cout << ((tablero[i] >> bit) & 1);
            } else {
                cout << ".";
            }
        }
        cout << "] ";
    }
    cout << "\n\n";
}

bool detectar_y_marcar_combinaciones(const unsigned char* tablero, int filas, int cols, bool* eliminados) {
    bool hay_combos = false;

    // Escaneo Horizontal
    for (int f = 0; f < filas; ++f) {
        for (int c = 0; c < cols - 2; ++c) {
            unsigned char v = obtener_ficha(tablero, f, c, cols);
            if (v < 6 && v == obtener_ficha(tablero, f, c + 1, cols) && v == obtener_ficha(tablero, f, c + 2, cols)) {
                int k = c;
                while (k < cols && obtener_ficha(tablero, f, k, cols) == v) {
                    eliminados[f * cols + k] = true;
                    k++;
                }
                hay_combos = true;
            }
        }
    }

    // Escaneo Vertical
    for (int c = 0; c < cols; ++c) {
        for (int f = 0; f < filas - 2; ++f) {
            unsigned char v = obtener_ficha(tablero, f, c, cols);
            if (v < 6 && v == obtener_ficha(tablero, f + 1, c, cols) && v == obtener_ficha(tablero, f + 2, c, cols)) {
                int k = f;
                while (k < filas && obtener_ficha(tablero, k, c, cols) == v) {
                    eliminados[k * cols + c] = true;
                    k++;
                }
                hay_combos = true;
            }
        }
    }

    return hay_combos;
}

// CORRECCIÓN DE LA GRAVEDAD PARA EVITAR CLONACIONES EN EMBAJADA
int aplicar_gravedad_y_relleno(unsigned char* tablero, int filas, int cols, const bool* eliminados) {
    int total_eliminadas = 0;
    for (int c = 0; c < cols; ++c) {
        unsigned char* columna_temporal = new unsigned char[filas];
        int pos_escribir = filas - 1;

        for (int f = filas - 1; f >= 0; --f) {
            if (!eliminados[f * cols + c]) {
                columna_temporal[pos_escribir] = obtener_ficha(tablero, f, c, cols);
                pos_escribir--;
            } else {
                total_eliminadas++;
            }
        }

        while (pos_escribir >= 0) {
            columna_temporal[pos_escribir] = rand() % 6;
            pos_escribir--;
        }

        for (int f = 0; f < filas; ++f) {
            fijar_ficha(tablero, f, c, cols, columna_temporal[f]);
        }
        delete[] columna_temporal;
    }
    return total_eliminadas;
}

void procesar_cascadas(unsigned char* tablero, int filas, int cols, int& puntaje, int& total_fichas_destruidas, int& combinaciones, int& cascadas) {
    int nivel_cascada = 0;
    while (true) {
        bool* eliminados = new bool[filas * cols]();
        bool hay_combos = detectar_y_marcar_combinaciones(tablero, filas, cols, eliminados);

        if (!hay_combos) {
            delete[] eliminados;
            break;
        }

        nivel_cascada++;
        combinaciones++;
        if (nivel_cascada > 1) cascadas++;

        int destruidas = aplicar_gravedad_y_relleno(tablero, filas, cols, eliminados);
        total_fichas_destruidas += destruidas;
        puntaje += (destruidas * 10 * nivel_cascada);

        delete[] eliminados;
    }
}


void eliminar_ficha_usuario(unsigned char* tablero, int filas, int cols, int fila_sel, int col_sel, int& puntaje, int& total_fichas_destruidas, int& combinaciones, int& cascadas) {
    bool* eliminados = new bool[filas * cols]();
    eliminados[fila_sel * cols + col_sel] = true;

    aplicar_gravedad_y_relleno(tablero, filas, cols, eliminados);
    delete[] eliminados;
    procesar_cascadas(tablero, filas, cols, puntaje, total_fichas_destruidas, combinaciones, cascadas);
}

void mostrar_tablero(const unsigned char* tablero, int filas, int cols) {
    const char simbolos[] = {'A', 'B', 'C', 'D', 'E', 'F', ' ', '*'};

    cout << "\n   ";
    for (int c = 0; c < cols; ++c) cout << c << " ";

    cout << "\n  +";
    for (int i = 0; i < cols * 2; ++i) cout << "-";
    cout << "+\n";

    for (int f = 0; f < filas; ++f) {
        cout << f << " |";
        for (int c = 0; c < cols; ++c) {
            unsigned char v = obtener_ficha(tablero, f, c, cols);
            cout << simbolos[v] << " ";
        }
        cout << "|\n";
    }

    cout << "  +";
    for (int i = 0; i < cols * 2; ++i) cout << "-";
    cout << "+\n\n";
}

void registrar_estado_memoria(const unsigned char* tablero_actual, size_t bytes_reservados, const char* nombre_archivo) {
    if (tablero_actual == nullptr) return;

    std::ofstream archivo(nombre_archivo, std::ios::binary);

    if (archivo.is_open()) {
        archivo.write(reinterpret_cast<const char*>(tablero_actual), bytes_reservados);
        archivo.close();
    } else {
        cout << "Error: No se pudo crear el archivo de respaldo.\n";
    }
}



void exportar_reporte_bits(const unsigned char* tablero, size_t bytes_reservados, int filas, int cols, const char* nombre_archivo) {
    if (tablero == nullptr) return;

    std::ofstream archivo(nombre_archivo);

    if (archivo.is_open()) {
        size_t bits_utilizados = filas * cols * 3;

        // Imprimimos todos los bits de corrido
        for (size_t i = 0; i < bytes_reservados; ++i) {
            for (int bit = 7; bit >= 0; --bit) {
                size_t bit_global = (i * 8) + bit;
                if (bit_global < bits_utilizados) {
                    archivo << ((tablero[i] >> bit) & 1);
                } else {
                    archivo << "."; // Bits sobrantes al final
                }
            }
            // Opcional: un espacio en blanco entre bytes para no marearte leyendo,
            // si lo quieres 100% pegado, puedes borrar la siguiente línea:
            archivo << " ";
        }

        archivo.close();
    } else {
        cout << "Error: No se pudo crear el reporte de texto.\n";
    }
}

bool guardar_partida_txt(const char* nombre_archivo, const unsigned char* tablero, int filas, int cols, size_t bytes_reservados, int puntaje, int elim_usr, int dest_fichas, int comb, int casc) {
    std::ofstream archivo(nombre_archivo);
    if (!archivo.is_open()) return false;

    // 1. Encabezado con dimensiones y memoria reservada
    archivo << filas << " " << cols << " " << bytes_reservados << "\n";

    // 2. Estadísticas acumuladas
    archivo << puntaje << " " << elim_usr << " " << dest_fichas << " " << comb << " " << casc << "\n";

    // 3. Tira de bytes parseada a valores numéricos en texto
    for (size_t i = 0; i < bytes_reservados; ++i) {
        archivo << (int)tablero[i] << " ";
    }
    archivo << "\n";

    archivo.close();
    return true;
}

unsigned char* cargar_partida_txt(const char* nombre_archivo, int& filas, int& cols, size_t& bytes_reservados, int& puntaje, int& elim_usr, int& dest_fichas, int& comb, int& casc) {
    std::ifstream archivo(nombre_archivo);
    if (!archivo.is_open()) return nullptr;

    // Leer encabezado
    archivo >> filas >> cols >> bytes_reservados;
    archivo >> puntaje >> elim_usr >> dest_fichas >> comb >> casc;

    // Asignar memoria dinámica limpia para el nuevo puntero
    unsigned char* nuevo_tablero = new unsigned char[bytes_reservados]();

    // Parsear los números de texto directamente a los bytes del puntero
    for (size_t i = 0; i < bytes_reservados; ++i) {
        int val;
        archivo >> val;
        nuevo_tablero[i] = (unsigned char)val;
    }

    archivo.close();
    return nuevo_tablero;
}
