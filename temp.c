#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#define MAX_IMAGENS 100000
#define NOME_ARQUIVO_ENTRADA "dados_satelite.txt"
#define NOME_ARQUIVO_SAIDA "resultados_ordenacao.txt"

// Estrutura para representar dados de imagem de satélite
typedef struct {
    int fid;                    // Identificador único
    char classname[100];        // Tipo de classificação
    char quadrant[20];          // Quadrante
    char path_row[20];          // Caminho/linha
    char view_date[20];         // Data de visualização (YYYY/MM/DD)
    char sensor[20];            // Sensor utilizado
    char satellite[20];         // Satélite
    double areauckm;            // Área em km²
    char uc[100];              // Unidade de Conservação
    double areamunkm;          // Área do município em km²
    char municipali[100];       // Município
    char geocodibge[20];       // Código IBGE
    char uf[3];                // Unidade Federativa
} ImagemSatelite;

// Estrutura para armazenar resultados de desempenho
typedef struct {
    char algoritmo[50];
    double tempo_execucao;
    int num_elementos;
    int num_comparacoes;
    int num_trocas;
} ResultadoDesempenho;

// Variáveis globais para métricas
long long comparacoes = 0;
long long trocas = 0;

// Protótipos das funções
void gerarDadosAleatorios(ImagemSatelite arr[], int n);
void salvarDadosArquivo(ImagemSatelite arr[], int n, const char* nomeArquivo);
int carregarDadosArquivo(ImagemSatelite arr[], const char* nomeArquivo);
void copiarArray(ImagemSatelite origem[], ImagemSatelite destino[], int n);
void exibirImagem(ImagemSatelite img);
void exibirResultados(ResultadoDesempenho resultado);
void salvarResultadosArquivo(ResultadoDesempenho resultados[], int numResultados);
void resetarMetricas();

// Algoritmos de Ordenação
void bubbleSort(ImagemSatelite arr[], int n);
void quickSort(ImagemSatelite arr[], int low, int high);
int partition(ImagemSatelite arr[], int low, int high);
void mergeSort(ImagemSatelite arr[], int left, int right);
void merge(ImagemSatelite arr[], int left, int mid, int right);
void heapSort(ImagemSatelite arr[], int n);
void heapify(ImagemSatelite arr[], int n, int i);
void insertionSort(ImagemSatelite arr[], int n);
void selectionSort(ImagemSatelite arr[], int n);

// Função para trocar elementos
void trocar(ImagemSatelite *a, ImagemSatelite *b) {
    ImagemSatelite temp = *a;
    *a = *b;
    *b = temp;
    trocas++;
}

// Função para comparar elementos (por prioridade e depois por ID)
int comparar(ImagemSatelite a, ImagemSatelite b) {
    comparacoes++;
    
    // Proteção contra dados inválidos
    if (!a.view_date[0] || !b.view_date[0]) {
        // Se alguma data estiver vazia, comparar por FID
        return a.fid - b.fid;
    }
    
    // Comparar primeiro por data de visualização
    int result = strcmp(a.view_date, b.view_date);
    if (result != 0) {
        return result;
    }
    
    // Se as datas forem iguais, comparar por FID
    return a.fid - b.fid;
}

// Implementação do Bubble Sort
void bubbleSort(ImagemSatelite arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        bool trocou = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (comparar(arr[j], arr[j + 1]) > 0) {
                trocar(&arr[j], &arr[j + 1]);
                trocou = true;
            }
        }
        if (!trocou) break;
    }
}

// Implementação do Quick Sort
void quickSort(ImagemSatelite arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int partition(ImagemSatelite arr[], int low, int high) {
    ImagemSatelite pivot = arr[high];
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        if (comparar(arr[j], pivot) < 0) {
            i++;
            trocar(&arr[i], &arr[j]);
        }
    }
    trocar(&arr[i + 1], &arr[high]);
    return (i + 1);
}

// Implementação do Merge Sort
void mergeSort(ImagemSatelite arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void merge(ImagemSatelite arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    ImagemSatelite *L = (ImagemSatelite*)malloc(n1 * sizeof(ImagemSatelite));
    ImagemSatelite *R = (ImagemSatelite*)malloc(n2 * sizeof(ImagemSatelite));
    
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];
    
    int i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2) {
        // A contagem de trocas não é aplicável da mesma forma no Merge Sort,
        // pois ele não "troca" elementos, mas os mescla.
        // A métrica principal aqui é o número de comparações.
        if (comparar(L[i], R[j]) <= 0) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
    
    free(L);
    free(R);
}

