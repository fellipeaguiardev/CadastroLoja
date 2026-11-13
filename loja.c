#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100
#define ARQUIVO "loja.txt"
#define TEMP "temp.txt"
#define CSV "loja.csv"

typedef struct {
    int id;
    char produto[50];
    char categoria[30];
    float preco;
    int quantidade;
    char fornecedor[30];
} Produto;

void lerTexto(char *texto, int tamanho) {
    if (fgets(texto, tamanho, stdin) == NULL) {
        printf("\nEntrada encerrada.\n");
        exit(0);
    }
    if (strchr(texto, '\n') == NULL) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
    texto[strcspn(texto, "\r\n")] = '\0';
    for (int i = 0; texto[i] != '\0'; i++) {
        if (texto[i] == ';') {
            texto[i] = ',';
        }
    }
}

int lerInteiro() {
    char linha[30];
    int numero;
    while (1) {
        lerTexto(linha, 30);
        if (sscanf(linha, "%d", &numero) == 1 && numero >= 0) {
            return numero;
        }
        printf("Valor invalido, digite novamente: ");
    }
}

float lerFloat() {
    char linha[30];
    float numero;
    while (1) {
        lerTexto(linha, 30);
        for (int i = 0; linha[i] != '\0'; i++) {
            if (linha[i] == ',') {
                linha[i] = '.';
            }
        }
        if (sscanf(linha, "%f", &numero) == 1 && numero >= 0) {
            return numero;
        }
        printf("Valor invalido, digite novamente (ex: 49.90): ");
    }
}

void minusculo(char *origem, char *destino) {
    int i = 0;
    while (origem[i] != '\0') {
        destino[i] = tolower((unsigned char)origem[i]);
        i++;
    }
    destino[i] = '\0';
}

int compararTexto(char *a, char *b) {
    char x[100], y[100];
    minusculo(a, x);
    minusculo(b, y);
    return strcmp(x, y);
}

void gravarLinha(FILE *arquivo, Produto *p) {
    fprintf(arquivo, "%d;%s;%s;%.2f;%d;%s\n",
            p->id, p->produto, p->categoria, p->preco, p->quantidade, p->fornecedor);
}

int carregar(Produto lista[]) {
    FILE *arquivo = fopen(ARQUIVO, "r");
    if (arquivo == NULL) {
        return 0;
    }
    char linha[200];
    int total = 0;
    while (fgets(linha, 200, arquivo) != NULL && total < MAX) {
        Produto p;
        int lidos = sscanf(linha, "%d;%49[^;];%29[^;];%f;%d;%29[^\r\n]",
                           &p.id, p.produto, p.categoria, &p.preco, &p.quantidade, p.fornecedor);
        if (lidos == 6) {
            lista[total] = p;
            total++;
        }
    }
    fclose(arquivo);
    return total;
}

int salvar(Produto lista[], int total) {
    FILE *temp = fopen(TEMP, "w");
    if (temp == NULL) {
        printf("Erro ao criar o arquivo temporario.\n");
        return 0;
    }
    for (int i = 0; i < total; i++) {
        gravarLinha(temp, &lista[i]);
    }
    fclose(temp);
    remove(ARQUIVO);
    if (rename(TEMP, ARQUIVO) != 0) {
        printf("Erro ao substituir o arquivo.\n");
        return 0;
    }
    return 1;
}

int arquivoExiste() {
    FILE *arquivo = fopen(ARQUIVO, "r");
    if (arquivo == NULL) {
        return 0;
    }
    fclose(arquivo);
    return 1;
}

void criarArquivo() {
    Produto iniciais[4] = {
        {1, "Camiseta Basica", "Vestuario", 49.90, 30, "Moda Brasil"},
        {2, "Mouse Optico USB", "Informatica", 59.99, 15, "TecnoShop"},
        {3, "Shampoo 300ml", "Higiene Pessoal", 22.50, 40, "Beleza Pura"},
        {4, "Cafe 500g", "Alimentos", 18.00, 25, "Graos do Sul"}
    };
    FILE *arquivo = fopen(ARQUIVO, "w");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo.\n");
        return;
    }
    for (int i = 0; i < 4; i++) {
        gravarLinha(arquivo, &iniciais[i]);
    }
    fclose(arquivo);
    printf("Arquivo %s criado com 4 produtos.\n", ARQUIVO);
}

void linhaTabela() {
    printf("------------------------------------------------------------------------------------------\n");
}

void cabecalho() {
    linhaTabela();
    printf("| %-3s | %-20s | %-15s | %-10s | %-5s | %-15s |\n",
           "ID", "Produto", "Categoria", "Preco (R$)", "Qtd", "Fornecedor");
    linhaTabela();
}

void mostrarProduto(Produto *p) {
    printf("| %-3d | %-20s | %-15s | %10.2f | %5d | %-15s |\n",
           p->id, p->produto, p->categoria, p->preco, p->quantidade, p->fornecedor);
}

