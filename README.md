# 🚀 Núcleo Multitarefas Cooperativo em C

Este projeto consiste no desenvolvimento de um **Núcleo (Kernel) Multitarefas Cooperativo** de nível de usuário para Windows. Ele simula a execução concorrente de múltiplos processos em uma única CPU através da técnica de co-rotinas (utilizando a API nativa de **Fibers** do Windows), com suporte a escalonamento Round-Robin, ciclo de vida de processos e semáforos de Dijkstra.

O repositório foi construído para a disciplina de **Sistemas Operacionais** do *Instituto Federal de Educação, Ciência e Tecnologia de São Paulo (IFSP) - Campus Salto*.

---

## 🛠️ O que o projeto faz

*   **Troca de Contexto Manual:** Executada em espaço de usuário usando a API Windows Fibers (`CreateFiber`, `SwitchToFiber`), salvando/restaurando registradores e pilhas de execução de forma eficiente.
*   **Escalonamento Cooperativo (Round-Robin):** Os processos cedem voluntariamente a CPU chamando `yield()` ou ao serem bloqueados por falta de recursos.
*   **Semáforos de Dijkstra (P e V):** Controle de concorrência com uma fila lógica de bloqueados interna aos semáforos, evitando inserções/remoções físicas dispendiosas na fila de prontos.
*   **Validação (Produtor/Consumidor):** Simulação clássica concorrente atuando sobre um buffer circular compartilhado.

---

## 🏗️ Como Funciona a Arquitetura

O sistema é dividido em quatro camadas estruturadas:

1.  **Suporte para Co-rotinas (`system.c` / `system.h`):** Interface física que inicializa o fluxo principal do Windows como Fiber (`system_init_main`), cria novas rotinas (`newprocess`) e efetua a troca física de registradores (`transfer`).
2.  **Núcleo Multitarefas (`nucleo.c` / `nucleo.h`):** Gerencia a fila circular de processos através de estruturas **Process Control Block (PCB)** (`DESCRITOR_PROC`). Inclui um invólucro (*trampolim*) para garantir o encerramento seguro e liberação do processo ao término de sua função.
3.  **Semáforos (`semaforo.c` / `semaforo.h`):** Primitivas `P()` e `V()`. Processos bloqueados alteram seu estado para `BLOQ_P` e são encadeados na fila do semáforo por meio de ponteiros lógicos, sendo ignorados temporariamente pelo escalonador.
4.  **Aplicação (`main.c`):** Executa os processos concorrentes `produtor` e `consumidor` sincronizados por semáforos (`mutex`, `vazio` e `cheio`).

---

## 🧠 Conceitos Aprendidos

*   **Gerenciamento de Contexto:** Como pilhas de execução e registradores de CPU são congelados e restaurados manualmente.
*   **Multiprogramação Cooperativa:** O funcionamento de um escalonador em espaço de usuário sem interrupções de hardware (timer ticks).
*   **Ciclo de Vida de Processos:** Estados de execução (Ativo, Bloqueado e Terminado) e controle de transições em filas encadeadas circulares.
*   **Sincronização e Regiões Críticas:** Resolução de condições de corrida, implementação de exclusão mútua e tratamento/evitação de *Deadlocks*.

---

## 📂 Estrutura do Repositório

*   `docs/` — Documentação e especificações originais do projeto.
*   `include/` — Cabeçalhos com definições de tipos e funções:
    *   [system.h](include/system.h) — Controle físico das Fibers.
    *   [nucleo.h](include/nucleo.h) — Estruturas do PCB e funções de escalonamento.
    *   [semaforo.h](include/semaforo.h) — Primitivas do semáforo de Dijkstra.
    *   [pausa.h](include/pausa.h) — Utilitários de temporização.
*   `src/` — Implementação do código:
    *   [system.c](src/system.c) — Integração com a API Windows Fibers.
    *   [nucleo.c](src/nucleo.c) — Escalonador e gerenciamento de processos.
    *   [semaforo.c](src/semaforo.c) — Lógica das operações P e V.
    *   [pausa.c](src/pausa.c) — Funções de delay.
    *   [main.c](src/main.c) — Simulação do Produtor/Consumidor.

---

## 🚀 Como Executar o Projeto

### Pré-requisitos
*   Sistema Operacional: **Windows** (necessário para a API nativa de Fibers).
*   Compilador: **GCC** (ex: MinGW-w64).

### Compilação e Execução
No terminal da raiz do projeto, execute:
```bash
# Compilar o projeto
gcc -Iinclude src/system.c src/nucleo.c src/semaforo.c src/pausa.c src/main.c -o nucleo.exe

# Executar a simulação
.\nucleo.exe
```