// Implementação do Heap Sort
void heapSort(ImagemSatelite arr[], int n) {
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);
    
    for (int i = n - 1; i > 0; i--) {
        trocar(&arr[0], &arr[i]);
        heapify(arr, i, 0);
    }
}

void heapify(ImagemSatelite arr[], int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < n && comparar(arr[left], arr[largest]) > 0)
        largest = left;
    
    if (right < n && comparar(arr[right], arr[largest]) > 0)
        largest = right;
    
    if (largest != i) {
        trocar(&arr[i], &arr[largest]);
        heapify(arr, n, largest);
    }
}

// Implementação do Insertion Sort
void insertionSort(ImagemSatelite arr[], int n) {
    for (int i = 1; i < n; i++) {
        ImagemSatelite key = arr[i];
        int j = i - 1;
        
        while (j >= 0 && comparar(arr[j], key) > 0) {
            arr[j + 1] = arr[j];
            j--;
            trocas++;
        }
        arr[j + 1] = key;
        trocas++;
    }
}

// Implementação do Selection Sort
void selectionSort(ImagemSatelite arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (comparar(arr[j], arr[min_idx]) < 0) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            trocar(&arr[i], &arr[min_idx]);
        }
    }
}

// Gerar dados aleatórios de imagens
void gerarDadosAleatorios(ImagemSatelite arr[], int n) {
    srand(time(NULL));
    
    const char* classes[] = {"CICATRIZ_DE_QUEIMADA", "DESMATAMENTO_CR", "MINERACAO", "DEGRADACAO_CR"};
    const char* sensores[] = {"CBERS-4", "AWF1", "RESOURCESAT-2"};
    const char* satelites[] = {"CBERS-4.0.0", "AMAZONIA-1.0.0"};
    const char* municipios[] = {"Altamira", "Itaituba", "Santarem", "Porto de Moz", "Obidos", "Brasil Novo"};
    const char* ufs[] = {"PA", "AM", "RO"};
    
    for (int i = 0; i < n; i++) {
        arr[i].fid = 100000 + i;
        
        // Seleciona classe aleatória
        strcpy(arr[i].classname, classes[rand() % 4]);
        
        // Gera quadrante e path_row
        sprintf(arr[i].quadrant, "Q%d", (rand() % 4) + 1);
        sprintf(arr[i].path_row, "%d", 160000 + (rand() % 10000));
        
        // Gera data entre 2018 e 2024
        sprintf(arr[i].view_date, "%d/%02d/%02d", 
                2018 + (rand() % 7),
                (rand() % 12) + 1,
                (rand() % 28) + 1);
        
        // Seleciona sensor e satélite
        strcpy(arr[i].sensor, sensores[rand() % 3]);
        strcpy(arr[i].satellite, satelites[rand() % 2]);
        
        // Gera áreas aleatórias
        arr[i].areauckm = (rand() % 10000) / 100.0;
        strcpy(arr[i].uc, "AREA DE PROTECAO AMBIENTAL");
        arr[i].areamunkm = (rand() % 20000) / 100.0;
        
        // Seleciona município e UF
        strcpy(arr[i].municipali, municipios[rand() % 6]);
        strcpy(arr[i].uf, ufs[rand() % 3]);
        
        // Gera código IBGE
        sprintf(arr[i].geocodibge, "%d", 1500000 + (rand() % 100000));
    }
}

// Salvar dados em arquivo
void salvarDadosArquivo(ImagemSatelite arr[], int n, const char* nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo %s\n", nomeArquivo);
        return;
    }
    
    // Escreve o cabeçalho
    fprintf(arquivo, "FID,CLASSNAME,QUADRANT,PATH_ROW,VIEW_DATE,SENSOR,SATELLITE,AREAUCKM,UC,AREAMUNKM,MUNICIPALI,GEOCODIBGE,UF\n");
    
    // Escreve os dados
    for (int i = 0; i < n; i++) {
        fprintf(arquivo, "%d,%s,%s,%s,%s,%s,%s,%.8f,%s,%.8f,%s,%s,%s\n",
                arr[i].fid,
                arr[i].classname,
                arr[i].quadrant,
                arr[i].path_row,
                arr[i].view_date,
                arr[i].sensor,
                arr[i].satellite,
                arr[i].areauckm,
                arr[i].uc,
                arr[i].areamunkm,
                arr[i].municipali,
                arr[i].geocodibge,
                arr[i].uf);
    }
    
    fclose(arquivo);
    printf("Dados salvos com sucesso em %s\n", nomeArquivo);
}

