#ifndef INDIVIDUO_H
#define INDIVIDUO_H

#include "ponto.h"

typedef struct {
    int *rota;
    double distancia;
    double aptidao;
} Individuo;

/* Cria um indivíduo com uma rota */
Individuo criarIndividuo(int quantidade);

/* Calcula a distância total da rota */
double calcularDistancia(const Individuo *individuo,
                         const Ponto *pontos,
                         int quantidade);

/* Calcula a aptidão do indivíduo */
double calcularAptidao(double distancia);

/* Libera a memória da rota */
void liberarIndividuo(Individuo *individuo);

#endif
