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
int create_string_from_input_attributes(char* _attributes_buf, int is_testing_phase);
void create_dataset_linked_list(hypotesis* _dataset_hyp_head, FILE* _stream_dataset);
void print_training_options();
void testing_phase(hypotesis* _dataset_hyp_head);
void do_test(hypotesis* _dataset_hyp_head);
void compare_hypotesis(hypotesis* current_node, char _hypotesis_to_calculate[][30]);
void compare_attributes(char* attribute, int attribute_position, char _hypotesis_to_calculate[][30]);

int main(){
    // Head della linked list generata dal dataset.
    hypotesis* dataset_hyp_head = (hypotesis*)malloc(sizeof(hypotesis));
    dataset_hyp_head->next = NULL;
        
    // Head della linked list di ipotesi aggiunte manualmente dall' utente.
    hypotesis* user_hyp_head = (hypotesis*)malloc(sizeof(hypotesis));
    user_hyp_head->next = NULL;

    training_phase(dataset_hyp_head, user_hyp_head, &dataset_hyp_head);
    testing_phase(dataset_hyp_head);

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

void print_testing_options(){
    CLS;
    printf("Find-S Algorithm (Testing Phase)");
    printf("\n\nScegli un opzione:");
    printf("\n    1 - Prova l'algoritmo con ipotesi da calcolare");
    printf("\n    2 - Esci dal programma\n"); 

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

    // Gestione errore
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

// Creo la linked list soltanto con le ipotesi gia presenti nel dataset.
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
    int is_string_created;

    // Creo una stringa contenente tutti gli attributi dell' ipotesi
    is_string_created = create_string_from_input_attributes(attributes_buf, 0);
    if(!is_string_created) {
        attributes_buf[0] = ',';
        attributes_buf[1] = '\0';
        return;
    }

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
    PAUSE;
}

void create_node_from_string(char* _attributes_buf, hypotesis* _new_hypotesys){
    fflush(stdout);
    fflush(stdin);
    sscanf(_attributes_buf, ",%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%30[^,],%3[^\n]", _new_hypotesys->has_alternative, _new_hypotesys->has_bar, _new_hypotesys->weekend, _new_hypotesys->hungry, _new_hypotesys->crowded, _new_hypotesys->price, _new_hypotesys->raining, _new_hypotesys->reservation, _new_hypotesys->restaurant_type, _new_hypotesys->estimated_wait, _new_hypotesys->wait);
}

 // Restituisce un intero positivo se è stata creata la stringa correttamente, 0 altrimenti
int create_string_from_input_attributes(char* _attributes_buf, int is_testing_phase){
    char user_answer[USER_BUF];
    char* attributi[NUMB_ATTR] = {"has_alternative (yes/no)", "bar (yes/no)", "weekend (yes/no)", "hungry (yes/no)", "crowded (none/someone/full)", "price ($/$$/$$$)", "raining (yes/no)", "reservation (yes/no)", "restaurant_type (french/italian/thai/fast_food)", "estimated_wait (<10/10-29/30-60/>60)", "wait (yes/no)"};
    
    CLS;
    printf("Inserisci gli attributi dell' ipotesi:\n\n");

    int numb_attr = NUMB_ATTR;
    if(is_testing_phase == 1){
        numb_attr--;
    }

    for(int i = 0; i<numb_attr; i++){
        fflush(stdout);
        fflush(stdin);
        printf("    - %s >> ", attributi[i]);

        fgets(user_answer, USER_BUF-1, stdin);
        int len = strlen(user_answer);
        user_answer[len-1] = '\0'; // Perchè l'ultimo carattere acquisito era '\n'

        if(i != 0 && i != numb_attr){
            strcat(_attributes_buf, ",");
        }
        strcat(_attributes_buf,  user_answer); // Concateno in _attributes_buf tutti gli attributi formando una singola stringa
    }
    if(is_testing_phase == 1){
        strcat(_attributes_buf, ",");
        strcat(_attributes_buf,  "yes");
    }
    fflush(stdout);
    printf("I dati inseriti sono corretti? (yes/no) >> ");
    fgets(user_answer, USER_BUF, stdin);
    if(strcmp(user_answer, "yes\n") == 0){
        return 1;
    } else {
        return 0;
    }
}

void testing_phase(hypotesis* _dataset_hyp_head){
    char user_answer;
    int exit = 0;
    do{
        print_testing_options();
        printf("\n>> ");
        fflush(stdin);
        user_answer = getchar();

        switch (user_answer){
            case '1': // Testa il modello.
                CLS;
                do_test(_dataset_hyp_head);
                break;
            case '2': // Esci dal programma.
                exit = 1;
                break;
            default:
                break;
        }
    } while(exit != 1);

}

void do_test(hypotesis* _dataset_hyp_head){
    char attributes_buf[ROW_BUF] = {','};
    char hypotesis_to_calculate[][30] = { "0", "0", "0", "0", "0", "0", "0", "0", "0", "0" }; // 10 campi da comparare
    int is_string_created = create_string_from_input_attributes(attributes_buf, 1);
    int any = 0;
    int matched = 0;
    char user_answer[USER_BUF];
    if(!is_string_created) {
        attributes_buf[0] = ',';
        attributes_buf[1] = '\0';
        return;
    }

    hypotesis* user_test_hypotesis = (hypotesis*)malloc(sizeof(hypotesis));
    user_test_hypotesis->next = NULL;
    create_node_from_string(attributes_buf, user_test_hypotesis);

    // Aggiungo l' ipotesi di test dell'utente in coda alla linked list totale.
    hypotesis* aux = _dataset_hyp_head;
    while(aux->next != NULL){
        aux = aux->next;
    }
    aux->next = user_test_hypotesis;

    // Per fare la comparazione dei singoli campi delle strutture del dataset con quelli della stringa intanto devo estrarre i campi singolarmente dalle strutture, e poi avendo l' intero della loro posizione in memoria sapro gia con quale compararli

    aux = _dataset_hyp_head;
    printf("\n");
    while(aux != NULL){
        if(strcmp(aux->wait, "yes") == 0){
            // printf("%s", aux->restaurant_type);
            compare_hypotesis(aux, hypotesis_to_calculate);
        }
        aux = aux->next;
    }

    printf("\nIpotesi piu specifica possibile: ");

    for(int i = 0; i<10; i++){
        if(strcmp(hypotesis_to_calculate[i], "?") == 0){
            any++;
        } else {
            matched++;
        }
        printf("\na%d: %s", i, hypotesis_to_calculate[i]);
    }
    // printf("\nany: %d, matched: %d", any, matched);

    if(any > matched){
        printf("\nRESULT: Conviene aspettare? NO");
        sscanf("no,", "%2[^,]", user_test_hypotesis->wait);
    } else {
        printf("\nRESULT: Conviene aspettare? SI");
        sscanf("yes,", "%3[^,]", user_test_hypotesis->wait);
    }

    printf("\nIl risultato ottenuto e' corretto? (yes/no) >> ");
    fflush(stdin);
    fgets(user_answer, USER_BUF-1, stdin);

    if(strcmp(user_answer, "no\n") == 0){
        // Inverto l' esito.
        if(strcmp(user_test_hypotesis->wait, "yes") == 0){
            sscanf("no,", "%2[^,]", user_test_hypotesis->wait);
            printf("\nHo cambiato l'esito in: %s", user_test_hypotesis->wait);
        } else {
            sscanf("yes,", "%3[^,]", user_test_hypotesis->wait);
            printf("\nHo cambiato l'esito in: %s", user_test_hypotesis->wait);
        }
    }

    PAUSE;
}

// Comparazione tra ipotesi.
void compare_hypotesis(hypotesis* current_node, char _hypotesis_to_calculate[][30]){
    compare_attributes(current_node->has_alternative, 0, _hypotesis_to_calculate);
    compare_attributes(current_node->has_bar, 1, _hypotesis_to_calculate);
    compare_attributes(current_node->weekend, 2, _hypotesis_to_calculate);
    compare_attributes(current_node->hungry, 3, _hypotesis_to_calculate);
    compare_attributes(current_node->crowded, 4, _hypotesis_to_calculate);
    compare_attributes(current_node->price, 5, _hypotesis_to_calculate);
    compare_attributes(current_node->raining, 6, _hypotesis_to_calculate);
    compare_attributes(current_node->reservation, 7, _hypotesis_to_calculate);
    compare_attributes(current_node->restaurant_type, 8, _hypotesis_to_calculate);
    compare_attributes(current_node->estimated_wait, 9, _hypotesis_to_calculate);
}

// Comparazione dei singoli attributi dell' ipotesi.
void compare_attributes(char* attribute, int attribute_position, char _hypotesis_to_calculate[][30]){
    if(strcmp(_hypotesis_to_calculate[attribute_position], "0") == 0){
        strcpy(_hypotesis_to_calculate[attribute_position], attribute);
    } else {
        int are_equal = strcmp(attribute, _hypotesis_to_calculate[attribute_position]);
        if(are_equal != 0){
            strcpy(_hypotesis_to_calculate[attribute_position], "?");
        }
    }
}