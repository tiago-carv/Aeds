#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int num_clientes = 0;

typedef enum { VERMELHO, PRETO } Cor;
typedef struct No {
    int chave;
    Cor cor;
    struct No *esq, *dir, *pai;
} No;

No *SENTINELA;

void init_SENTINELA() {
    SENTINELA = malloc(sizeof(No));
    SENTINELA->cor = PRETO;
    SENTINELA->esq = SENTINELA->dir = SENTINELA->pai = NULL;
}

No *criar_no(int chave) {
    No *node = malloc(sizeof(No));
    node->chave = chave;
    node->cor = VERMELHO;  // novos nós são vermelhos
    node->esq = node->dir = node->pai =
        SENTINELA;  // filhos e pai temporariamente apontam para o sentinela
    return node;
}

void rot_esq(No **raiz, No *x)  // Rotação à esquerda em torno de x.
{
    No *y = x->dir;   // y aponta para o filho direito de x
    x->dir = y->esq;  // filho esquerdo de y passa a ser filho direito de x
    if (y->esq != SENTINELA) y->esq->pai = x;  // corrige o pai
    y->pai = x->pai;
    if (x->pai == SENTINELA)
        *raiz = y;  //  agora y vira raiz no lugar de x
    else if (x == x->pai->esq)
        x->pai->esq = y;  // se x era filho esq
    else
        x->pai->dir = y;  // se x era filho dir
    y->esq = x;
    x->pai = y;
}

void rot_dir(No **raiz, No *y)  // rotação à direita em torno de y.
{
    No *x = y->esq;
    y->esq = x->dir;
    if (x->dir != SENTINELA) x->dir->pai = y;
    x->pai = y->pai;
    if (y->pai == SENTINELA)
        *raiz = x;
    else if (y == y->pai->dir)
        y->pai->dir = x;
    else
        y->pai->esq = x;
    x->dir = y;
    y->pai = x;
}

void fix_insercao(No **raiz, No *k) {
    No *u;  // u será o “tio” de k
    while (k->pai->cor == VERMELHO) {
        if (k->pai == k->pai->pai->esq) {
            u = k->pai->pai->dir;    // tio do lado direito
            if (u->cor == VERMELHO){  // Caso 1: tio vermelho  apenas cores
                k->pai->cor = u->cor = PRETO;
            k->pai->pai->cor = VERMELHO;
            k = k->pai->pai;
            
        } else {
            if (k == k->pai->dir)  // Caso 2: pai vermellho e tio preto k é filho direito
                                   // → rotaciona à esquerda
            {
                k = k->pai;
                rot_esq(raiz, k);
            }
            k->pai->cor = PRETO;  // filho esquerdo → rotaciona à direita no avô
            k->pai->pai->cor = VERMELHO;
            rot_dir(raiz, k->pai->pai);
        }
    }
    else {  // mesma lógica do lado direito (espelhado)
        u = k->pai->pai->esq;
        if (u->cor == VERMELHO) {
            k->pai->cor = u->cor = PRETO;
            k->pai->pai->cor = VERMELHO;
            k = k->pai->pai;
        } else {
            if (k == k->pai->esq) {
                k = k->pai;
                rot_dir(raiz, k);
            }
            k->pai->cor = PRETO;
            k->pai->pai->cor = VERMELHO;
            rot_esq(raiz, k->pai->pai);
        }
    }
}
(*raiz)->cor = PRETO;
}

void inserir(No **raiz, int chave) {
    No *node = criar_no(chave);
    No *y = SENTINELA;
    No *x = *raiz;

    while (x != SENTINELA) {
        y = x;
        x = (node->chave < x->chave ? x->esq : x->dir);
    }
    node->pai = y;            // define o pai do novo nó
    if (y == SENTINELA)
        *raiz = node;
    else if (node->chave < y->chave)
        y->esq = node;
    else
        y->dir = node;

    if (node->pai == SENTINELA) {    // Se o pai é o sentinela (novo nó é raiz), pinta de preto
        node->cor = PRETO;
        return;
    }
    if (node->pai->pai == SENTINELA) return;  // Se o avô é sentinela, não há conflito
    fix_insercao(raiz, node);
}

No *minimo(No *node) {
    while (node->esq != SENTINELA) node = node->esq;
    return node;
}

No *maximo(No *node) {
    while (node->dir != SENTINELA) node = node->dir;
    return node;
}

void transplant(No **raiz, No *u, No *v) {
    if (u->pai == SENTINELA)
        *raiz = v;                 // u era raiz → v vira raiz
    else if (u == u->pai->esq)
        u->pai->esq = v;             // u era filho esquerdo -> conecta v aqui
    else
        u->pai->dir = v;                // u era filho direito → conecta v aqui
    v->pai = u->pai;                       // v aponta agora para o pai de u
}