void mostrarTabela(Produto lista[], int total) {
    if (total == 0) {
        printf("Nenhum produto cadastrado.\n");
        return;
    }
    cabecalho();
    for (int i = 0; i < total; i++) {
        mostrarProduto(&lista[i]);
    }
    linhaTabela();
    printf("Total: %d produto(s)\n", total);
}

int buscarId(Produto lista[], int total, int id) {
    for (int i = 0; i < total; i++) {
        if (lista[i].id == id) {
            return i;
        }
    }
    return -1;
}

int existeNome(Produto lista[], int total, char *nome, int idIgnorar) {
    for (int i = 0; i < total; i++) {
        if (lista[i].id != idIgnorar && compararTexto(lista[i].produto, nome) == 0) {
            return 1;
        }
    }
    return 0;
}

int proximoId(Produto lista[], int total) {
    int maior = 0;
    for (int i = 0; i < total; i++) {
        if (lista[i].id > maior) {
            maior = lista[i].id;
        }
    }
    return maior + 1;
}

void listar() {
    Produto lista[MAX];
    int total = carregar(lista);
    printf("\n");
    mostrarTabela(lista, total);
}

void inserir() {
    Produto lista[MAX];
    int total = carregar(lista);
    Produto novo;

    if (total >= MAX) {
        printf("Limite de produtos atingido.\n");
        return;
    }

    printf("\n--- INSERIR PRODUTO ---\n");
    printf("Nome do produto: ");
    lerTexto(novo.produto, 50);
    if (strlen(novo.produto) == 0) {
        printf("O nome nao pode ficar vazio.\n");
        return;
    }
    if (existeNome(lista, total, novo.produto, 0)) {
        printf("Ja existe um produto com o nome '%s'.\n", novo.produto);
        return;
    }

    printf("Categoria: ");
    lerTexto(novo.categoria, 30);
    printf("Preco: ");
    novo.preco = lerFloat();
    printf("Quantidade: ");
    novo.quantidade = lerInteiro();
    printf("Fornecedor: ");
    lerTexto(novo.fornecedor, 30);

    novo.id = proximoId(lista, total);
    while (buscarId(lista, total, novo.id) != -1) {
        novo.id++;
    }

    FILE *arquivo = fopen(ARQUIVO, "a");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }
    gravarLinha(arquivo, &novo);
    fclose(arquivo);
    printf("Produto cadastrado com o ID %d.\n", novo.id);
}

void alterar() {
    Produto lista[MAX];
    int total = carregar(lista);
    char texto[50];

    printf("\n");
    mostrarTabela(lista, total);
    if (total == 0) {
        return;
    }

    printf("Digite o ID do produto que deseja alterar: ");
    int id = lerInteiro();
    int pos = buscarId(lista, total, id);
    if (pos == -1) {
        printf("Produto nao encontrado.\n");
        return;
    }

    Produto *p = &lista[pos];
    printf("Aperte Enter para manter o valor atual.\n");

    printf("Nome (%s): ", p->produto);
    lerTexto(texto, 50);
    if (strlen(texto) > 0) {
        if (existeNome(lista, total, texto, p->id)) {
            printf("Ja existe outro produto com esse nome. Nome mantido.\n");
        } else {
            strcpy(p->produto, texto);
        }
    }

    printf("Categoria (%s): ", p->categoria);
    lerTexto(texto, 30);
    if (strlen(texto) > 0) {
        strcpy(p->categoria, texto);
    }

    printf("Preco (%.2f): ", p->preco);
    lerTexto(texto, 30);
    if (strlen(texto) > 0) {
        for (int i = 0; texto[i] != '\0'; i++) {
            if (texto[i] == ',') {
                texto[i] = '.';
            }
        }
        float preco;
        if (sscanf(texto, "%f", &preco) == 1 && preco >= 0) {
            p->preco = preco;
        } else {
            printf("Preco invalido, mantido o atual.\n");
        }
    }

    printf("Quantidade (%d): ", p->quantidade);
    lerTexto(texto, 30);
    if (strlen(texto) > 0) {
        int qtd;
        if (sscanf(texto, "%d", &qtd) == 1 && qtd >= 0) {
            p->quantidade = qtd;
        } else {
            printf("Quantidade invalida, mantida a atual.\n");
        }
    }

    printf("Fornecedor (%s): ", p->fornecedor);
    lerTexto(texto, 30);
    if (strlen(texto) > 0) {
        strcpy(p->fornecedor, texto);
    }

    if (salvar(lista, total)) {
        printf("Produto alterado com sucesso.\n");
    }
}

void excluir() {
    Produto lista[MAX];
    int total = carregar(lista);
    char resposta[10];

    printf("\n");
    mostrarTabela(lista, total);
    if (total == 0) {
        return;
    }

    printf("Digite o ID do produto que deseja excluir: ");
    int id = lerInteiro();
    int pos = buscarId(lista, total, id);
    if (pos == -1) {
        printf("Produto nao encontrado.\n");
        return;
    }

    printf("Tem certeza que deseja excluir '%s'? (s/n): ", lista[pos].produto);
    lerTexto(resposta, 10);
    if (resposta[0] != 's' && resposta[0] != 'S') {
        printf("Exclusao cancelada.\n");
        return;
    }

    for (int i = pos; i < total - 1; i++) {
        lista[i] = lista[i + 1];
    }
    total--;

    if (salvar(lista, total)) {
        printf("Produto excluido com sucesso.\n");
    }
}

