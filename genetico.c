#include "genetico.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_TORNEIO 3
#define ELITISMO 1

static void avaliarPopulacao(Individuo *populacao, int tamanho,
                             const Ponto *pontos, int quantidadePontos)
{
    for (int i = 0; i < tamanho; i++) {
        populacao[i].distancia = calcularDistancia(&populacao[i], pontos,
                                                   quantidadePontos);
        populacao[i].aptidao = calcularAptidao(populacao[i].distancia);
    }
}

static void registrarEstatisticas(
    FILE *arquivo,
    int geracao,
    const Individuo *populacao,
    int tamanho)
{
    if (arquivo == NULL || populacao == NULL || tamanho <= 0) {
        return;
    }

    double melhor = populacao[0].distancia;
    double pior = populacao[0].distancia;
    double soma = 0.0;

    for (int i = 0; i < tamanho; i++) {
        double distancia = populacao[i].distancia;

        if (distancia < melhor) {
            melhor = distancia;
        }

        if (distancia > pior) {
            pior = distancia;
        }

        soma += distancia;
    }

    double media = soma / tamanho;

    fprintf(arquivo, "%d,%.6f,%.6f,%.6f\n",
            geracao, melhor, media, pior);
}

static int copiarIndividuo(Individuo *destino, const Individuo *origem,
                           int quantidadePontos)
{
    destino->rota = malloc((size_t)quantidadePontos * sizeof(int));
    if (destino->rota == NULL) return 0;

    memcpy(destino->rota, origem->rota,
           (size_t)quantidadePontos * sizeof(int));
    destino->distancia = origem->distancia;
    destino->aptidao = origem->aptidao;
    return 1;
}

static int indiceMelhor(const Individuo *populacao, int tamanho)
{
    int melhor = 0;
    for (int i = 1; i < tamanho; i++) {
        if (populacao[i].distancia < populacao[melhor].distancia)
            melhor = i;
    }
    return melhor;
}

static int selecionarTorneio(const Individuo *populacao, int tamanho)
{
    int melhor = rand() % tamanho;
    for (int i = 1; i < TAMANHO_TORNEIO; i++) {
        int candidato = rand() % tamanho;
        if (populacao[candidato].distancia < populacao[melhor].distancia)
            melhor = candidato;
    }
    return melhor;
}

/* Ranking: melhores recebem peso maior (N para o melhor, 1 para o pior). */
static int selecionarRanking(const Individuo *populacao, int tamanho)
{
    int *ordem = malloc((size_t)tamanho * sizeof(int));
    if (ordem == NULL) return selecionarTorneio(populacao, tamanho);

    for (int i = 0; i < tamanho; i++) ordem[i] = i;
    for (int i = 0; i < tamanho - 1; i++) {
        for (int j = i + 1; j < tamanho; j++) {
            if (populacao[ordem[j]].distancia < populacao[ordem[i]].distancia) {
                int temp = ordem[i]; ordem[i] = ordem[j]; ordem[j] = temp;
            }
        }
    }

    int totalPesos = tamanho * (tamanho + 1) / 2;
    int sorteio = rand() % totalPesos;
    int acumulado = 0;
    int escolhido = ordem[tamanho - 1];

    for (int pos = 0; pos < tamanho; pos++) {
        acumulado += tamanho - pos;
        if (sorteio < acumulado) {
            escolhido = ordem[pos];
            break;
        }
    }

    free(ordem);
    return escolhido;
}

static int selecionarPai(const Individuo *populacao, int tamanho,
                         MetodoSelecao metodo)
{
    if (metodo == SELECAO_RANKING)
        return selecionarRanking(populacao, tamanho);
    return selecionarTorneio(populacao, tamanho);
}

