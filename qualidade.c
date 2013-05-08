/* Verifica um programa em C existem erros de qualidade.

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

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// carregar função ProgramasTeste, com a lista de e-fólios entregues
//#include "efolios.c"

#define MAX_STR 1024

/*******************************************************************************
 * Dicionário:
 *   a utilizar pela função NomeSemSignificado()
 * 
 * Estrutura Palavra:
 *   estrutura para armazenamento de uma palavra do dicionario
 * 
 * Funções relacionadas:
 *   unsigned Hash(char *palavra)
 *   Palavra **CriaDicionario()
 *   void EliminaDicionario(Palavra **dicionario)
 *   int DPalavraExiste(Palavra **dicionario, char *palavra)
 *   int DInserePalavra(Palavra **dicionario, char *palavra)
 *   int DRemovePalavra(Palavra **dicionario, char *palavra)
 *   int Valida(char *entrada, char *palavra)
 *   int LeDicionario(Palavra **dicionario, char* nomeFicheiro)
 *   int EscreveDicionario(Palavra **dicionario, char *nomeFicheiro)
 */

/* Erros em runtime */
#define ERR_FICHEIRO     -1       /* erro no acesso a um ficheiro      */
#define ERR_MEMORIA      -2       /* erro na alocação de memória       */
#define ERR_INEXISTENTE   1       /* palavra a remover não encontrada  */

#define TAM_DICIONARIO 65521      /* tamanho da tabela de dispersão
                                   * maior primo menor que 2^16        */


typedef struct SPalavra {
    char *palavra;
    struct SPalavra *seguinte;
} Palavra;

/*
 * Função de dispersão, devolve número entre 0 e TAM_DICIONARIO,
 * Método CRC Variant,
 * referência (consultada em 2012/Ago.11): http://tinyurl.com/dg3kp4
 */
unsigned Hash(char *palavra)
{
    unsigned bitsMaisSignificantes, hash = 0;
    for (; *palavra; palavra++) {
        bitsMaisSignificantes = hash & 0xf8000000;
        hash <<= 5;
        hash ^= bitsMaisSignificantes >> 27;
        hash ^= *palavra;
    }
    
    return hash % TAM_DICIONARIO;
}

Palavra **CriaDicionario()
{
    return (Palavra**) calloc(TAM_DICIONARIO, sizeof(Palavra*));
}

void EliminaDicionario(Palavra **dicionario)
{
    int i = TAM_DICIONARIO;
    Palavra *sPalavra, *auxiliar;
    
    while (i--) {
        sPalavra = dicionario[i];
        while (sPalavra!=NULL) {
            auxiliar = sPalavra;
            free(sPalavra->palavra);
            sPalavra = sPalavra->seguinte;
            free(auxiliar);
        }
    }
    free(dicionario);
}

int DPalavraExiste(Palavra **dicionario, char *palavra)
{
    Palavra *sPalavra;

    for (sPalavra = dicionario[Hash(palavra)]; sPalavra!=NULL; sPalavra = sPalavra->seguinte) 
        if (!strcmp(sPalavra->palavra, palavra))
            break;

    // caso a palavra não exista, e estiver no plural, tentar a versão no singular
    if(sPalavra==NULL && palavra[strlen(palavra)-1]=='s')
    {
        palavra[strlen(palavra)-1]='\0';
        return DPalavraExiste(dicionario,palavra);
    }
    
    return sPalavra != NULL;
}

int DInserePalavra(Palavra **dicionario, char *palavra)
{
    Palavra *sPalavra = NULL;
    unsigned hash = Hash(palavra);

    if (!DPalavraExiste(dicionario, palavra)) {
        if (!(sPalavra = (Palavra*) malloc(sizeof(Palavra))))
            return ERR_MEMORIA;
        if (!(sPalavra->palavra = strdup(palavra)))
            return ERR_MEMORIA;
        sPalavra->seguinte = dicionario[hash];
        dicionario[hash] = sPalavra;
    }

    return sPalavra == NULL;
}

int DRemovePalavra(Palavra **dicionario, char *palavra)
{
    Palavra *sPalavra, *anterior = NULL;
    unsigned hash = Hash(palavra);

    for (sPalavra = dicionario[hash]; sPalavra!=NULL; sPalavra = sPalavra->seguinte) {
        if (strcmp(sPalavra->palavra, palavra) == 0) {
            if (anterior!=NULL)
                anterior->seguinte = sPalavra->seguinte;
            else
                dicionario[hash] = sPalavra->seguinte;
            free(sPalavra->palavra);
            free(sPalavra);
            return 0;
        }
        anterior = sPalavra;
    }
    
    return ERR_INEXISTENTE;
}

int Valida(char *entrada, char *palavra)
{
    int controlo = 0, tamanho = 0;
    static unsigned char buffer[MAX_STR];
    static unsigned char *p;
    
    if (entrada) {
        strcpy((char*) buffer, entrada);
        p = buffer;
    }
    
    while (*p) {
        if (controlo) {
            switch (*p) {
                /* À à Á á Â â Ã ã */
                case 0x80: case 0xa0: case 0x81: case 0xa1:
                case 0x82: case 0xa2: case 0x83: case 0xa3:
                palavra[tamanho++] = 'a';
                break;

                /* É é Ê ê */
                case 0x89: case 0xa9: case 0x8a: case 0xaa:
                palavra[tamanho++] = 'e';
                break;

                /* Í í */
                case 0x8d: case 0xad:
                palavra[tamanho++] = 'i';
                break;

                /* Ó ó Ô ô Õ õ */
                case 0x93: case 0xb3: case 0x94:
                case 0xb4: case 0x95: case 0xb5:
                palavra[tamanho++] = 'o';
                break;

                /* Ú ú */
                case 0x9a: case 0xba:
                palavra[tamanho++] = 'u';
                break;

                /* Ç ç */
                case 0x87: case 0xa7:
                palavra[tamanho++] = 'c';
                break;

                default:
                palavra[tamanho] = '\0';
                p++;
                return tamanho;
            }
            controlo = 0;
        } else if ('A' <= *p && *p <= 'Z') {
            palavra[tamanho++] = *p + 0x20;
        } else if ('a' <= *p && *p <= 'z') {
            palavra[tamanho++] = *p;
        } else if (*p == 0xc3) {
            controlo = 1;
        } else {
            palavra[tamanho] = '\0';
            p++;
            return tamanho;
        }
        
        p++;
    }
    
    return EOF;
}

int LeDicionario(Palavra **dicionario, char* nomeFicheiro)
{
    FILE *f;
    char palavra[MAX_STR];
    char auxiliar[MAX_STR];
    int v, erro;
    
    if (!(f = fopen(nomeFicheiro, "r")))
        return ERR_FICHEIRO;
    
    while (fgets(auxiliar, MAX_STR, f)) {
        if (strlen(auxiliar) < MAX_STR)
            for (v = Valida(auxiliar, palavra); v >= 0;
                v = Valida(NULL, palavra)) {
                if (v > 2) {
                    if ((erro = DInserePalavra(dicionario, palavra) < 0))
                        return erro;
                }
            }
        else
            puts("Aviso: linha demasiado longa ignorada ao ler o dicionário.");
    }
    
    return 0;
}

int EscreveDicionario(Palavra **dicionario, char *nomeFicheiro)
{
    FILE *f;
    Palavra *sPalavra;
    int i = TAM_DICIONARIO;
    
    if (!(f = fopen(nomeFicheiro, "w")))
        return ERR_FICHEIRO;

    while (i--) {
        sPalavra = dicionario[i];
        while (sPalavra!=NULL) {
            fprintf(f, "%s\n", sPalavra->palavra);
            sPalavra = sPalavra->seguinte;
        }
    }
    
    return 0;
}


/************************************************************************** 
   ErroQualidade: 
   estrutura para registo de erros de qualidade.
   como apenas se regista um erro de cada tipo, 
   não há necessidade de se registar o número do erro 
*/
typedef struct
{
    int linha; /* referência ao local onde o erro ocorre, 0 para não ocorrência */
    int auxiliar; /* referência a uma segunda linha auxiliar, se aplicável */
    int valor; /* quantidade associada ao erro, ou uma segunda linha, se aplicável */
} ErroQualidade;

