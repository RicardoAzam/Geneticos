#include "ponto.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.14159265358979323846

Ponto *gerarPontosAleatorios(int quantidade,
                             double limite_x,
                             double limite_y,
                             unsigned int seed)
{
    if (quantidade <= 0 || limite_x < 0 || limite_y < 0) {
        return NULL;
    }

    Ponto *pontos = malloc(quantidade * sizeof(Ponto));

    if (pontos == NULL) {
        return NULL;
    }

    srand(seed);

    for (int i = 0; i < quantidade; i++) {
        pontos[i].x = ((double)rand() / RAND_MAX) * limite_x;
        pontos[i].y = ((double)rand() / RAND_MAX) * limite_y;
    }

    return pontos;
}

Ponto *gerarPontosCirculares(int quantidade,
                             double centro_x,
                             double centro_y,
                             double raio)
{
    if (quantidade <= 0 || raio < 0) {
        return NULL;
    }

    Ponto *pontos = malloc(quantidade * sizeof(Ponto));

    if (pontos == NULL) {
        return NULL;
    }

    for (int i = 0; i < quantidade; i++) {

        double angulo = (2.0 * PI * i) / quantidade;

        pontos[i].x = centro_x + raio * cos(angulo);
        pontos[i].y = centro_y + raio * sin(angulo);
    }

    return pontos;
}

double calcularPerimetroOtimoCirculo(int quantidade, double raio)
{
    if (quantidade < 3 || raio < 0) {
        return 0.0;
    }

    /* Lado do poligono regular de N vertices inscrito no circulo: 2 r sen(pi/N). */
    double ladoDoPoligono = 2.0 * raio * sin(PI / quantidade);

    return quantidade * ladoDoPoligono;
}

void imprimirPontos(const Ponto *pontos, int quantidade)
{
    if (pontos == NULL || quantidade <= 0) {
        return;
    }

    for (int i = 0; i < quantidade; i++) {
        printf("Ponto %d: (%.2f, %.2f)\n",
               i,
               pontos[i].x,
               pontos[i].y);
    }
}

void liberarPontos(Ponto *pontos)
{
    free(pontos);
}

int salvarPontosCSV(const char *nomeArquivo, const Ponto *pontos, int quantidade)
{
    if (nomeArquivo == NULL || pontos == NULL || quantidade <= 0) {
        return 0;
    }

    FILE *arquivo = fopen(nomeArquivo, "w");

    if (arquivo == NULL) {
        return 0;
    }

    fprintf(arquivo, "indice,x,y\n");

    for (int i = 0; i < quantidade; i++) {
        fprintf(arquivo, "%d,%.6f,%.6f\n", i, pontos[i].x, pontos[i].y);
    }

    fclose(arquivo);
    return 1;
}
