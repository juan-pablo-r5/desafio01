#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <cstddef>

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

void imprimir_tira_binaria(const unsigned char* tablero, size_t bytes_reservados, int filas, int cols);

// --- NUEVAS FUNCIONES: MOTOR DE JUEGO Y CASCADAS ---
bool detectar_y_marcar_combinaciones(const unsigned char* tablero, int filas, int cols, bool* eliminados);
int aplicar_gravedad_y_relleno(unsigned char* tablero, int filas, int cols, const bool* eliminados);
void procesar_cascadas(unsigned char* tablero, int filas, int cols, int& puntaje, int& total_fichas_destruidas, int& combinaciones, int& cascadas);
void eliminar_ficha_usuario(unsigned char* tablero, int filas, int cols, int fila_sel, int col_sel, int& puntaje, int& total_fichas_destruidas, int& combinaciones, int& cascadas);

void mostrar_tablero(const unsigned char* tablero, int filas, int cols);


void registrar_estado_memoria(const unsigned char* tablero_actual, size_t bytes_reservados, const char* nombre_archivo);

void leer_registro_historial(unsigned char* tablero_actual, size_t bytes_reservados, const char* nombre_archivo);

void exportar_reporte_bits(const unsigned char* tablero, size_t bytes_reservados, int filas, int cols, const char* nombre_archivo);

#endif // FUNCIONES_H