/* o número de máximo de erros, é um valor acima do real (o erro 0 não conta) */
#define MAX_ERROS 32

/************************************************************************** 
   LinhaDeCodigo: 

   estrutura lista reutilizado de uma AF, 
   adicionando número da linha no ficheiro 
   
   Funções:
   LinhaDeCodigo *LCAdiciona(LinhaDeCodigo *lista, char*texto, int linha);
   LinhaDeCodigo *LCRemove(LinhaDeCodigo *lista);
   LinhaDeCodigo *LCLiberta(LinhaDeCodigo *lista);
   LinhaDeCodigo *LCInverte(LinhaDeCodigo *lista);
   LinhaDeCodigo *LCCopia(LinhaDeCodigo *lista);
   LinhaDeCodigo *LCPosicao(LinhaDeCodigo *lista,int i);
*/

typedef struct SLinhaDeCodigo
{
    char *texto;
    int linha;
    struct SLinhaDeCodigo *seguinte;
} LinhaDeCodigo;

LinhaDeCodigo *LCAdiciona(LinhaDeCodigo *lista, char*texto, int linha)
{
    LinhaDeCodigo *elemento=(LinhaDeCodigo*)malloc(sizeof(LinhaDeCodigo));
    if(elemento!=NULL) {
        elemento->texto=(char*)malloc(strlen(texto)+1);
        strcpy(elemento->texto,texto);
        elemento->linha=linha;
        elemento->seguinte=lista;
    }
    /* em caso de falha, retorna NULL */
    return elemento;
}

LinhaDeCodigo *LCRemove(LinhaDeCodigo *lista)
{
    LinhaDeCodigo *auxiliar;
    if(lista!=NULL) 
    {
        auxiliar=lista->seguinte;
        free(lista->texto);
        free(lista);
        return auxiliar;
    }
    return NULL;
}

LinhaDeCodigo *LCLiberta(LinhaDeCodigo *lista)
{
    while(lista!=NULL)
        lista=LCRemove(lista);
    return lista;
}

LinhaDeCodigo *LCInverte(LinhaDeCodigo *lista)
{
    LinhaDeCodigo *auxiliar=NULL,*novaLista=NULL;
    while(lista!=NULL) 
    {
        auxiliar=novaLista; // cópia do início da lista já invertida
        novaLista=lista; // primeiro elemento da novaLista passa a ser o que está em lista
        lista=lista->seguinte; // remover o primeiro elemento da lista
        novaLista->seguinte=auxiliar; // atualizar o resto da lista de novaLista
    }
    return novaLista;
}

LinhaDeCodigo *LCCopia(LinhaDeCodigo *lista)
{
    LinhaDeCodigo *auxiliar=NULL;
    while(lista!=NULL) 
    {
        auxiliar=LCAdiciona(auxiliar,lista->texto,lista->linha);
        lista=lista->seguinte;
    }
    /* deixar a cópia na mesma ordem */
    return LCInverte(auxiliar);
}

LinhaDeCodigo *LCPosicao(LinhaDeCodigo *lista, int i)
{
    while(lista!=NULL && i>0) 
    {
        lista=lista->seguinte;
        i--;
    }
    return lista;
}


/************************************************************************** 
    Funções auxiliares no processamento de código C:
    int CaracterValido(char caracter);
    void EspacosCodigo(LinhaDeCodigo *lista);
    int Indentacao(char *texto, int tabSize);
    void RemoverStrings(LinhaDeCodigo *lista);
    void JuntarExpressoesInstrucoes(LinhaDeCodigo *lista);
    void AdicionarBlocos(LinhaDeCodigo *lista);
    void SepararInstrucoes(LinhaDeCodigo *lista);
    int LinhaVazia(char *texto);
    LinhaDeCodigo *RemoverLinhasVazias(LinhaDeCodigo *lista);
    LinhaDeCodigo *TiposValidos(LinhaDeCodigo *lista);
    int DeclaracaoDeVariavel(char *texto,LinhaDeCodigo *tipos);
    int ContaChavetas(char *texto, int parentisis);
    int PrefixoIgual(char *nome1, char *nome2);
    int ComecaCom(char *nome1, char *nome2);
    int Instrucao(char *texto);
*/

/* verifica se o caracter é válido no nome de uma função/variável */
int CaracterValido(char caracter)
{
    return isalnum(caracter) || caracter=='_';
}

/* substituir espaços brancos por espaços, e duplos espaços por um só espaço */
void EspacosCodigo(LinhaDeCodigo *lista)
{
    char *espacos="\n\r\t", *ocorrencia;
    int i,caracteres;

    while(lista!=NULL)
    {
        /* colocar todos os caracteres brancos em espaços */
        for(i=0;espacos[i]!='\0';i++)
            while((ocorrencia=strchr(lista->texto,espacos[i]))!=NULL)
                *ocorrencia=' ';

        /* colocar todos os duplos espaços como um só espaço
           (remover também espaços desnecessários:
             no início/fim mantendo apenas os separadores de identificadores,
             que separam dois nomes válidos) */
        for(i=0, caracteres=0; lista->texto[i]!='\0'; i++)
            if(lista->texto[i]!=' ' || 
                caracteres>0 && 
                CaracterValido(lista->texto[caracteres-1]) && 
                CaracterValido(lista->texto[i+1]))
                lista->texto[caracteres++]=lista->texto[i];
        lista->texto[caracteres]='\0';

        lista=lista->seguinte;
    }
}

/* retorna a indentação, considerando tabs com um determinado valor */
int Indentacao(char *texto, int tabSize)
{
    int valor=0;
    while(*texto==' ' || *texto=='\t')
    {
        if(*texto==' ')
            valor++;
        else if(*texto=='\t')
            valor+=tabSize;
        texto++;
    }
    return valor;
}

/* reutilizado de uma AF, remove o conteúdo de strings no código */
void RemoverStrings(LinhaDeCodigo *lista)
{
    char *ocorrencia,tipo=' ',ultimoCaracter=' ',duplaBarra=' ';
    LinhaDeCodigo *topoLista=lista;

    /* remoção de comentários e strings, antes de remover macros */
    while(lista!=NULL) 
    {
        ocorrencia=lista->texto;
        while(*ocorrencia!='\0') 
        {
            /* verificar se há início de string entre 
               aspas / plicas / comentários */
            if(tipo!=' ' || ocorrencia[0]=='\'' || ocorrencia[0]=='"' || 
                ocorrencia[0]=='/' && (ocorrencia[1]=='*' || ocorrencia[1]=='/') ) 
            {
                if(tipo==' ') 
                {
                    tipo=ocorrencia[0];
                    ocorrencia++;
                    /* distinguir entre os dois tipos de comentários */
                    if(tipo=='/' && ocorrencia[0]=='/') 
                        tipo='\0'; // termina com o fim da linha 
                }

                while(*ocorrencia!='\0' && 
                    (ocorrencia[0]!=tipo ||
                    tipo=='/' && ultimoCaracter!='*' ||
                    (*ocorrencia=='\'' || *ocorrencia=='"') && 
                    ultimoCaracter=='\\' && duplaBarra!='\\')) 
                {
                    duplaBarra=ultimoCaracter;
                    ultimoCaracter=ocorrencia[0];
                    /* o conteúdo é anulado e colocado um espaço, 
                       de forma a não influenciar o indicador */
                    *ocorrencia=' '; 
                    ocorrencia++;
                }
                if(*ocorrencia!='\0' || tipo=='\0') 
                {
                    if(tipo!='\0')
                        ocorrencia++;
                    tipo=' ';
                }
            } else 
                ocorrencia++;
        }
        lista=lista->seguinte;
    }

    /* efectuar uma segunda passagem para remover as macros */
    lista=topoLista;
    while(lista!=NULL) 
    {
        /* apagar as macros */
        if(lista->texto[0]=='#') 
        {
            ocorrencia=(char*)malloc(1);
            *ocorrencia='\0';
            free(lista->texto);
            lista->texto=ocorrencia;
        }
        lista=lista->seguinte;
    }

}

/* a parte inicial de nome1, tem de ser igual a nome2, não podendo estar incompleto, 
   c.c. "do" faria match com "double" */
