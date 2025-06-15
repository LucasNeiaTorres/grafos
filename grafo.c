#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdbool.h>
#include <limits.h>
#include "grafo.h"

#define MAX_LINE 2048
#define MAX_NAME 128
#define MAX_VERTICES 1024

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
#define INF INT_MAX

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

    // Remove o '\n' do final do nome
    size_t len = strlen(nome);
    if (len > 0 && nome[len - 1] == '\n') {
        nome[len - 1] = '\0';
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
                    while (v_destino && strcmp(v_destino->nome, a->destino) != 0) 
                        v_destino = v_destino->prox;
                    
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

static int mapear_vertices(grafo *g, vertice_t **vmap) {
    int n = 0;
    for (vertice_t *v = g->vertices; v; v = v->prox)
        vmap[n++] = v;
    return n;
}

static int dijkstra_mais_distante(int start, vertice_t **vmap, int n, int *dist, int *visitado) {
    int usado[MAX_VERTICES] = {0};

    for (int i = 0; i < n; i++) 
        dist[i] = INF;

    dist[start] = 0;

    for (int i = 0; i < n; i++) {
        int u = -1;
        for (int j = 0; j < n; j++) {
            if (!usado[j] && (u == -1 || dist[j] < dist[u])) {
                u = j;
            }
        }

        if (dist[u] == INF) 
            break;
        usado[u] = 1;
        visitado[u] = 1;

        vertice_t *vu = vmap[u];
        for (aresta_t *a = vu->adj; a; a = a->prox) {
            for (int v = 0; v < n; v++) {
                if (strcmp(vmap[v]->nome, a->destino) == 0) {
                    if (dist[u] + a->peso < dist[v]) 
                        dist[v] = dist[u] + a->peso;
                    break;
                }
            }
        }
    }

    int max_dist = 0, idx = start;
    for (int i = 0; i < n; i++) {
        if (dist[i] != INF && dist[i] > max_dist) {
            max_dist = dist[i];
            idx = i;
        }
    }
    return idx;
}

static int calcula_diametro_componente(int start, vertice_t **vmap, int n, int *visitado) {
    int dist[MAX_VERTICES];
    int u = dijkstra_mais_distante(start, vmap, n, dist, visitado);

    // reset visitado, porque o segundo Dijkstra vai atualizar de novo
    for (int i = 0; i < n; i++) 
        visitado[i] = 0;

    int v = dijkstra_mais_distante(u, vmap, n, dist, visitado);
    return dist[v];
}

char *diametros(grafo *g) {
    vertice_t *vmap[MAX_VERTICES];
    int n = mapear_vertices(g, vmap);
    int visitado[MAX_VERTICES] = {0};
    int diams[MAX_VERTICES], n_diams = 0;

    for (int i = 0; i < n; i++) {
        if (!visitado[i]) {
            int d = calcula_diametro_componente(i, vmap, n, visitado);
            diams[n_diams++] = d;
        }
    }

    // Ordena os diâmetros
    for (int i = 0; i < n_diams; i++) {
        for (int j = i + 1; j < n_diams; j++) {
            if (diams[i] > diams[j]) {
                int tmp = diams[i]; diams[i] = diams[j]; diams[j] = tmp;
            }
        }
    }

    // Cria string resultado
    size_t tam = (size_t)n_diams * 12;
    char *res = malloc(tam);
    res[0] = '\0';
    char buffer[16];
    for (int i = 0; i < n_diams; i++) {
        sprintf(buffer, "%d ", diams[i]);
        strcat(res, buffer);
    }
    if (n_diams > 0) res[strlen(res) - 1] = '\0'; // remove espaço final

    return res;
}

static void dfs_articulacao(int u, int parent, vertice_t **vmap, int n, int *vis, int *tin, int *low, int *is_corte, int *timer) {
    vis[u] = 1;
    tin[u] = low[u] = (*timer)++;
    int filhos = 0;

    for (aresta_t *a = vmap[u]->adj; a; a = a->prox) {
        int v = -1;

        // Busca o índice do destino no vmap
        for (int i = 0; i < n; i++) {
            if (strcmp(vmap[i]->nome, a->destino) == 0) {
                v = i;
                break;
            }
        }

        if (v == -1 || v == parent) continue;

        if (vis[v]) {
            // Aresta de retorno
            if (low[u] > tin[v])
                low[u] = tin[v];
        } else {
            // Aresta de árvore
            dfs_articulacao(v, u, vmap, n, vis, tin, low, is_corte, timer);

            if (low[u] > low[v])
                low[u] = low[v];

            if (low[v] >= tin[u] && parent != -1)
                is_corte[u] = 1;

            filhos++;
        }
    }

    if (parent == -1 && filhos > 1)
        is_corte[u] = 1;
}

// Função de comparação para qsort, para ordenar um array de strings (char **)
static int comparar_strings(const void *a, const void *b) {
    char * const *string_a = a;
    char * const *string_b = b;

    return strcmp(*string_a, *string_b);
}


char *vertices_corte(grafo *g) {
    vertice_t *vmap[MAX_VERTICES];
    int n = 0;

    // Mapeia vértices
    for (vertice_t *v = g->vertices; v; v = v->prox)
        vmap[n++] = v;

    int vis[MAX_VERTICES] = {0};
    int tin[MAX_VERTICES] = {0};
    int low[MAX_VERTICES] = {0};
    int is_corte[MAX_VERTICES] = {0};
    int timer = 0;

    for (int i = 0; i < n; i++) {
        if (!vis[i]) {
            dfs_articulacao(i, -1, vmap, n, vis, tin, low, is_corte, &timer);
        }
    }

    // Coleta os nomes dos vértices de corte
    char *nomes[MAX_VERTICES];
    int n_cortes = 0;
    for (int i = 0; i < n; i++) {
        if (is_corte[i]) {
            nomes[n_cortes++] = vmap[i]->nome;
        }
    }

    // Ordena os nomes alfabeticamente
    qsort(nomes, (size_t)n_cortes, sizeof(char *), comparar_strings);

    // Constrói a string final
    char *res = malloc((size_t)n_cortes * 64);
    res[0] = '\0';

    for (int i = 0; i < n_cortes; i++) {
        strcat(res, nomes[i]);
        strcat(res, " ");
    }

    size_t len = strlen(res);
    if (len > 0 && res[len - 1] == ' ')
        res[len - 1] = '\0';

    return res;
}

static void dfs(int u, int parent, vertice_t **vmap, int n, int *vis, int *tin, int *low, int *tempo, char **arestas, size_t *n_arestas) {
    vis[u] = 1;
    tin[u] = low[u] = (*tempo)++;

    for (aresta_t *a = vmap[u]->adj; a; a = a->prox) {
        int v = -1;
        for (int i = 0; i < n; i++) {
            if (strcmp(vmap[i]->nome, a->destino) == 0) {
                v = i;
                break;
            }
        }

        if (v == -1 || v == parent) continue;

        if (vis[v]) {
            if (low[u] > tin[v])
                low[u] = tin[v];
        } else {
            dfs(v, u, vmap, n, vis, tin, low, tempo, arestas, n_arestas);
            if (low[u] > low[v])
                low[u] = low[v];

            if (low[v] > tin[u]) {
                // Aresta de corte encontrada
                char *linha = malloc(2 * MAX_NAME + 2);
                if (!linha) continue;

                if (strcmp(vmap[u]->nome, vmap[v]->nome) < 0)
                    sprintf(linha, "%s %s", vmap[u]->nome, vmap[v]->nome);
                else
                    sprintf(linha, "%s %s", vmap[v]->nome, vmap[u]->nome);

                arestas[(*n_arestas)++] = linha;
            }
        }
    }
}

char *arestas_corte(grafo *g) {
    if (!g) return NULL;

    vertice_t *vmap[MAX_VERTICES];
    int n = mapear_vertices(g, vmap);

    int vis[MAX_VERTICES] = {0};
    int tin[MAX_VERTICES], low[MAX_VERTICES];
    int tempo = 0;

    // Aloca espaço grande o suficiente para guardar todas as arestas
    char *saida = malloc(MAX_VERTICES * MAX_VERTICES * MAX_NAME);
    if (!saida) return NULL;
    saida[0] = '\0';

    // Lista temporária de strings de arestas (ALOCADA NA HEAP)
    char **arestas = malloc(MAX_VERTICES * MAX_VERTICES * sizeof(char *));
    if (!arestas) {
        free(saida); // Libera a outra memória alocada antes de sair
        return NULL;
    }
    size_t n_arestas = 0;
    for (int i = 0; i < n; i++)
        if (!vis[i])
            dfs(i, -1, vmap, n, vis, tin, low, &tempo, arestas, &n_arestas);    

    // Ordena as strings de arestas
    qsort(arestas, n_arestas, sizeof(char *), comparar_strings);

    // Concatena na saída
    for (size_t i = 0; i < n_arestas; i++) {
        strcat(saida, arestas[i]);
        strcat(saida, " ");
        free(arestas[i]);
    }

    free(arestas); 

    // Remove o espaço extra final
    size_t len = strlen(saida);
    if (len > 0 && saida[len - 1] == ' ')
        saida[len - 1] = '\0';

    return saida;
}