#include "funciones.h"

#include <cstdlib> // Para rand() si lo necesitas después

unsigned char obtener_ficha(const unsigned char* tablero, int fila, int col, int cols) {
    int indice = (fila * cols) + col;
    int bit_inicio = indice * 3;
    int byte_idx = bit_inicio / 8;
    int offset = bit_inicio % 8;

    if (offset <= 5) {
        // Caso 1: Los 3 bits están contenidos en un solo byte
        return (tablero[byte_idx] >> offset) & 0x07;
    } else {
        // Caso 2: Los 3 bits se dividen entre dos bytes consecutivos (offset == 6 o 7)
        int bits_b1 = 8 - offset;   // Bits en Byte N (2 bits si offset=6, 1 bit si offset=7)
        int bits_b2 = 3 - bits_b1;  // Bits en Byte N+1 (1 bit si offset=6, 2 bits si offset=7)

        // Extraer la parte del primer byte
        unsigned char parte1 = (tablero[byte_idx] >> offset) & ((1 << bits_b1) - 1);

        // Extraer la parte del segundo byte
        unsigned char parte2 = tablero[byte_idx + 1] & ((1 << bits_b2) - 1);

        // Reconstruir la ficha uniendo ambas partes
        return parte1 | (parte2 << bits_b1);
    }
}

void fijar_ficha(unsigned char* tablero, int fila, int col, int cols, unsigned char valor) {
    valor &= 0x07; // Asegurar que solo usamos los 3 bits menos significativos (0 a 7)

    int indice = (fila * cols) + col;
    int bit_inicio = indice * 3;
    int byte_idx = bit_inicio / 8;
    int offset = bit_inicio % 8;

    if (offset <= 5) {
        // Caso 1: Un solo byte
        // 1. Poner en cero los 3 bits destino usando una máscara invertida
        tablero[byte_idx] &= ~(0x07 << offset);
        // 2. Grabar la nueva ficha con el operador OR
        tablero[byte_idx] |= (valor << offset);
    } else {
        // Caso 2: Repartido entre dos bytes consecutivos
        int bits_b1 = 8 - offset;
        int bits_b2 = 3 - bits_b1;

        // --- BYTE N ---
        unsigned char mascara_b1 = ((1 << bits_b1) - 1) << offset;
        tablero[byte_idx] &= ~mascara_b1; // Limpiar bits
        tablero[byte_idx] |= ((valor & ((1 << bits_b1) - 1)) << offset); // Escribir bits inferiores

        // --- BYTE N+1 ---
        unsigned char mascara_b2 = (1 << bits_b2) - 1;
        tablero[byte_idx + 1] &= ~mascara_b2; // Limpiar bits
        tablero[byte_idx + 1] |= ((valor >> bits_b1) & mascara_b2); // Escribir bits superiores
    }
}

unsigned char* crear_tablero(int filas, int cols, size_t& bytes_reservados) {
    size_t bits_totales = filas * cols * 3;
    bytes_reservados = (bits_totales + 7) / 8; // Redondeo hacia arriba sin usar double
    return new unsigned char[bytes_reservados]();
}

void inicializar_tablero_aleatorio(unsigned char* tablero, int filas, int cols) {
    for (int f = 0; f < filas; ++f) {
        for (int c = 0; c < cols; ++c) {
            unsigned char ficha_random = rand() % 6; // Fichas A-F (000 a 101)
            fijar_ficha(tablero, f, c, cols, ficha_random);
        }
    }
}


// --- ELIMINAR FILA O COLUMNA (REGLA DEL 65%) ---
unsigned char* eliminar_linea(unsigned char* tablero, int& filas, int& cols, size_t& bytes_reservados, int pos, bool es_fila) {
    int nuevas_filas = es_fila ? filas - 1 : filas;
    int nuevas_cols = es_fila ? cols : cols - 1;

    size_t bits_nuevos = nuevas_filas * nuevas_cols * 3;
    size_t bytes_nuevos = (bits_nuevos + 7) / 8; // Memoria mínima exacta requerida

    // Tablero temporal para empaquetar las fichas que se conservan
    unsigned char* temp = new unsigned char[bytes_nuevos]();

    // Copiar fichas ignorando la fila o columna eliminada
    for (int f = 0; f < nuevas_filas; ++f) {
        for (int c = 0; c < nuevas_cols; ++c) {
            int orig_f = (es_fila && f >= pos) ? f + 1 : f;
            int orig_c = (!es_fila && c >= pos) ? c + 1 : c;

            unsigned char ficha = obtener_ficha(tablero, orig_f, orig_c, cols);
            fijar_ficha(temp, f, c, nuevas_cols, ficha);
        }
    }

    // Evaluar la regla del 65% respecto a la memoria reservada actual
    double porcentaje_uso = ((double)bytes_nuevos / bytes_reservados) * 100.0;

    unsigned char* resultado = tablero;

    if (porcentaje_uso < 65.0) {
        // La ocupación cayó por debajo del 65%: Reasignar a un bloque más pequeño
        delete[] tablero;
        resultado = temp;
        bytes_reservados = bytes_nuevos;
    } else {
        // La ocupación es >= 65%: Mantener la memoria física existente
        // Copiar los datos empaquetados al bloque original y limpiar sobrante
        for (size_t i = 0; i < bytes_reservados; ++i) {
            tablero[i] = (i < bytes_nuevos) ? temp[i] : 0;
        }
        delete[] temp;
    }

    filas = nuevas_filas;
    cols = nuevas_cols;

    return resultado;
}

// --- AGREGAR FILA O COLUMNA ---
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

// --- VISUALIZACIÓN DE LA TIRA DE BITS EN MEMORIA ---
void imprimir_tira_binaria(const unsigned char* tablero, size_t bytes_reservados, int filas, int cols) {
    size_t bits_utilizados = filas * cols * 3;

    std::cout << "Secuencia empaquetada de bytes en memoria (" << bytes_reservados << " bytes reservado(s)):\n";

    for (size_t i = 0; i < bytes_reservados; ++i) {
        std::cout << "Byte " << i << ": [";
        for (int bit = 7; bit >= 0; --bit) {
            size_t bit_global = (i * 8) + bit;
            if (bit_global < bits_utilizados) {
                // Imprime el bit válido del tablero
                std::cout << ((tablero[i] >> bit) & 1);
            } else {
                // Imprime 'X' o '0' para indicar bits de relleno sobrantes a la izquierda
                std::cout << ".";
            }
        }
        std::cout << "] ";
    }
    std::cout << "\n\n";
}
