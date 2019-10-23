// Projeto 4 - EDA 1 - Mendelson
// Nome: Pedro Rodrigues Pereira 
// Matrícula: 17/0062686

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// constantes de tempo
#define TEMPO_UNIDADE 5

// Estruturas para tratar os voos
typedef struct Lista {
    char *codigo;
    char *status;
    char *pista;
    int combustivel;
    int tipo;
    int processo;
    int horainicio;
    int horafinal;
    struct Lista *prox;
} lista;

typedef struct Fila {
    lista *ini;
    lista *fim;
} fila;

// Estrutura para tratar as pistas
typedef struct Pista{
    int horarioUsado;
    int emUso;
} pista;

// headers
void printaCabecalho(int, int, lista *);
void liberaMatrix(char **,int);
void printaEventos(lista *, lista *, fila *);
void liberaFila(fila *);

int geraNumAleatorio(int, int);
int checaNumVoos(lista *);
int checaEmergencia(lista *);

char **lerCodigosVoos();

lista *listaVazia();
lista *geraVoo(char **,int, int, int);
lista *insereLista(lista *, lista *);
lista *copiaElemento(lista *);
lista *ordenaLista(lista *, int);
lista *reduzCombustivel(lista *);
lista *definirPistas(lista *, lista *, lista *,int, int);
lista *checaVoos(lista *, int);

fila *geraFilaDecolagens(lista *);
fila *insereFinalFila(fila *, lista *);
fila *geraFilaAprox(lista *);
fila *insereInicioFila(fila *, lista *);
fila *ordenaAproxPorComb(lista *);
fila *geraDadosAleatorios(int *, int *);

pista *pistaStart(int);


int main(){
    fila *voos, *aproximacoes, *decolagens;
    int num_Voos,num_Aprox,tempo;

    // seta os dados aleatorios e as filas
    voos = geraDadosAleatorios(&num_Voos, &num_Aprox);
    aproximacoes = geraFilaAprox(voos->ini);
    decolagens = geraFilaDecolagens(voos->ini);

    // Programa começa as 10:00 am => em minutos(600 min)
    tempo = 600;

    // define as pistas para cada voo
    voos->ini = definirPistas(voos->ini, aproximacoes->ini, decolagens->ini, tempo, num_Aprox);

    // printa os resultados
    printaEventos(aproximacoes->ini, decolagens->ini, voos);

    // libera as filas
    liberaFila(voos);

    return 0;
}


void printaCabecalho(int numVoos, int numAprox, lista *voos){
    printf("---------------------------------------------------------------------------------\n");
    printf("“Aeroporto Internacional de Brasília”\n");
    printf("Hora Inicial: 10:00\n");
    printf("Fila de Pedidos:\n");
    lista *it;
    for(it = voos; it != NULL; it = it->prox){
        printf("\t\t[%s - ",it->codigo);
        if(it->tipo == 0){
            printf("A - ");
            if(it->combustivel == 0){
                printf("ALTA]\n");
            }else{
                printf("BAIXA]\n");
            }
        }else{
            printf("D - BAIXA]\n");
        }
    }
    printf("Numero de Voos: %d\n", numVoos);
    printf("Numero de aproximações: %d\n", numAprox);
    printf("Numero de Decolagens: %d\n",(numVoos-numAprox));
    printf("---------------------------------------------------------------------------------\n");
}

void printaEventos(lista *aprox, lista *deco, fila *v){
    lista *itAprox = aprox;
    lista *itDeco = deco;
    lista *it;
    int h,m;
    printf("Listagem de eventos:\n\n");
    for(it = v->ini; it != NULL; it = it->prox){
        if(itAprox != NULL){
            if(strcmp(itAprox->status, "ALERTA CRÍTICO, AERONAVE IRÁ CAIR") == 0){
                printf("ALERTA CRÍTICO, AERONAVE IRÁ CAIR\n\n");
            }else{
                if(strcmp(itAprox->pista,"Pista C") == 0){
                    printf("ALERTA GERAL DE DESVIO DE AERONAVE\n\n");
                }
                printf("Código do voo: %s\n",itAprox->codigo);
                printf("Status: %s\n",itAprox->status);
                h = (itAprox->horainicio/60);
                m = itAprox->horainicio%60;
                printf("Horário do início do procedimento: %02d:%02d\n",h,m);
                printf("Número da pista: %s\n\n",itAprox->pista);
            }

            itAprox = itAprox->prox;
        }
        if(itDeco != NULL){
            printf("Código do voo: %s\n",itDeco->codigo);
            printf("Status: %s\n",itDeco->status);
            h = (itDeco->horainicio/60);
            m = itDeco->horainicio%60;
            printf("Horário do início do procedimento: %02d:%02d\n",h,m);
            printf("Número da pista: %s\n\n",itDeco->pista);

            itDeco = itDeco->prox;
        }
    }
}