void buscar(int tipo) {
    Produto lista[MAX];
    int total = carregar(lista);
    char termo[50], termoMin[50], campoMin[100];
    int achou = 0;

    if (tipo == 1) {
        printf("\nDigite o nome (ou parte dele): ");
    } else {
        printf("\nDigite a categoria (ou parte dela): ");
    }
    lerTexto(termo, 50);
    minusculo(termo, termoMin);

    for (int i = 0; i < total; i++) {
        if (tipo == 1) {
            minusculo(lista[i].produto, campoMin);
        } else {
            minusculo(lista[i].categoria, campoMin);
        }
        if (strstr(campoMin, termoMin) != NULL) {
            if (achou == 0) {
                cabecalho();
            }
            mostrarProduto(&lista[i]);
            achou++;
        }
    }

    if (achou > 0) {
        linhaTabela();
        printf("%d produto(s) encontrado(s).\n", achou);
    } else {
        printf("Nenhum produto encontrado.\n");
    }
}

void trocar(Produto *a, Produto *b) {
    Produto aux = *a;
    *a = *b;
    *b = aux;
}

int precisaTrocar(Produto *a, Produto *b, int tipo) {
    if (tipo == 1) {
        return compararTexto(a->produto, b->produto) > 0;
    }
    if (tipo == 2) {
        return compararTexto(a->produto, b->produto) < 0;
    }
    if (tipo == 3) {
        return a->preco > b->preco;
    }
    if (tipo == 4) {
        return a->preco < b->preco;
    }
    return 0;
}

void bubbleSort(Produto lista[], int total, int tipo) {
    for (int i = 0; i < total - 1; i++) {
        int trocou = 0;
        for (int j = 0; j < total - 1 - i; j++) {
            if (precisaTrocar(&lista[j], &lista[j + 1], tipo)) {
                trocar(&lista[j], &lista[j + 1]);
                trocou = 1;
            }
        }
        if (trocou == 0) {
            break;
        }
    }
}

void ordenar() {
    Produto lista[MAX];
    int total = carregar(lista);

    if (total == 0) {
        printf("\nNenhum produto cadastrado.\n");
        return;
    }

    printf("\n--- ORDENAR ---\n");
    printf("1 - Nome (A-Z)\n");
    printf("2 - Nome (Z-A)\n");
    printf("3 - Preco (menor para maior)\n");
    printf("4 - Preco (maior para menor)\n");
    printf("Escolha: ");
    int tipo = lerInteiro();

    if (tipo < 1 || tipo > 4) {
        printf("Opcao invalida.\n");
        return;
    }

    bubbleSort(lista, total, tipo);
    printf("\n");
    mostrarTabela(lista, total);
    printf("(a ordem mudou so na tela, o arquivo continua igual)\n");
}

void exportarCsv() {
    Produto lista[MAX];
    int total = carregar(lista);

    FILE *csv = fopen(CSV, "w");
    if (csv == NULL) {
        printf("Erro ao criar o arquivo CSV.\n");
        return;
    }
    fprintf(csv, "ID,Produto,Categoria,Preco,Quantidade,Fornecedor\n");
    for (int i = 0; i < total; i++) {
        fprintf(csv, "%d,\"%s\",\"%s\",%.2f,%d,\"%s\"\n",
                lista[i].id, lista[i].produto, lista[i].categoria,
                lista[i].preco, lista[i].quantidade, lista[i].fornecedor);
    }
    fclose(csv);
    printf("\n%d produto(s) exportado(s) para %s\n", total, CSV);
}

int main() {
    int opcao;
    char resposta[10];

    if (!arquivoExiste()) {
        criarArquivo();
    }

    do {
        printf("\n==============================\n");
        printf("      CADASTRO DA LOJA\n");
        printf("==============================\n");
        printf("1 - Criar arquivo\n");
        printf("2 - Listar produtos\n");
        printf("3 - Inserir produto\n");
        printf("4 - Alterar produto\n");
        printf("5 - Excluir produto\n");
        printf("6 - Buscar por nome\n");
        printf("7 - Buscar por categoria\n");
        printf("8 - Ordenar produtos\n");
        printf("9 - Exportar para CSV\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");
        opcao = lerInteiro();

        switch (opcao) {
            case 1:
                printf("Isso apaga os dados atuais. Confirma? (s/n): ");
                lerTexto(resposta, 10);
                if (resposta[0] == 's' || resposta[0] == 'S') {
                    criarArquivo();
                }
                break;
            case 2:
                listar();
                break;
            case 3:
                inserir();
                break;
            case 4:
                alterar();
                break;
            case 5:
                excluir();
                break;
            case 6:
                buscar(1);
                break;
            case 7:
                buscar(2);
                break;
            case 8:
                ordenar();
                break;
            case 9:
                exportarCsv();
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    return 0;
}
