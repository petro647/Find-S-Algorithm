
// TODO: permettere all' utente di poter reinserire l'ipotesi nel caso si fosse sbagliato

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define USER_BUF 100
#define ROW_BUF 500
#define TRUE 1
#define FALSE 0
#define PAUSE printf("\nPremere Invio per continuare... "); fflush(stdin); getchar()
#define NUMB_ATTR 11

#ifdef _WIN32
    #define CLS system("cls")
#else 
    #define CLS system("clear")
#endif

typedef int bool;

typedef struct hypotesis {
    char has_alternative[30];
    char has_bar[30];
    char weekend[30];
    char hungry[30];
    char crowded[30];
    char price[30];
    char raining[30];
    char reservation[30];
    char restaurant_type[30];
    char estimated_wait[30];
    char wait[30];

    struct hypotesis* next;
} hypotesis;
 
typedef struct most_spec_hypotesis {
    bool has_alternative;
    bool weekend;
    bool hungry;
    bool crowded;
    bool price;
    bool raining;
    bool reservation; 
    bool restaurant_type; 
    bool estimated_wait;
    bool wait;
} most_spec_hypotesis;

void training_phase(hypotesis* _dataset_hyp_head, hypotesis* _user_hyp_head, hypotesis** head_dataset_refer);
void init_dataset_hyp_linked_list(hypotesis* _dataset_hyp_head, bool* _trained);
void init_user_hyp_linked_list(hypotesis* _user_hyp_head);
void create_node_from_string(char* _attributes_buf, hypotesis* _new_hypotesys);
void create_string_from_input_attributes(char* _attributes_buf);
void create_dataset_linked_list(hypotesis* _dataset_hyp_head, FILE* _stream_dataset);
void print_training_options();

int main(){
    // Head della linked list generata dal dataset.
    hypotesis* dataset_hyp_head = (hypotesis*)malloc(sizeof(hypotesis));
    dataset_hyp_head->next = NULL;
        
    // Head della linked list di ipotesi aggiunte manualmente dall' utente.
    hypotesis* user_hyp_head = (hypotesis*)malloc(sizeof(hypotesis));
    user_hyp_head->next = NULL;

    PAUSE;
    training_phase(dataset_hyp_head, user_hyp_head, &dataset_hyp_head);
    
    // Stampa di prova di attributi delle ipotesi della lista
    hypotesis* aux = dataset_hyp_head;
    while(aux != NULL){
        printf("\nrestaurant type: %s", aux->restaurant_type );
        aux = aux->next;
    }
    PAUSE;

    return 0;
}

void training_phase(hypotesis* _dataset_hyp_head, hypotesis* _user_hyp_head, hypotesis** head_dataset_refer){
    char user_answer;
    int exit = 0;
    bool trained = FALSE;

    do {
        print_training_options();
        printf("\n>> ");
        fflush(stdin);
        user_answer = getchar();

        switch (user_answer){
            case '1': // Allenamento tramite dataset
                if(trained == FALSE){ // Verifico se è gia stato esguito l' allenamento.
                    init_dataset_hyp_linked_list(_dataset_hyp_head, &trained);

                    if(trained == TRUE){
                        fflush(stdout);
                        puts("Allenamento tramite dataset eseguito con successo.");
                        PAUSE;
                        break;
                    }
                
                } else {
                    fflush(stdout);
                    CLS;
                    puts("!! Allenamento tramite dataset gia' eseguito.");
                    PAUSE;
                    break;    
                }
                break;
            case '2': // Inserisci un ipotesi
                init_user_hyp_linked_list(_user_hyp_head);
                break;
            case '3': // Conclude training
                // Qui devo congiungere le due liste create (quella tramite dataset e quella tramite ipotesi inserite manualmente)

                // Elimino l' ultimo nodo della linked list del dataset (siccome è vuoto)
                hypotesis* aux = _dataset_hyp_head;
                hypotesis* before_aux = _dataset_hyp_head;
                while(aux->next != NULL){
                    before_aux = aux;
                    aux = aux->next;
                }
                free(aux);

                if(_dataset_hyp_head->next == NULL){ // Se non è stato usato il dataset
                    *head_dataset_refer = _user_hyp_head;
                }

                if(_user_hyp_head->next == NULL){ // Se non sono state inserite ipotesi manualmente
                    before_aux->next = NULL;
                    exit = 1;
                    break;
                }

                // Unisco le linked list del dataset e quella con ipotesi dell' utente
                before_aux->next = _user_hyp_head;

                // Elimino l' ultimo nodo della linked list delle ipotesi dell' utente (siccome è vuoto)
                aux = _user_hyp_head;
                before_aux = _user_hyp_head;
                while(aux->next != NULL){
                    before_aux = aux;
                    aux = aux->next;
                }
                before_aux->next = NULL;
                free(aux);

                exit = 1;
                break;
            default:
                break;
        }
    } while(exit != 1);
}

void print_training_options(){
    CLS;
    printf("Find-S Algorithm (Training Phase)");
    printf("\n\nScegli un opzione:");
    printf("\n    1 - Allenami tramite dataset");
    printf("\n    2 - Inserisci un ipotesi");
    printf("\n    3 - Concludi sessione di training\n"); 

    fflush(stdout);
}