void liberaMatrix(char **codigos,int size){
    for(int i = 0; i < size; i++){
        free(codigos[i]);
    }
    free(codigos);
}

void liberaFila(fila *v){
    lista *it;
    for(it = v->ini; it != NULL; it = it->prox){
        free(it->codigo);
        free(it->status);
        free(it->pista);
        free(it);
    }
    free(v);
}


/* Função responsável por gerar um número aleatório entre inicio e final*/
int geraNumAleatorio(int inicio, int final){
    int numero = (rand() % (final - inicio + 1)) + inicio;

    return numero;
}


int checaNumVoos(lista *voos){
    int cont = 0;
    lista *it;
    for(it = voos; it != NULL; it = it->prox){
        if(it->processo == 0){
            cont++;
        }
    }
    
    return cont;
}


int checaEmergencia(lista *voos){
    int cont = 0;
    lista *it;
    for(it = voos; it != NULL; it = it->prox){
        if(it->combustivel == 0 && it->processo == 1){
            cont++;
        }
    }

    return cont;
}


char **lerCodigosVoos(){
    
    char **codigos = (char **) malloc(64*sizeof(char*));
    FILE *f = fopen("codigos.txt", "r");

    if(codigos == NULL || f == NULL){
        printf("Erro de alocação\n");
        exit(-1);
    }

    for(int i = 0; i < 64; i++){
        codigos[i] = (char *) malloc(10*(sizeof(char)));
        
        if(codigos == NULL){
            printf("Erro de alocação\n");
            exit(-1);
        }

        fscanf(f," %s",codigos[i]);
    }
    fclose(f);
    return codigos;
}


lista *listaVazia(){
    return NULL;
}


lista *ordenaLista(lista *voos, int numAprox){
    for(int i = 0; i < numAprox; i++){
        lista *it = voos;
        for(int j = 0; j < (numAprox-1); j++){
            if(it->combustivel > it->prox->combustivel){
                char *aux = (char *) malloc(10*(sizeof(char)));
                int combAux;

                strcpy(aux, it->codigo);
                strcpy(it->codigo, it->prox->codigo);
                strcpy(it->prox->codigo, aux);

                combAux = it->combustivel;
                it->combustivel = it->prox->combustivel;
                it->prox->combustivel = combAux;
            }
            it=it->prox;
        }
    }

    return voos;
}


