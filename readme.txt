Autor: Lucas Néia Torres

---

### Descrição

Este trabalho implementa uma biblioteca para manipulação de grafos não-direcionados, representada pelos arquivos `grafo.h` e `grafo.c`. O programa principal, `teste.c`, lê um grafo de uma fonte de entrada, executa uma série de análises sobre ele e imprime os resultados na saída padrão.

As seguintes propriedades do grafo são calculadas e exibidas:
* Nome do grafo
* Número de vértices e arestas
* Número de componentes conexas
* Verificação se o grafo é bipartido
* Diâmetro de cada componente conexa
* Vértices de corte (pontos de articulação)
* Arestas de corte (pontes)

---

### Compilação e Execução

**Para compilar:**
O programa não possui dependências externas além da biblioteca padrão do C. Utilize o seguinte comando para compilar:

make

**Para executar:**
O programa lê a definição do grafo da entrada padrão (`stdin`). Para executar, redirecione um arquivo de entrada:

./teste < /caminho/para/seu/arquivo_de_grafo.in

---

### Observações sobre a Implementação

* **Estrutura de Dados:** O grafo foi implementado utilizando uma **Lista de Adjacência**.

* **Funções Implementadas:** Todas as funções especificadas no cabeçalho `grafo.h` foram implementadas em `grafo.c`.

* **Algoritmos Utilizados:**
    * **Componentes Conexas e Bipartição:** Utilizam uma implementação de **Busca em Largura (BFS)**.
    * **Diâmetros:** O diâmetro de cada componente é calculado através de uma heurística de **dupla busca em largura**.
    * **Vértices e Arestas de Corte:** Foi implementado o algoritmo baseado em **Busca em Profundidade (DFS)** que utiliza os tempos de descoberta (`tin`) e o valor `low-point` de cada vértice para identificar os pontos de articulação e as pontes.

* **Ordenação:** A lista de vértices de corte e de arestas de corte é ordenada alfabeticamente com qsort. Para arestas, os nomes dos vértices são ordenados dentro de cada par para garantir unicidade.

* **Gerenciamento de Memória:** As estruturas que podem crescer dinamicamente, como listas de adjacência e arestas de corte, são alocadas na heap com malloc. Toda memória alocada é devidamente liberada ao final da execução pela função `destroi_grafo`.
