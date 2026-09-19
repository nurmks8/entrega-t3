#include "calc.h"
#include <stdbool.h>
#include <stdlib.h>
#include "dicionario.h"

static bool chave_igual (chave_t a, chave_t b)
{
  return s_igual((Str)a, (Str)b);
}

static bool chave_menor (chave_t a, chave_t b)
{
  Str sa = (Str)a;
  Str sb = (Str)b;
  int tam_a = s_tam(sa);
  int tam_b = s_tam(sb);
  int min_tam = tam_a < tam_b ? tam_a : tam_b;

  for (int i = 0; i < min_tam; i++) {
    unichar ca = s_ch(sa, i);
    unichar cb = s_ch(sb, i);
    if (ca < cb) {
      return true;
    } else if (ca > cb) {
      return false;
    }
  }
  return tam_a < tam_b;
}

static Dicionário variaveis = NULL;

Lista tokeniza(Str txt)
{
  Lista tokens = l_cria();

  int pos = 0;
  int inicio = 0;
  Str token;

  while (pos < s_tam(txt)) {
    unichar c = s_ch(txt, pos);

    if (c == ' ' || c == '\t' || c == '\n') {
      pos++;
      continue;
    }

    if ((c >= '0' && c <= '9') || c == '.') {
      inicio = pos;

      while (pos < s_tam(txt) &&
             (((s_ch(txt, pos) >= '0') &&
               (s_ch(txt, pos) <= '9')) ||
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
             (((s_ch(txt, pos) >= 'a') &&
               (s_ch(txt, pos) <= 'z')) ||

              ((s_ch(txt, pos) >= 'A') &&
               (s_ch(txt, pos) <= 'Z')) ||

              ((s_ch(txt, pos) >= '0') &&
               (s_ch(txt, pos) <= '9')) ||

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


Str calculadora(Str expressao)
{
  Lista tokens = tokeniza(expressao);
  Lista operandos = l_cria();
  Lista operadores = l_cria();
  Str erro = NULL;

  if (variaveis == NULL) {
    variaveis = dic_cria(chave_menor, chave_igual);
  }

  while (!l_vazia(tokens)) {
    Str token = l_remove_inicio(tokens);
    if (s_tam(token) == 1) {
      unichar c = s_ch(token, 0);
      if ((c == '+' || c == '-' || c == '*' || c == '/') ||
          (c == '^' || c == '(' || c == ')' || c == '=')) {

        int processado = 0;

        while (!processado) {
          unichar topo;
          if (l_vazia(operadores)) {
            topo = 'V';
          } else {
            Str token_topo = l_topo(operadores);
            topo = s_ch(token_topo, 0);
          }
          if (topo == 'V') {
            if (c == ')') {
              erro = s_cria("#ERRO Parêntese não aberto.");
              break;
            } else {
              l_empilha(operadores, token);
              processado = 1;
            }
          }
          else if (topo == '+' || topo == '-') {
            if (c == '*' || c == '/' || c == '^' || c == '(') {
              l_empilha(operadores, token);
              processado = 1;
            }
            else if (c == '+' || c == '-' || c == ')') {
              Str op = l_desempilha(operadores);
              unichar op_c = s_ch(op, 0);
              if (l_tam(operandos) < 2) {
                erro = s_cria("#ERRO Operando insuficiente para operador.");
                break;
              }
              Str direita = l_desempilha(operandos);
              Str esquerda = l_desempilha(operandos);

              double numero_esquerda = s_número(esquerda);
              double numero_direita = s_número(direita);

              if (op_c == '+') {
                double resultado = numero_esquerda + numero_direita;
                Str resultado_str = s_cria_número(resultado);
                l_empilha(operandos, resultado_str);

              } else if (op_c == '-') {
                double resultado = numero_esquerda - numero_direita;
                Str resultado_str = s_cria_número(resultado);
                l_empilha(operandos, resultado_str);

              } else if (op_c == '*') {
                double resultado = numero_esquerda * numero_direita;
                Str resultado_str = s_cria_número(resultado);
                l_empilha(operandos, resultado_str);

              } else if (op_c == '/') {
                if (numero_direita == 0) {
                  erro = s_cria("#ERRO Divisão por zero.");
                  break;
                }
                double resultado = numero_esquerda / numero_direita;
                Str resultado_str = s_cria_número(resultado);
                l_empilha(operandos, resultado_str);

              } else if (op_c == '^') {
                double resultado = 1;
                for (int i = 0; i < numero_direita; i++) {
                  resultado = resultado * numero_esquerda;
                }

                Str resultado_str = s_cria_número(resultado);
                l_empilha(operandos, resultado_str);
              }
              s_destroi(esquerda);
              s_destroi(direita);
              s_destroi(op);
            }
          }
          else if (topo == '*' || topo == '/') {
            if (c == '^' || c == '(') {
              l_empilha(operadores, token);
              processado = 1;

            } else if (c == '+' || c == '-' ||
                       c == '*' || c == '/' || c == ')') {
              Str op = l_desempilha(operadores);
              unichar op_c = s_ch(op, 0);
              if (l_tam(operandos) < 2) {
                erro = s_cria("#ERRO Operando insuficiente para operador.");
                break;
              }

              Str direita = l_desempilha(operandos);
              Str esquerda = l_desempilha(operandos);

              double numero_esquerda = s_número(esquerda);
              double numero_direita = s_número(direita);

              if (op_c == '*') {
                double resultado = numero_esquerda * numero_direita;
                Str resultado_str =
                s_cria_número(resultado);
                l_empilha(operandos, resultado_str);

              } else if (op_c == '/') {
                if (numero_direita == 0) {
                  erro = s_cria("#ERRO Divisão por zero.");
                  break;
                }
                double resultado = numero_esquerda / numero_direita;
                Str resultado_str = s_cria_número(resultado);
                l_empilha(operandos, resultado_str);
              }
              s_destroi(esquerda);
              s_destroi(direita);
              s_destroi(op);
            }
          }
          else if (topo == '^') {
            if (c == '^' || c == '(') {
              l_empilha(operadores, token);
              processado = 1;

            } else if (c == '+' || c == '-' ||
                       c == '*' || c == '/' ||
                       c == ')') {
              Str op = l_desempilha(operadores);
              unichar op_c = s_ch(op, 0);
              if (l_tam(operandos) < 2) {
                erro = s_cria("#ERRO Operando insuficiente para operador.");
                break;
              }

              Str direita = l_desempilha(operandos);
              Str esquerda = l_desempilha(operandos);

              double numero_esquerda = s_número(esquerda);
              double numero_direita = s_número(direita);

              if (op_c == '^') {
                double resultado = 1;
                for (int i = 0; i < numero_direita; i++) {
                  resultado = resultado * numero_esquerda;
                }
                Str resultado_str = s_cria_número(resultado);
                l_empilha(operandos, resultado_str);
              }

              s_destroi(esquerda);
              s_destroi(direita);
              s_destroi(op);
            }
          }
          else if (topo == '(') {
            if (c == ')') {
              Str abre = l_desempilha(operadores);
              s_destroi(abre);
              s_destroi(token);
              processado = 1;
            } else {
              l_empilha(operadores, token);
              processado = 1;
            }
          }
          if (erro != NULL) {
            break;
          }
        }
        continue;
      }

    }
    l_empilha(operandos, token);
  }
  while (!l_vazia(operadores)) {
    Str op = l_desempilha(operadores);
    unichar op_c = s_ch(op, 0);
    if (op_c == '(') {
      erro = s_cria("#ERRO Parêntese não fechado.");
      s_destroi(op);
      break;
    }

    if (l_tam(operandos) < 2) {
      erro = s_cria("#ERRO Operando insuficiente para operador.");
      s_destroi(op);
      break;
    }

    Str direita = l_desempilha(operandos);
    Str esquerda = l_desempilha(operandos);

    double numero_esquerda = s_número(esquerda);
    double numero_direita = s_número(direita);

    if (op_c == '+') {
      double resultado = numero_esquerda + numero_direita;
      Str resultado_str = s_cria_número(resultado);
      l_empilha(operandos, resultado_str);

    } else if (op_c == '-') {
      double resultado = numero_esquerda - numero_direita;
      Str resultado_str = s_cria_número(resultado);
      l_empilha(operandos, resultado_str);

    } else if (op_c == '*') {
      double resultado = numero_esquerda * numero_direita;
      Str resultado_str = s_cria_número(resultado);
      l_empilha(operandos, resultado_str);

    } else if (op_c == '/') {
      if (numero_direita == 0) {
        erro = s_cria("#ERRO Divisão por zero.");
        s_destroi(op);
        s_destroi(esquerda);
        s_destroi(direita);
        break;
      }
      double resultado = numero_esquerda / numero_direita;
      Str resultado_str = s_cria_número(resultado);
      l_empilha(operandos, resultado_str);

    } else if (op_c == '^') {
      double resultado = 1;
      for (int i = 0; i < numero_direita; i++) {
        resultado = resultado * numero_esquerda;
      }
      Str resultado_str = s_cria_número(resultado);
      l_empilha(operandos, resultado_str);
    }
    s_destroi(esquerda);
    s_destroi(direita);
    s_destroi(op);
  }
  if (erro == NULL) {
    while (!l_vazia(operadores)) {
      Str op = l_desempilha(operadores);
      unichar op_c = s_ch(op, 0);
      if (op_c == '(') {
        erro = s_cria("#ERRO Parêntese não fechado.");
        s_destroi(op);
        break;
      }
      if (l_tam(operandos) < 2) {
        erro = s_cria("#ERRO Operando insuficiente para operador.");
        s_destroi(op);
        break;
      }

      Str direita = l_desempilha(operandos);
      Str esquerda = l_desempilha(operandos);

      double numero_esquerda = s_número(esquerda);
      double numero_direita = s_número(direita);


      if (op_c == '+') {
        double resultado = numero_esquerda + numero_direita;
        Str resultado_str = s_cria_número(resultado);
        l_empilha(operandos, resultado_str);

      } else if (op_c == '-') {
        double resultado = numero_esquerda - numero_direita;
        Str resultado_str = s_cria_número(resultado);
        l_empilha(operandos, resultado_str);

      } else if (op_c == '*') {
        double resultado = numero_esquerda * numero_direita;
        Str resultado_str = s_cria_número(resultado);
        l_empilha(operandos, resultado_str);

      } else if (op_c == '/') {
        if (numero_direita == 0) {
          erro = s_cria("#ERRO Divisão por zero.");
          s_destroi(op);
          s_destroi(esquerda);
          s_destroi(direita);
          break;
        }
        double resultado = numero_esquerda / numero_direita;
        Str resultado_str = s_cria_número(resultado);
        l_empilha(operandos, resultado_str);

      } else if (op_c == '^') {
        double resultado = 1;
        for (int i = 0; i < numero_direita; i++) {
          resultado = resultado * numero_esquerda;
        }
        Str resultado_str = s_cria_número(resultado);
        l_empilha(operandos, resultado_str);
      }
      s_destroi(esquerda);
      s_destroi(direita);
      s_destroi(op);
    }
  }
  if (erro == NULL && l_tam(operandos) != 1) {
    erro = s_cria("#ERRO Expressão inválida.");
  }
  if (erro != NULL) {
    while (!l_vazia(operandos)) {
      Str s = l_desempilha(operandos);
      s_destroi(s);
    }
    while (!l_vazia(operadores)) {
      Str s = l_desempilha(operadores);
      s_destroi(s);
    }
    while (!l_vazia(tokens)) {
      Str s = l_desempilha(tokens);
      s_destroi(s);
    }
    l_destroi(operandos);
    l_destroi(operadores);
    l_destroi(tokens);
    return erro;
  }
  Str resultado_final = l_desempilha(operandos);
  l_destroi(operandos);
  l_destroi(operadores);
  l_destroi(tokens);
  return resultado_final;
}