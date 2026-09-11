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
