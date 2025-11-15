#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <windows.h>

//MERGESORT 

void merge(int *A, int *B, int esq, int meio, int dir) {
    int i = esq, j = meio + 1, k = esq;

    while (i <= meio && j <= dir) {
        B[k++] = (A[i] <= A[j]) ? A[i++] : A[j++];
    }
    while (i <= meio)  B[k++] = A[i++];
    while (j <= dir)   B[k++] = A[j++];

    memcpy(A + esq, B + esq, (dir - esq + 1) * sizeof(int));
}

void mergesort_helper(int *A, int *B, int esq, int dir) {
    if (esq >= dir) return;
    int meio = (esq + dir) / 2;
    mergesort_helper(A, B, esq, meio);
    mergesort_helper(A, B, meio + 1, dir);
    merge(A, B, esq, meio, dir);
}

void mergesort2(int *A, int n) {
    if (n < 2) return;
    int *B = malloc(n * sizeof(int));
    if (!B) {
        fprintf(stderr, "Erro: falha no MergeSort.\n");
        return;
    }
    mergesort_helper(A, B, 0, n - 1);
    free(B);
}

// QUICKSORT

static inline void trocar(int *a, int *b) {
    int t = *a; *a = *b; *b = t;
}

int particionar(int *A, int inicio, int fim) {
    int pivo = A[fim];
    int i = inicio - 1;

    for (int j = inicio; j < fim; j++) {
        if (A[j] < pivo) {
            trocar(&A[++i], &A[j]);
        }
    }
    trocar(&A[i + 1], &A[fim]);
    return i + 1;
}

void recursao(int *A, int inicio, int fim) {
    if (inicio < fim) {
        int pi = particionar(A, inicio, fim);
        recursao(A, inicio, pi - 1);
        recursao(A, pi + 1, fim);
    }
}

void quicksort(int *A, int n) {
    if (n < 2) return;
    recursao(A, 0, n - 1);
}

// INSERTION

void insertion(int *A, int n) {
    for (int i = 1; i < n; i++) {
        int chave = A[i];
        int j = i - 1;
        while (j >= 0 && A[j] > chave) {
            A[j + 1] = A[j];
            j--;
        }
        A[j + 1] = chave;
    }
}

// VETOR

void gerar_vetor(int *A, int n, int tipo) {
    switch (tipo) {
        case 0: // Crescente
            for (int i = 0; i < n; i++) A[i] = i;
            break;

        case 1: // Decrescente
            for (int i = 0; i < n; i++) A[i] = n - 1 - i;
            break;

        case 2: // Aleatório
            for (int i = 0; i < n; i++) A[i] = rand() % 1000000;
            break;
    }
}

// TEMPO

typedef void (*Ordenacao)(int*, int);

double tempo_execucao(Ordenacao func, int *A, int n) {
    LARGE_INTEGER freq, ini, fim;
    QueryPerformanceFrequency(&freq);

    QueryPerformanceCounter(&ini);
    func(A, n);
    QueryPerformanceCounter(&fim);

    return (double)(fim.QuadPart - ini.QuadPart) / freq.QuadPart;
}

// MAIN

int main() {
    srand(time(NULL));

    const int tamanhos[] = {1000, 5000, 10000, 20000, 50000};
    const int num = sizeof(tamanhos) / sizeof(tamanhos[0]);

    Ordenacao algs[] = {insertion, mergesort2, quicksort};
    const char *nomes[] = {"InsertionSort", "MergeSort", "QuickSort"};
    const char *tipos[] = {"Crescente", "Decrescente", "Aleatorio"};

    FILE *csv = fopen("resultados.csv", "w");
    if (!csv) {
        perror("Erro ao abrir resultados.csv");
        return 1;
    }

    fprintf(csv, "algoritmo,tipo_vetor,tamanho_n,tempo_s\n");

    printf("Iniciando testes...\n");

    // A: Vetor original (populado)
    int *A = NULL;
    // vetor_teste: Cópia de A que será ordenada para cada teste
    int *vetor_teste = NULL; 

    setvbuf(csv, NULL, _IOLBF, 0); // Mantido, mas fflush é a solução principal.

    for (int t = 0; t < num; t++) {
        int n = tamanhos[t];
        printf("-> n = %d\n", n);

        // Realoca A (vetor original) e vetor_teste (cópia)
        A = realloc(A, n * sizeof(int));
        vetor_teste = realloc(vetor_teste, n * sizeof(int));

        if (!A || !vetor_teste) {
            fprintf(stderr, "Erro: memória insuficiente.\n");
            free(A); free(vetor_teste);
            return 1;
        }

        for (int tipo = 0; tipo < 3; tipo++) {
            gerar_vetor(A, n, tipo);

            for (int algo = 0; algo < 3; algo++) {
                // Copia A para vetor_teste antes de cada teste
                memcpy(vetor_teste, A, n * sizeof(int));

                double tempo = tempo_execucao(algs[algo], vetor_teste, n);

                fprintf(csv, "%s,%s,%d,%lf\n",
                        nomes[algo], tipos[tipo], n, tempo);
                
                // Força a escrita no arquivo para evitar que fique vazio!
                fflush(csv); 
            }
        }
    }

    free(A);
    free(vetor_teste); // Libera o vetor de teste
fclose(csv);

printf("Finalizado! Resultados salvos em resultados.csv\n");
 return 0;
}