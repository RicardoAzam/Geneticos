#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ponto.h"
#include "genetico.h"

#define MINIMO_PONTOS 8
#define LIMITE_X 100.0
#define LIMITE_Y 100.0
#define CENTRO_CIRCULO_X 50.0
#define CENTRO_CIRCULO_Y 50.0
#define RAIO_CIRCULO 50.0
#define LIMITE_PONTOS_PARA_IMPRIMIR 30

typedef enum {
    CENARIO_ALEATORIO = 1,
    CENARIO_CIRCULAR = 2
} Cenario;

static void descartarLinha(void)
{
    int caractere = getchar();

    while (caractere != '\n' && caractere != EOF) {
        caractere = getchar();
    }
}

static int lerInteiro(const char *mensagem, int minimo, int maximo, int *valor)
{
    printf("%s", mensagem);

    if (scanf("%d", valor) != 1 || *valor < minimo || *valor > maximo) {
        printf("Entrada invalida. Use um valor entre %d e %d.\n", minimo, maximo);
        descartarLinha();
        return 0;
    }

    return 1;
}

static int lerReal(const char *mensagem, double minimo, double maximo, double *valor)
{
    printf("%s", mensagem);

    if (scanf("%lf", valor) != 1 || *valor < minimo || *valor > maximo) {
        printf("Entrada invalida. Use um valor entre %.2f e %.2f.\n", minimo, maximo);
        descartarLinha();
        return 0;
    }

    return 1;
}

static const char *nomeCenario(Cenario cenario)
{
    if (cenario == CENARIO_CIRCULAR) {
        return "circular";
    }

    return "aleatorio";
}

static const char *nomeSelecao(MetodoSelecao metodo)
{
    if (metodo == SELECAO_RANKING) {
        return "ranking";
    }

    return "torneio";
}

static const char *nomeMutacao(MetodoMutacao metodo)
{
    if (metodo == MUTACAO_INVERSAO) {
        return "inversao";
    }

    return "troca";
}

static const char *descricaoMotivoParada(MotivoParada motivo)
{
    if (motivo == PARADA_TOLERANCIA) {
        return "erro abaixo da tolerancia";
    }

    if (motivo == PARADA_ESTAGNACAO) {
        return "estagnacao (sem melhora)";
    }

    return "maximo de epocas";
}

/* Le todos os parametros do AG. Retorna 0 se alguma entrada for invalida. */
static int lerConfiguracao(Cenario cenario, ConfigGenetico *config,
                           unsigned int *semente)
{
    int opcaoSelecao;
    int opcaoMutacao;
    int valorSemente;

    if (!lerInteiro("Quantidade de pontos (minimo 8): ",
                    MINIMO_PONTOS, 100000, &config->quantidadePontos) ||
        !lerInteiro("Metodo de selecao (1 = Torneio, 2 = Ranking): ",
                    1, 2, &opcaoSelecao) ||
        !lerInteiro("Metodo de mutacao (1 = Troca, 2 = Inversao): ",
                    1, 2, &opcaoMutacao) ||
        !lerInteiro("Tamanho da populacao (minimo 3): ",
                    3, 100000, &config->tamanhoPopulacao) ||
        !lerInteiro("Numero maximo de epocas (minimo 1): ",
                    1, 10000000, &config->maxEpocas) ||
        !lerReal("Taxa de mutacao (0 a 1, por exemplo 0.10): ",
                 0.0, 1.0, &config->taxaMutacao) ||
        !lerReal("Taxa de cruzamento (0 a 1, por exemplo 0.90): ",
                 0.0, 1.0, &config->taxaCruzamento) ||
        !lerInteiro("Parar apos quantas epocas sem melhora (0 desativa): ",
                    0, 10000000, &config->limiteEstagnacao)) {
        return 0;
    }

    config->toleranciaErroPercentual = 0.0;

    if (cenario == CENARIO_CIRCULAR) {
        if (!lerReal("Parar quando o erro for menor que (%%, 0 desativa): ",
                     0.0, 100.0, &config->toleranciaErroPercentual)) {
            return 0;
        }
    }

    if (!lerInteiro("Registrar a rota a cada quantas epocas (minimo 1): ",
                    1, 10000000, &config->intervaloRegistro) ||
        !lerInteiro("Semente aleatoria (0 usa o relogio): ",
                    0, 2147483647, &valorSemente)) {
        return 0;
    }

    config->metodoSelecao = (MetodoSelecao)opcaoSelecao;
    config->metodoMutacao = (MetodoMutacao)opcaoMutacao;

    if (valorSemente == 0) {
        *semente = (unsigned int)time(NULL);
    } else {
        *semente = (unsigned int)valorSemente;
    }

    return 1;
}

