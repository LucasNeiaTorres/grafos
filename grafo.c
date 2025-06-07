#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "grafo.h"

#define MAX_LINE 2048
#define MAX_NAME 128

typedef struct aresta_t {
    char destino[MAX_NAME];
    int peso;
    struct aresta_t *prox;
} aresta_t;

typedef struct vertice_t {
    char nome[MAX_NAME];
    aresta_t *adj;
    struct vertice_t *prox;
} vertice_t;

typedef struct grafo {
    char nome[MAX_NAME];
    vertice_t *vertices;
} grafo;


static char *le_nome(FILE *f) {
    char nome[MAX_NAME];
    char *strnome = malloc(MAX_NAME);
    while(fgets(nome, sizeof(nome), f)) {
        if (nome[0] != '/' && nome[1] != '/' && nome[0] != '\n' && nome[0] != '\0') {
            strncpy(strnome, nome, MAX_NAME);
            return strnome;
        }
    }
    // Retorna NULL se não encontrar um nome válido
    return NULL; 
}

static vertice_t *encontra_ou_cria_vertice(grafo *g, const char *nome) {
    vertice_t *v = g->vertices;
    while (v) {
        if (strcmp(v->nome, nome) == 0) return v;
        v = v->prox;
    }

    // cria novo vértice
    vertice_t *novo = malloc(sizeof(vertice_t));
    strcpy(novo->nome, nome);
    novo->adj = NULL;
    novo->prox = g->vertices;
    g->vertices = novo;
    return novo;
}

static void printa_grafo(grafo *g) {
    if (!g) {
        printf("Grafo vazio.\n");
        return;
    }
    
    printf("Grafo: %s\n", g->nome);
    vertice_t *v = g->vertices;
    while (v) {
        printf("Vértice: %s\n", v->nome);
        aresta_t *a = v->adj;
        while (a) {
            printf("  Aresta para %s com peso %d\n", a->destino, a->peso);
            a = a->prox;
        }
        v = v->prox;
    }
}

static void adiciona_aresta(grafo *g, const char *origem, const char *destino, int peso) {
    vertice_t *v = encontra_ou_cria_vertice(g, origem);
    aresta_t *a = malloc(sizeof(aresta_t));
    strcpy(a->destino, destino);
    a->peso = peso;
    a->prox = v->adj;
    v->adj = a;
}

grafo *le_grafo(FILE *f) {
    grafo *g = malloc(sizeof(grafo));
    if (!g) {
        fprintf(stderr, "Erro ao alocar memória para o grafo.\n");
        return NULL;
    }

    char *nome = le_nome(f);
    if (!nome) {
        fprintf(stderr, "Erro ao ler o nome do grafo.\n");
        free(g);
        return NULL;
    }
    
    strncpy(g->nome, nome, MAX_NAME);
    free(nome);
    
    g->vertices = NULL;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "//", 2) == 0) continue;
        if (strlen(line) > 0 && line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';

        char v1[MAX_NAME], v2[MAX_NAME];
        int peso;
        if (sscanf(line, "%s -- %s %d", v1, v2, &peso) >= 2) {
            if (sscanf(line, "%*s -- %*s %d", &peso) != 1) 
                peso = 1; // peso opcional
            adiciona_aresta(g, v1, v2, peso);
            adiciona_aresta(g, v2, v1, peso); 
        } else if (sscanf(line, "%s", v1) == 1) {
            encontra_ou_cria_vertice(g, line);
        } else {
            continue;
        }
    }

    printa_grafo(g);
    return g;
}

//------------------------------------------------------------------------------
// desaloca toda a estrutura de dados alocada em g
//
// devolve 1 em caso de sucesso e 0 em caso de erro

unsigned int destroi_grafo(grafo *g) {
    if (!g) return 0;

    vertice_t *v = g->vertices;
    while (v) {
        aresta_t *a = v->adj;
        while (a) {
            aresta_t *temp = a;
            a = a->prox;
            free(temp);
        }
        vertice_t *temp = v;
        v = v->prox;
        free(temp);
    }
    
    free(g);
    return 1; // sucesso
}

//------------------------------------------------------------------------------
// devolve o nome de g

char *nome(grafo *g) {
    return g ? g->nome : NULL;
}

//------------------------------------------------------------------------------
// devolve 1 se g é bipartido e 0 caso contrário

unsigned int bipartido(grafo *g) {
    // Implementação da verificação de bipartição
    // Retorna 1 se bipartido, 0 caso contrário
    return 0; // Placeholder
}

//------------------------------------------------------------------------------
// devolve o número de vértices em g

unsigned int n_vertices(grafo *g) {
    // Implementação para contar o número de vértices
    // Retorna o número de vértices
    return 0; // Placeholder
}

//------------------------------------------------------------------------------
// devolve o número de arestas em g

unsigned int n_arestas(grafo *g) {
    return 0; // Placeholder
}

//------------------------------------------------------------------------------
// devolve o número de componentes em g

unsigned int n_componentes(grafo *g) {
    // Implementação para contar o número de componentes
    // Retorna o número de componentes
    return 0; // Placeholder
}

//------------------------------------------------------------------------------
// devolve uma "string" com os diâmetros dos componentes de g separados por brancos
// em ordem não decrescente

char *diametros(grafo *g) {
    // Implementação para calcular os diâmetros dos componentes
    // Retorna uma string com os diâmetros separados por brancos
    return NULL; // Placeholder
}

//------------------------------------------------------------------------------
// devolve uma "string" com os nomes dos vértices de corte de g em
// ordem alfabética, separados por brancos

char *vertices_corte(grafo *g) {
    // Implementação para encontrar os vértices de corte
    // Retorna uma string com os nomes dos vértices de corte separados por brancos
    return NULL; // Placeholder
}

//------------------------------------------------------------------------------
// devolve uma "string" com as arestas de corte de g em ordem alfabética, separadas por brancos
// cada aresta é o par de nomes de seus vértices em ordem alfabética, separadas por brancos
//
// por exemplo, se as arestas de corte são {z, a}, {x, b} e {y, c}, a resposta será a string
// "a z b x c y"

char *arestas_corte(grafo *g) {
    return NULL; // Placeholder
}
