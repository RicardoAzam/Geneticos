#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ponto.h"
#include "genetico.h"

#define QUANTIDADE_PONTOS 8

static int lerInteiro(const char *mensagem, int minimo, int *valor)
{
    printf("%s", mensagem);
    if (scanf("%d", valor) != 1 || *valor < minimo) {
        printf("Entrada invalida. O valor minimo permitido e %d.\n", minimo);
        return 0;
    }
    return 1;
}

int main(void)
{
    int opcao;
    srand((unsigned int)time(NULL));

    for (;;) {
        printf("\n====================================\n");
        printf(" ALGORITMO GENETICO - TSP (8 PONTOS)\n");
        printf("====================================\n");
        printf("1 - Selecao por Torneio\n");
        printf("2 - Selecao por Ranking\n");
        printf("0 - Sair\n");
        printf("Escolha o metodo: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Entrada invalida.\n");
            return 1;
        }
        if (opcao == 0) break;
        if (opcao != 1 && opcao != 2) {
            printf("Opcao invalida. Escolha 1, 2 ou 0.\n");
            continue;
        }

        int tamanhoPopulacao, geracoes;
        if (!lerInteiro("Tamanho da populacao (minimo 3): ", 3, &tamanhoPopulacao) ||
            !lerInteiro("Numero de geracoes (minimo 1): ", 1, &geracoes)) {
            continue;
        }

        double taxaMutacao;
        printf("Taxa de mutacao (0 a 1, por exemplo 0.10): ");
        if (scanf("%lf", &taxaMutacao) != 1 || taxaMutacao < 0.0 || taxaMutacao > 1.0) {
            printf("Taxa invalida. Use um valor entre 0 e 1.\n");
            continue;
        }

        /* Semente diferente para gerar coordenadas e evoluir a populacao. */
        unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)rand();
        Ponto *pontos = gerarPontosAleatorios(QUANTIDADE_PONTOS, 100.0, 100.0, seed);
        if (pontos == NULL) {
            fprintf(stderr, "Erro ao gerar os pontos.\n");
            return 1;
        }

        printf("\n===== PONTOS DO PROBLEMA =====\n");
        imprimirPontos(pontos, QUANTIDADE_PONTOS);

        MetodoSelecao metodo = (opcao == 1) ? SELECAO_TORNEIO : SELECAO_RANKING;
        executarAlgoritmoGenetico(pontos, QUANTIDADE_PONTOS,
                                  tamanhoPopulacao, geracoes,
                                  taxaMutacao, metodo);
        liberarPontos(pontos);
    }

    printf("Programa encerrado.\n");
    return 0;
}