lista *definirPistas(lista *voos,lista *aprox, lista *decolagens, int tempo, int numAprox){
    
    pista *pistaA = pistaStart(tempo);
    pista *pistaB = pistaStart(tempo);
    pista *pistaC = pistaStart(tempo);

    int tempoAux = tempo + (10*TEMPO_UNIDADE);
    int num = 1;
    int num2;
    int emergencias;
    aprox = ordenaLista(aprox, numAprox);

    lista *itAprox = aprox;
    lista *itDecolagens = decolagens;
    
    while((checaNumVoos(aprox) + checaNumVoos(decolagens)) > 0){
        num = checaNumVoos(aprox);
        num2 = checaNumVoos(decolagens);

        aprox = checaVoos(aprox, tempo);
        decolagens = checaVoos(decolagens, tempo);

        emergencias = checaEmergencia(aprox);

        if(pistaA->horarioUsado < tempo){
            if(checaNumVoos(aprox) == 0){
                if(itDecolagens != NULL){
                    if(itDecolagens->processo == 0){
                        itDecolagens->horainicio = tempo;
                        itDecolagens->horafinal = tempo+10;
                        pistaA->horarioUsado = tempo+10;
                        strcpy(itDecolagens->pista, "Pista A");
                        itDecolagens = itDecolagens->prox;
                    }else{
                        itDecolagens = itDecolagens->prox;
                    }
                }
            }else{
                if(itAprox != NULL){
                    if(itAprox->processo == 0){
                        itAprox->horainicio = tempo;
                        itAprox->horafinal = tempo+20;
                        pistaA->horarioUsado = tempo+20;
                        strcpy(itAprox->pista, "Pista A");
                        itAprox = itAprox->prox;
                    }else{
                        itAprox = itAprox->prox;
                    }
                }
            }
        }

        if(pistaB->horarioUsado < tempo){

            if(checaNumVoos(aprox) == 0){
                if(itDecolagens != NULL){
                    if(itDecolagens->processo == 0){
                        itDecolagens->horainicio = tempo;
                        itDecolagens->horafinal = tempo+10;
                        pistaB->horarioUsado = tempo+10;
                        strcpy(itDecolagens->pista, "Pista B");
                        itDecolagens = itDecolagens->prox;
                    }else{
                        itDecolagens = itDecolagens->prox;
                    }
                }
            }else{
                if(itAprox != NULL){
                    if(itAprox->processo == 0){
                        itAprox->horainicio = tempo;
                        itAprox->horafinal = tempo+20;
                        pistaB->horarioUsado = tempo+20;
                        strcpy(itAprox->pista, "Pista B");
                        itAprox = itAprox->prox;
                    }else{
                        itAprox = itAprox->prox;
                    }
                }
            }
        }

        if(pistaC->horarioUsado < tempo){
            if(emergencias > 2){
                if(itAprox != NULL){
                    if(itAprox->processo == 0){
                        itAprox->horainicio = tempo;
                        itAprox->horafinal = tempo+20;
                        pistaC->horarioUsado = tempo+20;
                        strcpy(itAprox->pista, "Pista C");
                        itAprox = itAprox->prox;
                    }else{
                        itAprox = itAprox->prox;
                    }
                }
            }
            else if(itDecolagens != NULL){
                if(itDecolagens->processo == 0){
                    itDecolagens->horainicio = tempo;
                    itDecolagens->horafinal = tempo+10;
                    pistaC->horarioUsado = tempo+10;
                    strcpy(itDecolagens->pista, "Pista C");
                    itDecolagens = itDecolagens->prox;
                }else{
                    itDecolagens = itDecolagens->prox;
                }
            }
        }

        if(tempo == tempoAux){
            aprox = reduzCombustivel(aprox);
            decolagens = reduzCombustivel(decolagens);
            tempoAux = tempo + (10*TEMPO_UNIDADE);
        }

        tempo+=TEMPO_UNIDADE;
    }

    return voos;
}


lista *geraVoo(char **codigos,int numCodigo, int combustivel, int tipo){
    lista *new = (lista *) malloc(sizeof(lista));

    new->pista = (char *) malloc(10*(sizeof(char)));
    
    new->codigo = (char *) malloc(10*(sizeof(char)));
    strcpy(new->codigo, codigos[numCodigo]);


    new->combustivel = combustivel;
    new->tipo = tipo;

    new->processo = 0;
    new->horainicio = -1;
    new->horafinal = 10000000;
    new->status = (char *) malloc(100*(sizeof(char)));

    new->prox = NULL;

    return new;
}


lista *insereLista(lista *l, lista *new){
    if(l == NULL){
        return new;
    }

    new->prox = l;

    return new;
}


lista *checaVoos(lista *voos, int tempo){
    lista *it;
    for(it = voos; it != NULL; it = it->prox){
        if(it->tipo == 0){
            if(it->horafinal <= tempo){
                if(it->processo == 0){
                    it->processo = 1;
                    strcpy(it->status,"aeronave pousou");
                }
            }
        }else{
            if(it->horafinal <= tempo){
                if(it->processo == 0){
                    it->processo = 1;
                    strcpy(it->status,"aeronave decolou");
                }
            }
        }
    }

    return voos;
}


lista *reduzCombustivel(lista *voos){
    lista *it;
    for(it = voos; it != NULL; it = it->prox){
        if(it->processo == 0){
            it->combustivel-=1;
            if(it->combustivel < 0 && it->tipo == 0){
                strcpy(it->status, "ALERTA CRÍTICO, AERONAVE IRÁ CAIR");
                printf("CAIU\n");
                it->processo = 1;
            }
        }
    }

    return voos;
}