int ComecaCom(char *nome1, char *nome2)
{
    while(*nome1 == *nome2 && *nome1!='\0' && *nome2!='\0')
    {
        nome1++;
        nome2++;
    }
    return *nome2=='\0';
}

/* função auxiliar a SepararInstrucoes */

void SIAdiciona(LinhaDeCodigo*auxiliar, char*ocorrencia, int i)
{
    /* cria uma nova linha a começar em ocorrência, 
        mas mantém o mesmo número de linha de código */
    auxiliar->seguinte=LCAdiciona(auxiliar->seguinte,ocorrencia+i,auxiliar->linha);
    /* na string desta linha, acabar em ocorrência */
    *(ocorrencia+i)='\0';
}

/* separar instruções nas mesmas linhas */
void SepararInstrucoes(LinhaDeCodigo *lista)
{
    char *ocorrencia, *verificarCast, *inicio="{}", *fim="{};";
    int i;
    LinhaDeCodigo *auxiliar;

    /* localizar caracteres que devem estar no início/fim das linhas */
    auxiliar=lista;
    while(auxiliar!=NULL)
    {
        for(i=0;inicio[i]!='\0';i++)
            if((ocorrencia=strchr(auxiliar->texto,inicio[i]))!=NULL &&
                ocorrencia!=auxiliar->texto)
                SIAdiciona(auxiliar,ocorrencia,0);

        for(i=0;fim[i]!='\0';i++)
            if((ocorrencia=strchr(auxiliar->texto,fim[i]))!=NULL &&
                *(ocorrencia+1)!='\0')
                /* identico à linha anterior, mas o corte é após o caracter */
                SIAdiciona(auxiliar,ocorrencia,1);
 
        /* após um parêntisis curvo a fechar, se ocorrer um caracter válido
           para um nome, então mudar de linha: if(i!=3)printf(""); 
           ou então um comentário de fim de linha:
           "while(...)/" ou "while(...)//"
           Pode também ser um cast, "(int)valor" dificultando, pelo que nesse
           caso não cortar a linha */
        ocorrencia=auxiliar->texto;
        while((ocorrencia=strchr(ocorrencia,')'))!=NULL)
        {
            verificarCast=ocorrencia-1;
            while(CaracterValido(*verificarCast) && verificarCast>auxiliar->texto)
                verificarCast--;
            if(*verificarCast!='(' &&
                (CaracterValido(*(ocorrencia+1)) ||
                *(ocorrencia+1)=='/' && (
                *(ocorrencia+2)=='/' || *(ocorrencia+2)=='\0')))
            {
                SIAdiciona(auxiliar,ocorrencia,1);
                break;
            } else /* caracter seguinte não é um nome, manter na mesma linha */
                ocorrencia++;
        }

        /* uma instrução após else, sem ser o else if, tem também de ser quebrada */
        ocorrencia=auxiliar->texto;
        while((ocorrencia=strstr(ocorrencia,"else"))!=NULL)
        {
            if(!ComecaCom(ocorrencia+4," if") && strlen(ocorrencia)>4)
            {
                /* um else sem um if */
                auxiliar->seguinte=LCAdiciona(auxiliar->seguinte,ocorrencia+5,auxiliar->linha);
                *(ocorrencia+4)='\0';
                break;
            } else
                ocorrencia++;
        }

        auxiliar=auxiliar->seguinte;
    }
}

/* linha vazia (após processada) */
int LinhaVazia(char *texto)
{
    return strlen(texto)==0 ||
        strcmp(texto,"/")==0 ||
        strcmp(texto,"//")==0;
}

/* Remover linhas vazias */
LinhaDeCodigo *RemoverLinhasVazias(LinhaDeCodigo *lista)
{
    LinhaDeCodigo *auxiliar;
    /* remover linhas iniciais vazias */
    while(lista!=NULL && LinhaVazia(lista->texto))
        lista=LCRemove(lista);
    auxiliar=lista;
    /* remover restantes linhas vazias */
    while(lista!=NULL)
    {
        while(lista->seguinte!=NULL && LinhaVazia(lista->seguinte->texto))
            lista->seguinte=LCRemove(lista->seguinte);
        lista=lista->seguinte;
    }

    return auxiliar;
}

/* calcula saldo do número de chavetas/parêntisis em "texto" */
int ContaChavetas(char *texto, int parentisis)
{
    char *ocorrencia;
    int saldo=0;
    /* contabilizar chavetas */
    for(ocorrencia=texto;(ocorrencia=strchr(ocorrencia,'{'))!=NULL;ocorrencia++)
        saldo++;

    for(ocorrencia=texto;(ocorrencia=strchr(ocorrencia,'}'))!=NULL;ocorrencia++)
        saldo--;

    /* contabilizar parêntisis para não contabilizar parâmetros de funções
       como variáveis globais */
    if(parentisis!=0)
    {
        for(ocorrencia=texto;(ocorrencia=strchr(ocorrencia,'('))!=NULL;ocorrencia++)
            saldo++;

        for(ocorrencia=texto;(ocorrencia=strchr(ocorrencia,')'))!=NULL;ocorrencia++)
            saldo--;
    }
    return saldo;
}

/* verificar se existe uma instrução em "texto" (retorna 2 se instrução de fluxo) */
int Instrucao(char *texto)
{
    static char *instrucoesFluxo[]={"if(","while(","do","for(","switch(","else",NULL};
    int i;
    for(i=0;instrucoesFluxo[i]!=NULL;i++)
        if(ComecaCom(texto,instrucoesFluxo[i]))
            return 2;
    if(strchr(texto,';')!=0) 
        return 1;
    return 0;
}

/* expressões e instruções apenas numa linha */
void JuntarExpressoesInstrucoes(LinhaDeCodigo *lista)
{
    char *auxiliar;
    LinhaDeCodigo *expressao;
    while(lista!=NULL && lista->seguinte!=NULL)
    {
        /* os parêntisis têm de bater certo, 
           e tem de acabar com ponto e vírgula, 
           excepto os ciclos for */
        expressao=NULL;
        if(ComecaCom(lista->texto,"for("))
        {
            if(strstr(lista->texto,";")==NULL) 
                /* 1ª linha acaba com ; */
                expressao=lista;
            else if(strstr(lista->seguinte->texto,";")==NULL)
                /* 2ª linha acaba com ; */
                expressao=lista->seguinte;
            else if(lista->seguinte->seguinte!=NULL &&
                strstr(lista->seguinte->seguinte->texto,")")==NULL)
                /* 3ª linha acaba com ) */
                expressao=lista->seguinte->seguinte;
            else
                expressao=NULL;
        } else if(!ComecaCom(lista->texto,"{") && 
            !ComecaCom(lista->texto,"}") &&
            !ComecaCom(lista->seguinte->texto,"{") &&
            !ComecaCom(lista->seguinte->texto,"}") &&
            (ContaChavetas(lista->texto,1)!=0 || Instrucao(lista->texto)==0))
        {
            expressao=lista;
        }

        if(expressao!=NULL)
        {
            auxiliar=malloc(strlen(expressao->texto)+strlen(expressao->seguinte->texto)+1);
            if(auxiliar!=NULL)
            {
                strcpy(auxiliar,expressao->texto);
                strcat(auxiliar,expressao->seguinte->texto);
                expressao->seguinte=LCRemove(expressao->seguinte);
                free(expressao->texto);
                expressao->texto=auxiliar;
            }
        }

        /* não avançar se houve junções (pode ter mais para processar).
           no caso do ciclo for avança logo 3 linhas */
        if(expressao==NULL) {
            if(ComecaCom(lista->texto,"for("))
                lista=lista->seguinte->seguinte->seguinte;
            else
                lista=lista->seguinte;
        }
    }
}

/* adicionar blocos de instrução única
   (para facilitar o calculo da indentação) */
