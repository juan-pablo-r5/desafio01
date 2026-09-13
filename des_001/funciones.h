#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <iostream>
using namespace std;


#include <cstddef> // Para size_t

// --- MANIPULACIÓN A NIVEL DE BITS ---
// Extrae el valor de 3 bits de la celda (fila, col)
unsigned char obtener_ficha(const unsigned char* tablero, int fila, int col, int cols);

// Modifica los 3 bits de la celda (fila, col) con el nuevo valor (0 a 7)
void fijar_ficha(unsigned char* tablero, int fila, int col, int cols, unsigned char valor);


// --- GESTIÓN DE MEMORIA DINÁMICA ---
// Reserva el bloque de bytes inicial necesario para el tablero
unsigned char* crear_tablero(int filas, int cols, size_t& bytes_reservados);

// Llena el tablero con fichas aleatorias uniformes (000 a 101)
void inicializar_tablero_aleatorio(unsigned char* tablero, int filas, int cols);


// --- MODIFICACIÓN ESTRUCTURAL Y REGLA DEL 65% ---
// Inserta una fila o columna en una posición intermedia
unsigned char* agregar_linea(unsigned char* tablero, int& filas, int& cols, size_t& bytes_reservados, int pos, bool es_fila);

// Elimina una fila o columna y evalúa la regla del 65% para liberar/redimensionar memoria física
unsigned char* eliminar_linea(unsigned char* tablero, int& filas, int& cols, size_t& bytes_reservados, int pos, bool es_fila);

// CAIDA Y REEMPLAZO

// Desplaza las fichas hacia abajo si hay espacios vacíos
void aplicar_gravedad(unsigned char* tablero, int filas, int cols);

// Llena los huecos superiores que quedaron vacíos tras la caída con fichas nuevas
void rellenar_fichas_superiores(unsigned char* tablero, int filas, int cols);

#endif // FUNCIONES_H
