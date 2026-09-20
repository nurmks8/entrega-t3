#include "calc.h"
#include "lista.h"
#include "str.h"

int main(void)
{
  Str texto = s_cria_de_arquivo("entrada.txt");
  Str quebra = s_cria("\n");

  Lista linhas = l_cria_separando(texto, quebra);
  Lista resultados = l_cria();

  while (!l_vazia(linhas)) {

    Str expressao = l_remove_inicio(linhas);

    Str resultado = calculadora(expressao);

    l_insere_fim(resultados, resultado);

    s_destroi(expressao);
  }

  Str saida = s_cria_unindo(resultados, quebra);

  s_grava_arquivo(saida, "saida.txt");

  while (!l_vazia(resultados)) {
    Str resultado = l_remove_inicio(resultados);
    s_destroi(resultado);
  }

  l_destroi(linhas);
  l_destroi(resultados);

  s_destroi(texto);
  s_destroi(quebra);
  s_destroi(saida);

  return 0;
}