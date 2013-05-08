Qualidade.C
=========

Verifica um programa em C existem erros de qualidade.

   Projeto desenvolvido de forma aberta, no espaço café de informática (2012)

   Contribuições de código:
   - José Coelho
   - António Dias

   É tido em conta o seguinte:
   - Identificar apenas a primeira ocorrência de cada erro.
   - Eliminar falsos positivos em strings e comentários
   - Eliminar macros.

   Erros identificados:
     #EQ1: funções com parâmetros no nome
     #EQ2: variáveis globais
     #EQ3: variáveis com nomes quase iguais
     #EQ4: nomes sem significado 
     #EQ5: goto 
     #EQ6: funções muito grandes 
     #EQ7: funções com muitos argumentos
     #EQ8: indentação variável
     #EQ9: código repetido seguido 
     #EQ10: código repetido não seguido 
     #EQ11: system 
     #EQ22: duas instruções na mesma linha 
     #EQ23: Ciclos contendo apenas uma variável lógica como teste de paragem
     #EQ26: nomes muito longos 

  Funções gerais:

    void AnaliseErrosQualidade(
        LinhaDeCodigo *lista,  
        LinhaDeCodigo *tipos, 
        ErroQualidade erros[MAX_ERROS]);
    void AnalisaFicheiro(char *nome);

  Erros de qualidade de código:
    void MostraErroQualidade(
        LinhaDeCodigo *lista,
        int numeroErro, 
        ErroQualidade erro);
    void UtilizaInstrucao(
        LinhaDeCodigo *lista, 
        ErroQualidade erros[MAX_ERROS], 
        char *instrucao, 
        int erro)
    void ElementosGlobais(
        LinhaDeCodigo *lista,
        ErroQualidade erros[MAX_ERROS],
        LinhaDeCodigo *tipos);
    void NomesQuaseIguais(
        LinhaDeCodigo *variaveis, 
        ErroQualidade erros[MAX_ERROS]);
    void NomesMuitoLongos(
        LinhaDeCodigo *variaveis, 
        ErroQualidade erros[MAX_ERROS]);
    LinhaDeCodigo *AdicionarVariaveisLocais(
        LinhaDeCodigo *lista,
        int i, 
        LinhaDeCodigo **variaveis,
        int *chavetas);
    LinhaDeCodigo *ExtraiTestesDeParagem(
        LinhaDeCodigo *lista,
        LinhaDeCodigo *testesDeParagem);
    int NomeSemSignificado(
        char *texto,
        LinhaDeCodigo *testesDeParagem,
        Palavra **dicionario);
    void ElementosLocais(
        LinhaDeCodigo *lista, 
        ErroQualidade erros[MAX_ERROS],
        LinhaDeCodigo *tipos);
    void DuasInstrucoes(
        LinhaDeCodigo *lista, 
        ErroQualidade erros[MAX_ERROS]);
    void VerificaIndentacao(
        LinhaDeCodigo *lista, 
        LinhaDeCodigo *original, 
        ErroQualidade erros[MAX_ERROS]);
    void VerificaCodigoRepetido(
        LinhaDeCodigo *lista,
        ErroQualidade erros[MAX_ERROS]);


  Funções auxiliares no processamento de código C:

    int CaracterValido(char caracter);
    void EspacosCodigo(LinhaDeCodigo *lista);
    int Indentacao(char *texto, int tabSize);
    void RemoverStrings(LinhaDeCodigo *lista);
    void JuntarExpressoesInstrucoes(LinhaDeCodigo *lista);
    void AdicionarBlocos(LinhaDeCodigo *lista);
    void SepararInstrucoes(LinhaDeCodigo *lista);
    LinhaDeCodigo *RemoverLinhasVazias(LinhaDeCodigo *lista);
    LinhaDeCodigo *TiposValidos(LinhaDeCodigo *lista);
    int DeclaracaoDeVariavel(char *texto,LinhaDeCodigo *tipos);
    int ContaChavetas(char *texto, int parentisis);
    int PrefixoIgual(char *nome1, char *nome2);
    int ComecaCom(char *nome1, char *nome2);
    int Instrucao(char *texto);


  Funções da estrutura LinhaDeCodigo:
    
    LinhaDeCodigo *LCAdiciona(LinhaDeCodigo *lista, char*texto, int linha);
    LinhaDeCodigo *LCRemove(LinhaDeCodigo *lista);
    LinhaDeCodigo *LCLiberta(LinhaDeCodigo *lista);
    LinhaDeCodigo *LCInverte(LinhaDeCodigo *lista);
    LinhaDeCodigo *LCCopia(LinhaDeCodigo *lista);
    LinhaDeCodigo *LCPosicao(LinhaDeCodigo *lista,int i);

