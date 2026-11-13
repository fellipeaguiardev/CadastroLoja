// ============================================================
// CADASTRO DE LOJA
// Algoritmo de ordenacao (Bubble Sort) + manipulacao de arquivos
// Usa struct, vetor de structs e ponteiros
// ============================================================

#include <stdio.h>   // printf, fopen, fgets, fprintf, sscanf, remove, rename
#include <stdlib.h>  // exit
#include <string.h>  // strcpy, strcmp, strlen, strstr, strchr, strcspn
#include <ctype.h>   // tolower

#define MAX 100              // quantidade maxima de produtos no vetor
#define ARQUIVO "loja.txt"   // arquivo principal com os dados
#define TEMP "temp.txt"      // arquivo temporario usado ao alterar e excluir
#define CSV "loja.csv"       // arquivo gerado na exportacao

// Ficha de um produto da loja
typedef struct {
    int id;               // codigo do produto, gerado automaticamente
    char produto[50];     // nome do produto (ate 49 letras + '\0')
    char categoria[30];   // categoria do produto
    float preco;          // preco em reais
    int quantidade;       // quantidade em estoque
    char fornecedor[30];  // nome do fornecedor
} Produto;

// ============================================================
// LEITURA DO TECLADO
// ============================================================

// Le uma linha de texto digitada, sem passar do tamanho do vetor
void lerTexto(char *texto, int tamanho) {
    if (fgets(texto, tamanho, stdin) == NULL) {  // fgets devolve NULL se a entrada acabou
        printf("\nEntrada encerrada.\n");        // avisa o motivo
        exit(0);                                 // encerra o programa sem travar
    }
    if (strchr(texto, '\n') == NULL) {           // sem '\n' = digitou mais do que cabia
        int c;                                   // guarda cada caractere que sobrou
        while ((c = getchar()) != '\n' && c != EOF);  // joga fora o resto da linha
    }
    texto[strcspn(texto, "\r\n")] = '\0';        // tira o Enter do final do texto
    for (int i = 0; texto[i] != '\0'; i++) {     // percorre o texto letra por letra
        if (texto[i] == ';') {                   // ';' e o separador do arquivo
            texto[i] = ',';                      // troca para nao quebrar as colunas
        }
    }
}

// Le um numero inteiro >= 0, repetindo ate a pessoa digitar certo
int lerInteiro() {
    char linha[30];                              // guarda o que foi digitado
    int numero;                                  // numero convertido
    while (1) {                                  // repete ate dar certo
        lerTexto(linha, 30);                     // le como texto
        if (sscanf(linha, "%d", &numero) == 1 && numero >= 0) {  // converteu e nao e negativo
            return numero;                       // devolve o numero
        }
        printf("Valor invalido, digite novamente: ");  // pede de novo
    }
}

// Le um numero decimal >= 0 (aceita 49.90 ou 49,90)
float lerFloat() {
    char linha[30];                              // guarda o que foi digitado
    float numero;                                // numero convertido
    while (1) {                                  // repete ate dar certo
        lerTexto(linha, 30);                     // le como texto
        for (int i = 0; linha[i] != '\0'; i++) { // percorre o texto
            if (linha[i] == ',') {               // se tiver virgula
                linha[i] = '.';                  // troca por ponto
            }
        }
        if (sscanf(linha, "%f", &numero) == 1 && numero >= 0) {  // converteu e nao e negativo
            return numero;                       // devolve o numero
        }
        printf("Valor invalido, digite novamente (ex: 49.90): ");  // pede de novo
    }
}

// ============================================================
// COMPARACAO SEM DIFERENCIAR MAIUSCULA E MINUSCULA
// ============================================================

// Copia o texto de origem para destino, tudo em minusculo
void minusculo(char *origem, char *destino) {
    int i = 0;                                   // posicao atual
    while (origem[i] != '\0') {                  // ate o fim do texto
        destino[i] = tolower((unsigned char)origem[i]);  // converte a letra
        i++;                                     // proxima letra
    }
    destino[i] = '\0';                           // fecha o novo texto
}

