#ifndef GENETICO_H
#define GENETICO_H

#include "individuo.h"
#include "ponto.h"

typedef enum {
    SELECAO_TORNEIO = 1,
    SELECAO_RANKING = 2
} MetodoSelecao;

/* Executa o algoritmo genético mantendo exatamente N indivíduos por geração. */
int executarAlgoritmoGenetico(const Ponto *pontos,
                              int quantidadePontos,
                              int tamanhoPopulacao,
                              int numeroGeracoes,
                              double taxaMutacao,
                              MetodoSelecao metodo);

#endif
