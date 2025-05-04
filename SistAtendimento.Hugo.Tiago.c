#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int num_clientes = 0;
int teste = 1;

typedef enum
{
    VERMELHO,
    PRETO
} Cor;
typedef struct No
{
    int chave;
    Cor cor;
    struct No *esq, *dir, *pai;
} No;

No *SENTINELA;

void init_SENTINELA()
{
    SENTINELA = malloc(sizeof(No));
    SENTINELA->cor = PRETO;
    SENTINELA->esq = SENTINELA->dir = SENTINELA->pai = NULL;
}

No *criar_no(int chave)
{
    No *node = malloc(sizeof(No));
    node->chave = chave;
    node->cor = VERMELHO;
    node->esq = node->dir = node->pai = SENTINELA;
    return node;
}

void rot_esq(No **raiz, No *x)
{
    No *y = x->dir;
    x->dir = y->esq;
    if (y->esq != SENTINELA)
        y->esq->pai = x;
    y->pai = x->pai;
    if (x->pai == SENTINELA)
        *raiz = y;
    else if (x == x->pai->esq)
        x->pai->esq = y;
    else
        x->pai->dir = y;
    y->esq = x;
    x->pai = y;
}

void rot_dir(No **raiz, No *y)
{
    No *x = y->esq;
    y->esq = x->dir;
    if (x->dir != SENTINELA)
        x->dir->pai = y;
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

void fix_insercao(No **raiz, No *k)
{
    No *u;
    while (k->pai->cor == VERMELHO)
    {
        if (k->pai == k->pai->pai->esq)
        {
            u = k->pai->pai->dir;
            if (u->cor == VERMELHO)
            {
                k->pai->cor = u->cor = PRETO;
                k->pai->pai->cor = VERMELHO;
                k = k->pai->pai;
            }
            else
            {
                if (k == k->pai->dir)
                {
                    k = k->pai;
                    rot_esq(raiz, k);
                }
                k->pai->cor = PRETO;
                k->pai->pai->cor = VERMELHO;
                rot_dir(raiz, k->pai->pai);
            }
        }
        else
        {
            u = k->pai->pai->esq;
            if (u->cor == VERMELHO)
            {
                k->pai->cor = u->cor = PRETO;
                k->pai->pai->cor = VERMELHO;
                k = k->pai->pai;
            }
            else
            {
                if (k == k->pai->esq)
                {
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

void inserir(No **raiz, int chave)
{
    No *node = criar_no(chave);
    No *y = SENTINELA;
    No *x = *raiz;

    while (x != SENTINELA)
    {
        y = x;
        x = (node->chave < x->chave ? x->esq : x->dir);
    }
    node->pai = y;
    if (y == SENTINELA)
        *raiz = node;
    else if (node->chave < y->chave)
        y->esq = node;
    else
        y->dir = node;

    if (node->pai == SENTINELA)
    {
        node->cor = PRETO;
        return;
    }
    if (node->pai->pai == SENTINELA)
        return;
    fix_insercao(raiz, node);
}

No *minimo(No *node)
{
    while (node->esq != SENTINELA)
        node = node->esq;
    return node;
}

No *maximo(No *node)
{
    while (node->dir != SENTINELA)
        node = node->dir;
    return node;
}

void transplant(No **raiz, No *u, No *v)
{
    if (u->pai == SENTINELA)
        *raiz = v;
    else if (u == u->pai->esq)
        u->pai->esq = v;
    else
        u->pai->dir = v;
    v->pai = u->pai;
}

void fix_remocao(No **raiz, No *x)
{
    No *w;
    while (x != *raiz && x->cor == PRETO)
    {
        if (x == x->pai->esq)
        {
            w = x->pai->dir;
            if (w->cor == VERMELHO)
            {
                w->cor = PRETO;
                x->pai->cor = VERMELHO;
                rot_esq(raiz, x->pai);
                w = x->pai->dir;
            }
            if (w->esq->cor == PRETO && w->dir->cor == PRETO)
            {
                w->cor = VERMELHO;
                x = x->pai;
            }
            else
            {
                if (w->dir->cor == PRETO)
                {
                    w->esq->cor = PRETO;
                    w->cor = VERMELHO;
                    rot_dir(raiz, w);
                    w = x->pai->dir;
                }
                w->cor = x->pai->cor;
                x->pai->cor = PRETO;
                w->dir->cor = PRETO;
                rot_esq(raiz, x->pai);
                x = *raiz;
            }
        }
        else
        {
            w = x->pai->esq;
            if (w->cor == VERMELHO)
            {
                w->cor = PRETO;
                x->pai->cor = VERMELHO;
                rot_dir(raiz, x->pai);
                w = x->pai->esq;
            }
            if (w->dir->cor == PRETO && w->esq->cor == PRETO)
            {
                w->cor = VERMELHO;
                x = x->pai;
            }
            else
            {
                if (w->esq->cor == PRETO)
                {
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

void remover(No **raiz, int chave)
{
    No *z = *raiz, *x, *y;
    while (z != SENTINELA && z->chave != chave)
        z = (chave < z->chave ? z->esq : z->dir);
    if (z == SENTINELA)
        return;

    y = z;
    Cor y_orig_cor = y->cor;
    if (z->esq == SENTINELA)
    {
        x = z->dir;
        transplant(raiz, z, z->dir);
    }
    else if (z->dir == SENTINELA)
    {
        x = z->esq;
        transplant(raiz, z, z->esq);
    }
    else
    {
        // **Remo��o pelo antecessor**
        y = maximo(z->esq);
        y_orig_cor = y->cor;
        x = y->esq;
        if (y->pai == z)
        {
            x->pai = y;
        }
        else
        {
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
    if (y_orig_cor == PRETO)
        fix_remocao(raiz, x);
}

void processar_atendimento(No **raiz){
    int op;
    *raiz = SENTINELA;
    while (scanf("%d", &op) == 1)
    {
        if (op == -1)
        {
            if (*raiz != SENTINELA)
            {
                No *m = minimo(*raiz);
                remover(raiz, m->chave);
            }
        }
        if(op == 0){
                break;
        }
        else
        {
            inserir(raiz, op);
            num_clientes++;
        }
    }
}

void pre_ordem(No *node)
{
    if (node == SENTINELA)
        return;
    printf("%d %s\n",
           node->chave,
           node->cor == VERMELHO ? "RED" : "BLACK");
    pre_ordem(node->esq);
    pre_ordem(node->dir);
}

void busca(No *raiz, int chave){
    if(raiz == SENTINELA){
        printf("Chave %d nao encontrada\n", chave);
        return;
    }
    if(raiz->chave == chave)
        printf("Chave %d encontrada\n", chave);
    else if(chave < raiz->chave)
        busca(raiz->esq, chave);
    else
        busca(raiz->dir, chave);
    return;
}
int main()
{
    init_SENTINELA();
    No *raiz;
    int chave_procurada;
    
    scanf("%d", &teste);
    //O primeiro inteiro lido indica se o programa vai ler dados de um arquivo ou do teclado.
    //Se for 0, o programa lê do teclado. Se for 1, lê do arquivo.
    clock_t inicio = clock();

    processar_atendimento(&raiz);
    
    if(teste == 0)
    {
        printf("chave procurada: ");
        scanf("%d", &chave_procurada);
        busca(raiz, chave_procurada);
    }

    pre_ordem(raiz);
    clock_t fim = clock();

    double tempo_ms = ((double)(fim - inicio)) * 1000.0 / CLOCKS_PER_SEC;

    printf("Numero de clientes inseridos: %d\n", num_clientes);
    printf("Tempo total: %.2f ms\n", tempo_ms);

    return 0;
}