void fix_remocao(No **raiz, No *x) {
    No *w;                                   // w = irmão de x
    while (x != *raiz && x->cor == PRETO) {   // enquanto não voltarmos à raiz e x for preto
        if (x == x->pai->esq) {    //x é filho esquerdo
            w = x->pai->dir;         // w aponta para o irmão à direita
            if (w->cor == VERMELHO) {   //Subcaso 1: irmão vermelho
                w->cor = PRETO;
                x->pai->cor = VERMELHO;
                rot_esq(raiz, x->pai);
                w = x->pai->dir;
            }
            if (w->esq->cor == PRETO && w->dir->cor == PRETO) {   // 2: irmão e os filhos de w são pretos
                w->cor = VERMELHO;  // pinta w de vermelho
                x = x->pai;     // sobe x para cima e repete
            } else {
                if (w->dir->cor == PRETO) {  //3: w preto, filho esquerdo de w vermelho, direito preto
                    w->esq->cor = PRETO;
                    w->cor = VERMELHO;
                    rot_dir(raiz, w);
                    w = x->pai->dir;
                }
                w->cor = x->pai->cor;  //4: w preto, filho direito de w vermelho
                x->pai->cor = PRETO;
                w->dir->cor = PRETO;
                rot_esq(raiz, x->pai);
                x = *raiz;
            }
        } else {
            w = x->pai->esq;   //x é filho direito (espelho)
            if (w->cor == VERMELHO) {
                w->cor = PRETO;
                x->pai->cor = VERMELHO;
                rot_dir(raiz, x->pai);
                w = x->pai->esq;
            }
            if (w->dir->cor == PRETO && w->esq->cor == PRETO) {
                w->cor = VERMELHO;
                x = x->pai;
            } else {
                if (w->esq->cor == PRETO) {
                    w->dir->cor = PRETO;
                    w->cor = VERMELHO;
                    rot_esq(raiz, w);
                    w = x->pai->esq;
                }
                w->cor = x->pai->cor;
                x->pai->cor = PRETO;
                w->esq->cor = PRETO;
                rot_dir(raiz, x->pai);
                x = *raiz;
            }
        }
    }
    x->cor = PRETO;
}

void remover(No **raiz, int chave) {
    No *z = *raiz, *x, *y;
    while (z != SENTINELA && z->chave != chave) z = (chave < z->chave ? z->esq : z->dir);   // busca o nó com a chave
    if (z == SENTINELA) return;  // nao achou 

    y = z;
    Cor y_orig_cor = y->cor;         // cor original de y
    if (z->esq == SENTINELA) {
        x = z->dir;
        transplant(raiz, z, z->dir);
    } else if (z->dir == SENTINELA) {
        x = z->esq;
        transplant(raiz, z, z->esq);
    } else {
        // **Remo��o pelo antecessor**
        y = maximo(z->esq);
        y_orig_cor = y->cor;
        x = y->esq;
        if (y->pai == z) {
            x->pai = y;
        } else {
            transplant(raiz, y, y->esq);
            y->esq = z->esq;
            y->esq->pai = y;
        }
        transplant(raiz, z, y);
        y->dir = z->dir;
        y->dir->pai = y;
        y->cor = z->cor;
    }
    free(z);              
    if (y_orig_cor == PRETO) fix_remocao(raiz, x);         // se removido/predecessor era preto, corrige possíveis violações
}

void busca(No *raiz, int chave) {
    if (raiz == SENTINELA) {
        printf("Chave %d nao encontrada\n", chave);
        return;
    }
    if (raiz->chave == chave)
        printf("Chave %d encontrada\n", chave);
    else if (chave < raiz->chave)
        busca(raiz->esq, chave);
    else
        busca(raiz->dir, chave);
}

void processar_atendimento(No **raiz) {
    int op;
    *raiz = SENTINELA;
    while (scanf("%d", &op) == 1) {
        if (op == -1) {
            if (*raiz != SENTINELA) {
                No *m = minimo(*raiz);
                remover(raiz, m->chave);
            }
        } else {
            inserir(raiz, op);
            num_clientes++;
        }
    }
}

void pre_ordem(No *node) {
    if (node == SENTINELA) return;
    printf("%d %s\n", node->chave, node->cor == VERMELHO ? "RED" : "BLACK");
    pre_ordem(node->esq);
    pre_ordem(node->dir);
}

int main() {
    init_SENTINELA();
    No *raiz = SENTINELA;

    int chave_procurada;

    clock_t inicio = clock();
    processar_atendimento(&raiz);
    pre_ordem(raiz);

    clock_t fim = clock();

    double tempo_ms = ((double)(fim - inicio)) * 1000.0 / CLOCKS_PER_SEC;

    printf("N�mero de clientes inseridos: %d\n", num_clientes);
    printf("Tempo total: %.2f ms\n", tempo_ms);

    // freopen("/dev/tty", "r", stdin);
    // printf("Chave procurada: ");
    // scanf("%d", &chave_procurada);
    // busca(raiz,chave_procurada);

    return 0;
}
