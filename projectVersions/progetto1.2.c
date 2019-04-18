#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FLAG_DIM 4

//dichiarazione liste dinamiche
typedef struct {
    int current_state; //stato iniziale 
    char input;		//lettura in input 
    char overwrite;	//Riscrivo su nastro 
    char move; //mossa della testina (R,L,S)
    int next_state; //stato successivo 
}transaction;
//nodo lista dinamica
typedef struct el{
    transaction tr;
    struct el* next;
}tr_node;

typedef tr_node* tr_list; //puntatore al primo nodo della lista
/*
tr_list list = NULL;	//inizializzo lista globale per gestione transazioni
*/

//dichiarazione array dinamico
typedef struct{
    tr_list* list;   //tr_list definito da typedef come
    int dim;
}arrayDinamico_tr;
arrayDinamico_tr tr_Array;

//gestione degli stati di accettazione
typedef struct {
    int* array;
    int dim;
}arrayDinamico_int;
arrayDinamico_int acc;

//gestione del massimo numero di computazioni
unsigned long int max;

//prototipi di tutte le funzioni utilizzate
tr_list headAdd (tr_list, transaction);
int readInput_tr();
int readInput_acc();
int readInput_max();
void stampa_tr();

//main function
int main(){
	int returnCode_tr, returnCode_acc, i;
    int returnCode_max;

	returnCode_tr = readInput_tr();
    stampa_tr();
    returnCode_acc = readInput_acc();
    returnCode_max = readInput_max();

    printf("\nStati di accettazione:");
    for(i = 0; i < acc.dim; i++){
        printf("\n%d", acc.array[i]);
    }

	printf ("\n\nTransizioni: %d \nAccettazioni: %d \nMax: %d", returnCode_tr, returnCode_acc,returnCode_max);
    printf("\n\nMassimo numeri di computazioni: %lu", max);
    
	return 0;
}


tr_list headAdd (tr_list list, transaction t){ 
//funzione che aggiunge in testa alla lista passata le nuove transizioni lette in input (da stdtin)
	tr_node* temp;
	temp = (tr_node*) malloc(sizeof(tr_node));

	temp->tr.current_state = t.current_state; //tr è nella lista - t arriva da input
	temp->tr.input = t.input;
	temp->tr.overwrite = t.overwrite;
	temp->tr.move = t.move;
	temp->tr.next_state = t.next_state;

	temp->next = list;

	return temp;
}

int readInput_tr(){ /*genera struttura dati a semi accesso diretto
                        costo creazione: lineare con il num di transizioni
                        costo utilizzo: pessimo lineare
                        medio: semi costante (quasi diretto)
                    */
    char flag[FLAG_DIM];
    int scanfReturn = 0;
    int new_Dim = 0;
    transaction temp_tr;

    scanf("%s", flag);

    if (strcmp(flag, "tr") == 0){
        //alloco subito il primo elemento dell' array dinamico array[0]
        tr_Array.list = (tr_list*) malloc (sizeof(tr_node*));
        tr_Array.dim = 1;
        tr_Array.list[0] = NULL;

        do{
            scanfReturn = scanf("%d %c %c %c %d", &temp_tr.current_state, 
                &temp_tr.input, &temp_tr.overwrite, &temp_tr.move, &temp_tr.next_state);
            //se trovo transizione con id alto, devo riallocare per creare cella per inserirla
            if (scanfReturn == 5 && temp_tr.current_state + 1 > tr_Array.dim) {
                new_Dim = (temp_tr.current_state + 1); //dimensione array aggiornata
                tr_Array.list = (tr_list*) realloc(tr_Array.list, new_Dim*sizeof(tr_node*));
                tr_Array.dim = new_Dim;
                tr_Array.list[temp_tr.current_state] = NULL;
            }
            //in ogni caso: aggiungo nel posto corretto la transizione appena letta in input con funzione headAdd
            tr_Array.list[temp_tr.current_state] = headAdd(tr_Array.list[temp_tr.current_state], temp_tr);
        }while(scanfReturn == 5);

        return 1;
    }

    else
        return 0;
}

int readInput_acc(){
    char flag[FLAG_DIM];
    int scanfReturn = 0;
    int tempAcc = 0;
    int i = 0;
    acc.dim = 0;

    acc.array = (int*) malloc (sizeof(int));
    scanf("%s", flag);

    if (strcmp(flag, "acc") == 0){ //sono nella sezione in cui trovo gli stati di acc
        
        do{
            scanfReturn = scanf("%d", &tempAcc);
            if(scanfReturn == 1){
                acc.dim++;
                acc.array = realloc(acc.array, acc.dim*sizeof(int));
                acc.array[i] = tempAcc;
                i++;  
            }
        }while(scanfReturn == 1);

        return 1;

    }

    else
        return 0;
}

int readInput_max(){
    char flag[FLAG_DIM];
    int scanfReturn = 0;

    scanf("%s", flag);

    if(strcmp(flag, "max") == 0){
        scanfReturn = scanf("%lu",&max);
        if(scanfReturn == 1)
            return 1;
    }

	return 0;
}

//PER VERIFICA CORRETTO SALVATAGGIO IN STRUTTURA DATI
void stampa_tr(){
    int i = 0;
    tr_node* curr;

    for(i = 0; i < 18; i++){
        curr = tr_Array.list[i];
        printf("\nTransizioni con stato iniziale %d\n", i);
        while (curr != NULL){
            printf("\n%d %c %c %c %d", curr->tr.current_state,curr->tr.input,
                curr->tr.overwrite,curr->tr.move,curr->tr.next_state);
            curr = curr->next;
        }
        printf("\n\n");
    }

}