// Compara dois textos ignorando maiuscula (0 = iguais)
int compararTexto(char *a, char *b) {
    char x[100], y[100];                         // copias em minusculo
    minusculo(a, x);                             // a em minusculo
    minusculo(b, y);                             // b em minusculo
    return strcmp(x, y);                         // <0 a vem antes, 0 iguais, >0 a vem depois
}

// ============================================================
// ARQUIVO
// ============================================================

// Grava um produto no arquivo: id;produto;categoria;preco;quantidade;fornecedor
void gravarLinha(FILE *arquivo, Produto *p) {
    fprintf(arquivo, "%d;%s;%s;%.2f;%d;%s\n",    // formato da linha
            p->id, p->produto, p->categoria, p->preco, p->quantidade, p->fornecedor);  // -> acessa o campo pelo ponteiro
}

// Le o arquivo e coloca os produtos no vetor. Retorna quantos leu
int carregar(Produto lista[]) {
    FILE *arquivo = fopen(ARQUIVO, "r");         // abre para leitura
    if (arquivo == NULL) {                       // arquivo nao existe
        return 0;                                // nenhum produto
    }
    char linha[200];                             // guarda cada linha lida
    int total = 0;                               // quantos produtos ja foram lidos
    while (fgets(linha, 200, arquivo) != NULL && total < MAX) {  // le linha por linha
        Produto p;                               // produto temporario
        int lidos = sscanf(linha, "%d;%49[^;];%29[^;];%f;%d;%29[^\r\n]",  // separa os campos pelo ';'
                           &p.id, p.produto, p.categoria, &p.preco, &p.quantidade, p.fornecedor);
        if (lidos == 6) {                        // leu os 6 campos certinho
            lista[total] = p;                    // guarda no vetor
            total++;                             // um produto a mais
        }
    }
    fclose(arquivo);                             // fecha o arquivo
    return total;                                // devolve a quantidade
}

// Grava o vetor inteiro usando um arquivo temporario
int salvar(Produto lista[], int total) {
    FILE *temp = fopen(TEMP, "w");               // cria o arquivo temporario
    if (temp == NULL) {                          // nao conseguiu criar
        printf("Erro ao criar o arquivo temporario.\n");
        return 0;                                // falhou
    }
    for (int i = 0; i < total; i++) {            // percorre o vetor
        gravarLinha(temp, &lista[i]);            // grava cada produto no temporario
    }
    fclose(temp);                                // fecha o temporario
    remove(ARQUIVO);                             // apaga o arquivo original
    if (rename(TEMP, ARQUIVO) != 0) {            // o temporario vira o original
        printf("Erro ao substituir o arquivo.\n");
        return 0;                                // falhou
    }
    return 1;                                    // deu certo
}

// Verifica se o arquivo ja existe (1 = sim, 0 = nao)
int arquivoExiste() {
    FILE *arquivo = fopen(ARQUIVO, "r");         // tenta abrir
    if (arquivo == NULL) {                       // nao abriu
        return 0;                                // nao existe
    }
    fclose(arquivo);                             // fecha
    return 1;                                    // existe
}

// Cria o arquivo com os 4 produtos iniciais da tabela
void criarArquivo() {
    Produto iniciais[4] = {                      // dados da tabela do enunciado
        {1, "Camiseta Basica", "Vestuario", 49.90, 30, "Moda Brasil"},
        {2, "Mouse Optico USB", "Informatica", 59.99, 15, "TecnoShop"},
        {3, "Shampoo 300ml", "Higiene Pessoal", 22.50, 40, "Beleza Pura"},
        {4, "Cafe 500g", "Alimentos", 18.00, 25, "Graos do Sul"}
    };
    FILE *arquivo = fopen(ARQUIVO, "w");         // "w" cria ou apaga e recria o arquivo
    if (arquivo == NULL) {                       // nao conseguiu criar
        printf("Erro ao criar o arquivo.\n");
        return;
    }
    for (int i = 0; i < 4; i++) {                // percorre os 4 produtos
        gravarLinha(arquivo, &iniciais[i]);      // grava cada um
    }
    fclose(arquivo);                             // fecha e salva
    printf("Arquivo %s criado com 4 produtos.\n", ARQUIVO);
}

