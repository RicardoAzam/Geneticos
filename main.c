#include <stdio.h>
#include <math.h>
#include "ponto.h"
#include "individuo.h"

int main(void)
{
    int x, i = 1;

    while (i == 1)
    {
        printf("\n1 - Testar criacao de pontos, distancia e aptidao\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &x);

        switch (x)
        {
            case 1:
            {
                int quantidade = 8;

                // TESTE 1: PONTOS ALEATORIOS
                printf("\n=== TESTE DE PONTOS ALEATORIOS ===\n");

                Ponto *aleatorios = gerarPontosAleatorios(
                    quantidade, 100.0, 100.0, 12345
                );

                if (aleatorios == NULL) {
                    printf("Erro ao criar pontos aleatorios.\n");
                    break;
                }

                imprimirPontos(aleatorios, quantidade);
                liberarPontos(aleatorios);

                // TESTE 2: PONTOS CIRCULARES
                printf("\n=== TESTE DE PONTOS CIRCULARES ===\n");

                Ponto *circulares = gerarPontosCirculares(
                    quantidade, 50.0, 50.0, 40.0
                );

                if (circulares == NULL) {
                    printf("Erro ao criar pontos circulares.\n");
                    break;
                }

                imprimirPontos(circulares, quantidade);
                liberarPontos(circulares);

                // TESTE 3: DISTANCIA E APTIDAO
                printf("\n=== TESTE DE DISTANCIA E APTIDAO ===\n");

                Ponto pontos[3] = {
                    {0.0, 0.0},
                    {3.0, 0.0},
                    {3.0, 4.0}
                };

                Individuo individuo = criarIndividuo(3);

                if (individuo.rota == NULL) {
                    printf("Erro ao criar individuo.\n");
                    break;
                }

                printf("\nPontos utilizados:\n");

                for (int j = 0; j < 3; j++) {
                    printf("Ponto %d: (%.1f, %.1f)\n",
                           j, pontos[j].x, pontos[j].y);
                }

                printf("\nRota: ");

                for (int j = 0; j < 3; j++) {
                    printf("%d ", individuo.rota[j]);
                }

                printf("\n\nDistancias entre os pontos:\n");

                for (int j = 0; j < 3; j++) {
                    int atual = individuo.rota[j];
                    int proximo = individuo.rota[(j + 1) % 3];

                    double dx = pontos[atual].x - pontos[proximo].x;
                    double dy = pontos[atual].y - pontos[proximo].y;

                    double distancia = sqrt(dx * dx + dy * dy);

                    printf("Ponto %d -> Ponto %d: %.2f\n",
                           atual, proximo, distancia);
                }

                individuo.distancia = calcularDistancia(
                    &individuo, pontos, 3
                );

                individuo.aptidao = calcularAptidao(
                    individuo.distancia
                );

                printf("\nDistancia total: %.2f\n",
                       individuo.distancia);

                printf("Aptidao: %.4f\n",
                       individuo.aptidao);

                liberarIndividuo(&individuo);

                break;
            }

            default:
                printf("Opcao invalida.\n");
                break;
        }

        printf("\nDigite 1 para continuar ou 0 para sair: ");
        scanf("%d", &i);
    }

    printf("\nPrograma encerrado.\n");

    return 0;
}
