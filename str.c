#include "str.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MIN_ALLOC 8

struct str {
  byte *dados;
  int nbytes;
  int nchars;
  int alocado;
};

static void s_ok(Str_c s)
{
  if (s == NULL) {
    return;
  }
  assert(s->nbytes >= 0);
  assert(s->nchars >= 0);
  assert(s->alocado >= 0);
  assert(s->nchars <= s->nbytes);
  if (s->nbytes == 0) {
    assert(s->dados == NULL);
    assert(s->nchars == 0);
    assert(s->alocado == 0);
  }
  if (s->nbytes > 0) {
  assert(s->dados != NULL);
  assert(s->alocado >= s->nbytes);
  assert(s->alocado >= MIN_ALLOC);
  assert((s->alocado & (s->alocado - 1)) == 0);
  if (s->alocado != MIN_ALLOC) {
    assert(s->alocado <= 3 * s->nbytes);
  }
  int quantidade_real = u8_conta_unichar_nos_bytes(s->nbytes, s->dados);
  assert(quantidade_real == s->nchars);
  }
}

// operações de criação e destruição {{{1

Str s_cria(char const *strC)
{
  Str s = malloc(sizeof(*s));
  assert(s != NULL);

  if (strC == NULL) {
    s->dados = NULL;
    s->nbytes = 0;
    s->nchars = 0;
    s->alocado = 0;
    return s;
  }

 int nbytes = strlen(strC);
 int nchars = u8_conta_unichar_nos_bytes(
  nbytes, (byte *)strC
);
if (nchars == -1) {
  s->dados = NULL;
  s->nbytes = 0;
  s->nchars = 0;
  s->alocado = 0;
  return s;
 }
 if (nbytes == 0) {
    s->dados = NULL;
    s->nbytes = 0;
    s->nchars = 0;
    s->alocado = 0;
    return s;
  }
  s->alocado = MIN_ALLOC;
  while (s->alocado < nbytes) {
    s->alocado = 2 * s->alocado;
  }
  s->dados = malloc(s->alocado);
  assert(s->dados != NULL);
  memcpy(s->dados, strC, nbytes);
  s->nbytes = nbytes;
  s->nchars = nchars;
  s_ok(s);
  return s;
}

Str s_cria_número(double num)
{
    char texto[400];

    sprintf(texto, "%g", num);

    return s_cria(texto);
}

Str s_cria_unindo(Lista l, Str sep)
{
   Str resultado = s_cria("");

   for (int i = 0; i < l_tam(l); i++) {
     Str atual = l_dado_pos(l, i);

     s_anexa(resultado, atual);

     if (i < l_tam(l) - 1) {
       s_anexa(resultado, sep);
   }
     }
     return resultado;
}
void s_destroi(Str s)
{
  s_ok(s);
  if (s == NULL) {
    return;
  }
  free(s->dados);
  free(s);
}

Str s_cria_substring(Str_c s, int pos, int tam)
{
   Str nova = s_cria("");
   s_substring(nova, s, pos, tam);
   return nova;
}

Str s_cria_cópia(Str_c s)
{
   return s_cria_substring(s, 0, -1);
}

// Retorna uma nova string com o conteúdo do arquivo chamado nome.
// Retorna uma string vazia em caso de erro.
Str s_cria_de_arquivo(char *nome)
{
  Str s = s_cria("");
  if (nome == NULL) {
    return s;
  }
  FILE *arq = fopen(nome, "rb");
  if (arq == NULL) {
    return s;
  }
  fseek(arq, 0, SEEK_END);
  long nbytes = ftell(arq);
  fseek(arq, 0, SEEK_SET);
  if (nbytes <= 0) {
    fclose(arq);
    return s;
  }
  byte *dados = malloc(nbytes);
  assert(dados != NULL);
  size_t lidos = fread(dados, 1, nbytes, arq);
  if (lidos != (size_t)nbytes) {
    free(dados);
    fclose(arq);
    return s;
  }
  int nchars = u8_conta_unichar_nos_bytes(nbytes, dados);
  if (nchars == -1) {
  free(dados);
  fclose(arq);
  return s;
  }
  s->alocado = MIN_ALLOC;
  while (s->alocado < nbytes) {
    s->alocado = 2 * s->alocado;
  }
  s->dados = malloc(s->alocado);
  assert(s->dados != NULL);
  memcpy(s->dados, dados, nbytes);
  s->nbytes = nbytes;
  s->nchars = nchars;
  free(dados);
  fclose(arq);
  s_ok(s);
  return s;
}