// ============================================================
// TABELA NA TELA
// ============================================================

// Imprime a linha de tracinhos da tabela
void linhaTabela() {
    printf("------------------------------------------------------------------------------------------\n");
}

// Imprime os titulos das colunas
void cabecalho() {
    linhaTabela();                               // linha de cima
    printf("| %-3s | %-20s | %-15s | %-10s | %-5s | %-15s |\n",  // %-20s = 20 espacos alinhado a esquerda
           "ID", "Produto", "Categoria", "Preco (R$)", "Qtd", "Fornecedor");
    linhaTabela();                               // linha de baixo
}

// Imprime um produto em uma linha da tabela
void mostrarProduto(Produto *p) {
    printf("| %-3d | %-20s | %-15s | %10.2f | %5d | %-15s |\n",  // %10.2f = 2 casas decimais
           p->id, p->produto, p->categoria, p->preco, p->quantidade, p->fornecedor);
}

// Imprime a tabela com todos os produtos do vetor
void mostrarTabela(Produto lista[], int total) {
    if (total == 0) {                            // vetor vazio
        printf("Nenhum produto cadastrado.\n");
        return;
    }
    cabecalho();                                 // titulos
    for (int i = 0; i < total; i++) {            // percorre o vetor
        mostrarProduto(&lista[i]);               // mostra cada produto
    }
    linhaTabela();                               // fecha a tabela
    printf("Total: %d produto(s)\n", total);
}

// ============================================================
// BUSCAS NO VETOR
// ============================================================

// Procura o ID no vetor. Retorna a posicao ou -1 se nao achar
int buscarId(Produto lista[], int total, int id) {
    for (int i = 0; i < total; i++) {            // percorre o vetor
        if (lista[i].id == id) {                 // achou o ID
            return i;                            // devolve a posicao
        }
    }
    return -1;                                   // nao achou
}

// Verifica se ja existe produto com esse nome (ignora o produto idIgnorar)
int existeNome(Produto lista[], int total, char *nome, int idIgnorar) {
    for (int i = 0; i < total; i++) {            // percorre o vetor
        if (lista[i].id != idIgnorar && compararTexto(lista[i].produto, nome) == 0) {  // nome igual em outro produto
            return 1;                            // duplicado
        }
    }
    return 0;                                    // nao tem igual
}

// Gera o proximo ID: maior ID existente + 1
int proximoId(Produto lista[], int total) {
    int maior = 0;                               // maior ID encontrado
    for (int i = 0; i < total; i++) {            // percorre o vetor
        if (lista[i].id > maior) {               // achou um ID maior
            maior = lista[i].id;                 // atualiza o maior
        }
    }
    return maior + 1;                            // proximo ID
}

// ============================================================
// OPCOES DO MENU
// ============================================================

// LISTAR: mostra todos os produtos
void listar() {
    Produto lista[MAX];                          // vetor de produtos
    int total = carregar(lista);                 // le o arquivo para o vetor
    printf("\n");
    mostrarTabela(lista, total);                 // mostra a tabela
}

// INSERIR: cadastra um produto novo
void inserir() {
    Produto lista[MAX];                          // vetor com os produtos ja cadastrados
    int total = carregar(lista);                 // carrega para checar duplicata e ID
    Produto novo;                                // ficha do novo produto

    if (total >= MAX) {                          // vetor cheio
        printf("Limite de produtos atingido.\n");
        return;
    }

    printf("\n--- INSERIR PRODUTO ---\n");
    printf("Nome do produto: ");
    lerTexto(novo.produto, 50);                  // le o nome
    if (strlen(novo.produto) == 0) {             // nome vazio
        printf("O nome nao pode ficar vazio.\n");
        return;
    }
    if (existeNome(lista, total, novo.produto, 0)) {  // verificacao de nome duplicado
        printf("Ja existe um produto com o nome '%s'.\n", novo.produto);
        return;                                  // cancela o cadastro
    }

    printf("Categoria: ");
    lerTexto(novo.categoria, 30);                // le a categoria
    printf("Preco: ");
    novo.preco = lerFloat();                     // le o preco
    printf("Quantidade: ");
    novo.quantidade = lerInteiro();              // le a quantidade
    printf("Fornecedor: ");
    lerTexto(novo.fornecedor, 30);               // le o fornecedor

    novo.id = proximoId(lista, total);           // ID automatico
    while (buscarId(lista, total, novo.id) != -1) {  // verificacao de ID duplicado
        novo.id++;                               // se ja existir, tenta o proximo
    }

    FILE *arquivo = fopen(ARQUIVO, "a");         // "a" = adiciona no final do arquivo
    if (arquivo == NULL) {                       // erro ao abrir
        printf("Erro ao abrir o arquivo.\n");
        return;
    }
    gravarLinha(arquivo, &novo);                 // grava o produto novo
    fclose(arquivo);                             // fecha e salva
    printf("Produto cadastrado com o ID %d.\n", novo.id);
}

