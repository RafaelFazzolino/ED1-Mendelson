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

char **lerCodigosVoos();
void liberaMatrix(char **,int);
void *listaVazia();
void printaCabecalho(int, int, lista *);
fila *insereFinalFila(fila *, lista *);
fila *geraFilaAprox(lista *);
int geraNumAleatorio(int, int);
lista *geraVoo(char **,int, int, int);
lista *insereLista(lista *, lista *);
fila *insereInicioFila(fila *, lista *);
lista *copiaElemento(lista *);
fila *ordenaAproxPorComb(lista *);
fila *geraDadosAleatorios(int *, int *);
lista *reduzCombustivel(lista *);
int checaNumVoos(lista *);
int checaEmergencia(lista *);
pista *pistaStart(int);
lista *checaVoos(lista *, int);
lista *definirPistas(lista *, lista *, lista *,int);
fila *geraFila(lista *);


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


void liberaMatrix(char **codigos,int size){
    for(int i = 0; i < size; i++){
        free(codigos[i]);
    }
    free(codigos);
}

void *listaVazia(){
    return NULL;
}


void printaCabecalho(int numVoos, int numAprox, lista *voos){
    printf("---------------------------------------------------------------------------------\n");
    printf("“Aeroporto Internacional de Brasília”\n");    
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
    printf("NVoos: %d\n", numVoos);
    printf("Naproximações: %d\n", numAprox);
    printf("---------------------------------------------------------------------------------\n");
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


int geraNumAleatorio(int inicio, int final){
    int numero = (rand() % (final - inicio + 1)) + inicio;

    return numero;
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


fila *geraDadosAleatorios(int *num_Voos, int *num_Aprox){
    srand(time(NULL));

    int numVoos = geraNumAleatorio(20,64);
    int numAprox = geraNumAleatorio(10,(numVoos-10));
    int numDecola = (numVoos - numAprox);
    int contAprox = 0;
    int contDeco = 0;

    *(num_Voos) = numVoos;
    *(num_Aprox) = numAprox;
    
    printf("%d voos => %d Aproximacoes - %d Decolagens\n", numVoos,numAprox,numDecola);

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


pista *pistaStart(int tempo){
    pista *p = (pista *) malloc(sizeof(pista));
    if(p == NULL){
        printf("Erro de alocacao\n");
        exit(-1);
    }

    p->horarioUsado = (tempo-1);
    
    return p;
}


// lista *avancLista(lista *voos, int num){
//     lista *it = voos;
//     for(int i = 0; i < num; i++){
//         it = it->prox;
//     }

//     return it;
// }


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


lista *definirPistas(lista *voos,lista *aprox, lista *decolagens, int tempo){
    
    pista *pistaA = pistaStart(tempo);
    pista *pistaB = pistaStart(tempo);
    pista *pistaC = pistaStart(tempo);

    int tempoAux = tempo + (10*TEMPO_UNIDADE);
    int num = 1;
    int num2;
    lista *itAprox = aprox;
    lista *itDecolagens = decolagens;
    
    while((checaNumVoos(aprox) + checaNumVoos(decolagens)) > 0){
        num = checaNumVoos(aprox);
        num2 = checaNumVoos(decolagens);
        
        printf("%d <-> %d\n",num,num2);

        aprox = checaVoos(aprox, tempo);
        decolagens = checaVoos(decolagens, tempo);

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
            if(itDecolagens != NULL){
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


fila *geraFila(lista *voos){
    fila *new = (fila *) malloc(sizeof(fila));
    
    // inicio
    new->ini = voos;
    
    // final
    lista *it;
    for(it = voos; it != NULL; it = it->prox){
        new->fim = it;
    }

    return new;
}


int main(){
    fila *v;
    fila *a;
    fila *d;
    // lista **p;
    int num_Voos,num_Aprox;

    v = geraDadosAleatorios(&num_Voos, &num_Aprox);
    a = geraFilaAprox(v->ini);
    d = geraFilaDecolagens(v->ini);
    
    v->ini = definirPistas(v->ini, a->ini, d->ini, 600);

    // lista *it;
    // for(it = a->ini; it != NULL; it = it->prox){
    //     printf("(*) - %s\n",it->codigo);
    // }

    // lista *dt;
    // for(dt = d->ini; dt != NULL; dt = dt->prox){
    //     printf("(#) - %s\n",dt->codigo);
    // }
    

    // Free filas
    lista *at;
    for(at = a->ini; at != NULL; at = at->prox){
        free(at->codigo);
        free(at->status);
        free(at);
    }

    lista *dt;
    for(dt = d->ini; dt != NULL; dt = dt->prox){
        free(dt->codigo);
        free(dt->status);
        free(dt);
    }

    return 0;
}