/* Order Crossover (OX): preserva uma permutação válida dos pontos. */
static void cruzamentoOX(const Individuo *pai1, const Individuo *pai2,
                         Individuo *filho, int n)
{
    int inicio = rand() % n;
    int fim = rand() % n;
    if (inicio > fim) { int t = inicio; inicio = fim; fim = t; }

    for (int i = 0; i < n; i++) filho->rota[i] = -1;
    for (int i = inicio; i <= fim; i++) filho->rota[i] = pai1->rota[i];

    int posFilho = (fim + 1) % n;
    int posPai = (fim + 1) % n;
    int preenchidos = fim - inicio + 1;

    while (preenchidos < n) {
        int cidade = pai2->rota[posPai];
        int existe = 0;
        for (int j = inicio; j <= fim; j++) {
            if (filho->rota[j] == cidade) { existe = 1; break; }
        }
        if (!existe) {
            filho->rota[posFilho] = cidade;
            posFilho = (posFilho + 1) % n;
            preenchidos++;
        }
        posPai = (posPai + 1) % n;
    }
}

static void mutar(Individuo *individuo, int n, double taxaMutacao)
{
    double sorteio = (double)rand() / ((double)RAND_MAX + 1.0);
    if (n > 1 && sorteio < taxaMutacao) {
        int a = rand() % n;
        int b = rand() % n;
        while (b == a) b = rand() % n;
        int temp = individuo->rota[a];
        individuo->rota[a] = individuo->rota[b];
        individuo->rota[b] = temp;
    }
}

static void liberarPopulacao(Individuo *populacao, int tamanho)
{
    if (populacao == NULL) return;
    for (int i = 0; i < tamanho; i++) liberarIndividuo(&populacao[i]);
    free(populacao);
}

static void imprimirRota(const Individuo *individuo, int n)
{
    for (int i = 0; i < n; i++)
        printf("%d%s", individuo->rota[i], (i == n - 1) ? "" : " -> ");
    printf(" -> %d", individuo->rota[0]);
}