void AdicionarBlocos(LinhaDeCodigo *lista)
{
    /*char *ocorrencia;*/
    LinhaDeCodigo *auxiliar;
    int contagem,contagemIfs;
    /* procurar instruções "while", "for", "if/else" 
       de instrução única, para adicionar um bloco */
    while(lista!=NULL)
    {
        if(Instrucao(lista->texto)==2)
        {
            /* caso seja um condicional, contar if/else fora das chavetas */
            if(ComecaCom(lista->texto,"if(") || 
                ComecaCom(lista->texto,"else if(") )
                contagemIfs=0;
            else /* bloquear a contagem de ifs */
                contagemIfs=-1;
            /* caso seja o ciclo for, avançar duas linhas */
            if(ComecaCom(lista->texto,"for("))
            {
                lista=lista->seguinte->seguinte;
                contagemIfs=-2;
            }

            /* no caso do do-while, o segundo while termina logo com ;,
               não fazer nada nesse caso, para além de poderem existir
               ciclos while e for sem instrução, apenas com o ; */
            if((contagemIfs==-2 || ComecaCom(lista->texto,"while(")) &&
                strchr(lista->texto,';')!=NULL)
            {
                /* neste caso, não adicionar chavetas */
                lista=lista->seguinte;
                continue;
            }

            /* verificar se a linha seguinte é um bloco */
            if(lista->seguinte!=NULL && !ComecaCom(lista->seguinte->texto,"{"))
            {
                /* não é um bloco, adicionar */
                lista->seguinte=LCAdiciona(lista->seguinte,"{",lista->linha);
                contagem=0;
                auxiliar=lista->seguinte->seguinte;
                do
                {
                    /* atualizar a contagem de chavetas */
                    if(ComecaCom(auxiliar->texto,"{"))
                        contagem++;
                    else if(ComecaCom(auxiliar->texto,"}"))
                        contagem--;

                    /* caso existam chavetas a fechar em excesso,
                       cancelar a adição de chavetas */
                    if(contagem<0)
                        lista->seguinte=LCRemove(lista->seguinte);
                    else if(contagem==0 && (Instrucao(auxiliar->texto)==1 ||
                        ComecaCom(auxiliar->texto,"}")))
                    {
                        /* contagem nula, não há chavetas abertas e há uma 
                           instrução sem ser de fluxo. 
                           Verificar que a instrução seguinte 
                           não é um else de um if interno, 
                           já que nesse caso deve-se fechar após o else.
                           A excepção é o else ser do IF da instrução inicial,
                           nesse caso deve-se fechar à mesma */
                        if(!ComecaCom(auxiliar->seguinte->texto,"else") ||
                            contagemIfs==0)
                        {
                            auxiliar->seguinte=LCAdiciona(auxiliar->seguinte,"}",auxiliar->linha);
                            contagem=-1;
                        }
                    } else if(ComecaCom(auxiliar->texto,"for("))
                        /* o ciclo for avança sempre duas linhas */
                        auxiliar=auxiliar->seguinte->seguinte;
                    else if(contagem==0 && contagemIfs>=0 && ComecaCom(auxiliar->texto,"if("))
                        contagemIfs++;
                    else if(contagem==0 && contagemIfs>=0 && ComecaCom(auxiliar->texto,"else"))
                        contagemIfs--;

                    auxiliar=auxiliar->seguinte;

                } while(contagem>=0 && auxiliar!=NULL);
            }
        }
        lista=lista->seguinte;
    }
}


/* calcular tipos válidos (typedef/struct) */
LinhaDeCodigo *TiposValidos(LinhaDeCodigo *lista)
{
    char *tiposBasicos[]={
        "void", "int", "long", "float","char","double","short", "unsinged", NULL
    }, *ocorrencia;
    LinhaDeCodigo *tipos=NULL;
    int i,tamanho,chavetas;

    /* inicializar a lista com os tipos básicos */
    for(i=0;tiposBasicos[i]!=NULL;i++)
        tipos=LCAdiciona(tipos,tiposBasicos[i],0);

    /* percorrer o código à procura dos tipos typedef */
    while(lista!=NULL)
    {
        if((ocorrencia=strstr(lista->texto,"typedef"))==lista->texto)
        {
            tamanho=strlen(lista->texto);
            // obter o novo tipo (identificador antes do ponto-e-vírgula)
            if(lista->texto[tamanho-1]==';')
            {
                lista->texto[tamanho-1]='\0';
                ocorrencia+=tamanho-2;
                while(CaracterValido(*ocorrencia))
                    ocorrencia--;
                if(!CaracterValido(*ocorrencia))
                    ocorrencia++;
                /* adiciona novo tipo */
                tipos=LCAdiciona(tipos,ocorrencia,lista->linha);
                lista->texto[tamanho-1]=';';
            } else { /* ocupa mais que uma linha, pode ter struct */
                lista=lista->seguinte;
                chavetas=0;
                while(lista!=NULL)
                {
                    /* abre chavetas, avançar até fechar */
                    if(strcmp(lista->texto,"{")==0)
                        chavetas++;
                    else if(strcmp(lista->texto,"}")==0)
                        chavetas--;
                    else if(chavetas==0 && lista->texto[strlen(lista->texto)-1]==';')
                    {
                        tamanho=strlen(lista->texto);
                        lista->texto[tamanho-1]='\0';
                        ocorrencia=lista->texto+tamanho-2;
                        while(ocorrencia>lista->texto && CaracterValido(*ocorrencia))
                            ocorrencia--;
                        if(!CaracterValido(*ocorrencia))
                            ocorrencia++;
                        /* adiciona novo tipo */
                        tipos=LCAdiciona(tipos,ocorrencia,lista->linha);
                        lista->texto[tamanho-1]=';';
                        break;
                    }
                    lista=lista->seguinte;
                }
            }
        }

        lista=lista->seguinte;
    }

    /* os tipos utilizando struct são considerados automaticamamente,
       e não valeria a pena listá-los já que se pode nem sequer definir
       tipos struct e utilizar simplesmente a definição em cada declaração
       de variáveis desse tipo. */

    return tipos;
}

/* verificar se existe uma declaração de uma variável em "texto",
   retornando o início da variável no caso positivo */
int DeclaracaoDeVariavel(char *texto,LinhaDeCodigo *tipos)
{
    int tamanho,i;
    while(tipos!=NULL)
    {
        /* match no início da linha */
        if(ComecaCom(texto,tipos->texto) && !CaracterValido(texto[strlen(tipos->texto)]))
            return strlen(tipos->texto);
        tipos=tipos->seguinte;
    }
    /* verificar se existe uma estrutura declarada */
    if(ComecaCom(texto,"struct"))
    {
        /* caso a linha acabe em ponto-e-vírgula, a declaração acaba aqui */
        tamanho=strlen(texto);
        if(texto[tamanho-1]==';')
        {
            i=tamanho-2;
            while(i>6 && CaracterValido(texto[i]) || texto[i]==',')
                i--;
            return i+1; /* inicio da(s) variável(eis) declarada(s) */
        } else if(strchr(texto,'(')!=NULL) /* valor de retorno de uma função */
        {
            i=7;
            /* passa o nome da estrutura e retorna */
            while(CaracterValido(texto[i]))
                i++;
            return i;
        }
        /* variáveis declaradas com a definição da estrutura não são identificadas */
    }
    return 0;
}

/* calcular o número de caracteres iniciais/finais iguais, mas 
   no caso da diferença à esquerda ser um dígito, multiplicar por 4 */
int PrefixoIgual(char *nome1, char *nome2)
{
    int i=0,j,k,tamanho;
    while(nome1[i]!='\0' && nome2[i]!='\0' && nome1[i]==nome2[i])
        i++;
    if(isdigit(nome1[i]) && isdigit(nome2[i]))
        i*=4;

    /* verificar o sufixo */
    tamanho=j=strlen(nome1)-1;
    k=strlen(nome2)-1;
    while(j>0 && k>0 && nome1[j]==nome2[k])
    {
        j--;
        k--;
    }
    /* verificar se o sufixo é maior que o prefixo*/
    if(tamanho-j-1>i)
        i=tamanho-j-1;

    return i;
}

/************************************************************************** 
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
    LinhaDeCodigo *AdicionarVariaveisLocais(
        LinhaDeCodigo *lista,
        int i, 
        LinhaDeCodigo **variaveis,
        int *chavetas);
    int NumeroParametros(LinhaDeCodigo *lista);
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
*/

