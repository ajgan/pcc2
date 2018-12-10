Ferramenta ipmt de busca de padrões com uso de índices.

Implementada por Antonio Gadelha(ajgan) e Gabriel Melo(gvmgs) como Projeto 2 para a disciplina de Processamento de Cadeias de Caracteres(if767) do CIn-UFPE.

Antonio implementou os algoritmos de indexação por array de sufixos e de compressão por LZ77. Fez também a parte da leitura dos comandos.
Gabriel implementou os algoritmos de descompressão do LZ77 e de busca com.
Ambos contribuíram com a documentação e com os testes.

Para compilar o projeto, basta dar o comando:
make ipmt
dentro da pasta src. Após isso um executável é gerado e obedece ao seguinte comando:
./ipmt index [options] textfile
ou
./ipmt search [options] pattern indexfile
Um executável já foi previamente gerado pela equipa e se encontra dentro da pasta bin

As flags de opção implementadas foram:
-c ou --count
    Exibe contagem de ocorrências do padrão no texto(por default, não se exibe)
-p ou --pattern patternfile
    Indica que os padrões devem vir de um arquivo patternfile
-h ou --help
    Exibe informações básicas da ferramenta
-t ou --text
    Comprime o texto, sem indexar os sufixos
-s ou --suffix
    Indexa os sufixos, mas não comprime o texto
