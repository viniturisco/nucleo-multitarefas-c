#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "system.h"
#include "nucleo.h"
#include "semaforo.h"

/* Buffer circular compartilhado */
#define TAM_BUFFER 5
int buffer[TAM_BUFFER];
int in = 0;   /* índice para inserção */
int out = 0;  /* índice para retirada */

/* Semáforos de sincronização */
semaforo cheio;   /* conta itens no buffer (inicialmente 0) */
semaforo vazio;   /* conta espaços vazios (inicialmente TAM_BUFFER) */
semaforo mutex;   /* exclusão mútua para acesso ao buffer */

/* Contador de itens produzidos e consumidos para teste */
static int item_id = 0;

/* Função auxiliar para depositar no buffer */
void deposita(int valor) {
  buffer[in] = valor;
  in = (in + 1) % TAM_BUFFER;
  printf("[Produtor] Depositou item %d na posicao %d. Buffer: ", valor, (in - 1 + TAM_BUFFER) % TAM_BUFFER);
  for (int i = 0; i < TAM_BUFFER; i++) {
    printf("%d ", buffer[i]);
  }
  printf("\n");
}

/* Função auxiliar para retirar do buffer */
int retira(void) {
  int valor = buffer[out];
  out = (out + 1) % TAM_BUFFER;
  printf("[Consumidor] Retirou item %d da posicao %d. Buffer: ", valor, (out - 1 + TAM_BUFFER) % TAM_BUFFER);
  for (int i = 0; i < TAM_BUFFER; i++) {
    printf("%d ", buffer[i]);
  }
  printf("\n");
  return valor;
}

/* Processo Produtor */
void produtor(void) {
  int i = 0;
  printf("[Produtor] Iniciado\n");

  while (i < 10) {
    /* Aguarda um espaço livre no buffer */
    P(&vazio);

    /* Garante exclusão mútua ao acessar o buffer */
    P(&mutex);

    /* Produz e deposita o item */
    int item = item_id++;
    deposita(item);

    /* Libera exclusão mútua */
    V(&mutex);

    /* Avisa que há um item pronto */
    V(&cheio);

    /* Cede a CPU voluntariamente */
    yield();

    i++;
  }

  printf("[Produtor] Finalizado\n");
}

/* Processo Consumidor */
void consumidor(void) {
  int i = 0;
  int valor;
  printf("[Consumidor] Iniciado\n");

  while (i < 10) {
    /* Aguarda um item disponível no buffer */
    P(&cheio);

    /* Garante exclusão mútua ao acessar o buffer */
    P(&mutex);

    /* Consome e retira o item */
    valor = retira();

    /* Libera exclusão mútua */
    V(&mutex);

    /* Avisa que há um espaço livre */
    V(&vazio);

    /* Exibe o item consumido */
    printf("[Consumidor] Consumiu item: %d\n", valor);

    /* Cede a CPU voluntariamente */
    yield();

    i++;
  }

  printf("[Consumidor] Finalizado\n");
}

/* Programa principal */
int main(void) {
  printf("=== Sistema Multitarefa - Problema Produtor/Consumidor ===\n");
  printf("Tamanho do buffer circular: %d\n\n", TAM_BUFFER);

  /* Inicializa as estruturas internas do núcleo */
  inicia_fila_prontos();

  /* Inicializa o buffer */
  for (int i = 0; i < TAM_BUFFER; i++) {
    buffer[i] = 0;
  }

  /* Inicializa os semáforos */
  inicia_semaforo(&cheio, 0);      /* nenhum item no início */
  inicia_semaforo(&vazio, TAM_BUFFER); /* todos os espaços livres */
  inicia_semaforo(&mutex, 1);      /* exclusão mútua */

  /* Cria os processos produtor e consumidor */
  cria_processo(produtor, "Produtor");
  cria_processo(consumidor, "Consumidor");

  printf("Sistema pronto. Iniciando escalonador cooperativo...\n\n");

  /* Converte o main em Fiber e transfere o controle para o núcleo */
  dispara_sistema();

  printf("\n=== Sistema finalizado com sucesso ===\n");
  return 0;
}