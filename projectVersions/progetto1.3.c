#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FLAG_DIM 4
#define BLANK_SIZE 10

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

//dichiarazione array dinamico per gestione delle transizioni
typedef struct{
    tr_list* list;   //tr_list definito da typedef come
    int dim;
}arrayDinamico_tr;
arrayDinamico_tr tr_Array;

//array dinamico per gestione degli stati di accettazione
typedef struct {
    int* array;
    int dim;
}arrayDinamico_int;
arrayDinamico_int acc;

//gestione del massimo numero di computazioni
unsigned long int max;

//struttura dati che gestisce il nastro
typedef struct{
    char* right;
    char* left;
} def_tape;

def_tape tape;

int machineState = 0;

//prototipi di tutte le funzioni implementate
tr_list headAdd (tr_list, transaction);
int readInput_tr();
int readInput_acc();
int readInput_max();
void stampa_tr();
void moveMachine();
void runMachine();
char* getString();
char* initializeTape();
int trLookup(int , char);
int accLookup(int);

//main function
int main(){
	int returnCode_tr, returnCode_acc;
    int returnCode_max;

	returnCode_tr = readInput_tr();
    //stampa_tr();
    if(returnCode_tr == 1){
        returnCode_acc = readInput_acc();
        if(returnCode_acc == 1){
            returnCode_max = readInput_max();

            if(returnCode_max == 1)
                runMachine();
        }
        
    }
    /*
    printf("\nStati di accettazione:");
    for(i = 0; i < acc.dim; i++){
        printf("\n%d", acc.array[i]);
    }

	printf ("\n\nTransizioni: %d \nAccettazioni: %d \nMax: %d", returnCode_tr, returnCode_acc,returnCode_max);
    printf("\n\nMassimo numeri di computazioni: %lu", max);    
    */
	return 0;
}

//funzione che aggiunge in testa alla lista passata le nuove transizioni lette in input
tr_list headAdd (tr_list list, transaction t){ 
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

void runMachine(){
    char flag[FLAG_DIM];
    int scanfReturn = 0;

    tape.right = NULL;
    tape.left = NULL;

    scanfReturn = scanf("%s", flag);

    if(strcmp(flag, "run") == 0 &&scanfReturn == 1){
        do{
            tape.right = getString();
            if(tape.right != NULL){ //se succede significa che il file è finito, pertanto il programma termina
                tape.left = initializeTape();
                printf("%s%s\n",tape.left,tape.right);
                //moveMachine(); //moveMachine si occupa di processare la stringa appena letta. Eseguita per ogni str

            }
        }while(tape.right != NULL);     
    }

    //dopo questa funzione il programma termina.
}

char* getString(){ //la funzione restituisce la stringa con BLANK_SIZE caratteri blank in coda
    int scanfReturn = 0;
    char* string = NULL;
    int len = 0, i = 0;

    scanfReturn = scanf("%ms", &string);
    if (scanfReturn != EOF){
        len = strlen(string);
        string = (char*) realloc (string, ((len + BLANK_SIZE)*sizeof(char)));

        //aggiungo in coda alla striga BLANK_SIZE blank per sicurezza
        for (i = len; i < len + BLANK_SIZE; i++)
            string[i] = '_';
        
        return string;
    }

    return NULL; //se sono arrivato alla fine del file restituisco NULL (valenza di EOF);
}

//funzione che inizializza la parte sinistra del tape con BLANK_SIZE caratteri blank
//NB per convenzione il tape sinistro ha indici negativi !

char* initializeTape(){
    int i = 0;
    char* string;

    string = (char*) malloc (BLANK_SIZE * sizeof(char));

    for(i = 0; i < BLANK_SIZE; i++)
        string[i] = '_';
    

    return string;
}

/*
transaction my_tr;

void moveMachine(){
    machineState = 0;
    int i = 0; // contatore
    int end = 0;
    int returnCode_trLookup = 0;
    int returnCode_accLookup = 0;
    int count = 0;

    while(end == 0 && count < max){
        returnCode_accLookup = accLookup(machineState);
        if(returnCode_accLookup == 1){
            end = 1;
        }
        else{
            returnCode_trLookup = trLookup(machineState, tape.right[i]);
            if (returnCode_trLookup == 1){

                if(my_tr.overwrite != '_')
                    tape.right[i] = my_tr.overwrite;
                else
                    tape.right[i] = '_';

                if(my_tr.move == 'L')
                    i--;

                if(my_tr.move == 'R')
                    i++;

                machineState = my_tr.next_state;
                count++;

            }
        }

    }
    if(count > max)
        printf("U\n");
    else{
        if(end == 1)
            printf("1\n");
        if (end == 0)
            printf("0\n"); 
    }
}

int trLookup(int state, char c){
    tr_node* curr;

    curr = tr_Array.list[state];
    do{
        if(curr->tr.current_state == state && curr->tr.input == c){
            my_tr.current_state = curr->tr.current_state;
            my_tr.next_state = curr->tr.next_state;
            my_tr.overwrite = curr->tr.overwrite;
            my_tr.input = curr->tr.input;
            my_tr.move = curr->tr.move;

            return 1;
        }
        curr = curr->next;
    }
    while(curr->tr.input != c);

    return 0;

}


int accLookup(int state){
    int i;
    for(i = 0; i < acc.dim; i++){
        if(acc.array[i] == state)
            return 1;
    }

    return 0;
}

*/