static void salvarResumo(const ConfigGenetico *config,
                         const ResultadoGenetico *resultado,
                         Cenario cenario, unsigned int semente,
                         double erroPercentual)
{
    char nomeArquivo[200];
    snprintf(nomeArquivo, sizeof(nomeArquivo), "%s_resumo.csv",
             config->prefixoArquivos);

    FILE *arquivo = fopen(nomeArquivo, "w");

    if (arquivo == NULL) {
        fprintf(stderr, "Aviso: nao foi possivel criar %s.\n", nomeArquivo);
        return;
    }

    fprintf(arquivo,
            "cenario,pontos,populacao,epocas_maximas,epocas_executadas,"
            "taxa_mutacao,taxa_cruzamento,selecao,mutacao,semente,"
            "melhor_distancia,epoca_melhor,tempo_total_s,tempo_melhor_s,"
            "motivo_parada,distancia_referencia,erro_percentual\n");

    fprintf(arquivo, "%s,%d,%d,%d,%d,%.4f,%.4f,%s,%s,%u,%.6f,%d,%.4f,%.4f,%s,%.6f,%.6f\n",
            nomeCenario(cenario),
            config->quantidadePontos,
            config->tamanhoPopulacao,
            config->maxEpocas,
            resultado->epocasExecutadas,
            config->taxaMutacao,
            config->taxaCruzamento,
            nomeSelecao(config->metodoSelecao),
            nomeMutacao(config->metodoMutacao),
            semente,
            resultado->melhorIndividuo.distancia,
            resultado->epocaMelhorSolucao,
            resultado->tempoTotalSegundos,
            resultado->tempoMelhorSolucaoSegundos,
            descricaoMotivoParada(resultado->motivoParada),
            config->distanciaReferencia,
            erroPercentual);

    fclose(arquivo);
    printf("Resumo salvo em: %s\n", nomeArquivo);
}

static void apresentarResultado(const ConfigGenetico *config,
                                const ResultadoGenetico *resultado,
                                Cenario cenario, unsigned int semente)
{
    const Individuo *melhor = &resultado->melhorIndividuo;
    double erroPercentual = 0.0;

    printf("\n===== RESULTADO FINAL =====\n");
    printf("Cenario: %s | Pontos: %d | Semente: %u\n",
           nomeCenario(cenario), config->quantidadePontos, semente);

    if (config->quantidadePontos <= LIMITE_PONTOS_PARA_IMPRIMIR) {
        printf("Melhor rota: ");
        imprimirRota(melhor, config->quantidadePontos);
        printf("\n");
    }

    if (config->distanciaReferencia > 0.0) {
        erroPercentual = (melhor->distancia - config->distanciaReferencia) /
                         config->distanciaReferencia * 100.0;

        /* Ao atingir o otimo, a soma em ponto flutuante pode ficar 1e-14 abaixo da formula. */
        if (erroPercentual < 0.0) {
            erroPercentual = 0.0;
        }

        printf("Distancia otima de referencia: %.3f\n", config->distanciaReferencia);
    }

    printf("Melhor solucao encontrada:     %.3f\n", melhor->distancia);
    printf("Aptidao (1/distancia):         %.6f\n", melhor->aptidao);

    if (config->distanciaReferencia > 0.0) {
        printf("Erro:                          %.4f%%\n", erroPercentual);
    }

    printf("Epoca da melhor solucao:       %d\n", resultado->epocaMelhorSolucao);
    printf("Epocas executadas:             %d (parada por: %s)\n",
           resultado->epocasExecutadas,
           descricaoMotivoParada(resultado->motivoParada));
    printf("Tempo ate a melhor solucao:    %.3f s\n",
           resultado->tempoMelhorSolucaoSegundos);
    printf("Tempo total:                   %.3f s\n",
           resultado->tempoTotalSegundos);

    salvarResumo(config, resultado, cenario, semente, erroPercentual);
}