void init_dataset_hyp_linked_list(hypotesis* _dataset_hyp_head, bool* _trained){
    char dataset_path[USER_BUF];
    size_t len;

    CLS;
    printf("Inserire il percorso al dataset:");
    printf("\n(main dataset path: ./dataset.csv)");
    fflush(stdout);
    fflush(stdin);
    printf("\n\n>> ");

    fgets(dataset_path, USER_BUF, stdin);

    len = strlen(dataset_path);
    if (len > 0 && dataset_path[len - 1] == '\n') {
        dataset_path[len - 1] = '\0';
    }

    FILE* stream_dataset = fopen(dataset_path, "r");
    if(stream_dataset == NULL){
        CLS;
        perror("Non e' stato possibile leggere il dataset");
        PAUSE;
        *_trained = FALSE;
        return;
    }

    // Creo la lista contenente tutto il dataset
    create_dataset_linked_list(_dataset_hyp_head, stream_dataset);

    CLS;
    fclose(stream_dataset);
    *_trained = TRUE;
}

void create_dataset_linked_list(hypotesis* _dataset_hyp_head, FILE* _stream_dataset){
    char* row_buffer = (char*)malloc(sizeof(char)*ROW_BUF);
    hypotesis* current_hypotesis = _dataset_hyp_head;
    for(int row = 0; fgets(row_buffer, ROW_BUF, _stream_dataset) != NULL; row++){ // Scorro il dataset
        hypotesis* new_node_hyp = (hypotesis*)malloc(sizeof(hypotesis));
        new_node_hyp->next = NULL;
        if(row == 0){ // Verifico che la prima riga venga scartata
            continue;
        }

        // Memorizzazione attributi nel relativo nodo
        // 30 è la lunghezza massima dell' attributo (in questo caso specifico è sovrabbondante);
        sscanf(row_buffer, "%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%3[^\n]", current_hypotesis->has_alternative, current_hypotesis->has_bar, current_hypotesis->weekend, current_hypotesis->hungry, current_hypotesis->crowded, current_hypotesis->price, current_hypotesis->raining, current_hypotesis->reservation, current_hypotesis->restaurant_type, current_hypotesis->estimated_wait, current_hypotesis->wait);

        current_hypotesis->next = new_node_hyp;
        current_hypotesis = new_node_hyp;
    }
}

void init_user_hyp_linked_list(hypotesis* _user_hyp_head){
    char attributes_buf[ROW_BUF] = {','};

    CLS;
    printf("Inserisci gli attributi dell' ipotesi:\n\n");

    // Creo una stringa contenente tutti gli attributi dell' ipotesi
    create_string_from_input_attributes(attributes_buf);

    // Inserisco il nodo in coda
    hypotesis* aux = _user_hyp_head;
    while(aux->next != NULL){
        aux = aux->next;
    }
    create_node_from_string(attributes_buf, aux);

    // Dalla stringa ottenuta creo un nuovo nodo della lista di ipotesi e lo metto in coda
    hypotesis* new_hypotesis = (hypotesis*)malloc(sizeof(hypotesis));
    new_hypotesis->next = NULL;
    aux->next = new_hypotesis;

    // Stampa di PROVA di tutti gli attributi "has_alternative" della linked list dello user
    /*printf("\n\nStampa di has_alternative di tutti i nodi");
    aux = _user_hyp_head;
    while(aux->next != NULL){
        printf("\n%s", aux->has_alternative);
        aux = aux->next;
    }*/
    PAUSE;
}

void create_node_from_string(char* _attributes_buf, hypotesis* _new_hypotesys){
    fflush(stdout);
    fflush(stdin);
    sscanf(_attributes_buf, ",%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%3[^\n]", _new_hypotesys->has_alternative, _new_hypotesys->has_bar, _new_hypotesys->weekend, _new_hypotesys->hungry, _new_hypotesys->crowded, _new_hypotesys->price, _new_hypotesys->raining, _new_hypotesys->reservation, _new_hypotesys->restaurant_type, _new_hypotesys->estimated_wait, _new_hypotesys->wait);
}

void create_string_from_input_attributes(char* _attributes_buf){
    char user_answer[USER_BUF];
    char* attributi[NUMB_ATTR] = {"has_alternative", "bar", "weekend", "hungry", "crowded", "price", "raining", "reservation", "restaurant_type", "estimated_wait", "wait"};

    for(int i = 0; i<NUMB_ATTR; i++){
        fflush(stdout);
        fflush(stdin);
        printf("    - %s >> ", attributi[i]);

        fgets(user_answer, USER_BUF-1, stdin);
        int len = strlen(user_answer);
        user_answer[len-1] = '\0'; // Perchè l'ultimo carattere acquisito era '\n'

        if(i != 0 && i != NUMB_ATTR){
            strcat(_attributes_buf, ",");
        }
        strcat(_attributes_buf,  user_answer);
    }
    fflush(stdout);
}