#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- DEFINICOES GLOBAIS ---
#define TAMANHO_MAX 100
#define HASH_TABLE_SIZE 10 // Tamanho da nossa tabela hash.

// --- ESTRUTURAS DE DADOS ---

// Estrutura para os cómodos da mansão (Árvore Binária)
typedef struct Sala {
    char nome[TAMANHO_MAX];
    char pista[TAMANHO_MAX];
    char suspeitoAssociado[TAMANHO_MAX]; // Novo: suspeito ligado a pista
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

// Estrutura para os nós da árvore de pistas (BST)
typedef struct PistaNode {
    char texto[TAMANHO_MAX];
    struct PistaNode* esquerda;
    struct PistaNode* direita;
} PistaNode;

// Estrutura para os nós da Tabela Hash (Lista Encadeada para colisões)
typedef struct HashNode {
    char pista[TAMANHO_MAX];
    char suspeito[TAMANHO_MAX];
    struct HashNode* proximo;
} HashNode;

// A própria Tabela Hash e um array de ponteiros para HashNode
HashNode* tabelaHash[HASH_TABLE_SIZE];


// --- FUNÇÕES DA TABELA HASH ---

unsigned int hashFunction(const char* pista) {
    unsigned long hash = 0;
    for (int i = 0; pista[i] != '\0'; i++) {
        hash += pista[i];
    }
    return hash % HASH_TABLE_SIZE;
}

void inicializarHash() {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        tabelaHash[i] = NULL;
    }
}

void inserirNaHash(const char* pista, const char* suspeito) {
    unsigned int index = hashFunction(pista);

    // Cria um nó para a associação
    HashNode* novoNode = (HashNode*) malloc(sizeof(HashNode));
    if(novoNode == NULL) return;

    strncpy(novoNode->pista, pista, TAMANHO_MAX);
    strncpy(novoNode->suspeito, suspeito, TAMANHO_MAX);

    // Adiciona o novo nó no início da lista encadeada (tratamento de colisão)
    novoNode->proximo = tabelaHash[index];
    tabelaHash[index] = novoNode;
}

int contarPistasDoSuspeito(const char* nomeSuspeito) {
    int contador = 0;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* atual = tabelaHash[i];
        while (atual != NULL) {
            if (strcmp(atual->suspeito, nomeSuspeito) == 0) {
                contador++;
            }
            atual = atual->proximo;
        }
    }
    return contador;
}


// --- FUNÇÕES DA ÁRVORE DE PISTAS (BST) ---

PistaNode* criarPistaNode(const char* texto) {
    PistaNode* novoNode = (PistaNode*) malloc(sizeof(PistaNode));
    if(novoNode == NULL) return NULL;
    strncpy(novoNode->texto, texto, TAMANHO_MAX);
    novoNode->esquerda = novoNode->direita = NULL;
    return novoNode;
}

PistaNode* adicionarPista(PistaNode* raiz, const char* texto) {
    if (raiz == NULL) return criarPistaNode(texto);
    int cmp = strcmp(texto, raiz->texto);
    if (cmp < 0) raiz->esquerda = adicionarPista(raiz->esquerda, texto);
    else if (cmp > 0) raiz->direita = adicionarPista(raiz->direita, texto);
    return raiz;
}

void exibirPistas(PistaNode* raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda);
        printf("- %s\n", raiz->texto);
        exibirPistas(raiz->direita);
    }
}


// --- FUNÇÕES DA MANSÃO (ÁRVORE BINÁRIA) ---

Sala* criarSala(const char* nome, const char* pista, const char* suspeito) {
    Sala* novaSala = (Sala*) malloc(sizeof(Sala));
    if(novaSala == NULL) return NULL;
    strncpy(novaSala->nome, nome, TAMANHO_MAX);
    strncpy(novaSala->pista, pista, TAMANHO_MAX);
    strncpy(novaSala->suspeitoAssociado, suspeito, TAMANHO_MAX);
    novaSala->esquerda = novaSala->direita = NULL;
    return novaSala;
}

