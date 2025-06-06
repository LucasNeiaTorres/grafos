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


grafo *le_grafo(FILE *f) {
    grafo *g = malloc(sizeof(grafo));
    if (!g) {
        fprintf(stderr, "Erro ao alocar memória para o grafo.\n");
        return NULL;
    }

    // procura o nome do grafo na primeira linha do arquivo que não é um comentário ou vazia
    while (fgets(g->nome, sizeof(g->nome), f)) {
        if (g->nome[0] != '/' && g->nome[1] != '/' && g->nome[0] != '\n') {
            break; // Encontrou uma linha válida
        }
    }
    if (g->nome[0] == '\0') {
        fprintf(stderr, "Erro ao ler o nome do grafo ou arquivo vazio.\n");
        free(g);
        return NULL;
    }
    
    // Remove o newline do nome do grafo
    size_t len = strlen(g->nome);
    if (len > 0 && g->nome[len - 1] == '\n') {
        g->nome[len - 1] = '\0';
    }

    g->vertices = NULL;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        char nome_vertice[MAX_NAME];

        if (sscanf(line, "%s", nome_vertice) != 1) 
            continue;
        
        if( nome_vertice[0] == '\0' || nome_vertice[0] == '/') 
            continue; 

        vertice_t *v = malloc(sizeof(vertice_t));
        if (!v) {
            fprintf(stderr, "Erro ao alocar memória para o vértice.\n");
            free(g);
            return NULL;
        }
        strncpy(v->nome, nome_vertice, MAX_NAME);
        printf("Lendo vértice: %s\n", v->nome);
        v->adj = NULL;
        v->prox = g->vertices;
        g->vertices = v;

        char destino[MAX_NAME];
        int peso;
        while (sscanf(line, "%s %d", destino, &peso) == 2) {
            aresta_t *a = malloc(sizeof(aresta_t));
            if (!a) {
                fprintf(stderr, "Erro ao alocar memória para a aresta.\n");
                free(v);
                free(g);
                return NULL;
            }
            strncpy(a->destino, destino, MAX_NAME);
            a->peso = peso;
            a->prox = v->adj;
            v->adj = a;

            // Ler a próxima linha
            if (!fgets(line, sizeof(line), f)) {
                break; // Fim do arquivo
            }
        }
    }

    return g;
}

//------------------------------------------------------------------------------
// desaloca toda a estrutura de dados alocada em g
//
// devolve 1 em caso de sucesso e 0 em caso de erro

unsigned int destroi_grafo(grafo *g) {
    return g ? (free(g), 1) : 0;
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
