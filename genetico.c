#include "genetico.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAMANHO_TORNEIO 3

/* Melhora menor que isso e ruido de ponto flutuante, nao conta como progresso. */
#define MELHORIA_MINIMA 1e-9

/* Ate esse tamanho a rota tambem e escrita na tela a cada epoca registrada. */
#define LIMITE_PONTOS_PARA_IMPRIMIR_ROTA 30

typedef struct {
    double melhor;
    double media;
    double pior;
} EstatisticasEpoca;

typedef struct {
    int indice;
    double aptidao;
} ItemRanking;

static double segundosDesde(clock_t instanteInicial)
{
    return (double)(clock() - instanteInicial) / CLOCKS_PER_SEC;
}

/* Sorteia um numero real em [0, 1). */
static double sortearProbabilidade(void)
{
    return (double)rand() / ((double)RAND_MAX + 1.0);
}

static void avaliarPopulacao(Individuo *populacao, int tamanho,
                             const Ponto *pontos, int quantidadePontos)
{
    for (int i = 0; i < tamanho; i++) {
        populacao[i].distancia = calcularDistancia(&populacao[i], pontos,
                                                   quantidadePontos);
        populacao[i].aptidao = calcularAptidao(populacao[i].distancia);
    }
}

static void liberarPopulacao(Individuo *populacao, int tamanho)
{
    if (populacao == NULL) {
        return;
    }

    for (int i = 0; i < tamanho; i++) {
        liberarIndividuo(&populacao[i]);
    }

    free(populacao);
}

static Individuo *criarPopulacao(int tamanho, int quantidadePontos)
{
    Individuo *populacao = calloc((size_t)tamanho, sizeof(Individuo));

    if (populacao == NULL) {
        return NULL;
    }

    for (int i = 0; i < tamanho; i++) {
        populacao[i] = criarIndividuo(quantidadePontos);

        if (populacao[i].rota == NULL) {
            liberarPopulacao(populacao, tamanho);
            return NULL;
        }
    }

    return populacao;
}

/* Copia o conteudo de um individuo para outro que ja tem a rota alocada. */
static void copiarIndividuo(Individuo *destino, const Individuo *origem,
                            int quantidadePontos)
{
    memcpy(destino->rota, origem->rota, (size_t)quantidadePontos * sizeof(int));
    destino->distancia = origem->distancia;
    destino->aptidao = origem->aptidao;
}

static int indiceMelhor(const Individuo *populacao, int tamanho)
{
    int melhor = 0;

    for (int i = 1; i < tamanho; i++) {
        if (populacao[i].distancia < populacao[melhor].distancia) {
            melhor = i;
        }
    }

    return melhor;
}

static EstatisticasEpoca calcularEstatisticas(const Individuo *populacao,
                                              int tamanho)
{
    EstatisticasEpoca estatisticas;
    double soma = 0.0;

    estatisticas.melhor = populacao[0].distancia;
    estatisticas.pior = populacao[0].distancia;

    for (int i = 0; i < tamanho; i++) {
        double distancia = populacao[i].distancia;

        if (distancia < estatisticas.melhor) {
            estatisticas.melhor = distancia;
        }

        if (distancia > estatisticas.pior) {
            estatisticas.pior = distancia;
        }

        soma += distancia;
    }

    estatisticas.media = soma / tamanho;

    return estatisticas;
}

static void registrarEstatisticas(FILE *arquivo, int epoca,
                                  const EstatisticasEpoca *estatisticas)
{
    if (arquivo == NULL) {
        return;
    }

    fprintf(arquivo, "%d,%.6f,%.6f,%.6f\n",
            epoca, estatisticas->melhor, estatisticas->media,
            estatisticas->pior);
}

/* Grava a melhor rota da epoca no CSV e acompanha a evolucao na tela. */
static void registrarSolucao(FILE *arquivoRotas, int epoca, int maxEpocas,
                             const Individuo *melhor, int quantidadePontos,
                             const EstatisticasEpoca *estatisticas)
{
    if (arquivoRotas != NULL) {
        fprintf(arquivoRotas, "%d,%.6f,", epoca, melhor->distancia);

        for (int i = 0; i < quantidadePontos; i++) {
            if (i > 0) {
                fprintf(arquivoRotas, " ");
            }
            fprintf(arquivoRotas, "%d", melhor->rota[i]);
        }

        fprintf(arquivoRotas, "\n");
    }

    printf("Epoca %5d/%d | melhor: %10.3f | media: %10.3f\n",
           epoca, maxEpocas, estatisticas->melhor, estatisticas->media);

    if (quantidadePontos <= LIMITE_PONTOS_PARA_IMPRIMIR_ROTA) {
        printf("    Rota: ");
        imprimirRota(melhor, quantidadePontos);
        printf("\n");
    }
}

