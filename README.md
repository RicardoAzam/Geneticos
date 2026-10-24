# Geneticos
# Algoritmo Genético para o Problema do Caixeiro Viajante (TSP)

## Compilação e execução

### 1. Pré-requisitos

- GCC ou Clang para compilar os arquivos C.
- Python 3: https://www.python.org/downloads/
- Biblioteca Matplotlib para gerar os gráficos.

Instale o Matplotlib pelo terminal:

```bash
python3 -m pip install matplotlib
```

### 2. Compilar o projeto em C

Na pasta do projeto, execute:

```bash
gcc main.c ponto.c individuo.c genetico.c -lm -o tsp
```

Execute o programa:

```bash
./tsp
```

Escolha o método de seleção e informe os parâmetros solicitados pelo programa: tamanho da população, número de gerações e taxa de mutação.

### 3. Gerar os gráficos com Python

Após executar o algoritmo e gerar o arquivo CSV, execute o script Python correspondente.

**Seleção por torneio:**

```bash
python3 graficos.py resultados_torneio.csv
```

**Seleção por ranking:**

```bash
python3 graficos.py resultados_ranking.csv
```

Os gráficos serão exibidos na tela e salvos como imagens PNG.

### Observações

- Execute os comandos na pasta onde estão os arquivos do projeto.
- Os arquivos CSV precisam existir antes de gerar os gráficos.
- Cada nova execução pode sobrescrever o CSV anterior. Salve uma cópia se quiser preservar os resultados.
- No Windows, pode ser necessário usar `python` no lugar de `python3` e executar `.\tsp.exe` no lugar de `./tsp`.

## 1. Objetivo

O projeto consiste em desenvolver, em **C**, um **Algoritmo Genético** para encontrar uma solução localmente boa para o Problema do Caixeiro Viajante (TSP).

O problema será adaptado para pontos em duas dimensões. O algoritmo deverá encontrar um caminho cíclico que:

- percorra todos os pontos;
- visite cada ponto uma vez;
- retorne ao ponto inicial;
- minimize a distância euclidiana total.

Serão avaliados dois cenários:

1. **Pontos aleatórios**, distribuídos uniformemente;
2. **Pontos circulares**, utilizados como benchmark.

A quantidade de pontos deverá ser configurável e será de pelo menos 8.

---

# 2. O que o programa deverá fazer

O programa deverá permitir configurar os principais parâmetros do Algoritmo Genético, como:

- quantidade de pontos;
- tamanho da população;
- quantidade máxima de épocas;
- taxa de mutação;
- método de seleção;
- método de cruzamento;
- método de mutação;
- critério de parada;
- semente aleatória.

**Os métodos ainda não foram definidos.**

Por enquanto, eles serão identificados de forma genérica:

```text
Método 1
Método 2
Método 3
...
```

---

# 3. Representação da solução

Cada indivíduo representa uma possível rota.

Por exemplo, para 8 pontos:

```text
0 3 7 2 5 1 6 4
```

representa:

```text
0 → 3 → 7 → 2 → 5 → 1 → 6 → 4 → 0
```

O último `→ 0` representa o retorno ao ponto inicial.

A solução será representada como uma **permutação dos pontos**, garantindo que cada ponto apareça uma única vez.

---

# 4. Estrutura básica dos dados

A arquitetura inicial será simples.

## Ponto

```c
typedef struct {
    double x;
    double y;
} Ponto;
```

## Indivíduo

```c
typedef struct {
    int *rota;
    double distancia;
    double aptidao;
} Individuo;
```

## População

```c
typedef struct {
    Individuo *individuos;
    int tamanho;
} Populacao;
```

## Configuração

```c
typedef struct {
    int quantidade_pontos;
    int tamanho_populacao;
    int max_epocas;

    double taxa_mutacao;

    int metodo_selecao;
    int metodo_cruzamento;
    int metodo_mutacao;
    int criterio_parada;

    unsigned int seed;
} Config;
```

---

# 5. Arquitetura simplificada

```text
tsp-genetico/
│
├── README.md
│   ├── ponto.h
│   ├── individuo.h
│   ├── genetico.h
│   └── resultados.h
│   ├── main.c
│   ├── ponto.c
│   ├── individuo.c
│   ├── genetico.c
```

### Responsabilidade de cada arquivo

### `ponto.c / ponto.h`

Responsável por:

- criar os pontos aleatórios;
- criar os pontos circulares;
- armazenar as coordenadas.

---

### `individuo.c / individuo.h`

Responsável por:

- criar indivíduos;
- gerar rotas;
- calcular a distância;
- calcular a aptidão;
- copiar indivíduos;
- liberar memória.

---

### `genetico.c / genetico.h`

Será o núcleo do projeto.

Responsável por:

- criar a população;
- avaliar a população;
- seleção;
- cruzamento;
- mutação;
- geração de novas populações;
- elitismo, caso seja utilizado;
- critério de parada;
- guardar a melhor solução.

