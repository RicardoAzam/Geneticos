#include <stdio.h>

#include "ponto.h"

int main()
{
    int quantidade = 8;

    printf("========================================\n");
    printf("       Geneticos\n");
    printf("========================================\n\n");

    /*
     * Cenário 1: pontos aleatórios
     */
    printf("PONTOS ALEATORIOS:\n\n");

    Ponto *pontosAleatorios = gerarPontosAleatorios(
        quantidade,
        100.0,
        100.0,
        12345
    );

    if (pontosAleatorios == NULL) {
        printf("Erro ao gerar pontos aleatorios.\n");
        return 1;
    }

    imprimirPontos(pontosAleatorios, quantidade);

    liberarPontos(pontosAleatorios);

    /*
     * Cenário 2: pontos circulares
     */
    printf("\nPONTOS CIRCULARES:\n\n");

    Ponto *pontosCirculares = gerarPontosCirculares(
        quantidade,
        50.0,
        50.0,
        40.0
    );

    if (pontosCirculares == NULL) {
        printf("Erro ao gerar pontos circulares.\n");
        return 1;
    }

    imprimirPontos(pontosCirculares, quantidade);

    liberarPontos(pontosCirculares);

    return 0;
}