lista *copiaElemento(lista *it){
    lista *aux = (lista *) malloc(sizeof(lista));

    aux->codigo = it->codigo;
    aux->status = it->status;
    aux->combustivel = it->combustivel;
    aux->tipo = it->tipo;
    aux->pista = it->pista;
    aux->processo = it->processo;
    aux->horainicio = it->horainicio;
    aux->horafinal = it->horafinal;
    aux->prox = NULL;

    return aux;
}


fila *insereInicioFila(fila *f, lista *it){
    if(f->ini == NULL){
        f->ini = it;
        f->fim = it;

        return f;
    }

    it->prox = f->ini;
    f->ini = it;

    return f;
}


fila *geraFilaAprox(lista *voos){
    fila *f = (fila *) malloc(sizeof(fila));
    f->ini = NULL;
    f->fim = NULL;
    lista *aux;
    
    lista *it;
    for(it = voos; it != NULL; it = it->prox){
        if(it->tipo == 0){
            aux = copiaElemento(it);
            f = insereFinalFila(f, aux);
        }
    }

    return f;
}


fila *geraFilaDecolagens(lista *voos){
    fila *f = (fila *) malloc(sizeof(fila));
    f->ini = NULL;
    f->fim = NULL;
    
    lista *it,*aux;
    for(it = voos; it != NULL; it = it->prox){
        aux = copiaElemento(it);
        if(it->tipo == 1){
            f = insereFinalFila(f, aux);
        }
    }

    return f;
}


fila *ordenaAproxPorComb(lista *voos){
    
    lista *it;
    fila *newVoos =  (fila *) malloc(sizeof(fila));
    newVoos->ini = NULL;
    newVoos->fim = NULL;

    for(it = voos; it != NULL; it = it->prox){
        lista *aux = copiaElemento(it);
        
        if(it->combustivel == 0 && it->tipo == 0){
            newVoos = insereInicioFila(newVoos, aux);
        }else{
            newVoos = insereFinalFila(newVoos, aux);
        }
    }
    return newVoos;
}


fila *insereFinalFila(fila *f, lista *it){
    if(f->fim == NULL){
        f = insereInicioFila(f, it);

        return f;
    }
    
    f->fim->prox = it;
    it->prox = NULL;
    f->fim = it;

    return f;
}


fila *geraDadosAleatorios(int *num_Voos, int *num_Aprox){
    srand(time(NULL));

    int numVoos = geraNumAleatorio(20,64);
    int numAprox = geraNumAleatorio(10,(numVoos-10));
    int numDecola = (numVoos - numAprox);
    int contAprox = 0;
    int contDeco = 0;

    *(num_Voos) = numVoos;
    *(num_Aprox) = numAprox;

    lista *voos = listaVazia();
    
    int *codigosUsados = (int *) calloc(64, sizeof(int));

    char **codigos = lerCodigosVoos();

    // sorteia codigo voos e combustivel
    for(int i = 0; i < numVoos; i++){
        // sortei codigo
        int flag = 0;
        int numCodigo;
        lista *newVoo;

        while(flag == 0){
            numCodigo = geraNumAleatorio(0,63);
            if(codigosUsados[numCodigo] == 0){
                codigosUsados[numCodigo] = 1;
                flag = 1;
            }
        }
        int tipo = geraNumAleatorio(0,1);
        int combustivel;
        if(tipo == 0 && (contAprox < numAprox)){
            combustivel = geraNumAleatorio(0,12);
            contAprox++;
        }else if(tipo == 0 && (contAprox >= numAprox)){
            tipo = 1;
            combustivel = 0;
            contDeco++;
        }else if(tipo == 1 && (contDeco < numDecola)){
            combustivel = 0;
            contDeco++;
        }else{
            tipo = 0;
            combustivel = geraNumAleatorio(0,12);
            contAprox++;
        }

        newVoo = geraVoo(codigos, numCodigo, combustivel, tipo);
        voos = insereLista(voos, newVoo);

    }
    printaCabecalho(numVoos,numAprox, voos);
    fila *newVoos = ordenaAproxPorComb(voos);

    liberaMatrix(codigos,64);
    free(codigosUsados);

    return newVoos;
}


pista *pistaStart(int tempo){
    pista *p = (pista *) malloc(sizeof(pista));
    if(p == NULL){
        printf("Erro de alocacao\n");
        exit(-1);
    }

    p->horarioUsado = (tempo-1);
    
    return p;
}
