#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nucleo.h" 

void cria_processo(void (*end_proc)(void), const char *nome_p) {
    PTR_DESC_PROC novo_proc = (PTR_DESC_PROC)malloc(sizeof(PTR_DESC_PROC));
    strncpy(novo_proc->nome, nome_p, sizeof(novo_proc->nome) - 1);
    novo_proc->estado = ATIVO;
    novo_proc->codigo = end_proc;
    novo_proc->fila_sem = NULL;
    novo_proc->prox_desc = cria_desc();
    newprocess(processo_trampolim, novo_proc, novo_proc->prox_desc);

};

static void processo_trampolim(void *arg){
    PTR_DESC_PROC processo_desc = (PTR_DESC_PROC)arg;
    if (processo_desc != NULL && processo_desc->codigo != NULL) {
        processo_desc->codigo();
        termina_processo(processo_desc);
    }
}

void termina_processo(void) {
    if () {}
    // if (proc_desc != NULL) {
    //     proc_desc->estado = TERMINADO;
    //     free(proc_desc->contexto);
    //     free(proc_desc);
    // }
}

void inicia_fila_prontos(void) {
    prim = NULL;
    atual = NULL;
}