// Função auxiliar para extrair um campo de uma linha CSV.
// Lida corretamente com campos vazios.
// Modifica a string da linha, substituindo delimitadores por '\0'.
static char* get_csv_field(char** line_ptr) {
    if (*line_ptr == NULL) {
        return NULL;
    }

    char* token_start = *line_ptr;
    char* token_end = strchr(token_start, ',');

    if (token_end != NULL) {
        *token_end = '\0';
        *line_ptr = token_end + 1;
    } else {
        *line_ptr = NULL; // Não há mais vírgulas, este é o último campo.
    }
    return token_start;
}

// Carregar dados de arquivo
int carregarDadosArquivo(ImagemSatelite arr[], const char* nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", nomeArquivo);
        return 0;
    }

    char linha[1024];
    int n = 0;

    // Pular a primeira linha (cabeçalho)
    if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        fclose(arquivo);
        return 0;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL && n < MAX_IMAGENS) {
        linha[strcspn(linha, "\r\n")] = 0; // Remove \n ou \r\n

        char* line_ptr = linha;
        char* token;

        // Inicializar a estrutura com zeros para evitar dados lixo
        memset(&arr[n], 0, sizeof(ImagemSatelite));

        // A ordem dos campos no arquivo CSV:
        // FID,CLASSNAME,QUADRANT,PATH_ROW,VIEW_DATE,SENSOR,SATELLITE,AREAUCKM,UC,AREAMUNKM,MUNICIPALI,GEOCODIBGE,UF
        
        token = get_csv_field(&line_ptr); // 1. FID
        if (token != NULL) {
            char *underscore = strchr(token, '_');
            if (underscore) *underscore = '\0';
            arr[n].fid = atoi(token);
        }

        token = get_csv_field(&line_ptr); // 2. CLASSNAME
        if (token != NULL) strncpy(arr[n].classname, token, sizeof(arr[n].classname) - 1);

        token = get_csv_field(&line_ptr); // 3. QUADRANT
        if (token != NULL) strncpy(arr[n].quadrant, token, sizeof(arr[n].quadrant) - 1);

        token = get_csv_field(&line_ptr); // 4. PATH_ROW
        if (token != NULL) strncpy(arr[n].path_row, token, sizeof(arr[n].path_row) - 1);

        token = get_csv_field(&line_ptr); // 5. VIEW_DATE
        if (token != NULL) strncpy(arr[n].view_date, token, sizeof(arr[n].view_date) - 1);

        token = get_csv_field(&line_ptr); // 6. SENSOR
        if (token != NULL) strncpy(arr[n].sensor, token, sizeof(arr[n].sensor) - 1);

        token = get_csv_field(&line_ptr); // 7. SATELLITE
        if (token != NULL) strncpy(arr[n].satellite, token, sizeof(arr[n].satellite) - 1);

        token = get_csv_field(&line_ptr); // 8. AREAUCKM
        if (token != NULL && *token) arr[n].areauckm = atof(token); else arr[n].areauckm = 0.0;

        token = get_csv_field(&line_ptr); // 9. UC
        if (token != NULL) strncpy(arr[n].uc, token, sizeof(arr[n].uc) - 1);

        token = get_csv_field(&line_ptr); // 10. AREAMUNKM
        if (token != NULL && *token) arr[n].areamunkm = atof(token); else arr[n].areamunkm = 0.0;

        token = get_csv_field(&line_ptr); // 11. MUNICIPALI
        if (token != NULL) strncpy(arr[n].municipali, token, sizeof(arr[n].municipali) - 1);

        token = get_csv_field(&line_ptr); // 12. GEOCODIBGE
        if (token != NULL) strncpy(arr[n].geocodibge, token, sizeof(arr[n].geocodibge) - 1);
        
        token = get_csv_field(&line_ptr); // 13. UF
        if (token != NULL) strncpy(arr[n].uf, token, sizeof(arr[n].uf) - 1);

        // Garantir que todas as strings sejam terminadas em nulo.
        arr[n].classname[sizeof(arr[n].classname) - 1] = '\0';
        arr[n].quadrant[sizeof(arr[n].quadrant) - 1] = '\0';
        arr[n].path_row[sizeof(arr[n].path_row) - 1] = '\0';
        arr[n].view_date[sizeof(arr[n].view_date) - 1] = '\0';
        arr[n].sensor[sizeof(arr[n].sensor) - 1] = '\0';
        arr[n].satellite[sizeof(arr[n].satellite) - 1] = '\0';
        arr[n].uc[sizeof(arr[n].uc) - 1] = '\0';
        arr[n].municipali[sizeof(arr[n].municipali) - 1] = '\0';
        arr[n].geocodibge[sizeof(arr[n].geocodibge) - 1] = '\0';
        arr[n].uf[sizeof(arr[n].uf) - 1] = '\0';

        n++;
    }

    fclose(arquivo);
    printf("Carregados %d registros do arquivo %s\n", n, nomeArquivo);
    return n;
}

