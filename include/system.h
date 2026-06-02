#ifndef SYSTEM_H
#define SYSTEM_H

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#else
  #error "Esta versão (fibers) foi preparada para Windows (_WIN32)."
#endif

typedef void (*proc_fn)(void *arg);

typedef struct {
    LPVOID  fiber;
    proc_fn proc;
    void   *arg;
} descritor;

typedef descritor* PTR_DESC;

void system_init_main(PTR_DESC d_main);
PTR_DESC cria_desc(void);
void newprocess(proc_fn proc, void *arg, PTR_DESC d);
void transfer(PTR_DESC origem, PTR_DESC destino);
void deleta_desc(PTR_DESC d);

#endif
