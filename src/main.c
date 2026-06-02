#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "system.h"
#include "nucleo.h"
#include "semaforo.h"

#define TAM_BUFFER 5
int buffer[TAM_BUFFER];
int in = 0;
int out = 0;

semaforo cheio;
semaforo vazio;
semaforo mutex;

static int item_id = 0;

void deposita(int valor) {
  buffer[in] = valor;
  in = (in + 1) % TAM_BUFFER;
  printf("[Produtor] Depositou item %d na posicao %d. Buffer: ", valor, (in - 1 + TAM_BUFFER) % TAM_BUFFER);
  for (int i = 0; i < TAM_BUFFER; i++) {
    printf("%d ", buffer[i]);
  }
  printf("\n");
}

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

void produtor(void) {
  int i = 0;
  printf("[Produtor] Iniciado\n");

  while (i < 10) {
    P(&vazio);

    P(&mutex);

    int item = item_id++;
    deposita(item);

    V(&mutex);

    V(&cheio);

    yield();

    i++;
  }

  printf("[Produtor] Finalizado\n");
}

void consumidor(void) {
  int i = 0;
  int valor;
  printf("[Consumidor] Iniciado\n");

  while (i < 10) {
    P(&cheio);

    P(&mutex);

    valor = retira();

    V(&mutex);

    V(&vazio);

    printf("[Consumidor] Consumiu item: %d\n", valor);

    yield();

    i++;
  }

  printf("[Consumidor] Finalizado\n");
}

int main(void) {
  printf("=== Sistema Multitarefa - Problema Produtor/Consumidor ===\n");
  printf("Tamanho do buffer circular: %d\n\n", TAM_BUFFER);

  inicia_fila_prontos();

  for (int i = 0; i < TAM_BUFFER; i++) {
    buffer[i] = 0;
  }

  inicia_semaforo(&cheio, 0);
  inicia_semaforo(&vazio, TAM_BUFFER);
  inicia_semaforo(&mutex, 1);

  cria_processo(produtor, "Produtor");
  cria_processo(consumidor, "Consumidor");

  printf("Sistema pronto. Iniciando escalonador cooperativo...\n\n");

  dispara_sistema();
  deleta_fila_processos();

  printf("\n Sistema finalizado! \n");
  return 0;
}