// Copiar array
void copiarArray(ImagemSatelite origem[], ImagemSatelite destino[], int n) {
    for (int i = 0; i < n; i++) {
        destino[i] = origem[i];
    }
}

// Exibir dados de uma imagem
void exibirImagem(ImagemSatelite img) {
    printf("FID: %d\nClass: %s\nQuadrant: %s\nPath/Row: %s\nDate: %s\nSensor: %s\nSat: %s\n",
           img.fid, img.classname, img.quadrant, img.path_row, 
           img.view_date, img.sensor, img.satellite);
    printf("Areas(UC/Mun): %.2f/%.2f km\nMunicipality: %s-%s\n",
           img.areauckm, img.areamunkm, img.municipali, img.uf);
    printf("----------------------------------------\n");
}

// Resetar métricas
void resetarMetricas() {
    comparacoes = 0;
    trocas = 0;
}

// Exibir resultados de desempenho
void exibirResultados(ResultadoDesempenho resultado) {
    printf("\n========================================\n");
    printf("Algoritmo: %s\n", resultado.algoritmo);
    printf("Elementos: %d\n", resultado.num_elementos);
    printf("Tempo: %.6f segundos\n", resultado.tempo_execucao);
    printf("Comparacoes: %d\n", resultado.num_comparacoes);
    printf("Trocas: %d\n", resultado.num_trocas);
    printf("========================================\n");
}

// Salvar resultados em arquivo
void salvarResultadosArquivo(ResultadoDesempenho resultados[], int numResultados) {
    FILE *arquivo = fopen(NOME_ARQUIVO_SAIDA, "w");
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo de resultados\n");
        return;
    }
    
    fprintf(arquivo, "RESULTADOS DA ANALISE DE DESEMPENHO\n");
    fprintf(arquivo, "====================================\n\n");
    
    for (int i = 0; i < numResultados; i++) {
        fprintf(arquivo, "Algoritmo: %s\n", resultados[i].algoritmo);
        fprintf(arquivo, "Elementos: %d\n", resultados[i].num_elementos);
        fprintf(arquivo, "Tempo: %.6f segundos\n", resultados[i].tempo_execucao);
        fprintf(arquivo, "Comparacoes: %d\n", resultados[i].num_comparacoes);
        fprintf(arquivo, "Trocas: %d\n", resultados[i].num_trocas);
        fprintf(arquivo, "------------------------------------\n\n");
    }
    
    fclose(arquivo);
}

