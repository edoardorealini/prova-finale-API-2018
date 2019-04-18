#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FLAG_DIM 4
#define BLANK_SIZE 256

//dichiarazione liste dinamiche
typedef struct {
    int current_state; //stato iniziale 
    char input;     //lettura in input 
    char overwrite; //Riscrivo su nastro 
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
    int rightDim;
    int leftDim;
} def_tape;

def_tape tape;

unsigned int machineState = 0;

//struttura dati per gestire le configurazioni dei rami non deterministici
typedef struct{  
    int index;      //indice sul tape
    int rightorleft;//flag per sapere se sono a destra o sinistra
    int state;      //stato corrente della macchina quando fermata
    int det; //flag che indica se il ramo (transizione singola da eseguire) è determ
    int tr_id; //mi indica quale tr (in ordine)devo computare (per ND)
    unsigned long int dimension; //paramentro che serve per le realloc.
    int execute;
    def_tape tape;  //tape
}configuration;

typedef struct{
    configuration* array;
    int dim;
}arrayDinamico_config;
arrayDinamico_config config;

//prototipi di tutte le funzioni implementate
tr_list headAdd (tr_list, transaction);
int readInput_tr();
int readInput_acc();
int readInput_max();
void stampa_tr();
int moveMachine();
void runMachine();
char* getrightString();
char* initializeleftTape();
int isDeterministic(int, char);
int trLookup(int, char, int);
int accLookup(int);
char* strStretch(char*, int, int);
int moveExec(int k);
void addConfig(int k, int n);
void configFree();

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

            if(returnCode_max == 1){
                //stampa_tr();
                runMachine();
            }
        }
        
    }
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