// operações de acesso {{{1

int s_tam(Str_c s)
{
  s_ok(s);
  if (s == NULL) {
    return 0;
  }
  return s->nchars;
}

char *s_strc(Str_c s)
{
  s_ok(s);
  if (s == NULL) {
    char *nova = malloc(1);
    assert(nova != NULL);
    nova[0] = '\0';
    return nova;
  }
  char *nova = malloc(s->nbytes + 1);
  assert(nova != NULL);
  if (s->nbytes > 0) {
    memcpy(nova, s->dados, s->nbytes);
  }
  nova[s->nbytes] = '\0';
  return nova;
}

unichar s_ch(Str_c s, int pos)
{
  s_ok(s);
  if (s == NULL) {
    return UNI_INV;
  }
  if (pos < 0) {
  pos = s->nchars + pos + 1;
  }
  if (pos < 0 || pos >= s->nchars) {
  return UNI_INV;
  }
  byte *p = u8_avanca_unichar(s->dados, pos);
  unichar c;
  int bytes_restantes = s->nbytes - (p - s->dados);
  int nb = u8_unichar_nos_bytes(bytes_restantes, p, &c);
  assert(nb > 0);
  return c;
}

double s_número(Str_c s)
{
    double num;

    char *texto = s_strc(s);

    sscanf(texto, "%lf", &num);

    free(texto);

    return num;
}

// operações de busca e comparação {{{1

bool s_igual(Str_c s, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  if (s == NULL && sb == NULL) {
    return true;
  }
  if (s == NULL || sb == NULL) {
    return false;
  }
  if (s->nchars != sb->nchars) {
    return false;
  }
  if (s->nbytes != sb->nbytes) {
    return false;
  }
  if (s->nbytes == 0) {
  return true;
}
  if (memcmp(s->dados,sb->dados,s->nbytes) == 0) {
  return true;
}
return false;
}

int s_busca_c(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  if (s == NULL || sb == NULL) {
    return -1;
  }
  if ( pos < 0) {
    pos = s->nchars + pos + 1;
  }
  if (pos < 0) {
    pos = 0;
  }
  if (pos >= s->nchars) {
    return -1;
  }
  for (int i = pos; i < s->nchars; i++) {
    unichar c = s_ch(s, i);
    for (int j = 0; j < sb->nchars; j++) {
      unichar cb = s_ch(sb, j);
      if ( c == cb) {
        return i;
      }
    }
}
  return -1;
}

int s_busca_nc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  if (s == NULL) {
    return -1;
  }
  if (pos < 0) {
    pos = s->nchars + pos + 1;
  }
  if (pos < 0) {
    pos = 0;
  }
  if (pos >= s->nchars) {
    return -1;
  }
  if (sb == NULL || sb->nchars == 0) {
    return pos;
  }
  for (int i = pos; i < s->nchars; i++) {
    unichar c = s_ch(s, i);
    bool encontrou = false;
    for (int j = 0; j < sb->nchars; j++) {
      unichar cb = s_ch(sb, j);
      if (c == cb) {
        encontrou = true;
        break;
      }
    }
    if (!encontrou) {
      return i;
    }
  } 
  return -1;
}

int s_busca_rc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  if (s == NULL || sb == NULL) {
    return -1;
  }
  if (pos < 0) {
    pos = s->nchars + pos + 1;
  }
  if (pos > s->nchars) {
    pos = s->nchars;
  }
  if (pos <= 0) {
    return -1;
  }
  for (int i = pos - 1; i >= 0; i--) {
    unichar c = s_ch(s, i);
      for (int j = 0; j < sb->nchars; j++) {
        unichar cb = s_ch(sb, j);
        if ( c == cb) {
          return i;
        }
      }
  }
  return -1;
}

int s_busca_rnc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  if (s == NULL ) {
    return -1;
  }
  if (pos < 0) {
    pos = s->nchars + pos + 1;
  }
  if (pos > s->nchars) {
    pos = s->nchars;
  }
  if (pos<= 0) {
    return -1;
  }
  if (sb == NULL || sb->nchars == 0) {
    return pos - 1;
  }
  for (int i = pos - 1; i >= 0; i--) {
    unichar c = s_ch(s, i);
    bool encontrou = false;
    for (int j = 0; j < sb->nchars; j++) {
      unichar cb = s_ch(sb, j);
      if ( c == cb) {
        encontrou = true;
        break;
      }
    }
      if (!encontrou) {
        return i;
      }
  }
  return -1;
}

