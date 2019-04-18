#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    int current_state; //stato iniziale 
    char input;		//lettura in input 
    char overwrite;	//Riscrivo su nastro 
    char move; //mossa della testina (R,L,S)
    int next_state; //stato successivo 
} transaction;

typedef struct el{
    transaction tr;
    struct el * next;
}tr_node;

typedef  tr_node *tr_list; //puntatore al primo nodo della lista
tr_list list = NULL;	//inizializzo lista globale per gestione transazioni

tr_list head_Add (tr_list, transaction);
int read_Input_tr();

int main(){
	int return_code;

	return_code = read_Input_tr();

	printf ("%d", return_code);
	
	return 0;
}

tr_list head_Add (tr_list list, transaction t){ //funzione che aggiunge in testa alla lista le nuove transizioni lette in input

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


int read_Input_tr(){
    //devo verificare di leggere tr
    char flag[4];
    int error = 0;
    transaction temp_tr;

    scanf("%s", flag);

    if (strcmp(flag, "tr") == 0){ //inizio  a leggere input tr
    	do{
    		error = scanf("%d %c %c %c %d", &temp_tr.current_state, &temp_tr.input, &temp_tr.overwrite, &temp_tr.move, &temp_tr.next_state);
    		list = head_Add(list, temp_tr);

            //printf ("\nscanf = %d", error);
    	}while(error == 5);

    	return 1; //codice di avvvnuta lettura e memorizzazione corretta della parte tr del file di input.
    }

    else{
    	return 0; //return codice errore 0;
    }
}