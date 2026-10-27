#ifndef GENETICO_H
#define GENETICO_H

#include "individuo.h"
#include "ponto.h"

typedef enum {
    SELECAO_TORNEIO = 1,
    SELECAO_RANKING = 2
} MetodoSelecao;

typedef enum {
    MUTACAO_TROCA = 1,
    MUTACAO_INVERSAO = 2
} MetodoMutacao;

typedef enum {
    PARADA_MAX_EPOCAS = 1,
    PARADA_ESTAGNACAO = 2,
    PARADA_TOLERANCIA = 3
} MotivoParada;

typedef struct {
    int quantidadePontos;
    int tamanhoPopulacao;
    int maxEpocas;

    double taxaMutacao;
    double taxaCruzamento;

    MetodoSelecao metodoSelecao;
    MetodoMutacao metodoMutacao;

    /* Criterios de parada alem de maxEpocas (valor 0 desativa o criterio). */
    int limiteEstagnacao;              /* epocas seguidas sem melhora */
    double toleranciaErroPercentual;   /* so vale se distanciaReferencia > 0 */
    double distanciaReferencia;        /* otimo conhecido (circulo); 0 se desconhecido */

    /* De quantas em quantas epocas a rota e gravada e mostrada na tela. */
    int intervaloRegistro;

    /* Os arquivos gerados comecam com este prefixo (ex.: "circular_8pontos_torneio_troca"). */
    char prefixoArquivos[160];
} ConfigGenetico;

typedef struct {
    Individuo melhorIndividuo;   /* quem recebe deve liberar com liberarIndividuo */
    int epocasExecutadas;
    int epocaMelhorSolucao;
    double tempoTotalSegundos;
    double tempoMelhorSolucaoSegundos;
    MotivoParada motivoParada;
} ResultadoGenetico;

/*
 * Executa o algoritmo genetico mantendo exatamente tamanhoPopulacao individuos
 * por epoca. A semente do rand() deve ser definida por quem chama (srand).
 *
 * Arquivos gerados:
 *   <prefixo>_estatisticas.csv : epoca,melhor_distancia,distancia_media,pior_distancia
 *   <prefixo>_rotas.csv        : epoca,distancia,rota (melhor rota das epocas registradas)
 */
int executarAlgoritmoGenetico(const Ponto *pontos,
                              const ConfigGenetico *config,
                              ResultadoGenetico *resultado);

#endif
