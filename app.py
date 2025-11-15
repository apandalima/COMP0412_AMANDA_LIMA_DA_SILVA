import pandas as pd
import matplotlib.pyplot as plt
import os
ARQUIVO_CSV = 'resultados.csv'
PASTA_GRAFICOS = 'graficos'

ALGORITMO_INFO = {
    'InsertionSort': {'nome': 'Insertion Sort (O(n²))', 'cor': 'red'},
    'MergeSort':     {'nome': 'Merge Sort (O(n log n))', 'cor': 'blue'},
    'QuickSort':     {'nome': 'Quick Sort (O(n log n))', 'cor': 'green'},
}

ALGOS_GERAL = list(ALGORITMO_INFO.keys())
ALGOS_RAPIDOS = ['MergeSort', 'QuickSort']

# AUXILIARES

def garantir_pasta(pasta):
    """Garante que a pasta exista."""
    os.makedirs(pasta, exist_ok=True)

def carregar_dados(caminho):
    """Carrega o CSV e pré-processa os dados."""
    print(f"Carregando dados de '{caminho}'...")

    if not os.path.exists(caminho):
        print(f"❌ Erro: Arquivo '{caminho}' não encontrado.")
        exit()

    df = pd.read_csv(caminho)
    df['tempo_gasto_ms'] = df['tempo_s'] * 1000
    df = df[df['tempo_s'] > 0] 

    return df

def preparar_pivot(df, algoritmos):
    """
    Prepara a tabela dinâmica para plotagem.
    Retorna None se não existir dados suficientes.
    """
    df = df[df['algoritmo'].isin(algoritmos)]

    if df.empty:
        return None

    return df.pivot(
        index='tamanho_n',
        columns='algoritmo',
        values='tempo_gasto_ms'
    )


def salvar_grafico(ax, tipo_vetor, modo_zoom):
    """Salva imagem formatada corretamente."""
    titulo_modo = "ZOOM (O(n log n))" if modo_zoom else "GERAL"
    nome_arquivo = f"{PASTA_GRAFICOS}/grafico_{tipo_vetor}_{titulo_modo}.png"

    plt.title(f"Desempenho {titulo_modo} – Vetor {tipo_vetor}", fontsize=14)
    plt.xlabel("Tamanho da Instância (n)", fontsize=12)
    plt.ylabel("Tempo (ms)", fontsize=12)
    plt.grid(True, linestyle="--", alpha=0.5)
    plt.legend(title="Algoritmo")
    plt.tight_layout()

    plt.savefig(nome_arquivo)
    print(f"    ✔ Gráfico salvo em {nome_arquivo}")

    plt.close()


def plotar(df, tipo_vetor, algoritmos, modo_zoom=False):
    """Função principal de plotagem."""
    df_pivot = preparar_pivot(df, algoritmos)

    if df_pivot is None:
        print(f"    ⚠ Nenhum dado encontrado para {algoritmos} ({tipo_vetor})")
        return

    ax = df_pivot.plot(
        figsize=(10, 6),
        marker='o',
        linewidth=2,
        style=[ALGORITMO_INFO[col]['cor'] for col in df_pivot.columns]
    )

    # Renomeia legenda
    handles, labels = ax.get_legend_handles_labels()
    labels = [ALGORITMO_INFO[label]['nome'] for label in labels]
    ax.legend(handles, labels)

    salvar_grafico(ax, tipo_vetor, modo_zoom)

# MAIN

def main():
    garantir_pasta(PASTA_GRAFICOS)
    df = carregar_dados(ARQUIVO_CSV)

    tipos = df['tipo_vetor'].unique()
    print(f"Tipos encontrados: {', '.join(tipos)}")

    print("\nIniciando geração de gráficos...\n")

    for tipo in tipos:
        print(f"📌 Processando tipo: {tipo}")

        df_tipo = df[df['tipo_vetor'] == tipo]

        # Gráfico geral
        plotar(df_tipo, tipo, ALGOS_GERAL, modo_zoom=False)

        # Gráfico zoom
        plotar(df_tipo, tipo, ALGOS_RAPIDOS, modo_zoom=True)

    print("\n🎉 Finalizado! Gráficos salvos em:", PASTA_GRAFICOS)


if __name__ == "__main__":
    main()