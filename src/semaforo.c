#include "semaforo.h"
#include "nucleo.h"
#include "system.h"
#include <stdio.h>
#include <stdlib.h>

/* Inicializa um semáforo com valor inicial n */
void inicia_semaforo(semaforo *sem, int n) {
  if (!sem)
    return;
  sem->s = n;
  sem->Q = NULL;
}

/* Operação P (Down/Solicitar) - bloqueia se recurso indisponível */
void P(semaforo *sem) {
  PTR_DESC_PROC aux;
  PTR_DESC_PROC prox;

  if (!sem)
    return;

  if (!atual) {
    fprintf(stderr, "P: nenhum processo atual.\n");
    exit(1);
  }

  /* Se recurso disponível, apenas decrementa e retorna */
  if (sem->s > 0) {
    sem->s--;
    return;
  }

  /* Recurso indisponível - bloqueia o processo atual */
  atual->estado = BLOQ_P;

  /* Insere o processo no final da fila de espera do semáforo */
  atual->fila_sem = NULL;
  if (!sem->Q) {
    sem->Q = atual;
  } else {
    aux = sem->Q;
    while (aux->fila_sem)
      aux = aux->fila_sem;
    aux->fila_sem = atual;
  }

  /* Procura um processo ativo para rodar, senão haverá deadlock */
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

  /* Cede a CPU para outro processo ativo */
  yield();
}

/* Operação V (Up/Liberar) - libera recurso ou acorda processo */
void V(semaforo *sem) {
  PTR_DESC_PROC p;

  if (!sem)
    return;

  /* Se nenhum processo está esperando, apenas incrementa o contador */
  if (sem->Q == NULL) {
    sem->s++;
  } else {
    /* Remove o primeiro processo da fila de bloqueados */
    p = sem->Q;
    sem->Q = p->fila_sem;
    p->fila_sem = NULL;

    /* Muda o estado do processo para ATIVO */
    p->estado = ATIVO;
  }

  /* Nota: O processo atual não perde a CPU imediatamente.
     Ele continua até chamar yield() ou P() */
}