// ALTERAR: muda os dados de um produto (Enter mantem o valor atual)
void alterar() {
    Produto lista[MAX];                          // vetor de produtos
    int total = carregar(lista);                 // le o arquivo
    char texto[50];                              // guarda o que for digitado

    printf("\n");
    mostrarTabela(lista, total);                 // mostra os produtos para escolher
    if (total == 0) {                            // nada para alterar
        return;
    }

    printf("Digite o ID do produto que deseja alterar: ");
    int id = lerInteiro();                       // le o ID
    int pos = buscarId(lista, total, id);        // procura a posicao do produto
    if (pos == -1) {                             // ID nao existe
        printf("Produto nao encontrado.\n");
        return;
    }

    Produto *p = &lista[pos];                    // ponteiro direto para o produto no vetor
    printf("Aperte Enter para manter o valor atual.\n");

    printf("Nome (%s): ", p->produto);
    lerTexto(texto, 50);                         // le o novo nome
    if (strlen(texto) > 0) {                     // digitou alguma coisa
        if (existeNome(lista, total, texto, p->id)) {  // nome ja usado por outro produto
            printf("Ja existe outro produto com esse nome. Nome mantido.\n");
        } else {
            strcpy(p->produto, texto);           // altera o nome
        }
    }

    printf("Categoria (%s): ", p->categoria);
    lerTexto(texto, 30);                         // le a nova categoria
    if (strlen(texto) > 0) {                     // digitou alguma coisa
        strcpy(p->categoria, texto);             // altera a categoria
    }

    printf("Preco (%.2f): ", p->preco);
    lerTexto(texto, 30);                         // le o novo preco como texto
    if (strlen(texto) > 0) {                     // digitou alguma coisa
        for (int i = 0; texto[i] != '\0'; i++) { // percorre o texto
            if (texto[i] == ',') {               // virgula
                texto[i] = '.';                  // vira ponto
            }
        }
        float preco;                             // preco convertido
        if (sscanf(texto, "%f", &preco) == 1 && preco >= 0) {  // numero valido
            p->preco = preco;                    // altera o preco
        } else {
            printf("Preco invalido, mantido o atual.\n");
        }
    }

    printf("Quantidade (%d): ", p->quantidade);
    lerTexto(texto, 30);                         // le a nova quantidade como texto
    if (strlen(texto) > 0) {                     // digitou alguma coisa
        int qtd;                                 // quantidade convertida
        if (sscanf(texto, "%d", &qtd) == 1 && qtd >= 0) {  // numero valido
            p->quantidade = qtd;                 // altera a quantidade
        } else {
            printf("Quantidade invalida, mantida a atual.\n");
        }
    }

    printf("Fornecedor (%s): ", p->fornecedor);
    lerTexto(texto, 30);                         // le o novo fornecedor
    if (strlen(texto) > 0) {                     // digitou alguma coisa
        strcpy(p->fornecedor, texto);            // altera o fornecedor
    }

    if (salvar(lista, total)) {                  // grava usando o arquivo temporario
        printf("Produto alterado com sucesso.\n");
    }
}

