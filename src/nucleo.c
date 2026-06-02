#include "nucleo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PTR_DESC_PROC prim = NULL;
PTR_DESC_PROC atual = NULL;

static descritor main_desc;
static PTR_DESC main_ctx = &main_desc;
static int main_ready = 0;

static void processo_trampolim(void *arg);
static PTR_DESC_PROC proximo_ativo_depois(PTR_DESC_PROC a_partir);

static void processo_trampolim(void *arg) {
  PTR_DESC_PROC processo_desc = (PTR_DESC_PROC)arg;
  if (processo_desc != NULL && processo_desc->codigo != NULL) {
    processo_desc->codigo();
    termina_processo();
  }
}

void inicia_fila_prontos(void) {
  prim = NULL;
  atual = NULL;
}

void cria_processo(void (*end_proc)(void), const char *nome_p) {
  PTR_DESC_PROC novo_proc = (PTR_DESC_PROC)malloc(sizeof(DESCRITOR_PROC));
  if (!novo_proc) {
    fprintf(stderr, "cria_processo: malloc falhou\n");
    exit(1);
  }

  strncpy(novo_proc->nome, nome_p, sizeof(novo_proc->nome) - 1);
  novo_proc->nome[sizeof(novo_proc->nome) - 1] = '\0';
  novo_proc->estado = ATIVO;
  novo_proc->codigo = end_proc;
  novo_proc->fila_sem = NULL;
  novo_proc->contexto = cria_desc();
  newprocess(processo_trampolim, novo_proc, novo_proc->contexto);

  if (prim == NULL) {
    prim = novo_proc;
    novo_proc->prox_desc = novo_proc;
  } else {
    PTR_DESC_PROC ultimo = prim;
    while (ultimo->prox_desc != prim) {
      ultimo = ultimo->prox_desc;
    }
    ultimo->prox_desc = novo_proc;
    novo_proc->prox_desc = prim;
  }
}

static PTR_DESC_PROC proximo_ativo_depois(PTR_DESC_PROC a_partir) {
  if (prim == NULL)
    return NULL;

  if (a_partir == NULL)
    a_partir = prim;

  PTR_DESC_PROC iter = a_partir->prox_desc;

  while (iter != a_partir) {
    if (iter->estado == ATIVO) {
      return iter;
    }
    iter = iter->prox_desc;
  }

  if (a_partir->estado == ATIVO) {
    return a_partir;
  }

  return NULL;
}

void yield(void) {
  PTR_DESC_PROC prox;

  if (atual == NULL)
    return;

  prox = proximo_ativo_depois(atual);

  if (prox != NULL && prox != atual) {
    PTR_DESC_PROC antigo = atual;
    atual = prox;
    transfer(antigo->contexto, atual->contexto);
  }
}

void termina_processo(void) {
  PTR_DESC_PROC antigo;
  PTR_DESC_PROC prox;

  if (atual == NULL) {
    return;
  }

  atual->estado = TERMINADO;
  prox = proximo_ativo_depois(atual);

  if (prox != NULL && prox != atual) {
    antigo = atual;
    atual = prox;
    transfer(antigo->contexto, atual->contexto);
  } else {
    if (main_ready) {
      transfer(atual->contexto, main_ctx);
    } else {
      fprintf(stderr, "Erro: nenhum processo ativo e main nao pronto. Encerrando.\n");
      exit(1);
    }
  }
}

void dispara_sistema(void) {
  if (prim == NULL) {
    return;
  }

  system_init_main(main_ctx);
  main_ready = 1;

  if (prim->estado == ATIVO) {
    atual = prim;
  } else {
    atual = proximo_ativo_depois(prim);
  }

  if (atual != NULL) {
    transfer(main_ctx, atual->contexto);
  } else {
    fprintf(stderr, "Erro: nenhum processo ativo encontrado.\n");
    exit(1);
  }
}

void deleta_fila_processos(void) {
  if (prim == NULL)
    return;

  PTR_DESC_PROC iter = prim;
  PTR_DESC_PROC temp;

  do {
    temp = iter->prox_desc;
    
    if (iter->contexto != NULL) {
      deleta_desc(iter->contexto);
      iter->contexto = NULL;
    }

    free(iter);

    iter = temp;
  } while (iter != prim);

  prim = NULL;
  atual = NULL;
}