int executarAlgoritmoGenetico(const Ponto *pontos,
                              int quantidadePontos,
                              int tamanhoPopulacao,
                              int numeroGeracoes,
                              double taxaMutacao,
                              MetodoSelecao metodo)
{
    if (pontos == NULL || quantidadePontos < 2 || tamanhoPopulacao < 3 ||
        numeroGeracoes < 1 || taxaMutacao < 0.0 || taxaMutacao > 1.0) {
        fprintf(stderr, "Parametros invalidos. Use populacao >= 3, geracoes >= 1 e mutacao entre 0 e 1.\n");
        return 0;
    }

    Individuo *populacao = calloc((size_t)tamanhoPopulacao, sizeof(Individuo));
    if (populacao == NULL) {
        fprintf(stderr, "Nao foi possivel alocar a populacao.\n");
        return 0;
    }

    for (int i = 0; i < tamanhoPopulacao; i++) {
        populacao[i] = criarIndividuo(quantidadePontos);
        if (populacao[i].rota == NULL) {
            liberarPopulacao(populacao, tamanhoPopulacao);
            fprintf(stderr, "Erro ao criar individuo.\n");
            return 0;
        }
        embaralharRota(&populacao[i], quantidadePontos);
    }
    avaliarPopulacao(populacao, tamanhoPopulacao, pontos, quantidadePontos);

    int melhorGlobal = indiceMelhor(populacao, tamanhoPopulacao);
    Individuo melhorHistorico = {NULL, 0.0, 0.0};
    if (!copiarIndividuo(&melhorHistorico, &populacao[melhorGlobal], quantidadePontos)) {
        liberarPopulacao(populacao, tamanhoPopulacao);
        fprintf(stderr, "Erro ao guardar melhor individuo.\n");
        return 0;
    }

    printf("\nPopulacao inicial: %d individuos | pontos por rota: %d\n",
           tamanhoPopulacao, quantidadePontos);
    printf("Metodo de selecao: %s\n",
           metodo == SELECAO_RANKING ? "Ranking" : "Torneio");
    
    const char *nomeCSV =
    (metodo == SELECAO_RANKING)
    ? "resultados_ranking.csv"
    : "resultados_torneio.csv";

    FILE *arquivoCSV = fopen(nomeCSV, "w");

    if (arquivoCSV != NULL) {
        fprintf(arquivoCSV,"geracao,melhor_distancia,distancia_media,pior_distancia\n");

        /* Registra a população inicial, antes da evolução. */
        registrarEstatisticas(arquivoCSV, 0, populacao, tamanhoPopulacao);
    } else 
    {
        fprintf(stderr, "Aviso: nao foi possivel criar o CSV.\n");
    }

    for (int geracao = 1; geracao <= numeroGeracoes; geracao++) {
        Individuo *nova = calloc((size_t)tamanhoPopulacao, sizeof(Individuo));
        if (nova == NULL) {
            liberarIndividuo(&melhorHistorico);
            liberarPopulacao(populacao, tamanhoPopulacao);
            fprintf(stderr, "Erro ao alocar nova geracao.\n");
            return 0;
        }

        int melhorAtual = indiceMelhor(populacao, tamanhoPopulacao);
        int quantidadeNova = 0;

        /* Elitismo: conserva o melhor sem alteracao. */
        if (!copiarIndividuo(&nova[quantidadeNova++], &populacao[melhorAtual], quantidadePontos)) {
            liberarPopulacao(nova, tamanhoPopulacao);
            liberarIndividuo(&melhorHistorico);
            liberarPopulacao(populacao, tamanhoPopulacao);
            fprintf(stderr, "Erro ao copiar elite.\n");
            return 0;
        }

        while (quantidadeNova < tamanhoPopulacao) {
            int idxPai1 = selecionarPai(populacao, tamanhoPopulacao, metodo);
            int idxPai2 = selecionarPai(populacao, tamanhoPopulacao, metodo);
            Individuo filho = criarIndividuo(quantidadePontos);
            if (filho.rota == NULL) {
                liberarPopulacao(nova, tamanhoPopulacao);
                liberarIndividuo(&melhorHistorico);
                liberarPopulacao(populacao, tamanhoPopulacao);
                fprintf(stderr, "Erro ao criar filho.\n");
                return 0;
            }

            cruzamentoOX(&populacao[idxPai1], &populacao[idxPai2], &filho, quantidadePontos);
            mutar(&filho, quantidadePontos, taxaMutacao);
            filho.distancia = calcularDistancia(&filho, pontos, quantidadePontos);
            filho.aptidao = calcularAptidao(filho.distancia);
            nova[quantidadeNova++] = filho; /* adiciona apenas uma vaga por vez */
        }

        /* A nova populacao tem sempre o mesmo tamanho da anterior. */
        liberarPopulacao(populacao, tamanhoPopulacao);
        populacao = nova;
        avaliarPopulacao(populacao, tamanhoPopulacao, pontos, quantidadePontos);
        registrarEstatisticas(arquivoCSV,geracao,populacao,tamanhoPopulacao);

        melhorAtual = indiceMelhor(populacao, tamanhoPopulacao);
        if (populacao[melhorAtual].distancia < melhorHistorico.distancia) {
            Individuo copia = {NULL, 0.0, 0.0};
            if (copiarIndividuo(&copia, &populacao[melhorAtual], quantidadePontos)) {
                liberarIndividuo(&melhorHistorico);
                melhorHistorico = copia;
            }
        }

        printf("Geracao %4d/%d | melhor distancia: %10.3f | populacao: %d\n",
               geracao, numeroGeracoes, populacao[melhorAtual].distancia,
               tamanhoPopulacao);
    }

    printf("\n===== MELHOR ROTA ENCONTRADA =====\nRota: ");
    imprimirRota(&melhorHistorico, quantidadePontos);
    printf("\nDistancia total: %.3f\nAptidao (1/distancia): %.6f\n",
           melhorHistorico.distancia, melhorHistorico.aptidao);

    liberarIndividuo(&melhorHistorico);
    liberarPopulacao(populacao, tamanhoPopulacao);
    return 1;
}
