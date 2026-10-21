#ifndef PONTO_H
#define PONTO_H

typedef struct {
    double x;
    double y;
} Ponto;

Ponto *gerarPontosAleatorios(int quantidade,
                             double limite_x,
                             double limite_y,
                             unsigned int seed);

Ponto *gerarPontosCirculares(int quantidade,
                             double centro_x,
                             double centro_y,
                             double raio);

void imprimirPontos(const Ponto *pontos, int quantidade);

void liberarPontos(Ponto *pontos);

#endif // PONTO_H