/* mostrar relatório de erro de qualidade de código */
void MostraErroQualidade(LinhaDeCodigo *listaOriginal,int numeroErro, ErroQualidade erro)
{
    static char *textoErros[]={
        "",
        "Funções com parâmetros no nome",
        "Variáveis Globais",
        "Variáveis com nomes quase iguais",
        "Nomes sem significado",
        "Utilização do goto",
        "Funções muito grandes",
        "Funções com muitos argumentos",
        "Indentação variável",
        "Instruções parecidas seguidas",
        "Instruções parecidas não seguidas",
        "Utilização de system",
        "Má utilização do switch ou cadeia if-else",
        "Comentários explicam a linguagem C",
        "Má utilização da recursão",
        "Atribuições entre variáveis de tipos distintos",
        "Declarações ou atribuições a variáveis nunca utilizadas",
        "Linhas de código nunca executadas",
        "Alocar memória e não testar se a operação foi bem sucedida",
        "Funções específicas",
        "Variáveis desnecessárias",
        "Declaração de variáveis fora do início de funções",
        "Duas instruções na mesma linha",
        "Ciclos contendo apenas uma variável lógica como teste de paragem",
        "Nomes não normalizados",
        "Macros em todas as constantes",
        "Nomes muito longos",
        "Comentários inexistentes em partes complexas",
        "Uma atribuição, uma leitura",
        "Não libertar memória após alocar",
        "Abrir um ficheiro e não chegar a fechar",
        "Abrir um ficheiro e não testar se foi bem aberto",
    };
    char *texto="",*textoAuxiliar=NULL;
    LinhaDeCodigo *lista;

    /* obter as linhas de código associadas ao erro */

    /* linha auxiliar distinta de zero */
    if(erro.auxiliar!=0 && erro.linha!=erro.auxiliar) 
    {
        lista=listaOriginal;
        while(lista!=NULL && lista->linha<erro.auxiliar)
            lista=lista->seguinte;
        if(lista!=NULL && lista->linha==erro.auxiliar)
            textoAuxiliar=lista->texto;
    }
    /* localizar a linha do erro */
    while(listaOriginal!=NULL && listaOriginal->linha<erro.linha)
        listaOriginal=listaOriginal->seguinte;
    if(listaOriginal!=NULL && listaOriginal->linha==erro.linha)
        texto=listaOriginal->texto;

    if(textoAuxiliar!=NULL)
    {
        /* erro com linha auxiliar, mostrar ambas as linhas de código
           associada ao erro */
        if(erro.valor>0)
            printf("#EQ%d: %s (valor: %d)\n%d|%s%d|%s", numeroErro,
                textoErros[numeroErro],erro.valor,erro.linha,texto,
                erro.auxiliar,textoAuxiliar);
        else
            printf("#EQ%d: %s\n%d|%s%d|%s", numeroErro,
                textoErros[numeroErro],erro.linha,texto,
                erro.auxiliar,textoAuxiliar);
    } else
    {
        /* erro normal sem linha auxiliar, mostrar a linha de código
           associada ao erro */
        if(erro.valor>0)
            printf("#EQ%d: %s (valor: %d)\n%d|%s",numeroErro,
                textoErros[numeroErro],erro.valor,erro.linha,texto);
        else
            printf("#EQ%d: %s\n%d|%s",numeroErro,
                textoErros[numeroErro],erro.linha,texto);
    }
}

/* match de instruções */
void UtilizaInstrucao(LinhaDeCodigo *lista, ErroQualidade erros[MAX_ERROS], 
    char *instrucao, int erro)
{
    char *ocorrencia;

    while(lista!=NULL)
    {
        if((ocorrencia=strstr(lista->texto,instrucao))!=NULL)
        {
            // match perfeito, sem caracteres válidos para nomes depois
            if(ocorrencia==lista->texto && !CaracterValido(*(ocorrencia+strlen(instrucao))))
            {
                erros[erro].linha=lista->linha;
                return;
            }
        }
        lista=lista->seguinte;
    }
}

/* adiciona variáveis locais a "variaveis", atualizando "chavetas", 
   sendo a função chamada no caso de existir uma definição em lista,
   cujo tipo da variável acaba em i */
LinhaDeCodigo *AdicionarVariaveisLocais(LinhaDeCodigo *lista,
    int i, LinhaDeCodigo **variaveis,int *chavetas)
{
    int j;
    char variavel[MAX_STR];
    do {
        /* avançar até à primeira variável, ignorando apontadores */
        do {
            /* passar para a linha seguinte no caso de  
               multiplas linhas numa declaração */
            if(lista->texto[i]=='\0') 
            {
                lista=lista->seguinte;
                if(lista==NULL)
                    return NULL;
                i=0;
            } else if(!CaracterValido(lista->texto[i]))
                i++;
        } while(!CaracterValido(lista->texto[i]));

        /* copiar o nome da variável e adicioná-la */
        j=0;
        while(CaracterValido(lista->texto[i]) && j<MAX_STR-1) 
            variavel[j++]=lista->texto[i++];
        if(j>0)
        {
            variavel[j]='\0';
            (*variaveis)=LCAdiciona(*variaveis,variavel,lista->linha);
        }

        /* avançar até à próxima vírgula (passar para a próxima variável),
           ou ponto e vírgula (parar a declaração de variáveis).
           contando com fim de linha comentários e chavetas das inicializações */
        do {
            /* fim de linha */
            if(lista->texto[i]=='\0') 
            {
                lista=lista->seguinte;
                if(lista==NULL)
                    return NULL;
                i=0;
            } else 
            {
                if(lista->texto[i]=='{')
                    (*chavetas)++;
                else if(lista->texto[i]=='}')
                    (*chavetas)--;
            
                if((*chavetas)!=1 || lista->texto[i]!=',' && lista->texto[i]!=';')
                    i++;

                if(lista->texto[i]=='\0')
                    break;
            }
        } while((*chavetas)!=1 || lista->texto[i]!=',' && lista->texto[i]!=';');

        /* a única saída é com chavetas==1 numa vírgula ou ponto-e-virgula */
    } while(lista->texto[i]==','); /* no caso da vírgula, segue-se nova variável */

    return lista;
}

/* calcular o número de argumentos, com base no número de vírgulas entre parêntisis */
int NumeroParametros(LinhaDeCodigo *lista)
{
    char *caracter;
    int parametros=0;
    if((caracter=strchr(lista->texto,'('))!=NULL)
    {
        while(*caracter!='\0' && *caracter!=')')
        {
            if(parametros==0 && CaracterValido(*caracter))
                parametros=1;
            /* por cada vírgula, mais um parametro */
            if(*caracter==',')
                parametros++;
            caracter++;
            /* verificar se continuana próxima linha */
            while(*caracter=='\0')
            {
                lista=lista->seguinte;
                if(lista!=NULL)
                    caracter=lista->texto;
            }
        }
    }
    return parametros;
}

/* processa elementos globais do código (nível 0 de chavetas) */
void ElementosGlobais(LinhaDeCodigo *lista,ErroQualidade erros[MAX_ERROS],
    LinhaDeCodigo *tipos)
{
    int fimTipo,chavetas=0,tamanho,declaracao=0,instrucoes=0,argumentos=0;
    /*int erroVG=0, erroFPN=0, erroFMG=0; // contadores com os erros*/
    /*LinhaDeCodigo *variaveis=NULL,*aux;*/
    char *auxiliar; /* posição temporária dos parêntesis*/

    while(lista!=NULL)
    {
        if(chavetas==0)
        {
            if((fimTipo=DeclaracaoDeVariavel(lista->texto,tipos))>0)
            {
                tamanho=strlen(lista->texto);
                if(lista->texto[fimTipo]!=';' && lista->texto[tamanho-1]==';' && lista->texto[tamanho-2]!=')')
                {    /* variáveis globais 
                       (e não um typdef nem uma declaração de uma função) */
                    if(erros[2].linha==0)
                        erros[2].linha=lista->linha;

                } else if((auxiliar=strchr(lista->texto,'('))!=NULL)
                {    /* funções */
                    declaracao=lista->linha;
                    instrucoes=0;
                    /* Funções com parâmetros no nome, ver se termina num dígito */
                    if(erros[1].linha==0 && isdigit(*(auxiliar-1)))
                        erros[1].linha=lista->linha;
                    /* calcular o número de argumentos */
                    argumentos=NumeroParametros(lista);
                }
            }
        }

        chavetas+=ContaChavetas(lista->texto,1);
        /* se dentro de uma função, contar o número de instruções */
        if(declaracao>0 && chavetas>0)
        {   
            /* o ciclo "for" conta como duas instruções, dado que a 
               alternativa do "while" utilizaria mais duas instruções
               de qualquer modo */
            if(strchr(lista->texto,';')!=NULL)
                instrucoes++;
        } else if(chavetas==0 && declaracao>0 && instrucoes>0)
        {
            /* verifica se o número de instruções é inferior ao 
               número de argumentos, e existem mais de 3 argumentos */
            if(erros[7].linha==0 && argumentos>3 && instrucoes<argumentos)
            {
                erros[7].linha=declaracao;
                erros[7].valor=argumentos-instrucoes;
            }

            /* regista a maior função, verificar se é erro apenas no final */
            if(instrucoes>erros[6].valor)
            {
                erros[6].linha=declaracao;
                erros[6].valor=instrucoes;
            }
            instrucoes=0;
            argumentos=0;
            declaracao=0;
        }

        lista=lista->seguinte;
    }
}

