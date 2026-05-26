#ifndef NUCLEO_H
#define NUCLEO_H
#include "system.h" 

PTR_DESC_PROC prim = NULL;
PTR_DESC_PROC atual = NULL;

static descritor main_desc;
static PTR_DESC main_ctz = &main_desc;
static int main_ready = 0;


typedef enum {
    ATIVO,
    BLOQ_P,
    TERMINADO
} ESTADO_PROC;

typedef struct desc_p{
    char nome[35];
    ESTADO_PROC estado;
    PTR_DESC contexto;
    struct desc_p *fila_sem;
    struct desc_p *prox_desc;
    void (*codigo)(void);
} DESCRITOR_PROC;
typedef DESCRITOR_PROC* PTR_DESC_PROC;



#endif