// Função principal
int main() {
    ImagemSatelite *dados_originais = NULL;
    ImagemSatelite *dados_trabalho = NULL;
    ResultadoDesempenho resultados[30];
    int num_elementos = 0;
    int opcao_menu;
    int num_resultados = 0;
    
    printf("\n===========================================\n");
    printf("  ANALISE DE DESEMPENHO - ORDENACAO DE DADOS\n");
    printf("        IMAGENS DE SATELITE DA AMAZONIA\n");
    printf("===========================================\n\n");
    
    do {
        printf("\nMENU PRINCIPAL:\n");
        printf("1. Gerar dados aleatorios\n");
        printf("2. Carregar dados de arquivo\n");
        printf("3. Executar analise de desempenho\n");
        printf("4. Exibir ultimos resultados\n");
        printf("5. Salvar resultados em arquivo\n");
        printf("0. Sair\n");
        printf("\nEscolha uma opcao: ");
        scanf("%d", &opcao_menu);
        
        switch(opcao_menu) {
            case 1:
                printf("\nQuantas imagens deseja gerar (max %d): ", MAX_IMAGENS);
                scanf("%d", &num_elementos);
                
                if (num_elementos <= 0 || num_elementos > MAX_IMAGENS) {
                    printf("Numero invalido de elementos!\n");
                    break;
                }
                
                dados_originais = (ImagemSatelite*)malloc(num_elementos * sizeof(ImagemSatelite));
                gerarDadosAleatorios(dados_originais, num_elementos);
                
                printf("\nDeseja salvar os dados gerados em arquivo? (1-Sim/0-Nao): ");
                int salvar;
                scanf("%d", &salvar);
                if (salvar == 1) {
                    salvarDadosArquivo(dados_originais, num_elementos, NOME_ARQUIVO_ENTRADA);
                    printf("Dados salvos em %s\n", NOME_ARQUIVO_ENTRADA);
                }
                
                printf("\nPrimeiras 5 imagens geradas:\n");
                for (int i = 0; i < 5 && i < num_elementos; i++) {
                    exibirImagem(dados_originais[i]);
                }
                break;
                
            case 2:
                dados_originais = (ImagemSatelite*)malloc(MAX_IMAGENS * sizeof(ImagemSatelite));
                num_elementos = carregarDadosArquivo(dados_originais, NOME_ARQUIVO_ENTRADA);
                
                if (num_elementos == 0) {
                    printf("Erro ao carregar arquivo ou arquivo vazio!\n");
                    free(dados_originais);
                } else {
                    printf("Carregadas %d imagens do arquivo %s\n", num_elementos, NOME_ARQUIVO_ENTRADA);
                    printf("\nPrimeiras 5 imagens carregadas:\n");
                    for (int i = 0; i < 5 && i < num_elementos; i++) {
                        exibirImagem(dados_originais[i]);
                    }
                }
                break;
                
            case 3:
                if (dados_originais == NULL || num_elementos == 0) {
                    printf("Por favor, gere ou carregue dados primeiro!\n");
                    break;
                }
                
                dados_trabalho = (ImagemSatelite*)malloc(num_elementos * sizeof(ImagemSatelite));
                num_resultados = 0;
                
                printf("\n=== INICIANDO ANALISE DE DESEMPENHO ===\n");
                printf("Processando %d imagens...\n", num_elementos);
                
                // Teste com diferentes tamanhos
                int tamanhos_teste[] = {100, 1000, 5000, 10000};
                int num_tamanhos = 4;
                
                for (int t = 0; t < num_tamanhos; t++) {
                    int tamanho_atual = tamanhos_teste[t];
                    if (tamanho_atual > num_elementos) {
                        tamanho_atual = num_elementos;
                    }
                    
                    printf("\n--- Testando com %d elementos ---\n", tamanho_atual);
                    
                    // 1. Bubble Sort (apenas para conjuntos pequenos)
                    if (tamanho_atual <= 10000) {
                        copiarArray(dados_originais, dados_trabalho, tamanho_atual);
                        resetarMetricas();
                        
                        clock_t inicio = clock();
                        bubbleSort(dados_trabalho, tamanho_atual);
                        clock_t fim = clock();
                        
                        double tempo = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
                        
                        strcpy(resultados[num_resultados].algoritmo, "Bubble Sort");
                        resultados[num_resultados].tempo_execucao = tempo;
                        resultados[num_resultados].num_elementos = tamanho_atual;
                        resultados[num_resultados].num_comparacoes = comparacoes;
                        resultados[num_resultados].num_trocas = trocas;
                        
                        printf("Bubble Sort: %.6fs | Comp: %lld | Trocas: %lld\n", 
                               tempo, comparacoes, trocas);
                        num_resultados++;
                    }
                    
                    // 2. Insertion Sort
                    copiarArray(dados_originais, dados_trabalho, tamanho_atual);
                    resetarMetricas();
                    
                    clock_t inicio = clock();
                    insertionSort(dados_trabalho, tamanho_atual);
                    clock_t fim = clock();
                    
                    double tempo = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
                    
                    strcpy(resultados[num_resultados].algoritmo, "Insertion Sort");
                    resultados[num_resultados].tempo_execucao = tempo;
                    resultados[num_resultados].num_elementos = tamanho_atual;
                    resultados[num_resultados].num_comparacoes = comparacoes;
                    resultados[num_resultados].num_trocas = trocas;
                    
                    printf("Insertion Sort: %.6fs | Comp: %lld | Trocas: %lld\n", 
                           tempo, comparacoes, trocas);
                    num_resultados++;
                    
                    // 3. Selection Sort (apenas para conjuntos pequenos)
                    if (tamanho_atual <= 10000) {
                        copiarArray(dados_originais, dados_trabalho, tamanho_atual);
                        resetarMetricas();
                        
                        inicio = clock();
                        selectionSort(dados_trabalho, tamanho_atual);
                        fim = clock();
                        
                        tempo = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
                        
                        strcpy(resultados[num_resultados].algoritmo, "Selection Sort");
                        resultados[num_resultados].tempo_execucao = tempo;
                        resultados[num_resultados].num_elementos = tamanho_atual;
                        resultados[num_resultados].num_comparacoes = comparacoes;
                        resultados[num_resultados].num_trocas = trocas;
                        
                        printf("Selection Sort: %.6fs | Comp: %lld | Trocas: %lld\n", 
                               tempo, comparacoes, trocas);
                        num_resultados++;
                    }
                    
                    // 4. Quick Sort
                    copiarArray(dados_originais, dados_trabalho, tamanho_atual);
                    resetarMetricas();
                    
                    inicio = clock();
                    quickSort(dados_trabalho, 0, tamanho_atual - 1);
                    fim = clock();
                    
                    tempo = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
                    
                    strcpy(resultados[num_resultados].algoritmo, "Quick Sort");
                    resultados[num_resultados].tempo_execucao = tempo;
                    resultados[num_resultados].num_elementos = tamanho_atual;
                    resultados[num_resultados].num_comparacoes = comparacoes;
                    resultados[num_resultados].num_trocas = trocas;
                    
                    printf("Quick Sort: %.6fs | Comp: %lld | Trocas: %lld\n", 
                           tempo, comparacoes, trocas);
                    num_resultados++;
                    
                    // 5. Merge Sort
                    copiarArray(dados_originais, dados_trabalho, tamanho_atual);
                    resetarMetricas();
                    
                    inicio = clock();
                    mergeSort(dados_trabalho, 0, tamanho_atual - 1);
                    fim = clock();
                    
                    tempo = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
                    
                    strcpy(resultados[num_resultados].algoritmo, "Merge Sort");
                    resultados[num_resultados].tempo_execucao = tempo;
                    resultados[num_resultados].num_elementos = tamanho_atual;
                    resultados[num_resultados].num_comparacoes = comparacoes;
                    resultados[num_resultados].num_trocas = trocas;
                    
                    printf("Merge Sort: %.6fs | Comp: %lld | Trocas: %lld\n", 
                           tempo, comparacoes, trocas);
                    num_resultados++;
                    
                    // 6. Heap Sort
                    copiarArray(dados_originais, dados_trabalho, tamanho_atual);
                    resetarMetricas();
                    
                    inicio = clock();
                    heapSort(dados_trabalho, tamanho_atual);
                    fim = clock();
                    
                    tempo = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
                    
                    strcpy(resultados[num_resultados].algoritmo, "Heap Sort");
                    resultados[num_resultados].tempo_execucao = tempo;
                    resultados[num_resultados].num_elementos = tamanho_atual;
                    resultados[num_resultados].num_comparacoes = comparacoes;
                    resultados[num_resultados].num_trocas = trocas;
                    
                    printf("Heap Sort: %.6fs | Comp: %lld | Trocas: %lld\n", 
                           tempo, comparacoes, trocas);
                    num_resultados++;
                    
                    if (tamanho_atual >= num_elementos) break;
                }
                
                printf("\n=== ANALISE CONCLUIDA ===\n");
                free(dados_trabalho);
                break;
                
            case 4:
                if (num_resultados == 0) {
                    printf("Nenhum resultado disponivel. Execute a analise primeiro!\n");
                } else {
                    printf("\n=== ULTIMOS RESULTADOS ===\n");
                    for (int i = 0; i < num_resultados; i++) {
                        exibirResultados(resultados[i]);
                    }
                }
                break;
                
            case 5:
                if (num_resultados == 0) {
                    printf("Nenhum resultado para salvar!\n");
                } else {
                    salvarResultadosArquivo(resultados, num_resultados);
                    printf("Resultados salvos em %s\n", NOME_ARQUIVO_SAIDA);
                }
                break;
                
            case 0:
                printf("\nEncerrando programa...\n");
                if (dados_originais != NULL) free(dados_originais);
                break;
                
            default:
                printf("Opção invalida!\n");
        }
        
    } while (opcao_menu != 0);
    
    return 0;
}