/* verificar a existência de identificadores quase iguais */
void NomesQuaseIguais(LinhaDeCodigo *variaveis, ErroQualidade erros[MAX_ERROS])
{
    int valorPrefixo=0;
    LinhaDeCodigo *auxiliar;
    while(variaveis!=NULL && variaveis->seguinte!=NULL)
    {
        auxiliar=variaveis->seguinte;
        while(auxiliar!=NULL)
        {
            /* teste entre variaveis e auxiliar, se tiverem tamanhos idênticos
               e prefixos iguais */
            if(abs(strlen(variaveis->texto)-strlen(auxiliar->texto))<=2 &&
                (valorPrefixo=PrefixoIgual(variaveis->texto,auxiliar->texto))>
                    erros[3].valor)
            {
                erros[3].linha=variaveis->linha;
                erros[3].auxiliar=auxiliar->linha;
                erros[3].valor=valorPrefixo;
            }
            auxiliar=auxiliar->seguinte;
        }
        variaveis=variaveis->seguinte;
    }
}

/* verificar a dimensão dos identificadores */
void NomesMuitoLongos(LinhaDeCodigo *variaveis, ErroQualidade erros[MAX_ERROS])
{
    int tamanho;
    /* calcular a variável de maior tamanho */
    while(variaveis!=NULL)
    {
        tamanho=strlen(variaveis->texto);
        if(erros[26].valor<tamanho)
        {
            erros[26].linha=variaveis->linha;
            erros[26].valor=tamanho;
        }
        variaveis=variaveis->seguinte;
    }
}

/* extrai os testes de parágens em ciclos */
LinhaDeCodigo *ExtraiTestesDeParagem(
    LinhaDeCodigo *lista,LinhaDeCodigo *testesDeParagem)
{
    int i=0,j,parentisis=0,fase;
    char teste[MAX_STR];

    if(ComecaCom(lista->texto,"while"))
    {
        j=6; /* iteração a partir do primeiro parentisis */
        i=0;
        while(i<MAX_STR-1 && (lista->texto[j]!=')' || parentisis!=0)) 
        {
            if(lista->texto[j]=='\0' || lista->texto[j]=='/') /* fim de linha / comentário */
            {
                lista=lista->seguinte;
                if(lista==NULL)
                    return testesDeParagem;
                j=0;
            } else
            {
                if(lista->texto[j]=='(')
                    parentisis++;
                else if(lista->texto[j]==')')
                    parentisis--;
                teste[i++]=lista->texto[j++];
            }
        }
        teste[i]='\0';
        testesDeParagem=LCAdiciona(testesDeParagem,teste,lista->linha);
    }
    if(ComecaCom(lista->texto,"for"))
    {
        j=4; /* iteração a partir do primeiro parentisis */
        i=0;
        /* fase=0: avançar até ao próximo ponto-e-vírgula 
           fase=1: copiar até ao próximo ponto-e-vírgula
        */
        for(fase=0;fase<2;fase++)
        {
            while(fase==0 ? lista->texto[j]!=';' : i<MAX_STR-1 && lista->texto[j]!=';')
            {
                if(lista->texto[j]=='\0' || lista->texto[j]=='/') /* fim de linha / comentário */
                {
                    lista=lista->seguinte;
                    if(lista==NULL)
                        return testesDeParagem;
                    j=0;
                } else if(fase==0)
                    j++;
                else if(fase==1)
                    teste[i++]=lista->texto[j++];
            }
            j++;
        }
        teste[i]='\0';
        testesDeParagem=LCAdiciona(testesDeParagem,teste,lista->linha);
    }
    return testesDeParagem;
}

/* verifica se o nome tem uma dimensão reduzida, de 1 ou 2 caracteres, e não 
   pertence a nenhum teste de paragem, pelo que não é variável iteradora */
/* de futuro pode-se forçar a existência de uma palavra em português/inglês */
int NomeSemSignificado(char *texto, LinhaDeCodigo *testesDeParagem,
                       Palavra **dicionario)
{
    char *ocorrencia;
    char componente[MAX_STR];
    char *pLetra, *pTexto;

    if(strlen(texto) <=2 ) {
        /* verificar se pertence a algum teste de paragem */
        while(testesDeParagem != NULL) {
            ocorrencia = testesDeParagem->texto;
            while((ocorrencia = strstr(ocorrencia,texto)) != NULL) {
                /* verificar se é um match perfeito */
                if((ocorrencia == testesDeParagem->texto || 
                    !CaracterValido(*(ocorrencia - 1))) &&
                    !CaracterValido(*(ocorrencia + strlen(texto))))
                    /* faz parte de um teste de paragem, pelo que pode
                       ser uma variável iteradora, não ativar erro */
                    return 0;
                ocorrencia++;
            }
            testesDeParagem = testesDeParagem->seguinte;
        }
    } else if (dicionario!=NULL) {
        pLetra = componente;
        pTexto = texto;
        while(*pTexto!='\0') 
        {
            if(isalpha(*pTexto))
            {
                *pLetra = tolower(*pTexto);
                pLetra++;
            }
            /* se o caracter seguinte não é uma letra minúscula, 
               acabar a palavra e começar outra */
            if(pTexto[1] < 'a' || 'z' < pTexto[1]) { 
                *pLetra = '\0';
                if (strlen(componente) > 2 && 
                    DPalavraExiste(dicionario, componente)) 
                    /* uma componente da variável que existe, logo, nome com significado */
                    return 0;
                pLetra = componente;
            }
            pTexto++;
        } 
    }
    /* nome sem significado 
       (não é variável iteradora, 
       nem contém uma componente que seja uma palavra) */
    //printf("\n>>>%s.",texto);
    return 1;
}

/* Função auxiliar a ElementosLocais */

void ELLiberta(LinhaDeCodigo **variaveis,LinhaDeCodigo **testesDeParagem)
{
    *variaveis=LCLiberta(*variaveis);
    *testesDeParagem=LCLiberta(*testesDeParagem);
}

/* junta os nomes das variáveis locais de cada tipo */
void ElementosLocais(LinhaDeCodigo *lista, ErroQualidade erros[MAX_ERROS],
                     LinhaDeCodigo *tipos, Palavra **dicionario)
{
    int i, chavetas=0;
    LinhaDeCodigo *variaveis=NULL,*testesDeParagem=NULL;

    while(lista!=NULL)
    {
        /* linha de nível local, e com um tipo básico */
        if(chavetas==1 && (i=DeclaracaoDeVariavel(lista->texto,tipos))>0)
            /* adicionar variáveis */
            lista=AdicionarVariaveisLocais(lista,i,&variaveis,&chavetas);

        chavetas+=ContaChavetas(lista->texto,0);

        testesDeParagem=ExtraiTestesDeParagem(lista,testesDeParagem);

        if(chavetas==0 && variaveis!=NULL)
        {
            /* processar variáveis locais declaradas em bloco */
            NomesQuaseIguais(variaveis,erros);
            NomesMuitoLongos(variaveis,erros);

            if(erros[4].linha==0)
            {
                while(variaveis!=NULL)
                {
                    if(NomeSemSignificado(variaveis->texto, testesDeParagem,
                                          dicionario))
                    {
                        erros[4].linha=variaveis->linha;
                        break;
                    }
                    variaveis=LCRemove(variaveis);
                }
            }

            /* ciclos contendo apenas uma variável lógica como teste de paragem */
            if(erros[23].linha==0)
                while(testesDeParagem!=NULL)
                {
                    for(i=0;CaracterValido(testesDeParagem->texto[i]);i++);
                    if(testesDeParagem->texto[i]=='\0')
                        /* apenas caracteres válidos, trata-se de uma variável lógica */
                        erros[23].linha=testesDeParagem->linha;

                    testesDeParagem=LCRemove(testesDeParagem);
                }

            /* limpar as variáveis locais e testes de paragem */
            ELLiberta(&variaveis,&testesDeParagem);
        }

        lista=lista->seguinte;
    }

    /* limpar as variáveis locais e testes de paragem */
    ELLiberta(&variaveis,&testesDeParagem);
}