int s_busca_s(Str_c s, int pos, Str_c buscada)
{
  s_ok(s);
  s_ok(buscada);
  if (s == NULL && (buscada == NULL || buscada->nchars == 0)) {
    return 0;
  }
  if (s == NULL) {
    return -1;
  }
  if (pos < 0) {
    pos = s->nchars + pos + 1;
  }
  if (pos < 0) {
    pos = 0;
  }
  if (pos > s->nchars) {
    pos = s->nchars;
  }
  if (buscada == NULL || buscada->nchars == 0) {
    return pos;
  }
  if (buscada->nchars > s->nchars - pos) {
    return -1;
  }
  for (int i= pos; i <= s->nchars - buscada->nchars; i++) {
    bool igual = true;
    for (int j= 0; j < buscada->nchars; j++) {
      unichar i1=s_ch (s, i + j);
      unichar j1=s_ch(buscada, j);
      if (i1 != j1) {
        igual = false;
        break;
      }
    }
    if (igual) {
      return i;
    }
  }
  return -1;
}

// operações de alteração {{{1

void s_substitui(Str s, int pos, int tam, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  if (s == NULL) {
    return;
  }
  if (pos < 0) {
    pos = s->nchars + pos + 1;
  }
  if (pos < 0) {
    pos = 0;
  }
  if (pos > s->nchars) {
    pos = s->nchars;
  }
  if (tam < 0) {
    tam = s->nchars - pos;
  }
  if (tam > s->nchars - pos) {
    tam = s->nchars - pos;
  }
  
  int pos_byte;

  if (pos == s->nchars) {
    pos_byte = s->nbytes;
  } else {
  byte *p = u8_avanca_unichar(s->dados, pos);
  pos_byte = p - s->dados;
  }
  int fim_byte;

  if (pos + tam == s->nchars) {
    fim_byte = s->nbytes;
  } else {
  byte *p = u8_avanca_unichar(s->dados, pos + tam);
  fim_byte = p - s->dados;
  }
  int bytes_remover = fim_byte - pos_byte;
  int bytes_inserir;
  int chars_inserir;
   if (sb == NULL) {
     bytes_inserir = 0;
     chars_inserir = 0;
   } else {
    bytes_inserir = sb->nbytes;
    chars_inserir = sb->nchars;
   }
   int novo_nbytes = s->nbytes - bytes_remover + bytes_inserir;
   int novo_nchars = s->nchars - tam + chars_inserir;

   byte *temp = NULL;
   if (bytes_inserir > 0) {
     temp = malloc(bytes_inserir);
     assert(temp != NULL);
     memcpy(temp, sb->dados, bytes_inserir);
    }
     if (novo_nbytes == 0) {
     free(s->dados);
     s->dados = NULL;
     s->nbytes = 0;
     s->nchars = 0;
     s->alocado = 0;
     free(temp);
     s_ok(s);
      return;
    }
    int novo_alocado = MIN_ALLOC;
    while (novo_alocado < novo_nbytes) {
      novo_alocado = 2 * novo_alocado;
    }
    byte *novos_dados = malloc(novo_alocado);
    assert(novos_dados != NULL);
    if (pos_byte > 0) {
      memcpy(novos_dados, s->dados, pos_byte);
    }
    if (bytes_inserir > 0) {
      memcpy(novos_dados + pos_byte,
           temp,
           bytes_inserir);
    }
    if (s->nbytes - fim_byte > 0) {
      memcpy(novos_dados + pos_byte + bytes_inserir,
            s->dados + fim_byte,
            s->nbytes - fim_byte);
    }
    free(s->dados);
    s->dados = novos_dados;
    s->nbytes = novo_nbytes;
    s->nchars = novo_nchars;
    s->alocado = novo_alocado;
    free(temp);
    s_ok(s);
}