// EXCLUIR: remove um produto
void excluir() {
    Produto lista[MAX];                          // vetor de produtos
    int total = carregar(lista);                 // le o arquivo
    char resposta[10];                           // resposta s/n

    printf("\n");
    mostrarTabela(lista, total);                 // mostra os produtos para escolher
    if (total == 0) {                            // nada para excluir
        return;
    }

    printf("Digite o ID do produto que deseja excluir: ");
    int id = lerInteiro();                       // le o ID
    int pos = buscarId(lista, total, id);        // procura a posicao do produto
    if (pos == -1) {                             // ID nao existe
        printf("Produto nao encontrado.\n");
        return;
    }

    printf("Tem certeza que deseja excluir '%s'? (s/n): ", lista[pos].produto);
    lerTexto(resposta, 10);                      // le a confirmacao
    if (resposta[0] != 's' && resposta[0] != 'S') {  // nao confirmou
        printf("Exclusao cancelada.\n");
        return;
    }

    for (int i = pos; i < total - 1; i++) {      // a partir do produto excluido
        lista[i] = lista[i + 1];                 // puxa o proximo uma posicao para tras
    }
    total--;                                     // vetor tem um produto a menos

    if (salvar(lista, total)) {                  // grava usando o arquivo temporario
        printf("Produto excluido com sucesso.\n");
    }
}

// BUSCAR: tipo 1 = por nome, tipo 2 = por categoria (ignora maiuscula)
void buscar(int tipo) {
    Produto lista[MAX];                          // vetor de produtos
    int total = carregar(lista);                 // le o arquivo
    char termo[50], termoMin[50], campoMin[100]; // termo digitado e versoes em minusculo
    int achou = 0;                               // quantos produtos foram encontrados

    if (tipo == 1) {                             // busca por nome
        printf("\nDigite o nome (ou parte dele): ");
    } else {                                     // busca por categoria
        printf("\nDigite a categoria (ou parte dela): ");
    }
    lerTexto(termo, 50);                         // le o termo
    minusculo(termo, termoMin);                  // termo em minusculo

    for (int i = 0; i < total; i++) {            // percorre o vetor
        if (tipo == 1) {
            minusculo(lista[i].produto, campoMin);    // nome em minusculo
        } else {
            minusculo(lista[i].categoria, campoMin);  // categoria em minusculo
        }
        if (strstr(campoMin, termoMin) != NULL) { // strstr procura o termo dentro do campo
            if (achou == 0) {                    // primeiro encontrado
                cabecalho();                     // imprime os titulos uma vez
            }
            mostrarProduto(&lista[i]);           // mostra o produto
            achou++;                             // conta mais um
        }
    }

    if (achou > 0) {                             // encontrou algum
        linhaTabela();                           // fecha a tabela
        printf("%d produto(s) encontrado(s).\n", achou);
    } else {
        printf("Nenhum produto encontrado.\n");
    }
}

// ============================================================
// ORDENACAO - BUBBLE SORT (so na memoria, o arquivo nao muda)
// ============================================================

// Troca dois produtos de lugar usando ponteiros
void trocar(Produto *a, Produto *b) {
    Produto aux = *a;                            // guarda o conteudo de a
    *a = *b;                                     // a recebe o conteudo de b
    *b = aux;                                    // b recebe o que era de a
}

// Diz se os dois vizinhos estao na ordem errada para o tipo escolhido
int precisaTrocar(Produto *a, Produto *b, int tipo) {
    if (tipo == 1) {                             // nome A-Z
        return compararTexto(a->produto, b->produto) > 0;  // a vem depois de b
    }
    if (tipo == 2) {                             // nome Z-A
        return compararTexto(a->produto, b->produto) < 0;  // a vem antes de b
    }
    if (tipo == 3) {                             // preco menor para maior
        return a->preco > b->preco;              // a e mais caro
    }
    if (tipo == 4) {                             // preco maior para menor
        return a->preco < b->preco;              // a e mais barato
    }
    return 0;                                    // tipo invalido: nao troca
}