/* verifica se existem duas instruções na mesma linha */
void DuasInstrucoes(LinhaDeCodigo *lista, ErroQualidade erros[MAX_ERROS])
{
    LinhaDeCodigo *auxiliar;
    /* se a instrução anterior não for uma instrução de controle de fluxo,
       verificar apenas se a instrução seguinte está na mesma linha. 
       Nas instruções de controle de fluxo, avançar até ao parêntisis curvo,
       para ver se a instrução seguinte está em outra linha ou não */
    while(lista!=NULL)
    {
        /* verificar se se trata de uma instrução */
        if(Instrucao(lista->texto))
        {
            if(ComecaCom(lista->texto,"for"))
            {    /* saltar duas linhas correspondendo ao resto do ciclo for */
                if(lista->seguinte!=NULL)
                    lista=lista->seguinte->seguinte;
            }
            /* verificar há outra instrução nesta linha */
            auxiliar=lista->seguinte;
            while(auxiliar!=NULL && auxiliar->linha==lista->linha)
            {
                if(Instrucao(auxiliar->texto))
                {
                    erros[22].linha=lista->linha;
                    return;
                }
                auxiliar=auxiliar->seguinte;
            }
            /* passar para a próxima linha do ficheiro */
            lista=auxiliar;
        } else
            lista=lista->seguinte;
    }
}

/* verifica se o código original está bem indentado, 
   percorrendo em paralelo o código processado (sem strings) */
void VerificaIndentacao(
    LinhaDeCodigo *lista, 
    LinhaDeCodigo *original, 
    ErroQualidade erros[MAX_ERROS])
{
    int chavetas=0, indentacao=0, tabSize=4, espacos=0;
    LinhaDeCodigo *base;
    int chavetasBase;
    /* processar todo o código */
    while(lista!=NULL && original!=NULL && erros[8].linha==0)
    {
        /* sincronizar o número das linhas */
        while(lista->linha!=original->linha)
        {
            /* caso seja uma linha de chaveta, processar sempre */
            while(lista->linha<original->linha ||
                strstr("{}",lista->texto)!=NULL)
            {
                chavetas+=ContaChavetas(lista->texto,1);
                lista=lista->seguinte;
                if(lista==NULL)
                    return;
            }
            while(lista->linha>original->linha)
            {
                original=original->seguinte;
                if(original==NULL)
                    return;
            }
        }

        /* verificar se a indentação é válida, 
           ignorando as linhas com : (labels e casos dentro do switch) */
        if(strchr(original->texto,':')==NULL)
        {
            indentacao=Indentacao(original->texto,tabSize);

            if(indentacao>0 && chavetas==0)
            {
                /* erro de indentação, não há dúvida */
                erros[8].linha=original->linha;
                erros[8].valor=0;
            } else if(indentacao>0 && chavetas>0)
            {
                /* fixar os espaços por indentação */
                if(espacos==0)
                {
                    espacos=indentacao/chavetas;
                    base=original;
                    chavetasBase=chavetas;
                }

                /* teste de indentação */
                if(chavetas*espacos!=indentacao)
                {
                    /* incorreto, talvez exista possibilidade do tab não ser 4,
                       verificar outras possibilidades da linha base */
                    for(tabSize=2;tabSize<=8;tabSize++)
                    {
                        espacos=Indentacao(base->texto,tabSize)/chavetasBase;
                        if(chavetas*espacos==Indentacao(original->texto,tabSize))
                        {
                            /* correto, parar, este é o tab certo */
                            break;
                        }
                    }
                    if(tabSize>8)
                    {
                        /* não foi possível encontrar um tab coerente 
                           com ambas as linhas, gerar erro */
                        erros[8].linha=base->linha;
                        erros[8].auxiliar=original->linha;
                        erros[8].valor=chavetas;
                    }
                }
            }
        }
        original=original->seguinte;
    } 
    return;
}

/* detecção de linhas de código iguais, excepto os dígitos, que 
   devem ser considerados todos como iguais */
int LinhaIgual(char *nome, char *alternativo)
{
    int digito,dAlternativo;
    /* parar mal se chegue ao final de uma das strings */
    while(*nome!='\0' && *alternativo!='\0')
    {
        digito=dAlternativo=0;
        /* avançar todos os dígitos */
        while(isdigit(*nome))
        {
            nome++;
            digito=1;
        }
        while(isdigit(*alternativo))
        {
            alternativo++;
            dAlternativo=1;
        }
        /* se forem caracteres distintos, 
          ou existirem dígitos num lado e não no outro, 
          retornar */
        if(digito!=dAlternativo || *nome!=*alternativo)
            return 0;
        nome++;
        alternativo++;
    }

    /* são iguais se ambas as strings estiverem processadas */
    return *nome=='\0' && *alternativo=='\0';
}

/* detecção de linhas de código repetidas */
int LinhaRepetida(LinhaDeCodigo *lista)
{
    int comprimento=0;
    LinhaDeCodigo *auxiliar=lista;
    while(auxiliar->seguinte!=NULL && comprimento<20)
    {
        auxiliar=auxiliar->seguinte;
        comprimento++;
        if(LinhaIgual(lista->texto,auxiliar->texto))
            return comprimento;
    }
    return 0;
}

/* #EQ9/10: código repetido seguido (e não seguido) 
   TODO: se erro 9, e numa cadeia if-else ou switch, transformar em erro 12 */
