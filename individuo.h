
#ifndef INDIVIDUO_H
#define INDIVIDUO_H

#include "ponto.h"

typedef struct {
    int *rota;
    double distancia;
    double aptidao;
} Individuo;

Individuo criarIndividuo(int quantidade);

void embaralharRota(Individuo *individuo, int quantidade);

Individuo criarIndividuoAleatorio(int quantidade, int n_individuos);

double calcularDistancia(const Individuo *individuo,
                         const Ponto *pontos,
                         int quantidade);

double calcularAptidao(double distancia);

void liberarIndividuo(Individuo *individuo);

#endif
