
#include "individuo.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

Individuo criarIndividuo(int quantidade)
{
    Individuo individuo;

    individuo.rota = NULL;
    individuo.distancia = 0.0;
    individuo.aptidao = 0.0;

    if (quantidade <= 0) {
        return individuo;
    }

    individuo.rota = malloc(quantidade * sizeof(int));

    if (individuo.rota == NULL) {
        return individuo;
    }

    for (int i = 0; i < quantidade; i++) {
        individuo.rota[i] = i;
    }

    return individuo;
}

double calcularDistancia(const Individuo *individuo,
                         const Ponto *pontos,
                         int quantidade)
{
    //d = raiz /- {(x_2-x_1)^2+(y_2-y_1)^2 -
    if (individuo == NULL ||
        individuo->rota == NULL ||
        pontos == NULL ||
        quantidade <= 0) 
        {
        return -1.0;
        }

    double distanciaTotal = 0.0;

    for (int i = 0; i < quantidade; i++) {
        int atual = individuo->rota[i];
        int proximo = individuo->rota[(i + 1) % quantidade];

        double dx = pontos[atual].x - pontos[proximo].x;
        double dy = pontos[atual].y - pontos[proximo].y;

        distanciaTotal += sqrt(dx * dx + dy * dy);
    }

    return distanciaTotal;
}

double calcularAptidao(double distancia)
{
    //Cada vez menor a distancia menor a aptidao
    if (distancia <= 0.0) {
        return 0.0;
    }

    return 1.0 / distancia;
}

void imprimirRota(const Individuo *individuo, int quantidade)
{
    if (individuo == NULL || individuo->rota == NULL || quantidade <= 0) {
        return;
    }

    for (int i = 0; i < quantidade; i++) {
        printf("%d -> ", individuo->rota[i]);
    }

    /* Retorno ao ponto inicial, fechando o ciclo. */
    printf("%d", individuo->rota[0]);
}

void liberarIndividuo(Individuo *individuo)
{
    if (individuo == NULL) {
        return;
    }

    free(individuo->rota);

    individuo->rota = NULL;
    individuo->distancia = 0.0;
    individuo->aptidao = 0.0;
}

void embaralharRota(Individuo *individuo, int quantidade)
{
    if (individuo == NULL ||
        individuo->rota == NULL ||
        quantidade <= 1) {
        return;
    }

    for (int i = quantidade - 1; i > 0; i--) {
        int j = rand() % (i + 1);

        int temp = individuo->rota[i];
        individuo->rota[i] = individuo->rota[j];
        individuo->rota[j] = temp;
    }
}
