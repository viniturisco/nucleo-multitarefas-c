#ifndef SEMAFORO_H
#define SEMAFORO_H

#include "nucleo.h"

/* Estrutura do semáforo segundo Dijkstra */
typedef struct {
  int s;                    /* contador de recursos */
  PTR_DESC_PROC Q;          /* fila de processos bloqueados */
} semaforo;

typedef semaforo* PTR_SEMAFORO;

/* Inicializa um semáforo com valor inicial n */
void inicia_semaforo(semaforo *sem, int n);

/* Operação P (Down/Solicitar) */
void P(semaforo *sem);

/* Operação V (Up/Liberar) */
void V(semaforo *sem);

#endif