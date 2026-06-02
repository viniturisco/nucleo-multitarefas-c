#include "semaforo.h"
#include "nucleo.h"
#include "system.h"
#include <stdio.h>
#include <stdlib.h>

void inicia_semaforo(semaforo *sem, int n) {
  if (!sem)
    return;
  sem->s = n;
  sem->Q = NULL;
}

void P(semaforo *sem) {
  PTR_DESC_PROC aux;
  PTR_DESC_PROC prox;

  if (!sem)
    return;

  if (!atual) {
    fprintf(stderr, "P: nenhum processo atual.\n");
    exit(1);
  }

  if (sem->s > 0) {
    sem->s--;
    return;
  }

  atual->estado = BLOQ_P;

  atual->fila_sem = NULL;
  if (!sem->Q) {
    sem->Q = atual;
  } else {
    aux = sem->Q;
    while (aux->fila_sem)
      aux = aux->fila_sem;
    aux->fila_sem = atual;
  }

  prox = NULL;
  if (prim) {
    PTR_DESC_PROC scan = atual->prox_desc;
    while (scan && scan != atual) {
      if (scan->estado == ATIVO) {
        prox = scan;
        break;
      }
      scan = scan->prox_desc;
    }
    if (!prox && atual->estado == ATIVO)
      prox = atual;
  }

  if (!prox) {
    fprintf(stderr, "Deadlock: todos os processos estao bloqueados em P().\n");
    exit(1);
  }

  yield();
}

void V(semaforo *sem) {
  PTR_DESC_PROC p;

  if (!sem)
    return;

  if (sem->Q == NULL) {
    sem->s++;
  } else {
    p = sem->Q;
    sem->Q = p->fila_sem;
    p->fila_sem = NULL;

    p->estado = ATIVO;
  }
}
