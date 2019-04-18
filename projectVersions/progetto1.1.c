#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//dichiarazione liste dinamiche

typedef struct {
    int current_state; //stato iniziale 
    char input;		//lettura in input 
    char overwrite;	//Riscrivo su nastro 
    char move; //mossa della testina (R,L,S)
    int next_state; //stato successivo 
}transaction;

typedef struct el{
    transaction tr;
    struct el* next;
}tr_node;

typedef  tr_node* tr_list; //puntatore al primo nodo della lista
/*
tr_list list = NULL;	//inizializzo lista globale per gestione transazioni
*/


//dichiarazione array dinamico
typedef struct{
    tr_list* list;   //tr_list definito da typedef come
    int dim;
}array_Dinamico_tr;
array_Dinamico_tr tr_Array;

//gestione degli stati di accettazione
typedef struct {
    int* array;
    int dim;
}array_Dinamico_int;
array_Dinamico_int acc;

//gestione del massimo numero di computazioni
unsigned long int max;

//prototipi di tutte le funzioni utilizzate
tr_list head_Add (tr_list, transaction);
int read_Input_tr();
void stampa_Tr();

//main function
int main(){
	int return_code;

	return_code = read_Input_tr();
    stampa_Tr();

	printf ("\n\n%d", return_code);
	
	return 0;
}


tr_list head_Add (){ 
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

int read_Input_tr(){ /*genera struttura dati a semi accesso diretto
                        costo creazione: lineare con il num di transizioni
                        costo utilizzo: pessimo lineare
                        medio: semi costante (quasi diretto)
                    */
    char flag[4];
    int scanf_Return = 0;
    int new_Dim = 0;
    transaction temp_tr;

    scanf("%s", flag);

    if (strcmp(flag, "tr") == 0){
        //alloco subito il primo elemento dell' array dinamico array[0]
        tr_Array.list = (tr_list*) malloc (sizeof(tr_node));
        tr_Array.dim = 1;
        tr_Array.list[0] = NULL;

        do{

            scanf_Return = scanf("%d %c %c %c %d", &temp_tr.current_state, 
                &temp_tr.input, &temp_tr.overwrite, &temp_tr.move, &temp_tr.next_state);
            //se trovo transizione con id alto, devo riallocare per creare cella per inserirla
            if (scanf_Return == 5 && temp_tr.current_state + 1 > tr_Array.dim) {
                new_Dim = (temp_tr.current_state + 1); //dimensione array aggiornata
                tr_Array.list = (tr_list*) realloc(tr_Array.list, new_Dim*sizeof(tr_node));
                tr_Array.dim = new_Dim;
                tr_Array.list[temp_tr.current_state] = NULL;
            }
            //in ogni caso: aggiungo nel posto corretto la transizione appena letta in input con funzione head_Add
            tr_Array.list[temp_tr.current_state] = head_Add(tr_Array.list[temp_tr.current_state], temp_tr);

        }while(scanf_Return == 5);

        return 1;
    }

    else
        return 0;
}

void stampa_Tr(){
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