// Bubble Sort: compara vizinhos e troca os que estao fora de ordem
void bubbleSort(Produto lista[], int total, int tipo) {
    for (int i = 0; i < total - 1; i++) {        // cada volta coloca um produto no lugar certo
        int trocou = 0;                          // marca se houve troca nessa volta
        for (int j = 0; j < total - 1 - i; j++) {  // compara os vizinhos (o final ja esta ordenado)
            if (precisaTrocar(&lista[j], &lista[j + 1], tipo)) {  // estao fora de ordem
                trocar(&lista[j], &lista[j + 1]);  // troca os dois
                trocou = 1;                      // marca que trocou
            }
        }
        if (trocou == 0) {                       // nenhuma troca na volta
            break;                               // ja esta ordenado, para antes
        }
    }
}

// ORDENAR: mostra o submenu, ordena o vetor e mostra a tabela
void ordenar() {
    Produto lista[MAX];                          // vetor de produtos
    int total = carregar(lista);                 // le o arquivo

    if (total == 0) {                            // nada para ordenar
        printf("\nNenhum produto cadastrado.\n");
        return;
    }

    printf("\n--- ORDENAR ---\n");
    printf("1 - Nome (A-Z)\n");
    printf("2 - Nome (Z-A)\n");
    printf("3 - Preco (menor para maior)\n");
    printf("4 - Preco (maior para menor)\n");
    printf("Escolha: ");
    int tipo = lerInteiro();                     // le o tipo de ordenacao

    if (tipo < 1 || tipo > 4) {                  // opcao fora do submenu
        printf("Opcao invalida.\n");
        return;
    }

    bubbleSort(lista, total, tipo);              // ordena so o vetor
    printf("\n");
    mostrarTabela(lista, total);                 // mostra o resultado
    printf("(a ordem mudou so na tela, o arquivo continua igual)\n");
}

// EXPORTAR: gera o arquivo CSV separado por virgula
void exportarCsv() {
    Produto lista[MAX];                          // vetor de produtos
    int total = carregar(lista);                 // le o arquivo

    FILE *csv = fopen(CSV, "w");                 // cria o arquivo CSV
    if (csv == NULL) {                           // erro ao criar
        printf("Erro ao criar o arquivo CSV.\n");
        return;
    }
    fprintf(csv, "ID,Produto,Categoria,Preco,Quantidade,Fornecedor\n");  // linha de titulos
    for (int i = 0; i < total; i++) {            // percorre o vetor
        fprintf(csv, "%d,\"%s\",\"%s\",%.2f,%d,\"%s\"\n",  // textos entre aspas
                lista[i].id, lista[i].produto, lista[i].categoria,
                lista[i].preco, lista[i].quantidade, lista[i].fornecedor);
    }
    fclose(csv);                                 // fecha e salva
    printf("\n%d produto(s) exportado(s) para %s\n", total, CSV);
}

// ============================================================
// PROGRAMA PRINCIPAL
// ============================================================

int main() {
    int opcao;                                   // opcao escolhida no menu
    char resposta[10];                           // resposta s/n da opcao 1

    if (!arquivoExiste()) {                      // primeira vez rodando
        criarArquivo();                          // cria com os produtos iniciais
    }

    do {                                         // faca: mostra o menu e executa a opcao
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
        opcao = lerInteiro();                    // le a opcao

        switch (opcao) {                         // chama a funcao da opcao escolhida
            case 1:
                printf("Isso apaga os dados atuais. Confirma? (s/n): ");
                lerTexto(resposta, 10);          // le a confirmacao
                if (resposta[0] == 's' || resposta[0] == 'S') {  // confirmou
                    criarArquivo();              // recria o arquivo do zero
                }
                break;
            case 2:
                listar();                        // listar
                break;
            case 3:
                inserir();                       // inserir
                break;
            case 4:
                alterar();                       // alterar
                break;
            case 5:
                excluir();                       // excluir
                break;
            case 6:
                buscar(1);                       // buscar por nome
                break;
            case 7:
                buscar(2);                       // buscar por categoria
                break;
            case 8:
                ordenar();                       // ordenar
                break;
            case 9:
                exportarCsv();                   // exportar CSV
                break;
            case 0:
                printf("Saindo...\n");           // sair
                break;
            default:
                printf("Opcao invalida.\n");     // numero que nao existe no menu
        }
    } while (opcao != 0);                        // enquanto a opcao for diferente de 0

    return 0;                                    // fim do programa sem erro
}