/* ------------------------------ Selecao ------------------------------ */

/* Torneio: sorteia TAMANHO_TORNEIO individuos e vence o de maior aptidao. */
static int selecionarTorneio(const Individuo *populacao, int tamanho)
{
    int vencedor = rand() % tamanho;

    for (int disputa = 1; disputa < TAMANHO_TORNEIO; disputa++) {
        int desafiante = rand() % tamanho;

        if (populacao[desafiante].aptidao > populacao[vencedor].aptidao) {
            vencedor = desafiante;
        }
    }

    return vencedor;
}

static int compararAptidaoDecrescente(const void *itemA, const void *itemB)
{
    const ItemRanking *primeiro = (const ItemRanking *)itemA;
    const ItemRanking *segundo = (const ItemRanking *)itemB;

    if (primeiro->aptidao > segundo->aptidao) {
        return -1;
    }

    if (primeiro->aptidao < segundo->aptidao) {
        return 1;
    }

    return 0;
}

/* Ordena a populacao por aptidao uma unica vez por epoca. */
static void prepararRanking(const Individuo *populacao, int tamanho,
                            ItemRanking *ranking)
{
    for (int i = 0; i < tamanho; i++) {
        ranking[i].indice = i;
        ranking[i].aptidao = populacao[i].aptidao;
    }

    qsort(ranking, (size_t)tamanho, sizeof(ItemRanking),
          compararAptidaoDecrescente);
}

/* Ranking: o melhor tem peso N, o segundo N-1, ..., o pior tem peso 1. */
static int selecionarRanking(const ItemRanking *ranking, int tamanho)
{
    double totalPesos = (double)tamanho * (tamanho + 1) / 2.0;
    double sorteio = sortearProbabilidade() * totalPesos;
    double acumulado = 0.0;

    for (int posicao = 0; posicao < tamanho; posicao++) {
        acumulado += tamanho - posicao;

        if (sorteio < acumulado) {
            return ranking[posicao].indice;
        }
    }

    return ranking[tamanho - 1].indice;
}

static int selecionarPai(const Individuo *populacao, int tamanho,
                         MetodoSelecao metodo, const ItemRanking *ranking)
{
    if (metodo == SELECAO_RANKING) {
        return selecionarRanking(ranking, tamanho);
    }

    return selecionarTorneio(populacao, tamanho);
}

/* ----------------------------- Cruzamento ----------------------------- */

/*
 * Order Crossover (OX): o filho herda um trecho do pai1 na mesma posicao e
 * completa as demais posicoes com as cidades restantes na ordem do pai2.
 * O vetor cidadeUsada (tamanho n) evita buscas repetidas no trecho copiado.
 */
static void cruzamentoOX(const Individuo *pai1, const Individuo *pai2,
                         Individuo *filho, int n, int *cidadeUsada)
{
    int inicio = rand() % n;
    int fim = rand() % n;

    if (inicio > fim) {
        int temp = inicio;
        inicio = fim;
        fim = temp;
    }

    for (int i = 0; i < n; i++) {
        filho->rota[i] = -1;
        cidadeUsada[i] = 0;
    }

    for (int i = inicio; i <= fim; i++) {
        filho->rota[i] = pai1->rota[i];
        cidadeUsada[pai1->rota[i]] = 1;
    }

    int posicaoFilho = (fim + 1) % n;
    int posicaoPai = (fim + 1) % n;
    int preenchidos = fim - inicio + 1;

    while (preenchidos < n) {
        int cidade = pai2->rota[posicaoPai];

        if (cidadeUsada[cidade] == 0) {
            filho->rota[posicaoFilho] = cidade;
            cidadeUsada[cidade] = 1;
            posicaoFilho = (posicaoFilho + 1) % n;
            preenchidos++;
        }

        posicaoPai = (posicaoPai + 1) % n;
    }
}

/* ------------------------------- Mutacao ------------------------------- */

/* Troca a posicao de duas cidades sorteadas. */
static void mutarTroca(Individuo *individuo, int n)
{
    int posicaoA = rand() % n;
    int posicaoB = rand() % n;

    while (posicaoB == posicaoA) {
        posicaoB = rand() % n;
    }

    int temp = individuo->rota[posicaoA];
    individuo->rota[posicaoA] = individuo->rota[posicaoB];
    individuo->rota[posicaoB] = temp;
}

/* Inverte um trecho da rota (equivale a um movimento 2-opt). */
static void mutarInversao(Individuo *individuo, int n)
{
    int inicio = rand() % n;
    int fim = rand() % n;

    if (inicio > fim) {
        int temp = inicio;
        inicio = fim;
        fim = temp;
    }

    while (inicio < fim) {
        int temp = individuo->rota[inicio];
        individuo->rota[inicio] = individuo->rota[fim];
        individuo->rota[fim] = temp;

        inicio++;
        fim--;
    }
}

