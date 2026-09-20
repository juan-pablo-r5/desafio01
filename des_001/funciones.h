#ifndef FUNCIONES_H
#define FUNCIONES_H
#include <cstdlib>
#include <fstream>
#include <iostream>
using namespace std;
#include <cstddef>

unsigned char obtener_ficha(const unsigned char* tablero, int fila, int col, int cols);
void fijar_ficha(unsigned char* tablero, int fila, int col, int cols, unsigned char valor);
unsigned char* crear_tablero(int filas, int cols, size_t& bytes_reservados);
void inicializar_tablero_aleatorio(unsigned char* tablero, int filas, int cols);
unsigned char* agregar_linea(unsigned char* tablero, int& filas, int& cols, size_t& bytes_reservados, int pos, bool es_fila);
unsigned char* eliminar_linea(unsigned char* tablero, int& filas, int& cols, size_t& bytes_reservados, int pos, bool es_fila);

void imprimir_tira_binaria(const unsigned char* tablero, size_t bytes_reservados, int filas, int cols);

bool detectar_y_marcar_combinaciones(const unsigned char* tablero, int filas, int cols, bool* eliminados);
int aplicar_gravedad_y_relleno(unsigned char* tablero, int filas, int cols, const bool* eliminados);
void procesar_cascadas(unsigned char* tablero, int filas, int cols, int& puntaje, int& total_fichas_destruidas, int& combinaciones, int& cascadas);
void eliminar_ficha_usuario(unsigned char* tablero, int filas, int cols, int fila_sel, int col_sel, int& puntaje, int& total_fichas_destruidas, int& combinaciones, int& cascadas);

void mostrar_tablero(const unsigned char* tablero, int filas, int cols);


void registrar_estado_memoria(const unsigned char* tablero_actual, size_t bytes_reservados, const char* nombre_archivo);
void exportar_reporte_bits(const unsigned char* tablero, size_t bytes_reservados, int filas, int cols, const char* nombre_archivo);
bool guardar_partida_txt(const char* nombre_archivo, const unsigned char* tablero, int filas, int cols, size_t bytes_reservados, int puntaje, int elim_usr, int dest_fichas, int comb, int casc);
unsigned char* cargar_partida_txt(const char* nombre_archivo, int& filas, int& cols, size_t& bytes_reservados, int& puntaje, int& elim_usr, int& dest_fichas, int& comb, int& casc);

#endif // FUNCIONES_H