int main(void)
{
    for (;;) {
        int opcaoCenario;

        printf("\n====================================\n");
        printf(" ALGORITMO GENETICO - TSP\n");
        printf("====================================\n");
        printf("1 - Pontos aleatorios (distribuicao uniforme)\n");
        printf("2 - Pontos em circulo (benchmark)\n");
        printf("0 - Sair\n");
        printf("Escolha o cenario: ");

        int leitura = scanf("%d", &opcaoCenario);

        if (leitura == EOF) {
            break;
        }

        if (leitura != 1) {
            printf("Entrada invalida.\n");
            descartarLinha();
            continue;
        }

        if (opcaoCenario == 0) {
            break;
        }

        if (opcaoCenario != CENARIO_ALEATORIO && opcaoCenario != CENARIO_CIRCULAR) {
            printf("Opcao invalida. Escolha 1, 2 ou 0.\n");
            continue;
        }

        Cenario cenario = (Cenario)opcaoCenario;
        ConfigGenetico config;
        unsigned int semente;

        if (!lerConfiguracao(cenario, &config, &semente)) {
            continue;
        }

        /* Mesma semente para os pontos aleatorios e para a evolucao. */
        srand(semente);

        Ponto *pontos = NULL;
        config.distanciaReferencia = 0.0;

        if (cenario == CENARIO_CIRCULAR) {
            pontos = gerarPontosCirculares(config.quantidadePontos,
                                           CENTRO_CIRCULO_X, CENTRO_CIRCULO_Y,
                                           RAIO_CIRCULO);
            config.distanciaReferencia =
                calcularPerimetroOtimoCirculo(config.quantidadePontos, RAIO_CIRCULO);
        } else {
            pontos = gerarPontosAleatorios(config.quantidadePontos,
                                           LIMITE_X, LIMITE_Y, semente);
        }

        if (pontos == NULL) {
            fprintf(stderr, "Erro ao gerar os pontos.\n");
            return 1;
        }

        snprintf(config.prefixoArquivos, sizeof(config.prefixoArquivos),
                 "%s_%dpontos_%s_%s",
                 nomeCenario(cenario), config.quantidadePontos,
                 nomeSelecao(config.metodoSelecao),
                 nomeMutacao(config.metodoMutacao));

        if (config.quantidadePontos <= LIMITE_PONTOS_PARA_IMPRIMIR) {
            printf("\n===== PONTOS DO PROBLEMA =====\n");
            imprimirPontos(pontos, config.quantidadePontos);
        }

        char nomeArquivoPontos[200];
        snprintf(nomeArquivoPontos, sizeof(nomeArquivoPontos), "%s_pontos.csv",
                 config.prefixoArquivos);

        if (!salvarPontosCSV(nomeArquivoPontos, pontos, config.quantidadePontos)) {
            fprintf(stderr, "Aviso: nao foi possivel criar %s.\n", nomeArquivoPontos);
        }

        ResultadoGenetico resultado;

        if (executarAlgoritmoGenetico(pontos, &config, &resultado)) {
            apresentarResultado(&config, &resultado, cenario, semente);
            printf("Arquivos gerados com o prefixo: %s\n", config.prefixoArquivos);
            liberarIndividuo(&resultado.melhorIndividuo);
        }

        liberarPontos(pontos);
    }

    printf("Programa encerrado.\n");
    return 0;
}
