#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "grafo.h"

typedef struct grafo {
    char nome[2048];
    // lista ou matriz de adjacencia

} grafo;

grafo *le_grafo(FILE *f) {
    grafo *g = malloc(sizeof(grafo));
    if (!g) {
        fprintf(stderr, "Erro ao alocar memória para o grafo.\n");
        return NULL;
    }
    if (fgets(g->nome, sizeof(g->nome), f) == NULL) {
        fprintf(stderr, "Erro ao ler o nome do grafo.\n");
        free(g);
        return NULL;
    }
    // Remove newline character if present
    return;
}