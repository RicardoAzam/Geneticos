#include <stdio.h>
#include <math.h>

#include "ponto.h"
#include "individuo.h"

int main(void)
{
    int x, i = 1;

    while (i == 1)
    {
        printf("\n====================================\n");
        printf(" TESTES DO PROJETO TSP\n");
        printf("====================================\n");
        printf("1 - Testar pontos, rotas, distancia e aptidao\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &x) != 1) {
            printf("Entrada invalida.\n");
            return 1;
        }

        switch (x)
        {
            case 1:
            {
                int quantidade = 8, n_individuos;
                printf("Digite o numero de individuos para a populacao: ");
                if (scanf("%d", &n_individuos) != 1) {
                    printf("Entrada invalida.\n");
                    return 1;
                }

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

                // TESTE 2: ROTAS VARIADAS
                printf("\n=== TESTE DE ROTAS VARIADAS ===\n");

                for (int k = 0; k < 3; k++) {
                    Individuo individuo =
                        criarIndividuoAleatorio(quantidade,n_individuos);

                    if (individuo.rota == NULL) {
                        printf("Erro ao criar individuo.\n");
                        break;
                    }

                    individuo.distancia = calcularDistancia(
                        &individuo, aleatorios, quantidade
                    );

                    individuo.aptidao = calcularAptidao(
                        individuo.distancia
                    );

                    printf("\nRota %d: ", k + 1);

                    for (int j = 0; j < quantidade; j++) {
                        printf("%d ", individuo.rota[j]);
                    }

                    printf("\nDistancia: %.2f",
                           individuo.distancia);

                    printf(" | Aptidao: %.4f\n",
                           individuo.aptidao);

                    liberarIndividuo(&individuo);
                }

                // TESTE 3: POPULACAO INICIAL
                printf("\n=== TESTE DA POPULACAO INICIAL ===\n");

                for (int k = 0; k < 5; k++) {
                    Individuo individuo =
                        criarIndividuoAleatorio(quantidade,n_individuos);

                    if (individuo.rota == NULL) {
                        printf("Erro ao criar individuo.\n");
                        break;
                    }

                    individuo.distancia = calcularDistancia(
                        &individuo, aleatorios, quantidade
                    );

                    individuo.aptidao = calcularAptidao(
                        individuo.distancia
                    );

                    printf("\nIndividuo %d\n", k + 1);

                    printf("Rota: ");

                    for (int j = 0; j < quantidade; j++) {
                        printf("%d ", individuo.rota[j]);
                    }

                    printf("\nDistancia: %.2f",
                           individuo.distancia);

                    printf("\nAptidao: %.4f\n",
                           individuo.aptidao);

                    liberarIndividuo(&individuo);
                }

                // TESTE 4: PONTOS CIRCULARES
                printf("\n=== TESTE DE PONTOS CIRCULARES ===\n");

                Ponto *circulares = gerarPontosCirculares(
                    quantidade, 50.0, 50.0, 40.0
                );

                if (circulares == NULL) {
                    printf("Erro ao criar pontos circulares.\n");
                } else {
                    imprimirPontos(circulares, quantidade);
                    liberarPontos(circulares);
                }

                // LIBERA OS PONTOS ALEATORIOS
                liberarPontos(aleatorios);

                // TESTE 5: DISTANCIA E APTIDAO COM 3 PONTOS
                printf("\n=== TESTE DE DISTANCIA E APTIDAO ===\n");

                Ponto pontos[3] = {
                    {0.0, 0.0},
                    {3.0, 0.0},
                    {3.0, 4.0}
                };

                Individuo individuo = criarIndividuo(3);

                if (individuo.rota == NULL) {
                    printf("Erro ao criar individuo de teste.\n");
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

        if (scanf("%d", &i) != 1) {
            printf("Entrada invalida.\n");
            return 1;
        }
    }

    printf("\nPrograma encerrado.\n");

    return 0;
}