/* A taxa de mutacao e a probabilidade de cada filho sofrer uma mutacao. */
static void mutar(Individuo *individuo, int n, double taxaMutacao,
                  MetodoMutacao metodo)
{
    if (n < 2) {
        return;
    }

    if (sortearProbabilidade() >= taxaMutacao) {
        return;
    }

    if (metodo == MUTACAO_INVERSAO) {
        mutarInversao(individuo, n);
    } else {
        mutarTroca(individuo, n);
    }
}

/* ---------------------------- Algoritmo ---------------------------- */

static int configuracaoValida(const ConfigGenetico *config)
{
    if (config->quantidadePontos < 2) {
        return 0;
    }

    if (config->tamanhoPopulacao < 3) {
        return 0;
    }

    if (config->maxEpocas < 1) {
        return 0;
    }

    if (config->taxaMutacao < 0.0 || config->taxaMutacao > 1.0) {
        return 0;
    }

    if (config->taxaCruzamento < 0.0 || config->taxaCruzamento > 1.0) {
        return 0;
    }

    if (config->intervaloRegistro < 1) {
        return 0;
    }

    if (config->limiteEstagnacao < 0 || config->toleranciaErroPercentual < 0.0) {
        return 0;
    }

    return 1;
}

int executarAlgoritmoGenetico(const Ponto *pontos,
                              const ConfigGenetico *config,
                              ResultadoGenetico *resultado)
{
    if (pontos == NULL || config == NULL || resultado == NULL ||
        configuracaoValida(config) == 0) {
        fprintf(stderr, "Parametros invalidos para o algoritmo genetico.\n");
        return 0;
    }

    int n = config->quantidadePontos;
    int tamanho = config->tamanhoPopulacao;
    clock_t instanteInicial = clock();

    /* Toda a memoria e alocada uma vez; as duas populacoes se alternam. */
    Individuo *populacaoAtual = criarPopulacao(tamanho, n);
    Individuo *proximaPopulacao = criarPopulacao(tamanho, n);
    Individuo melhorHistorico = criarIndividuo(n);
    ItemRanking *ranking = malloc((size_t)tamanho * sizeof(ItemRanking));
    int *cidadeUsada = malloc((size_t)n * sizeof(int));

    if (populacaoAtual == NULL || proximaPopulacao == NULL ||
        melhorHistorico.rota == NULL || ranking == NULL || cidadeUsada == NULL) {
        fprintf(stderr, "Nao foi possivel alocar memoria.\n");
        liberarPopulacao(populacaoAtual, tamanho);
        liberarPopulacao(proximaPopulacao, tamanho);
        liberarIndividuo(&melhorHistorico);
        free(ranking);
        free(cidadeUsada);
        return 0;
    }

    for (int i = 0; i < tamanho; i++) {
        embaralharRota(&populacaoAtual[i], n);
    }
    avaliarPopulacao(populacaoAtual, tamanho, pontos, n);

    int indiceInicial = indiceMelhor(populacaoAtual, tamanho);
    copiarIndividuo(&melhorHistorico, &populacaoAtual[indiceInicial], n);

    char nomeEstatisticas[200];
    char nomeRotas[200];
    snprintf(nomeEstatisticas, sizeof(nomeEstatisticas), "%s_estatisticas.csv",
             config->prefixoArquivos);
    snprintf(nomeRotas, sizeof(nomeRotas), "%s_rotas.csv",
             config->prefixoArquivos);

    FILE *arquivoEstatisticas = fopen(nomeEstatisticas, "w");
    FILE *arquivoRotas = fopen(nomeRotas, "w");

    if (arquivoEstatisticas == NULL || arquivoRotas == NULL) {
        fprintf(stderr, "Aviso: nao foi possivel criar os arquivos CSV.\n");
    }

    if (arquivoEstatisticas != NULL) {
        fprintf(arquivoEstatisticas,
                "epoca,melhor_distancia,distancia_media,pior_distancia\n");
    }

    if (arquivoRotas != NULL) {
        fprintf(arquivoRotas, "epoca,distancia,rota\n");
    }

    printf("\nPopulacao inicial: %d individuos | pontos por rota: %d\n",
           tamanho, n);

    /* Epoca 0: populacao inicial, antes de qualquer evolucao. */
    EstatisticasEpoca estatisticasAtuais = calcularEstatisticas(populacaoAtual, tamanho);
    registrarEstatisticas(arquivoEstatisticas, 0, &estatisticasAtuais);
    registrarSolucao(arquivoRotas, 0, config->maxEpocas,
                     &populacaoAtual[indiceInicial], n, &estatisticasAtuais);

    int epocaAtual = 0;
    int ultimaEpocaRegistrada = 0;
    int epocasSemMelhora = 0;
    int epocaMelhorSolucao = 0;
    double tempoMelhorSolucao = segundosDesde(instanteInicial);
    MotivoParada motivoParada = PARADA_MAX_EPOCAS;
    int deveContinuar = 1;

    while (deveContinuar == 1 && epocaAtual < config->maxEpocas) {
        epocaAtual++;

        if (config->metodoSelecao == SELECAO_RANKING) {
            prepararRanking(populacaoAtual, tamanho, ranking);
        }

        /* Elitismo: o melhor da epoca anterior passa sem alteracao. */
        int indiceElite = indiceMelhor(populacaoAtual, tamanho);
        copiarIndividuo(&proximaPopulacao[0], &populacaoAtual[indiceElite], n);

        for (int posicao = 1; posicao < tamanho; posicao++) {
            int indicePai1 = selecionarPai(populacaoAtual, tamanho,
                                           config->metodoSelecao, ranking);
            int indicePai2 = selecionarPai(populacaoAtual, tamanho,
                                           config->metodoSelecao, ranking);
            Individuo *filho = &proximaPopulacao[posicao];

            if (sortearProbabilidade() < config->taxaCruzamento) {
                cruzamentoOX(&populacaoAtual[indicePai1],
                             &populacaoAtual[indicePai2],
                             filho, n, cidadeUsada);
            } else {
                memcpy(filho->rota, populacaoAtual[indicePai1].rota,
                       (size_t)n * sizeof(int));
            }

            mutar(filho, n, config->taxaMutacao, config->metodoMutacao);

            filho->distancia = calcularDistancia(filho, pontos, n);
            filho->aptidao = calcularAptidao(filho->distancia);
        }

        /* A nova populacao vira a atual; a antiga sera sobrescrita na proxima epoca. */
        Individuo *auxiliar = populacaoAtual;
        populacaoAtual = proximaPopulacao;
        proximaPopulacao = auxiliar;

        estatisticasAtuais = calcularEstatisticas(populacaoAtual, tamanho);
        registrarEstatisticas(arquivoEstatisticas, epocaAtual, &estatisticasAtuais);

        int indiceMelhorAtual = indiceMelhor(populacaoAtual, tamanho);

        if (populacaoAtual[indiceMelhorAtual].distancia <
            melhorHistorico.distancia - MELHORIA_MINIMA) {
            copiarIndividuo(&melhorHistorico, &populacaoAtual[indiceMelhorAtual], n);
            epocaMelhorSolucao = epocaAtual;
            tempoMelhorSolucao = segundosDesde(instanteInicial);
            epocasSemMelhora = 0;
        } else {
            epocasSemMelhora++;
        }

        if (epocaAtual % config->intervaloRegistro == 0) {
            registrarSolucao(arquivoRotas, epocaAtual, config->maxEpocas,
                             &populacaoAtual[indiceMelhorAtual], n,
                             &estatisticasAtuais);
            ultimaEpocaRegistrada = epocaAtual;
        }

        /* Criterios de parada: tolerancia atingida, estagnacao ou max. de epocas. */
        if (config->distanciaReferencia > 0.0 &&
            config->toleranciaErroPercentual > 0.0) {
            double erroPercentual =
                (melhorHistorico.distancia - config->distanciaReferencia) /
                config->distanciaReferencia * 100.0;

            if (erroPercentual <= config->toleranciaErroPercentual) {
                motivoParada = PARADA_TOLERANCIA;
                deveContinuar = 0;
            }
        }

        if (deveContinuar == 1 && config->limiteEstagnacao > 0 &&
            epocasSemMelhora >= config->limiteEstagnacao) {
            motivoParada = PARADA_ESTAGNACAO;
            deveContinuar = 0;
        }
    }

    /* A ultima epoca sempre fica registrada, mesmo fora do intervalo. */
    if (ultimaEpocaRegistrada != epocaAtual) {
        int indiceFinal = indiceMelhor(populacaoAtual, tamanho);
        registrarSolucao(arquivoRotas, epocaAtual, config->maxEpocas,
                         &populacaoAtual[indiceFinal], n, &estatisticasAtuais);
    }

    resultado->melhorIndividuo = melhorHistorico;
    resultado->epocasExecutadas = epocaAtual;
    resultado->epocaMelhorSolucao = epocaMelhorSolucao;
    resultado->tempoTotalSegundos = segundosDesde(instanteInicial);
    resultado->tempoMelhorSolucaoSegundos = tempoMelhorSolucao;
    resultado->motivoParada = motivoParada;

    if (arquivoEstatisticas != NULL) {
        fclose(arquivoEstatisticas);
    }

    if (arquivoRotas != NULL) {
        fclose(arquivoRotas);
    }

    liberarPopulacao(populacaoAtual, tamanho);
    liberarPopulacao(proximaPopulacao, tamanho);
    free(ranking);
    free(cidadeUsada);

    return 1;
}
