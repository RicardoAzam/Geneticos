import csv
import math
import sys
from pathlib import Path

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation, PillowWriter

QUANTIDADE_PAINEL_ROTAS = 6
MAXIMO_QUADROS_ANIMACAO = 60
PONTOS_ATE_MARCADOR_GRANDE = 60


def ler_csv(caminho):
    if not caminho.exists():
        print(f"Arquivo nao encontrado: {caminho}")
        sys.exit(1)

    with open(caminho, "r", newline="", encoding="utf-8") as arquivo:
        return list(csv.DictReader(arquivo))


def ler_estatisticas(prefixo):
    linhas = ler_csv(Path(f"{prefixo}_estatisticas.csv"))

    epocas = [int(linha["epoca"]) for linha in linhas]
    melhores = [float(linha["melhor_distancia"]) for linha in linhas]
    medias = [float(linha["distancia_media"]) for linha in linhas]
    piores = [float(linha["pior_distancia"]) for linha in linhas]

    return epocas, melhores, medias, piores


def ler_pontos(prefixo):
    linhas = ler_csv(Path(f"{prefixo}_pontos.csv"))

    coordenadas_x = [float(linha["x"]) for linha in linhas]
    coordenadas_y = [float(linha["y"]) for linha in linhas]

    return coordenadas_x, coordenadas_y


def ler_rotas(prefixo):
    linhas = ler_csv(Path(f"{prefixo}_rotas.csv"))
    rotas = []

    for linha in linhas:
        rotas.append({
            "epoca": int(linha["epoca"]),
            "distancia": float(linha["distancia"]),
            "rota": [int(cidade) for cidade in linha["rota"].split()],
        })

    return rotas


def ler_resumo(prefixo):
    linhas = ler_csv(Path(f"{prefixo}_resumo.csv"))
    return linhas[0]


def escolher_indices(total, quantidade):
    """Escolhe indices igualmente espacados, sempre incluindo o primeiro e o ultimo."""
    if total <= quantidade:
        return list(range(total))

    passo = (total - 1) / (quantidade - 1)
    indices = set()

    for posicao in range(quantidade):
        indices.add(round(posicao * passo))

    return sorted(indices)


def escolher_indices_por_progresso(rotas, quantidade):
    """
    Escolhe rotas cuja distancia diminui em escala logaritmica entre a primeira e
    a ultima registrada. Assim o painel mostra etapas visualmente diferentes da
    evolucao, mesmo que as epocas tenham sido registradas de forma esparsa.
    """
    if len(rotas) <= quantidade:
        return list(range(len(rotas)))

    distancia_inicial = rotas[0]["distancia"]
    distancia_final = rotas[-1]["distancia"]
    indices = {0, len(rotas) - 1}

    for posicao in range(1, quantidade - 1):
        fracao = posicao / (quantidade - 1)
        distancia_alvo = distancia_inicial * (distancia_final / distancia_inicial) ** fracao

        indice_mais_proximo = 0
        menor_diferenca = abs(math.log(rotas[0]["distancia"]) - math.log(distancia_alvo))

        for indice, solucao in enumerate(rotas):
            diferenca = abs(math.log(solucao["distancia"]) - math.log(distancia_alvo))

            if diferenca < menor_diferenca:
                menor_diferenca = diferenca
                indice_mais_proximo = indice

        indices.add(indice_mais_proximo)

    return sorted(indices)


def desenhar_rota(eixo, coordenadas_x, coordenadas_y, solucao):
    rota_fechada = solucao["rota"] + [solucao["rota"][0]]
    rota_x = [coordenadas_x[cidade] for cidade in rota_fechada]
    rota_y = [coordenadas_y[cidade] for cidade in rota_fechada]

    if len(coordenadas_x) <= PONTOS_ATE_MARCADOR_GRANDE:
        tamanho_marcador = 25
        camada_marcadores = 3
        camada_linha = 2
    else:
        # Muitos pontos: marcadores pequenos e linha por cima, para a rota continuar visivel.
        tamanho_marcador = 1.5
        camada_marcadores = 2
        camada_linha = 3

    eixo.plot(rota_x, rota_y, linewidth=0.8, color="tab:blue", zorder=camada_linha)
    eixo.scatter(coordenadas_x, coordenadas_y, s=tamanho_marcador, color="tab:red",
                 zorder=camada_marcadores)
    eixo.set_aspect("equal")
    eixo.set_xticks([])
    eixo.set_yticks([])