void s_substring(Str s, Str_c sb, int pos, int tam)
{
  s_ok(s);
  s_ok(sb);
  if (s == NULL) {
    return;
  }
  if (sb == NULL) {
    free (s->dados);
    s->dados = NULL;
    s->nbytes = 0;
    s->nchars = 0;
    s->alocado = 0;
    return;
  }
  if (pos < 0) {
    pos = sb->nchars + pos + 1;
  }
  if (pos < 0) {
    pos = 0;
  }
  if (pos > sb->nchars) {
    pos = sb->nchars;
  }
  if (tam < 0) {
    tam = sb->nchars - pos;
  }
  if (tam > sb->nchars - pos) {
    tam = sb->nchars - pos;
  }
  if (tam == 0) {
    free(s->dados);
    s->dados = NULL;
    s->nbytes = 0;
    s->nchars = 0;
    s->alocado = 0;
    return;
  }
  byte *inicio = u8_avanca_unichar(sb->dados, pos);
  byte *fim = u8_avanca_unichar(inicio, tam);

  int novo_nbytes = fim - inicio;
  byte *temporario = malloc(novo_nbytes);
  assert(temporario != NULL);
  memcpy(temporario, inicio, novo_nbytes);

  int novo_alocado = MIN_ALLOC;
  while (novo_alocado < novo_nbytes) {
    novo_alocado = 2 * novo_alocado;
  }
  byte *novo = realloc(s->dados, novo_alocado);
  assert(novo != NULL);
  s->dados = novo;
  s->alocado = novo_alocado;
  memcpy(s->dados, temporario, novo_nbytes);
  s->nbytes = novo_nbytes;
  s->nchars = tam;

  free(temporario);
  s_ok(s);
}

void s_copia(Str s, Str_c sb)
{
  s_substring(s, sb, 0, -1);
}

void s_insere(Str s, int pos, Str_c sb)
{
  s_substitui(s, pos, 0, sb);
}

void s_insere_c(Str s, int pos, unichar c)
{
  s_ok(s);
  if (s == NULL) {
    return;
  }

  int nbytes_c = u8_converte_pra_utf8(c, NULL);

  if (nbytes_c == -1) {
    return;
  }

  byte bytes_c[4];
  int nb = u8_converte_pra_utf8(c, bytes_c);
  assert(nb > 0);

  if (pos < 0) {
    pos = s->nchars + pos + 1;
  }
  if (pos < 0) {
    pos = 0;
  }
  if (pos > s->nchars) {
    pos = s->nchars;
  }
  
  int pos_byte;

  if (pos == s->nchars) {
    pos_byte = s->nbytes;
  } else {
    byte *p = u8_avanca_unichar(s->dados, pos);
    pos_byte = p - s->dados;
  }

  int novo_nbytes = s->nbytes + nbytes_c;

  if (novo_nbytes > s->alocado) {
    if (s->alocado == 0) {
      s->alocado = MIN_ALLOC;
    }

  while (s->alocado < novo_nbytes) {
    s->alocado = 2 * s->alocado;
  }

  byte *novo = realloc(s->dados, s->alocado);
    assert(novo != NULL);
    s->dados = novo;
  }

  memmove(
  s->dados + pos_byte + nbytes_c,
  s->dados + pos_byte,
  s->nbytes - pos_byte
);

memcpy(
  s->dados + pos_byte,
  bytes_c,
  nbytes_c
);

s->nbytes = novo_nbytes;
s->nchars++;

s_ok(s);
}

void s_anexa(Str s, Str_c sb)
{
  s_substitui(s, -1, 0, sb);
}

void s_anexa_c(Str s, unichar c)
{
  s_insere_c(s, -1, c);
}

void s_remove(Str s, int pos, int tam)
{
  s_substitui(s, pos, tam, NULL);
}

void s_apara(Str s, Str_c sobras)
{
  s_ok(s);
  s_ok(sobras);
  if (s == NULL) {
    return;
  }
  int inicio = s_busca_nc(s, 0, sobras);
  int fim = s_busca_rnc(s, -1, sobras);

  if (inicio == -1) {
    s_remove(s, 0, -1);
    return;
  }
  
  s_remove(s, fim + 1, -1);
  s_remove(s, 0, inicio);
}

// operações de E/S {{{1

void s_imprime(Str_c s)
{
  s_ok(s);
  if (s == NULL || s->nbytes == 0) {
    return;
  }
  fwrite(s->dados, 1, s->nbytes, stdout);
}

void s_grava_arquivo(Str_c s, char *nome)
{
  s_ok(s);
   if (nome == NULL) {
    return;
  }
  FILE *arq = fopen(nome, "wb");
   if (arq == NULL) {
    return;
  }
if (s != NULL && s->nbytes > 0) {
  fwrite(s->dados, 1, s->nbytes, arq);
}
  fclose(arq);
}

// vim: foldmethod=marker shiftwidth=2