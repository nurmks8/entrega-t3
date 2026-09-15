#include "lista.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

typedef struct no {
  dado_t dado;
  struct no *ant;
  struct no *prox;
} No;

struct lista {
  No *sentinela;
  int tamanho;
};

Lista l_cria()
{
   Lista l = malloc(sizeof(*l));
   assert(l != NULL);

   l->sentinela = malloc(sizeof(No));
   assert(l->sentinela != NULL);

   l->sentinela->prox = l->sentinela;
   l->sentinela->ant = l->sentinela;

   l->tamanho = 0;

   return l;
}

int l_tam(Lista l)
{
  return l->tamanho;
}

bool l_cheia (Lista l)
{
   (void) l;
   return false;
}

bool l_vazia(Lista l)
{
  return l->tamanho == 0;
}

void l_insere_inicio(Lista l, dado_t d)
{
   No *novo = malloc(sizeof(No));
   assert(novo !=  NULL);
   
   novo->dado = d;

   novo->ant = l->sentinela;
   novo->prox =l->sentinela->prox;
   
   l->sentinela->prox->ant = novo;
   l->sentinela->prox = novo;

   l->tamanho++;
}

void l_insere_fim(Lista l, dado_t d)
{
   No *novo = malloc(sizeof(No));
   assert(novo != NULL);

   novo->dado = d;

   novo->prox = l->sentinela;
   novo->ant = l->sentinela->ant;

   l->sentinela->ant->prox = novo;
   l->sentinela->ant = novo;

   l->tamanho++;
}

dado_t l_dado_inicio(Lista l)
{
  if (l_vazia(l)) {
    return NULL;
  }

  return l->sentinela->prox->dado;
}

dado_t l_dado_fim(Lista l)
{
  if (l_vazia(l)) {
    return NULL;
  }

  return l->sentinela->ant->dado;
}

dado_t l_remove_inicio(Lista l)
{
   if (l_vazia(l)) {
     return NULL;
   }
   No *remover = l->sentinela->prox;

   dado_t dado = remover->dado;

   l->sentinela->prox = remover->prox;

   remover->prox->ant = l->sentinela;

   free(remover);
   l->tamanho--;

   return dado;
}

dado_t l_remove_fim(Lista l)
{
   if (l_vazia(l)) {
     return NULL;
   }
  No *remover = l->sentinela->ant;

  dado_t dado = remover->dado;

  l->sentinela->ant = remover->ant;
  remover->ant->prox = l->sentinela;

  free(remover);

  l->tamanho--;

  return dado;
}

dado_t l_dado_pos(Lista l, int pos)
{
  if (pos < 0 || pos >= l->tamanho) {
    return NULL;
  }

  No *atual = l->sentinela->prox;

  for (int i = 0; i < pos; i++) {
    atual = atual->prox;
  }

  return atual->dado;
}

void l_insere_pos(Lista l, dado_t d, int p)
{
  if (p < 0 || p > l->tamanho) {
    return;
  }

  No *atual = l->sentinela->prox;

  for (int i = 0; i < p; i++) {
    atual = atual->prox;
  }

  No *novo = malloc(sizeof(No));
  assert(novo != NULL);

  novo->dado = d;
  novo->prox = atual;
  novo->ant = atual->ant;

  atual->ant->prox = novo;
  atual->ant = novo;

  l->tamanho++;
}

dado_t l_remove_pos(Lista l, int pos)
{
   if (pos < 0 || pos >= l->tamanho) {
     return NULL;
   }
   No *remover = l->sentinela->prox;

  for (int i = 0; i < pos; i++) {
    remover = remover->prox;
  }
  dado_t dado = remover->dado;

  remover->ant->prox = remover->prox;
  remover->prox->ant = remover->ant;

  free(remover);

  l->tamanho--;

  return dado;
}

void l_destroi(Lista l)
{
    if (l == NULL) {
      return;
    }

   No *atual = l->sentinela->prox;

   while (atual != l->sentinela) {
     No *prox = atual->prox;
     free(atual);
     atual = prox;
   }
   free(l->sentinela);
   free(l);
}


void l_imprime(Lista l)
{
   No *atual = l->sentinela->prox;

   while (atual != l->sentinela) {
     s_imprime(atual->dado);
     printf("\n");

     atual = atual->prox;
   }
}

dado_t l_primeiro(Lista l)
{
  return l_dado_inicio(l);
}

void l_insere(Lista l, dado_t d)
{
  l_insere_fim(l, d);
}

dado_t l_remove(Lista l)
{
  return l_remove_inicio(l);
}

dado_t l_topo(Lista l)
{
  return l_dado_inicio(l);
}

void l_empilha(Lista l, dado_t d)
{
  l_insere_inicio(l, d);
}

dado_t l_desempilha(Lista l)
{
  return l_remove_inicio(l);
}

Lista l_cria_separando(Str s, Str sep)
{
  Lista lista = l_cria();

  int inicio = 0;

  while (1) {
    inicio = s_busca_nc(s, inicio, sep);

    if (inicio == -1) {
      break;
    }

    int fim = s_busca_c(s, inicio, sep);

    int tamanho;

    if (fim == -1) {
      tamanho = s_tam(s) - inicio;
    } else {
      tamanho = fim - inicio;
    }

    Str token = s_cria_substring(s, inicio, tamanho);

    l_insere_fim(lista, token);

    if (fim == -1) {
      break;
    }

    inicio = fim + 1;
  }

  return lista;
}