void VerificaCodigoRepetido(
    LinhaDeCodigo *lista,
    ErroQualidade erros[MAX_ERROS])
{
    int comprimento=0; // comprimento da repetição
    int codigoSeguido=0;
    LinhaDeCodigo *auxiliar,*original,*linha9=NULL;
    /* detecção: (iguais a menos dos dígitos)
       - N linhas, iguais a outras N linhas (estilo: ABCDABCD)
         » erro 9 (linhas iguais seguidas), com N>=2
       - 3 linhas iguais seguidas (estilo: AAA, mas não aceitar A.A.A)
         » erro 9
       - 2 linhas seguidas, iguais a outras duas linhas (estilo: AB...AB)
         » erro 10
       A primeira linha tem de ter alguma complexidade 
       (uma atribuição ou uma chamada a uma função com dois ou mais argumentos)
    */

    while(lista!=NULL && lista->seguinte!=NULL)
    {
        // procurar dois hits à mesma distância, em duas linhas seguidas
        // (apenas linhas com instruções)
        if(Instrucao(lista->texto)>0)
        {
            if(comprimento==0)
            {
                /* requerer uma complexidade mínima para a primeira linha */
                if(strchr(lista->texto,'=')!=NULL || /* uma atribuição */
                    strchr(lista->texto,',')!=NULL) /* chamada a uma função com mais que um argumento */
                {
                    comprimento=LinhaRepetida(lista);
                    if(comprimento>0)
                    {
                        /* sabe-se A...A, para já registar o início da sequência */
                        original=lista;
                        linha9=LCPosicao(lista,comprimento);

                        /* tentar o match seguinte, caso consiga, manter o comprimento */
                        auxiliar=LCPosicao(lista,comprimento*2);
                        if(auxiliar!=NULL && LinhaIgual(lista->texto,auxiliar->texto))
                        {
                            // possibilidade de AAA, no caso de comprimento ser afinal unitário
                            codigoSeguido=1;
                        } else 
                            /* ainda não pode terminar desta forma, é preciso ter uma segunda linha igual */
                            codigoSeguido=0; 
                    }
                }
            } else 
            {
                if(linha9==lista)
                {
                    if(codigoSeguido)
                    {
                        /* todas as linhas entre original e linha9 têm 
                           linhas repetidas (ABCDABCD ou AAA), trata-se do erro 9! */

                        if(erros[10].linha==original->linha)
                        {
                            /* cancelar o erro 10, trata-se de um erro 9 */
                            erros[10].linha=erros[10].auxiliar=erros[10].valor=0;
                        }

                        erros[9].linha=original->linha;
                        erros[9].auxiliar=LCPosicao(original,comprimento)->linha;
                        erros[9].valor=0;

                        if(erros[10].linha>0)
                            return;
                    } else
                    {
                        /* situação de AA..., não assinalar como erro */
                        original=linha9=NULL;
                        codigoSeguido=comprimento=0;
                    }
                } else
                {
                    codigoSeguido=1;
                    /* tentar o match seguinte, caso consiga, hit do erro 10 pelo menos */
                    auxiliar=LCPosicao(lista,comprimento);
                    if(auxiliar!=NULL && LinhaIgual(lista->texto,auxiliar->texto))
                    {
                        /* apenas aceitar se a linha de código for distinta  da linha original */
                        if(original->linha!=lista->linha &&
                            erros[10].linha==0 && erros[9].linha!=original->linha)
                        {
                            /* registar para já o erro 10 (AB...AB) , 
                                ficando a 9 caso resulte em (ABCDABCD) */
                            erros[10].linha=original->linha;
                            erros[10].auxiliar=LCPosicao(original,comprimento)->linha;
                            erros[10].valor=0;

                            if(erros[9].linha>0)
                                return;
                        }
                    } else
                    {
                        /* desistir desta sequência para 
                           erro 9 ou 10 se não estiver já registado */
                        original=linha9=NULL;
                        codigoSeguido=comprimento=0;
                    }
                }
            }
        }

        lista=lista->seguinte;
    }
    return;
}


/************************************************************************** 
    Funções gerais:

    void AnaliseErrosQualidade(
        LinhaDeCodigo *lista,  
        LinhaDeCodigo *tipos, 
        ErroQualidade erros[MAX_ERROS]);
    void AnalisaFicheiro(char *nome);

*/

/* verificar os vários erros de qualidade implementados */
void AnaliseErrosQualidade(
    LinhaDeCodigo *lista,  
    LinhaDeCodigo *tipos, 
    ErroQualidade erros[MAX_ERROS],
    LinhaDeCodigo *original,
    Palavra **dicionario)
{

    /* #EQ1: funções com parâmetros no nome
       #EQ2: variáveis globais
       #EQ6: funções muito grandes
       #EQ7: funções com muitos argumentos */
    ElementosGlobais(lista,erros,tipos);

    /* #EQ8: indentação */
    VerificaIndentacao(lista,original,erros);

    /* #EQ9/10: código repetido seguido (e não seguido) */
    VerificaCodigoRepetido(lista,erros);

    /* #EQ3: variáveis com nomes quase iguais
       #EQ4: nomes sem significado 
       #EQ23: Ciclos contendo apenas uma variável lógica como teste de paragem
       #EQ26: nomes muito longos */
    ElementosLocais(lista, erros, tipos, dicionario);

    /* #EQ5: goto 
       #EQ11: system */
    UtilizaInstrucao(lista,erros,"goto",5);
    UtilizaInstrucao(lista,erros,"system",11);

    /* #EQ22: duas instruções na mesma linha */
    DuasInstrucoes(lista,erros);
}

/* processa um ficheiro de código */
void AnalisaFicheiro(char *nome, Palavra **dicionario)
{
    ErroQualidade erros[MAX_ERROS];
    FILE *f;
    char texto[MAX_STR];
    LinhaDeCodigo *lista=NULL, *auxiliar, *original,*tipos;
    int linha=1,i;
    static int limiteErros[4][2]={{3,6},{6,64},{26,20},{0,0}};
    /* Limites:
       - variáveis com nomes quase iguais    
       - funções muito grandes
       - nomes muito longos
    */

    f=fopen(nome,"rt");
    if(f==NULL)
    {
        printf("\nNao foi possivel abrir ficheiro %s.",nome);
        return /*0*/;
    }

    /* carregar todo o ficheiro para memória */
    while(!feof(f)) 
        if(fgets(texto,MAX_STR,f)!=NULL) 
            lista=LCAdiciona(lista,texto,linha++);
    fclose(f);

    lista=LCInverte(lista);
    original=LCCopia(lista);

    /* acertar os espaços nas linhas */
    EspacosCodigo(lista);
    /* congelar strings de comentários, strings e plicas */
    RemoverStrings(lista);
    /* separar instruções nas mesmas linhas */
    SepararInstrucoes(lista);
    /* voltar a acertar os espaços nas linhas */
    EspacosCodigo(lista);
    /* remover linhas vazias */
    lista=RemoverLinhasVazias(lista);
    /* expressões e instruções apenas numa linha */
    JuntarExpressoesInstrucoes(lista);
    /* adicionar blocos de instrução única 
       (para facilitar o calculo da indentação) */
    AdicionarBlocos(lista);

    /* calcular tipos válidos (typedef/struct) */
    tipos=TiposValidos(lista);

    /* limpar o vector de erros */
    for(i=0;i<MAX_ERROS;i++)
        erros[i].linha=erros[i].valor=erros[i].auxiliar=0;

    /* mostrar resultado da compactação 
       (descomentar para ver a estrutura) 
    auxiliar=lista;
    while(auxiliar!=NULL)
    {
        printf("\n%d|%s",auxiliar->linha,auxiliar->texto);
        auxiliar=auxiliar->seguinte;
    } */
    

    AnaliseErrosQualidade(lista, tipos, erros, original, dicionario);

    /* eliminar erros que não tenham atingido o limite */
    for(i=0;limiteErros[i][0]!=0;i++)
        if(erros[limiteErros[i][0]].valor<limiteErros[i][1])
            erros[limiteErros[i][0]].linha=0;

    /* mostrar os erros por ordem */
    for(i=1;i<MAX_ERROS;i++)
        if(erros[i].linha>0)
            MostraErroQualidade(original,i,erros[i]);

    /* libertar tudo */
    LCLiberta(lista);
    LCLiberta(original);
    LCLiberta(tipos);
}

/* função main, para já fazer apenas um modo de teste, e um ficheiro isolado,
   mas convém processar os argumentos para mais operações, por exemplo, 
   no contexto de correcção de um e-fólio */
int main(int argc, char **argv)
{
/*    char texto[MAX_STR];
    char **ficheiros;
    int i,caracteres=5,instrucoes=64; */
    Palavra** dicionario = NULL;

    if(argc > 1) {
        if (argc > 2) {
            if (!(dicionario = CriaDicionario())) {
                puts("Erro: não foi possível criar o dicionário.");
                exit(1);
            } else if (LeDicionario(dicionario, argv[2]) < 0) {
                puts("Não foi possível ler o ficheiro com o dicionário.");
                exit(1);
            }
        }
        
        AnalisaFicheiro(argv[1], dicionario);
        
        if (argc > 3 &&    EscreveDicionario(dicionario, argv[3]) < 0)
            puts("Não foi possível escrever o ficheiro com o dicionário.");
    } else {
        /* testes 

        if (!(dicionario = CriaDicionario())) {
            puts("Erro: não foi possível criar o dicionário.");
            exit(1);
        } else if (LeDicionario(dicionario, "ptwiktionary.txt") < 0) {
            puts("Não foi possível ler o ficheiro com o dicionário.");
            exit(1);
        }
            
        ficheiros=ProgramasTeste("efolios\\b0910\\",0);
        for(i=0;ficheiros[i]!=NULL;i++)
        {
            strcpy(texto,"efolios\\b0910\\");
            strcat(texto,ficheiros[i]);
            printf("\n-------------------\nPrograma: %s\n",ficheiros[i]);
            AnalisaFicheiro(texto, dicionario);
        } */
    }

    if (dicionario)
        EliminaDicionario(dicionario);
    
    return 0;
}
