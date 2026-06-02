#ifndef NUCLEO_H
#define NUCLEO_H
#include "system.h"

typedef enum { ATIVO, BLOQ_P, TERMINADO } ESTADO_PROC;

typedef struct desc_p {
  char nome[35];
  ESTADO_PROC estado;
  PTR_DESC contexto;
  struct desc_p *fila_sem;
  struct desc_p *prox_desc;
  void (*codigo)(void);
} DESCRITOR_PROC;
typedef DESCRITOR_PROC *PTR_DESC_PROC;

extern PTR_DESC_PROC prim;
extern PTR_DESC_PROC atual;

void inicia_fila_prontos(void);
void cria_processo(void (*end_proc)(void), const char *nome_p);
void dispara_sistema(void);
void yield(void);
void termina_processo(void);
void deleta_fila_processos(void);
static PTR_DESC_PROC proximo_ativo_depois(PTR_DESC_PROC a_partir);

#endif