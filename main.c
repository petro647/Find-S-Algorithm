#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define USER_BUF 100
#define ROW_BUF 500
#define TRUE 1
#define FALSE 0
#define PAUSE printf("\nPremere Invio per continuare... "); fflush(stdin); getchar()

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

void training_phase(hypotesis* _dataset_hyp_head, hypotesis* _user_hyp_head);
void create_dataset_hyp_linked_list(hypotesis* _dataset_hyp_head, bool* _trained);
void create_user_hyp_linked_list(hypotesis* _user_hyp_head);
void print_training_options();

int main(){
    // Head della linked list generata dal dataset.
    hypotesis* dataset_hyp_head = (hypotesis*)malloc(sizeof(hypotesis));
    dataset_hyp_head->next = NULL;
        
    // Head della linked list di ipotesi aggiunte manualmente dall' utente.
    hypotesis* user_hyp_head = (hypotesis*)malloc(sizeof(hypotesis));
    user_hyp_head->next = NULL;

    training_phase(dataset_hyp_head, user_hyp_head);

    return 0;
}

void training_phase(hypotesis* _dataset_hyp_head, hypotesis* _user_hyp_head){
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
                    create_dataset_hyp_linked_list(_dataset_hyp_head, &trained);

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
                create_user_hyp_linked_list(_user_hyp_head);
                break;
            case '3': // Conclude training
                // Qui devo congiungere le due liste create (quella tramite dataset e quella tramite ipotesi inserite manualmente)
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

void create_dataset_hyp_linked_list(hypotesis* _dataset_hyp_head, bool* _trained){
    char dataset_path[USER_BUF];
    char* row_buffer = (char*)malloc(sizeof(char)*ROW_BUF);
    hypotesis* current_hypotesis = _dataset_hyp_head;
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

    for(int row = 0; fgets(row_buffer, ROW_BUF, stream_dataset) != NULL; row++){ // Scorro il dataset
        hypotesis* new_node_hyp = (hypotesis*)malloc(sizeof(hypotesis));
        if(row == 0){ // Verifico che la prima riga venga scartata
            continue;
        }
        // Memorizzazione attributi nel relativo nodo
        // 30 è la lunghezza massima dell' attributo (in questo caso specifico è sovrabbondante);
        sscanf(row_buffer, "%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%3[^\n]", current_hypotesis->has_alternative, current_hypotesis->has_bar, current_hypotesis->weekend, current_hypotesis->hungry, current_hypotesis->crowded, current_hypotesis->price, current_hypotesis->raining, current_hypotesis->reservation, current_hypotesis->restaurant_type, current_hypotesis->estimated_wait, current_hypotesis->wait);

        current_hypotesis->next = new_node_hyp;
        current_hypotesis = new_node_hyp;

        // printf("\n%s", row_buffer);
    }

    /*// Stampa di prova di attributi della linked list.
    hypotesis* aux = _dataset_hyp_head;
    puts("\n");
    while(aux->next != NULL){
        printf("\n%s v", aux->restaurant_type);
        aux = aux->next;
    }*/
    CLS;
    fclose(stream_dataset);
    *_trained = TRUE;
}

void create_user_hyp_linked_list(hypotesis* _user_hyp_head){
    char* attributi[20] = {"has_alternative", "weekend", "hungry", "crowded", "price", "raining", "reservation", "restaurant_type", "estimated_wait", "wait"};
    CLS;
    printf("Inserisci un ipotesi manualmente:");    
    printf("\n\n>> ");

        
}