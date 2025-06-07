#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdbool.h>
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
    int cor; 
} vertice_t;

typedef struct grafo {
    char nome[MAX_NAME];
    vertice_t *vertices;
} grafo;

#define COR_1 1
#define COR_2 2
#define COR_NAO_VISITADA -1

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
                peso = 1; 
            adiciona_aresta(g, v1, v2, peso);
            adiciona_aresta(g, v2, v1, peso); 
        } else if (sscanf(line, "%s", v1) == 1) {
            encontra_ou_cria_vertice(g, line);
        } else {
            continue;
        }
    }

    return g;
}

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
    return 1;
}

char *nome(grafo *g) {
    return g ? g->nome : NULL;
}

/* se tem ciclo impar = 0
    se é acíclico = 1
*/
unsigned int bipartido(grafo *g) {
    if (!g) return 1;

    // Inicializa todas as cores como não visitadas
    for (vertice_t *v = g->vertices; v; v = v->prox) 
        v->cor = COR_NAO_VISITADA;

    // Para cada componente desconexo do grafo
    for (vertice_t *v_inicio = g->vertices; v_inicio; v_inicio = v_inicio->prox) {
        if (v_inicio->cor != COR_NAO_VISITADA) continue;

        // BFS manual com fila
        vertice_t *fila[1024];
        int ini = 0, fim = 0;

        v_inicio->cor = COR_1;
        fila[fim++] = v_inicio;

        while (ini < fim) {
            vertice_t *v_atual = fila[ini++];
            int cor_atual = v_atual->cor;
            int cor_vizinho = (cor_atual == COR_1) ? COR_2 : COR_1;

            for (aresta_t *a = v_atual->adj; a; a = a->prox) {
                // Achar o vértice de destino pelo nome
                vertice_t *v_destino = g->vertices;
                while (v_destino && strcmp(v_destino->nome, a->destino) != 0) {
                    v_destino = v_destino->prox;
                }

                if (!v_destino) continue; // vértice não encontrado

                if (v_destino->cor == COR_NAO_VISITADA) {
                    v_destino->cor = cor_vizinho;
                    fila[fim++] = v_destino;
                } else if (v_destino->cor == cor_atual) {
                    // Encontramos dois vértices adjacentes com mesma cor => ciclo ímpar
                    return 0;
                }
            }
        }
    }

    return 1;
}

unsigned int n_vertices(grafo *g) {
    unsigned int num_vertices = 0;
    if (!g) return num_vertices;

    vertice_t *v = g->vertices;
    while (v) {
        num_vertices++;
        v = v->prox;
    }
        
    return num_vertices;
}

unsigned int n_arestas(grafo *g) {
    unsigned int num_arestas = 0;
    if (!g) return num_arestas;

    vertice_t *v = g->vertices;
    while (v) {
        aresta_t *a = v->adj;
        while (a) {
            num_arestas++;
            a = a->prox;
        }
        v = v->prox;
    }
    // Cada aresta é contada duas vezes (uma para cada vértice), então dividimos por 2
    num_arestas /= 2;
    return num_arestas;
}


unsigned int n_componentes(grafo *g) {
    if (!g) return 0;
    unsigned int num_componentes = 0;
    // Inicializa todas as cores como não visitadas 
    for (vertice_t *v = g->vertices; v; v = v->prox) 
        v->cor = COR_NAO_VISITADA;
    // Para cada vértice, se não foi visitado, incrementa o contador de componentes
    for (vertice_t *v = g->vertices; v; v = v->prox) {
        if (v->cor == COR_NAO_VISITADA) {
            num_componentes++;
            // BFS manual para marcar todos os vértices conectados
            vertice_t *fila[1024];
            int ini = 0, fim = 0;

            v->cor = COR_1; // Marca como visitado
            fila[fim++] = v;

            while (ini < fim) {
                vertice_t *v_atual = fila[ini++];
                for (aresta_t *a = v_atual->adj; a; a = a->prox) {
                    vertice_t *v_destino = g->vertices;
                    while (v_destino && strcmp(v_destino->nome, a->destino) != 0) {
                        v_destino = v_destino->prox;
                    }
                    if (v_destino && v_destino->cor == COR_NAO_VISITADA) {
                        v_destino->cor = COR_1; // Marca como visitado
                        fila[fim++] = v_destino;
                    }
                }
            }
        }
    }   
    return num_componentes;
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
