import csv
import sys
from pathlib import Path

import matplotlib.pyplot as plt


def ler_resultados(nome_arquivo):
    geracoes = []
    melhores = []
    medias = []
    piores = []

    with open(nome_arquivo, "r", newline="", encoding="utf-8") as arquivo:
        leitor = csv.DictReader(arquivo)

        for linha in leitor:
            geracoes.append(int(linha["geracao"]))
            melhores.append(float(linha["melhor_distancia"]))
            medias.append(float(linha["distancia_media"]))
            piores.append(float(linha["pior_distancia"]))

    return geracoes, melhores, medias, piores


def gerar_grafico(nome_arquivo):
    caminho = Path(nome_arquivo)

    if not caminho.exists():
        print(f"Arquivo nao encontrado: {caminho}")
        return

    geracoes, melhores, medias, piores = ler_resultados(caminho)

    if not geracoes:
        print("O arquivo nao possui resultados.")
        return

    plt.figure(figsize=(10, 6))

    plt.plot(geracoes, melhores, label="Melhor distancia")
    plt.plot(geracoes, medias, label="Distancia media")
    plt.plot(geracoes, piores, label="Pior distancia")

    plt.xlabel("Geracao")
    plt.ylabel("Distancia da rota")
    plt.title(f"Evolucao da populacao - {caminho.stem}")
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    imagem = caminho.with_name(caminho.stem + "_grafico.png")
    plt.savefig(imagem, dpi=150)

    print(f"Grafico salvo em: {imagem}")
    plt.show()


if __name__ == "__main__":
    nome = (
        sys.argv[1]
        if len(sys.argv) > 1
        else "resultados_torneio.csv"
    )

    gerar_grafico(nome)