Os métodos serão inicialmente chamados de:

```text
método de seleção 1
método de cruzamento 1
método de mutação 1
...
```

Quando o grupo definir os métodos, eles serão implementados nesse módulo.

---

### `main.c`

Responsável apenas por organizar a execução:

```text
configuração
    ↓
geração dos pontos
    ↓
execução do AG
    ↓
apresentação dos resultados
```

---

# 6. Fluxo do Algoritmo Genético

A lógica geral será:

```text
Gerar pontos
     ↓
Criar população inicial
     ↓
Avaliar população
     ↓
Selecionar indivíduos
     ↓
Aplicar cruzamento
     ↓
Aplicar mutação
     ↓
Criar nova população
     ↓
Avaliar nova população
     ↓
Guardar melhor solução
     ↓
Verificar parada
     ↓
Repetir
```

Os detalhes de seleção, cruzamento, mutação e parada ainda serão definidos.

---

# 7. Distância

A distância entre dois pontos será calculada pela distância euclidiana:

```text
d(A,B) = √((Ax - Bx)² + (Ay - By)²)
```

A distância total de uma rota será a soma das distâncias entre pontos consecutivos, incluindo o retorno ao primeiro ponto.

Exemplo:

```text
0 → 3 → 7 → 2 → 5 → 1 → 6 → 4 → 0
```

A distância será:

```text
d(0,3) +
d(3,7) +
d(7,2) +
d(2,5) +
d(5,1) +
d(1,6) +
d(6,4) +
d(4,0)
```

---

# 8. Função de aptidão

A função de aptidão deverá representar a qualidade da solução.

Como o objetivo é **minimizar a distância**, uma possibilidade é:

```text
aptidão = 1 / distância
```

Porém, a fórmula definitiva ainda poderá ser alterada durante o desenvolvimento.

Por isso, a implementação deverá deixar a função isolada e fácil de modificar.

---

# 9. Cenário 1 - Pontos aleatórios

Os pontos serão distribuídos aleatoriamente em um plano.

Exemplo:

```text
100 +-------------------------------+
    |       •                       |
    |            •       •          |
    |  •                            |
    |                  •            |
    |       •                •      |
    |                            •  |
    |   •                           |
  0 +-------------------------------+
    0                             100
```

A quantidade de pontos será configurável.

Exemplos:

```text
8
20
50
100
```

---

# 10. Cenário 2 - Pontos circulares

Os pontos serão distribuídos em um círculo.

Para um círculo de centro `(cx, cy)` e raio `r`:

```text
x = cx + r × cos(θ)
y = cy + r × sin(θ)
```

Para pontos igualmente espaçados:

```text
θ = 2πi / N
```

Esse cenário será utilizado como benchmark.

Como conhecemos a estrutura da solução ótima para pontos igualmente espaçados no círculo, poderemos comparar o resultado encontrado pelo Algoritmo Genético com uma referência.

---

# 11. Acompanhamento das épocas

O programa deverá acompanhar a evolução do algoritmo.

Exemplo:

```text
Época 0
Melhor distância: 523.42

Época 1
Melhor distância: 498.32

Época 2
Melhor distância: 470.91

...
```

Idealmente, também será registrada a distância média da população.

Os resultados poderão ser salvos em CSV:

```text
epoca,melhor_distancia,distancia_media
0,523.42,741.81
1,498.32,702.15
2,470.91,681.32
```

Isso permitirá criar gráficos posteriormente.

---

# 12. Benchmark circular

O cenário circular será particularmente importante.

A comparação deverá apresentar algo semelhante a:

```text
Pontos: 50

Distância ótima de referência: 313.953
Melhor solução encontrada:     314.102

Erro: 0.047%
Tempo: 0.82 s
Época da melhor solução: 742
```

Isso fornece uma maneira objetiva de avaliar a qualidade do algoritmo.

---

# 13. Princípio de desenvolvimento

Durante a primeira fase, o grupo **não deve tentar otimizar o código prematuramente**.

A ordem será:

```text
1. Fazer funcionar
       ↓
2. Testar
       ↓
3. Medir
       ↓
4. Comparar
       ↓
5. Melhorar
```

Os métodos do Algoritmo Genético serão mantidos flexíveis até que o grupo faça a pesquisa e decida quais serão utilizados.

---

# 14. Estado inicial do projeto

Neste momento, as seguintes decisões estão definidas:

```text
Linguagem: C

Problema: TSP

Representação:
permutação dos pontos

Cenário 1:
pontos aleatórios

Cenário 2:
pontos circulares

Distância:
euclidiana

Método de seleção:
Método 1, Método 2, ...

Método de cruzamento:
Método 1, Método 2, ...

Método de mutação:
Método 1, Método 2, ...

Critério de parada:
Método 1, Método 2, ...

Parâmetros:
configuráveis
```

As decisões marcadas como **Método 1, Método 2, etc.** serão preenchidas depois da pesquisa do grupo.

---