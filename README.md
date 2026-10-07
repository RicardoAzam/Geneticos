# Geneticos
# Algoritmo Genético para o Problema do Caixeiro Viajante (TSP)

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

Depois que o grupo estudar e escolher os métodos, os nomes poderão ser substituídos.

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

A ideia é deixar os métodos configuráveis desde o início, mesmo que ainda não saibamos quais serão.

---

# 5. Arquitetura simplificada

Não haverá uma grande quantidade de módulos.

A primeira versão será organizada assim:

```text
tsp-genetico/
│
├── README.md
├── Makefile
│
├── include/
│   ├── ponto.h
│   ├── individuo.h
│   ├── genetico.h
│   └── resultados.h
│
├── src/
│   ├── main.c
│   ├── ponto.c
│   ├── individuo.c
│   ├── genetico.c
│   └── resultados.c
│
└── resultados/
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

### `resultados.c / resultados.h`

Responsável por:

- mostrar resultados no terminal;
- registrar resultados por época;
- salvar resultados em CSV;
- registrar tempo de execução.

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

# 12. Divisão do trabalho

A divisão será feita por grandes responsabilidades, evitando que cada pessoa precise conhecer todo o código.

## Pessoa 1 - Geração dos dados

Responsável por:

- `ponto.c`
- `ponto.h`
- geração aleatória;
- geração circular;
- testes dos pontos.

---

## Pessoa 2 - Indivíduos e avaliação

Responsável por:

- `individuo.c`
- `individuo.h`
- representação da rota;
- geração dos indivíduos;
- cálculo da distância;
- função de aptidão;
- gerenciamento de memória.

---

## Pessoa 3 - Operadores do AG

Responsável pela parte correspondente aos métodos dentro de:

```text
genetico.c
```

Inicialmente:

```text
Método de seleção
Método de cruzamento
Método de mutação
```

Os métodos ainda não serão definidos.

---

## Pessoa 4 - Integração e resultados

Responsável por:

- `genetico.c` — estrutura geral do algoritmo;
- `resultados.c`;
- `resultados.h`;
- `main.c`;
- medição do tempo;
- CSV;
- integração dos módulos.

A Pessoa 4 trabalhará junto com a Pessoa 3 na integração do algoritmo.

---

# 13. Cronograma de duas semanas

## Semana 1 — Implementação

### Dias 1–2

Definir:

- estruturas;
- interfaces dos `.h`;
- configuração;
- organização do Git;
- geração dos pontos.

### Dias 3–5

Desenvolver:

- indivíduos;
- distância;
- aptidão;
- população;
- métodos do AG.

### Dias 6–7

Integração:

```text
Pontos
  ↓
Indivíduos
  ↓
População
  ↓
Métodos do AG
  ↓
Nova população
```

Ao final da primeira semana, o algoritmo básico deverá estar funcionando.

---

# Semana 2 — Testes e experimentos

### Dias 8–9

- corrigir bugs;
- testar rotas;
- verificar se nenhuma rota possui pontos repetidos;
- testar os métodos;
- testar diferentes parâmetros.

### Dias 10–11

Executar os dois cenários:

```text
Aleatório
Circular
```

Com diferentes quantidades de pontos.

### Dias 12–13

- coletar resultados;
- gerar gráficos;
- analisar convergência;
- medir tempo;
- comparar soluções.

### Dia 14

- revisão do código;
- documentação;
- README;
- preparação da apresentação;
- demonstração final.

---

# 14. Experimentos

Os valores definitivos ainda serão definidos pelo grupo.

Uma primeira bateria poderia ser:

| Cenário | Pontos | População | Épocas |
|---|---:|---:|---:|
| Aleatório | 8 | a definir | a definir |
| Aleatório | 20 | a definir | a definir |
| Aleatório | 50 | a definir | a definir |
| Circular | 8 | a definir | a definir |
| Circular | 20 | a definir | a definir |
| Circular | 50 | a definir | a definir |

A ideia é **não fixar os parâmetros antes dos testes**.

Durante o desenvolvimento, o grupo poderá comparar diferentes configurações e justificar as escolhas no trabalho.

---

# 15. Critérios que serão avaliados

O programa deverá permitir analisar:

- qualidade da solução;
- distância encontrada;
- velocidade de convergência;
- tempo de execução;
- comportamento ao aumentar a quantidade de pontos;
- comportamento nos cenários aleatório e circular;
- influência dos parâmetros escolhidos.

Também deverá ser possível mostrar a solução e seu desempenho **a cada época**, conforme solicitado no enunciado.

---

# 16. Benchmark circular

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

# 17. Bônus

Depois de o programa principal estar funcionando, será realizado um experimento com uma quantidade elevada de pontos no cenário circular.

Por exemplo:

```text
500 pontos
1000 pontos
2000 pontos
```

Serão registrados:

- tempo de execução;
- épocas intermediárias;
- melhor distância;
- momento da convergência;
- erro em relação ao benchmark.

O bônus será implementado somente depois de todos os requisitos obrigatórios estarem funcionando.

---

# 18. Princípio de desenvolvimento

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

# 19. Estado inicial do projeto

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

# 20. Próximo passo

Antes de começar a implementar, o grupo deve definir:

1. quais métodos de seleção serão estudados;
2. quais métodos de cruzamento são adequados para permutações;
3. quais métodos de mutação serão estudados;
4. quais critérios de parada serão comparados;
5. quais valores iniciais de população, mutação e épocas serão testados.

Somente depois disso os métodos específicos deverão ser incorporados ao código.