int readInput_tr(){ 
    char flag[FLAG_DIM];
    int scanfReturn = 0;
    int new_Dim = 0;
    transaction temp_tr;

    scanf("%s", flag);

    if (strcmp(flag, "tr") == 0){
        //alloco subito il primo elemento dell' array dinamico array[0]
        tr_Array.list = (tr_list*) malloc (100*sizeof(tr_node*)); //MODIFICATA LA DIMENSIONE DI PARTENZA A 100
        tr_Array.dim = 100;  //MODIFICATA LA DIMENSIONE DI PARTENZA A 100;
        tr_Array.list[0] = NULL;
        scanfReturn = scanf("%d %c %c %c %d", &temp_tr.current_state, 
                &temp_tr.input, &temp_tr.overwrite, &temp_tr.move, &temp_tr.next_state);
        do{           
            //se trovo transizione con id alto, devo riallocare per creare cella per inserirla
            if (scanfReturn == 5 && temp_tr.current_state + 1 > tr_Array.dim) {
                new_Dim = (temp_tr.current_state + 1); //dimensione array aggiornata
                tr_Array.list = (tr_list*) realloc(tr_Array.list, new_Dim*sizeof(tr_node*));
                tr_Array.dim = new_Dim;
                tr_Array.list[temp_tr.current_state] = NULL;
            }
            //in ogni caso: aggiungo nel posto corretto la transizione appena letta in input con funzione headAdd
            tr_Array.list[temp_tr.current_state] = headAdd(tr_Array.list[temp_tr.current_state], temp_tr);
            scanfReturn = scanf("%d %c %c %c %d", &temp_tr.current_state, 
                &temp_tr.input, &temp_tr.overwrite, &temp_tr.move, &temp_tr.next_state);
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

    for(i = 0; i < tr_Array.dim; i++){
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
    int moveReturn = 0;

    tape.right = NULL;
    tape.left = NULL;

    scanfReturn = scanf("%s", flag);

    if(strcmp(flag, "run") == 0 && scanfReturn == 1){
        do{
            tape.right = getrightString();
            if(tape.right != NULL){ //se succede significa che il file è finito, pertanto il programma termina
                tape.left = initializeleftTape();
                //printf("%s%s\n",tape.left,tape.right);
                moveReturn = moveMachine(); //moveMachine si occupa di processare la stringa appena letta. Eseguita per ogni str   
                if(moveReturn == 1)
                    printf("1\n");
                if(moveReturn == -1)
                    printf("U\n");
                if(moveReturn == 0)
                    printf("0\n"); 

                //config.dim = 0;
                configFree();
                #ifdef FREE
                	printf("free OK | address config: %p\n", config.array);
                	printf("config.dim: %d\n", config.dim);
                #endif     
            }
        }while(tape.right != NULL);     
    }

    //dopo questa funzione il programma termina.
}

transaction my_tr; //transizione temporanea

int moveMachine(){ //l'idea è gestire piu macchine in contemporanea
    //per prima cosa genero una nuova configurazione, sempre
    config.array = (configuration*) malloc (sizeof(configuration));

    config.array[0].index = 0;
    config.array[0].rightorleft = 1; //parto sempre da destra
    config.array[0].state = 0; //corrisponde al vecchio machine state
    
    config.array[0].tape.right = (char*) malloc ((tape.rightDim)*sizeof(char));
    config.array[0].tape.right = strcpy(config.array[0].tape.right,tape.right); //parto sempre da destra
    config.array[0].tape.rightDim = tape.rightDim;

    config.array[0].tape.left = (char*) malloc ((tape.leftDim)*sizeof(char));
    config.array[0].tape.left = strcpy(config.array[0].tape.left,tape.left); //parto sempre da destra
    config.array[0].tape.leftDim = tape.leftDim;

    config.array[0].det = 0; //valore default deterministic settato a 0
    config.dim = 1;
    config.array[0].tr_id = 1; //inizializzo sempre alla prima che trovo, caso piu semplice, gestisce anche det. semplice

    config.array[0].dimension = 6*sizeof(int) + tape.rightDim + tape.leftDim + sizeof(long int);
    config.array[0].execute = 1;

    //qui ho solo una configurazione la ZERO, parto a computare tutto l'array di config
    //ipotesi deterministica
    int k = 0; //indice per muovermi tra le configurazioni
    int moves = 0;
    int accReturn = 0;
    int moveReturn = 0;
    int deterministic = 0;
    int zeroReturns = 0;
    //tutto va portato avanti finche non ottengo 1, 0 oppure tutte U (capire ancora come gestire tutte U)
    do{
        k = 0;
        while(k < config.dim && accReturn == 0){//computazione della singola configurazione
            //qui gestisco la computazione deterministica
            //se trovo altro bivio ND creo una nuova config aggiornata da eseguire e la metto in coda.
            //in questo passaggio computo già entrambe le  vie ND
            deterministic = 0;
            accReturn = accLookup (config.array[k].state); //controllo subito di essere in accettazione
            if(accReturn == 1){
                return 1;
            }
            if(accReturn == 0){
                //qui prima di computare verifico che la transizione non sia ND
                //se lo è chiamo la funzione che genera nuova config. e aggiorno la dim delle configurazioni.

                if(config.array[k].rightorleft == 1 && config.array[k].det == 0)
                    deterministic = isDeterministic(config.array[k].state, config.array[k].tape.right[config.array[k].index]);
                if(config.array[k].rightorleft == 0 && config.array[k].det == 0)
                    deterministic = isDeterministic(config.array[k].state, config.array[k].tape.left[config.array[k].index]);

                if(deterministic == -1 && config.array[k].execute == 1){
                    config.array[k].execute = 0; //significa che non devo più eseguire questa config
                    zeroReturns++;
                    //faccio direttamente anche le free
                    free(config.array[k].tape.right);
                    free(config.array[k].tape.left);
                    #ifdef FREEDBG
                    	printf("1 . free executed correctly\n");
                    	printf("config dim: %d \nzeroReturns: %d\n moves: %d\nmaxMoves: %lu\n\n", config.dim, zeroReturns, moves, max);
                    #endif
                }
                
               if(deterministic > 1){  //ho trovato piu transizioni per la configurazione in cui mi trovo ora
                                        //devo creare nuove config, quante? il numero che c'è in deterministic - 1 (una c'è gia)
                                        //alla funzione che genera le nuove confg passo anche k, deve sapere cosa copiare.
                    addConfig(k, deterministic - 1);
                }
                if(config.array[k].execute == 1){
                    config.array[k].det = 1;
                    moveReturn = moveExec(k);
                    if(moveReturn == 0){ //se arrivo a trovare zero, smetto di eseguire questa configurazione
                        config.array[k].execute = 0;
                        zeroReturns++;
                        free(config.array[k].tape.right); //libero il grosso della memoria occupato dagli array.
                    	free(config.array[k].tape.left);
                    	#ifdef FREEDBG
                    		printf("2 . free executed correctly\n");
                    		printf("config dim: %d \nzeroReturns: %d\n moves: %dmaxMoves: %lu\n\n", config.dim, zeroReturns, moves, max);
                    	#endif
                    }
                }
                
                k++;
                
            }
        }

        moves++;
    }while(moves<max && accReturn==0 && zeroReturns!=config.dim);    //per ogni ciclo di mossa singola devo provare tutte le configurazioni
        //controllo max mosse + stato di accettazione + presenza stato.
    if(moves >= max){
        return -1;
    }

    return 0; //esegue solo se moveReturn = 0 per tutte le vie ND.
}

void addConfig(int k, int n){ //k =  indice config da copiare, n = numero di volte che devo copiarla.
    configuration* tempConfig = NULL;
    unsigned int totalDimension = 0;
    int i = 0;

    for(i = 0; i < config.dim; i++)
        totalDimension = totalDimension + config.array[i].dimension;

    #ifdef ADDCONFIG
        printf("total dimension:%u\n",totalDimension);
        printf("new part:%lu\n",n*config.array[k].dimension);
        printf("total dimension + new part:%lu\n",totalDimension + n*config.array[k].dimension);

    #endif
    tempConfig = (configuration*) realloc (config.array, (totalDimension + n*config.array[k].dimension));

    #ifdef ADDCONFIG
        printf("realloc in addConfig OK\n" );
    #endif

    if(tempConfig == NULL) //errore della realloc gestito
        printf("ERROR IN ADDCONFIG REALLOC\n");
    else
        config.array = tempConfig;

    int counter = 2;
    for(i = config.dim; i < config.dim + n; i++){
        config.array[i].index = config.array[k].index;
        config.array[i].rightorleft = config.array[k].rightorleft; 
        config.array[i].state = config.array[k].state;

        config.array[i].tape.right = (char*) malloc ((config.array[k].tape.rightDim)*sizeof(char));
        config.array[i].tape.right = strcpy(config.array[i].tape.right, config.array[k].tape.right);
        config.array[i].tape.rightDim = config.array[k].tape.rightDim;

        config.array[i].tape.left = (char*) malloc ((config.array[k].tape.leftDim)*sizeof(char));
        config.array[i].tape.left = strcpy(config.array[i].tape.left, config.array[k].tape.left);
        config.array[i].tape.leftDim = config.array[k].tape.leftDim;

        config.array[i].execute = 1;
        config.array[i].det = 1; //genero sicuramente tutti rami che iniziano con una tr deterministica 
        config.array[i].tr_id = counter; //ordina le tr_id per eseguire la tr corretta.

        config.array[i].dimension = config.array[k].dimension;
        counter ++;

    }

    config.dim = config.dim + n;

}

int moveExec(int k){ //funzione che computa la k-esima configurazione
    //deterministica!
    int i = 0;
    int j = 0;
    int returnCode_trLookup = 0;

    if(config.array[k].rightorleft == 1){//right
        i = config.array[k].index;
        returnCode_trLookup = trLookup(config.array[k].state, config.array[k].tape.right[i], config.array[k].tr_id);
    }
        
    if(config.array[k].rightorleft == 0){//left
        j = config.array[k].index;
        returnCode_trLookup = trLookup(config.array[k].state, config.array[k].tape.left[j], config.array[k].tr_id);
    }
         

    if (returnCode_trLookup == 1){ //in mt_tr ho la transizione da eseguire, aggiorno le variabili
                
        switch(config.array[k].rightorleft){    
            case 0: //sono nel lato sx del nastro, uso j
                config.array[k].state = my_tr.next_state;
                config.array[k].det = 0; // devo verificare se dal nuovo stato sono det o no
                config.array[k].tr_id = 1;

                if(config.array[k].tape.left[j] != my_tr.overwrite)
                    config.array[k].tape.left[j] = my_tr.overwrite;

                    if(my_tr.move == 'R'){
                        if(j == 0){
                            config.array[k].index = 0;
                            config.array[k].rightorleft = 1;    
                    }
                    else 
                        config.array[k].index--;
                    }

                    if(my_tr.move == 'L'){
                        if(j == config.array[k].tape.leftDim - 2){
                            config.array[k].tape.left = strStretch(config.array[k].tape.left, config.array[k].rightorleft, k);
                            #ifdef FREEDBG
                            	printf("strStretch OK left\n");
                            #endif
                        }
                        
                        config.array[k].index++; //gestito al contrario
                    }
            break;

            case 1: //sono nel lato dx del nastro, uso i    
                config.array[k].state = my_tr.next_state;
                config.array[k].det = 0;      
                config.array[k].tr_id = 1;

                if(config.array[k].tape.right[i] != my_tr.overwrite)
                    config.array[k].tape.right[i] = my_tr.overwrite;

                if(my_tr.move == 'R'){
                    if(i == config.array[k].tape.rightDim - 2){
                        config.array[k].tape.right = strStretch(config.array[k].tape.right, config.array[k].rightorleft, k);
                        #ifdef FREEDBG
                           	printf("strStretch OK right\n");
                        #endif
                    }

                    config.array[k].index++;
                }
                            
                if(my_tr.move == 'L'){
                    if(i == 0){
                        config.array[k].rightorleft = 0;
                        config.array[k].index = 0;
                    }
                    else
                        config.array[k].index --;
                }       
            break;
        }
        return 1; //se ho computato
                    
    }
    return 0; //se non ho trovato tr --> termina con zero!

}

char* getrightString(){ //la funzione restituisce la stringa con BLANK_SIZE caratteri blank in coda
    int scanfReturn = 0;
    char* string = NULL;
    int len = 0, i = 0;

    scanfReturn = scanf("%ms", &string);

    if (scanfReturn != EOF){

        len = strlen(string);
        len ++;
        string = (char*) realloc (string, ((len + BLANK_SIZE + 1)*sizeof(char)));
        #ifdef ADDCONFIG
        printf("realloc in Getstring OK\n" );
        #endif
        //aggiungo in coda alla striga BLANK_SIZE blank per sicurezza
        for (i = len - 1; i < len - 1 + BLANK_SIZE; i++)
            string[i] = '_';
        
        string[len + BLANK_SIZE] = '\0';

        tape.rightDim = len + BLANK_SIZE + 1;

        return string;
    }

    return NULL; //se sono arrivato alla fine del file restituisco NULL (valenza di EOF);
}

//funzione che inizializza la parte sinistra del tape con BLANK_SIZE caratteri blank
//NB per convenzione il tape sinistro ha indici negativi !

char* initializeleftTape(){
    int i = 0;
    char* string;

    string = (char*) malloc ((BLANK_SIZE + 1) * sizeof(char));

    for(i = 0; i < BLANK_SIZE; i++)
        string[i] = '_';

    string[BLANK_SIZE] = '\0';

    tape.leftDim = BLANK_SIZE + 1;

    return string;
}

int isDeterministic(int state, char c){
    tr_node* curr;
    curr = tr_Array.list[state];
    int counter = 0;
    if(curr != NULL){
        do{
            if(curr->tr.input == c)
                counter ++;

            curr = curr->next;
        }
        while(curr != NULL);
    }

    if(counter == 1)
        return 1; //transizione deterministica

    if(counter > 1)
        return counter;  //se trovo non determinismo (counter > 0) 

    //ritorno il valore di transizioni trovate (numero di percorsi che devo eseguire)

    return -1; //caso peggiore, non trovo transizioni return -1

}

int trLookup(int state, char c, int tr_id){
    tr_node* curr;
    int counter = 1;
    curr = tr_Array.list[state];
    if(curr != NULL){
        do{
            if(curr->tr.current_state == state && curr->tr.input == c && counter == tr_id){
                my_tr.current_state = curr->tr.current_state;
                my_tr.next_state = curr->tr.next_state;
                my_tr.overwrite = curr->tr.overwrite;
                my_tr.input = curr->tr.input;
                my_tr.move = curr->tr.move;
                return 1;
            }

            if(curr->tr.current_state == state && curr->tr.input == c)
                counter++;

            curr = curr->next;
        }while(curr != NULL);
    }

    return 0;
}

int accLookup(int state){
    int i = 0;
    for(i = 0; i < acc.dim; i++){
        if(acc.array[i] == state)
            return 1;
    }

    return 0;
}

char* strStretch(char* string, int rightorleft, int k){ //CONTROLLARE, DA SEG FAULT in certi casi
    int i = 0; //contatore

    if(rightorleft == 0){ //left
        string = (char*) realloc (string, ((config.array[k].tape.leftDim + BLANK_SIZE)*sizeof(char)));
        #ifdef DEBUG
        printf("realloc in strStrerch OK\n" );
        #endif
        for (i = config.array[k].tape.leftDim - 1; i < config.array[k].tape.leftDim - 1 + BLANK_SIZE; i++)
            string[i] = '_';

        string[config.array[k].tape.leftDim + BLANK_SIZE] = '\0';

        #ifdef DEBUG
        printf("scrittura in strStrerch OK\n" );
        #endif

        config.array[k].tape.leftDim = config.array[k].tape.leftDim + BLANK_SIZE;
        config.array[k].dimension = config.array[k].dimension + BLANK_SIZE;
        return string;
    }
    if(rightorleft == 1){ //right
        string = (char*) realloc (string, ((config.array[k].tape.rightDim + BLANK_SIZE)*sizeof(char)));

        #ifdef DEBUG
        printf("realloc in strStrerch OK\n" );
        #endif

        for (i = config.array[k].tape.rightDim - 1; i < config.array[k].tape.rightDim - 1 + BLANK_SIZE; i++)
            string[i] = '_';

        string[config.array[k].tape.rightDim + BLANK_SIZE] = '\0';

        #ifdef DEBUG
        printf("scrittura in strStrerch OK\n" );
        #endif

        config.array[k].tape.rightDim = config.array[k].tape.rightDim + BLANK_SIZE;
        config.array[k].dimension = config.array[k].dimension + BLANK_SIZE;
        return string;
    }

    return NULL; //gestione caso di errore
}

void configFree(){
    int i = 0;  //contatore serve sempre
    for(i = 0; i < config.dim && config.array[i].execute == 1; i++){
        free(config.array[i].tape.right);
        free(config.array[i].tape.left);
    }
    free(config.array);
}