def gerar_grafico_convergencia(prefixo, resumo):
    epocas, melhores, medias, piores = ler_estatisticas(prefixo)
    distancia_referencia = float(resumo["distancia_referencia"])
    epoca_melhor = int(resumo["epoca_melhor"])

    plt.figure(figsize=(10, 6))

    plt.plot(epocas, melhores, label="Melhor distancia")
    plt.plot(epocas, medias, label="Distancia media")
    plt.plot(epocas, piores, label="Pior distancia", alpha=0.6)

    if distancia_referencia > 0.0:
        plt.axhline(distancia_referencia, color="green", linestyle="--",
                    label=f"Otimo (referencia): {distancia_referencia:.2f}")

    plt.axvline(epoca_melhor, color="gray", linestyle=":",
                label=f"Melhor solucao (epoca {epoca_melhor})")

    plt.xlabel("Epoca")
    plt.ylabel("Distancia da rota")
    plt.title(f"Evolucao da populacao - {Path(prefixo).name}")
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    imagem = f"{prefixo}_convergencia.png"
    plt.savefig(imagem, dpi=150)
    print(f"Grafico salvo em: {imagem}")


def gerar_painel_rotas(prefixo, coordenadas_x, coordenadas_y, rotas):
    indices = escolher_indices_por_progresso(rotas, QUANTIDADE_PAINEL_ROTAS)
    colunas = 3
    linhas = (len(indices) + colunas - 1) // colunas

    figura, eixos = plt.subplots(linhas, colunas, figsize=(4.5 * colunas, 4.8 * linhas),
                                 squeeze=False, constrained_layout=True)

    for posicao, eixo in enumerate(eixos.flatten()):
        if posicao >= len(indices):
            eixo.axis("off")
            continue

        solucao = rotas[indices[posicao]]
        desenhar_rota(eixo, coordenadas_x, coordenadas_y, solucao)
        eixo.set_title(f"Epoca {solucao['epoca']} | distancia {solucao['distancia']:.1f}")

    figura.suptitle(f"Melhor rota em etapas da evolucao - {Path(prefixo).name}")

    imagem = f"{prefixo}_rotas.png"
    figura.savefig(imagem, dpi=130)
    print(f"Grafico salvo em: {imagem}")


def gerar_animacao(prefixo, coordenadas_x, coordenadas_y, rotas, epoca_melhor):
    # Depois da melhor solucao nada muda; a animacao termina na primeira rota registrada ali.
    ultimo_quadro = len(rotas) - 1

    for indice, solucao in enumerate(rotas):
        if solucao["epoca"] >= epoca_melhor:
            ultimo_quadro = indice
            break

    quadros = escolher_indices(ultimo_quadro + 1, MAXIMO_QUADROS_ANIMACAO)

    figura, eixo = plt.subplots(figsize=(6, 6))

    def desenhar_quadro(indice):
        eixo.clear()
        solucao = rotas[indice]
        desenhar_rota(eixo, coordenadas_x, coordenadas_y, solucao)
        eixo.set_title(f"Epoca {solucao['epoca']} | distancia {solucao['distancia']:.1f}")

    animacao = FuncAnimation(figura, desenhar_quadro, frames=quadros, repeat=False)

    arquivo = f"{prefixo}_evolucao.gif"
    animacao.save(arquivo, writer=PillowWriter(fps=4))
    plt.close(figura)
    print(f"Animacao salva em: {arquivo}")


def gerar_graficos(prefixo):
    resumo = ler_resumo(prefixo)
    coordenadas_x, coordenadas_y = ler_pontos(prefixo)
    rotas = ler_rotas(prefixo)

    if not rotas:
        print("O arquivo de rotas nao possui resultados.")
        return

    gerar_grafico_convergencia(prefixo, resumo)
    gerar_painel_rotas(prefixo, coordenadas_x, coordenadas_y, rotas)
    gerar_animacao(prefixo, coordenadas_x, coordenadas_y, rotas,
                   int(resumo["epoca_melhor"]))

    plt.show()


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python3 graficos.py <prefixo>")
        print("Exemplo: python3 graficos.py circular_30pontos_torneio_inversao")
        sys.exit(1)

    gerar_graficos(sys.argv[1])
