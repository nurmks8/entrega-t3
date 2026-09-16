#include "calc.h"
#include <stdbool.h>


Str calculadora(Str expressão)
{
  Lista tokens = tokeniza(expressão);

  Lista operandos = l_cria();
  Lista operadores = l_cria();


  while (!l_vazia(tokens)) {
    Str token = l_remove_inicio(tokens);

    if (s_tam(token) == 1) {
      unichar c = s_ch(token, 0);

      if ((c == '+' || c == '-' || c == '*' || c == '/') ||
         (c == '^' || c == '(' || c == ')' || c == '=')) {

      unichar topo;
      if (l_vazia(operadores)) {
        topo = 'V';
      } else {
        Str token_topo = l_topo(operadores);
        topo = s_ch(token_topo, 0);
      }
      if (topo == 'V') {
        if (c == ')') {
      } else {
       l_empilha(operadores, token);
       continue;
      }
    }
    
      }
    }
  }
}


Lista tokeniza(Str txt)
{
   Lista tokens = l_cria();

   int pos = 0;
   int inicio = 0;
   Str token;

   while (pos < s_tam (txt)) {
     unichar c = s_ch(txt, pos);

   if (c == ' ' || c == '\t' || c == '\n') {
     pos++;
     continue;
   }

   if ((c >= '0' && c <= '9') || c == '.') {
     inicio = pos;

     while (pos < s_tam(txt) &&
             (((s_ch(txt, pos) >= '0') && (s_ch(txt, pos) <= '9')) ||
              s_ch(txt, pos) == '.')) {
      pos++;
     }
     token = s_cria_substring(txt, inicio, pos - inicio);
     l_insere_fim(tokens, token);

     continue;
    }

    if ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '_' ||
        c == '$') {
        inicio = pos;

        while (pos < s_tam(txt) &&
        (((s_ch(txt, pos) >= 'a') && (s_ch(txt, pos) <= 'z')) ||
         ((s_ch(txt, pos) >= 'A') && (s_ch(txt, pos) <= 'Z')) ||
         ((s_ch(txt, pos) >= '0') && (s_ch(txt, pos) <= '9')) ||
         s_ch(txt, pos) == '_' ||
         s_ch(txt, pos) == '$')) {
          pos++;
       }

     token = s_cria_substring(txt, inicio, pos - inicio);
     l_insere_fim(tokens, token);

     continue;
    }
   token = s_cria_substring(txt, pos, 1);
    l_insere_fim(tokens, token);
    pos++;
  }

  return tokens;
}