void explorarSalas(Sala* salaAtual, PistaNode** raizPistas) {
    char escolha;
    while (1) {
        printf("\n----------------------------------------\n");
        printf("Voce esta em: %s\n", salaAtual->nome);
        printf("----------------------------------------\n");

        if (strlen(salaAtual->pista) > 0) {
            printf("Voce encontrou uma pista: \"%s\"\n", salaAtual->pista);
            *raizPistas = adicionarPista(*raizPistas, salaAtual->pista);
            inserirNaHash(salaAtual->pista, salaAtual->suspeitoAssociado);
            salaAtual->pista[0] = '\0'; // Remove a pista apos coletar
        }

        printf("O que voce deseja fazer?\n");
        if (salaAtual->esquerda != NULL) printf(" (e) Ir para a Esquerda\n");
        if (salaAtual->direita != NULL) printf(" (d) Ir para a Direita\n");
        printf(" (v) Ver pistas coletadas\n");
        printf(" (s) Sair e fazer a acusacao\n");
        printf("Escolha uma opcao: ");
        scanf(" %c", &escolha);

        switch (escolha) {
            case 'e':
                if (salaAtual->esquerda != NULL) salaAtual = salaAtual->esquerda;
                else printf("Nao ha caminho para a esquerda.\n");
                break;
            case 'd':
                if (salaAtual->direita != NULL) salaAtual = salaAtual->direita;
                else printf("Nao ha caminho para a direita.\n");
                break;
            case 'v':
                printf("\n--- PISTAS COLETADAS ---\n");
                if (*raizPistas == NULL) printf("Nenhuma pista coletada.\n");
                else exibirPistas(*raizPistas);
                printf("------------------------\n");
                break;
            case 's':
                printf("Voce se sente pronto para acusar um suspeito.\n");
                return;
            default:
                printf("Opcao invalida!\n");
        }
    }
}

void verificarSuspeitoFinal() {
    char acusado[TAMANHO_MAX];
    printf("\n--- HORA DO JULGAMENTO ---\n");
    printf("Os suspeitos sao: Mordomo, Cozinheira, Jardineiro.\n");
    printf("Com base nas pistas, quem voce acusa? ");
    scanf("%s", acusado);

    int numPistas = contarPistasDoSuspeito(acusado);

    printf("\n--- VEREDITO ---\n");
    printf("Voce acusou %s. Esta acusacao e apoiada por %d pista(s).\n", acusado, numPistas);

    if (numPistas >= 2) {
        printf("Evidencias suficientes! Voce desvendou o misterio! Parabens, detetive!\n");
    } else {
        printf("Evidencias insuficientes... O verdadeiro culpado escapou por falta de provas.\n");
    }
}


// --- GERENCIAMENTO DE MEMÓRIA ---

void liberarMapa(Sala* raiz) {
    if(raiz == NULL) return;
    liberarMapa(raiz->esquerda);
    liberarMapa(raiz->direita);
    free(raiz);
}

void liberarPistas(PistaNode* raiz) {
    if(raiz == NULL) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

void liberarHash() {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* atual = tabelaHash[i];
        while (atual != NULL) {
            HashNode* temp = atual;
            atual = atual->proximo;
            free(temp);
        }
    }
}


// --- FUNÇÃO PRINCIPAL ---

int main() {
    Sala* hall = criarSala("Hall de Entrada", "", "");
    hall->esquerda = criarSala("Sala de Estar", "Um bilhete amassado com a palavra 'Jardim'.", "Jardineiro");
    hall->direita = criarSala("Sala de Jantar", "", "");
    hall->esquerda->esquerda = criarSala("Biblioteca", "Um livro sobre venenos fora do lugar.", "Cozinheira");
    hall->esquerda->direita = criarSala("Jardim de Inverno", "Pegadas de barro perto da janela.", "Jardineiro");
    hall->direita->esquerda = criarSala("Cozinha", "Uma faca de cozinha faltando no cepo.", "Cozinheira");
    hall->direita->direita = criarSala("Escritorio", "Um cofre aberto e vazio.", "Mordomo");

    // Inicialização das estruturas
    PistaNode* raizPistas = NULL;
    inicializarHash();

    printf("Associe as pistas aos suspeitos e resolva o caso!\n");

    // Inicia a exploração
    explorarSalas(hall, &raizPistas);

    // Fase final do jogo
    verificarSuspeitoFinal();

    // Liberação de toda a memória
    liberarMapa(hall);
    liberarPistas(raizPistas);
    liberarHash();
    printf("\nJogo encerrado. Memoria liberada.\n");

    return 0;
}