#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define CONVEYOR_SIZE 8
#define NODE_BUF_SIZE 150

pthread_mutex_t mutex; // critical region cook and students conveyor array
sem_t fillCount;
sem_t emptyCount;
sem_t studentArrived;
sem_t display_lock;

/*
 * Your simulation must keep the record of n (total number of trays filled),
 * m (total students that came to cafeteria) that are given above and
 * total number of students ate already (fetched tray).
 */


int n; // (total number of trays filled)
int m; // (total students that came to cafeteria)
int fetched_tray_num;
int filling_tray_no;
int waiting_line;
int entered_sleep_mode; // to obtain awake time.

enum States {EMPTY, FULL} trays_state[CONVEYOR_SIZE];
struct Cook {
    int started_to_sleep;
    int awake;
    int started_and_finished;
    pthread_t id;
} cook;

typedef struct Node {
    char buffer[NODE_BUF_SIZE];
    struct Node * next;
} times_node; // in order to write the console time which wanted from document.

times_node * head;

void initialize_code();
void * puts_the_tray(void *param);
void * create_student(void *param);
void * fetch_a_ready_tray(void *param);
void * detailed_output(void *param); // printing out times via monitor thread.
void change_tray_state(char *); // to change conveyor array elements state.
char * get_tray_state(enum States states); // enum to string conversion.
void print_tray_state();
int get_time();
void table(int trays_ready, int mode); // conveyor table in the center of the sample output.
void insert_time_node(char buffer[]); // in order to print times. Like cook started to sleep.
void print_time_nodes(); // it writes linked time nodes.


int main()
{

    initialize_code();
    pthread_create(&cook.id, NULL, puts_the_tray, NULL); //There is only ONE (1) cook.

    pthread_t student_creater_id;
    pthread_create(&student_creater_id, NULL, create_student, NULL);

    pthread_t monitor_id; // Your simulation must have another “monitor” thread for printing out
    pthread_create(&monitor_id, NULL, detailed_output, NULL);

    pthread_join(cook.id,NULL);


}
void table(int trays_ready, int mode) {
    int k;
    for (k = 0; k < 36-trays_ready*4; k++)
        printf(" ");
    for (k = 0; k < trays_ready; k++) {
        printf("... ");
    }
    printf(" .     Student(s) is/are \n");

    (mode == 1) ? printf("tray         .  ") : printf("             .  ");

    for (k = 0; k < 36-trays_ready*4; k++)
        printf(" ");
    for (k = 0; k < trays_ready; k++) {
        printf("| | ");
    }
    printf(" .           waiting\n");
    printf("             .  ");

    for (k = 0; k < 36-trays_ready*4; k++)
        printf(" ");
    for (k = 0; k < trays_ready; k++) {
        printf("... ");
    }
    printf(" .\n");
    printf("             ......................................... ");
}

void * detailed_output(void *param) {

    while (1) {
        sem_wait(&display_lock);
        system("clear");
        int i = 0;
        int trays_ready = 0;
        for (i = 0; i < CONVEYOR_SIZE; i++){
            if (strcmp(get_tray_state(trays_state[i]), "FULL") == 0)
                trays_ready++;
        }

        int valFillCount;
        sem_getvalue(&fillCount, &valFillCount);

        char * cook_state ;
        if (valFillCount == CONVEYOR_SIZE)
            cook_state = "Sleeping";
        else
            cook_state = "Working ";

        printf("-------------------------------------------------------------------------------\n");
        printf("                                                                               \n");
        printf("                                                                               \n");
        printf("                                CONVEYOR                                       \n");
        printf("                           ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯                                  \n");
        printf("                          TRAYS READY     :%d                   \n",trays_ready);
        printf("                          AVAILABLE PLACE :%d                                  \n",CONVEYOR_SIZE-trays_ready);
        printf("   COOK                                                         WAITING LINE   \n");
        printf("¯¯¯¯¯¯¯¯¯¯   .........................................       ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯\n");
        printf("%s     .                                       .              %d      \n",cook_state,waiting_line);

        if (strcmp(cook_state, "Working ") == 0){
            printf("Filling %d", filling_tray_no);
            if (filling_tray_no < 10)
                printf("  ");
            else if (filling_tray_no < 100)
                printf(" ");

            printf("  .  ");
            table(trays_ready,1);
        }
        else {
            printf("             .  ");
            table(CONVEYOR_SIZE,0);
        }


        printf("                                                                               \n");
        printf("                                                                               \n");
        printf("                         CAFETERIA STATISTICS                                  \n");
        printf("                     ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯                               \n");
        printf("                     TOTAL TRAYS FILLED       :%d                              \n",n);
        printf("                     TOTAL STUDENTS CAME      :%d                               \n",m);
        printf("                     TOTAL STUDENTS FETCHED   :%d                              \n",fetched_tray_num);
        printf("                                                                               \n");
        printf("-------------------------------------------------------------------------------\n");
        print_time_nodes();
    }

}

void initialize_code() {

    // Simulation has to start with a full conveyor. (With 8 trays ready to be fetched)
    int i;
    for (i = 0; i < CONVEYOR_SIZE; i++)
        trays_state[i] = FULL;

    pthread_mutex_init(&mutex, NULL);
    sem_init(&fillCount, 0, CONVEYOR_SIZE);
    sem_init(&emptyCount, 0, 0);
    sem_init(&studentArrived, 0, 1);
    sem_init(&display_lock, 0, 1);

    n = 8;
    m = 0;
    filling_tray_no = 8;
    waiting_line = 0;
    entered_sleep_mode = 0;
    head = NULL;

}

void * puts_the_tray(void * param) {

    while (1) { //Your simulation is to be continuous (should not end unless it is stopped or killed)

        int emptyCountVal;
        sem_getvalue(&emptyCount,&emptyCountVal);
        if (emptyCountVal == 0){
            // If the conveyor is full cook sleeps
            // emptyCountVal provides me conveyor is full or not!
            cook.started_to_sleep = get_time();
            char buff[NODE_BUF_SIZE];
            sprintf(buff, "The time cook started to sleep: %d in seconds. ", cook.started_to_sleep);
            insert_time_node(buff); // added to linked times node.
            entered_sleep_mode = 1; // obtain awake time when needed
        }
        sem_wait(&emptyCount);

        int started_n_tray = get_time();
        // Filling a tray takes random amount of time between 2-5 seconds.
        unsigned int filling_a_tray_time = rand() % 4 + 2;
        sleep(filling_a_tray_time); //time consuming task.
        filling_tray_no++;

        sem_post(&display_lock);

        pthread_mutex_lock(&mutex);

        change_tray_state("EMPTY");
        n++;

        //print_tray_state();
        int finished_n_tray = get_time();

        if (finished_n_tray > started_n_tray) // easy case
            cook.started_and_finished = finished_n_tray - started_n_tray;
        else
            cook.started_and_finished = 60 - started_n_tray + finished_n_tray;

        char buff[NODE_BUF_SIZE];
        sprintf(buff, "The time he started and finished to fill the %dth tray: %d in seconds.",n,cook.started_and_finished);
        insert_time_node(buff);

        sem_post(&display_lock);

        pthread_mutex_unlock(&mutex);

        sem_post(&fillCount);
        sem_post(&display_lock);
    }

}
void * fetch_a_ready_tray(void *param) {

    waiting_line++;
    sem_post(&display_lock);
    sem_wait(&studentArrived);

    int m_th_student_arrived = get_time();
    m++;
    char buff[NODE_BUF_SIZE];
    sprintf(buff, "The time %dth student arrived: at %d seconds.",m , m_th_student_arrived);
    insert_time_node(buff);
    memset(buff,' ',NODE_BUF_SIZE);

    sem_post(&display_lock);
    sem_wait(&fillCount);
    sem_post(&display_lock); //?

    // A student to fetch a ready tray takes exactly 1 second.
    unsigned int takeTrayTime = 1;
    sleep(takeTrayTime); // time consuming task.
    pthread_mutex_lock(&mutex); // One student can fetch a tray at a time (as it is in the real cafeteria)

    change_tray_state("FULL");

    int m_th_student_fetched = get_time();

    sprintf(buff, "The time %dth student fetched his tray: at %d seconds.",m,m_th_student_fetched);
    insert_time_node(buff);

    fetched_tray_num++;
    sem_post(&display_lock);

    int fillCountVal;
    sem_getvalue(&fillCount,&fillCountVal);

    if (entered_sleep_mode == 1 && fillCountVal == CONVEYOR_SIZE-1){
        memset(buff,' ',NODE_BUF_SIZE);
        cook.awake = get_time();
        sprintf(buff, "The time cook awake: at %d seconds", cook.awake);
        insert_time_node(buff);
        entered_sleep_mode = 0; // it have to enter again the sleep region.
    }

    pthread_mutex_unlock(&mutex);
    sem_post(&emptyCount);

    waiting_line--;
    sem_post(&display_lock);
    sem_post(&studentArrived);

    pthread_exit(NULL);

}
char * get_tray_state(enum States states){
    switch (states){
        case EMPTY: return "EMPTY";
        case  FULL: return "FULL";
    }
}
void * create_student(void *param) {
    do { //Your simulation is to be continuous (should not end unless it is stopped or killed)
        // Each student has to be a different thread and has to be created at random times.
        // A student comes to cafeteria in random intervals that changes between 1-4 seconds.
        unsigned int come_to_cafe_time = rand() % 4 + 1;
        sleep(come_to_cafe_time);
        pthread_t student_tid;
        pthread_create(&student_tid, NULL, fetch_a_ready_tray, NULL);

    } while (1);
}

void change_tray_state(char * tray_state) {

    enum States update_as;
    if (strcmp(tray_state, "EMPTY") == 0)
        update_as = FULL;
    else
        update_as = EMPTY;

    int i;
    for (i = 0; i < CONVEYOR_SIZE; i++) {
        if (strcmp(get_tray_state(trays_state[i]), tray_state) == 0) {
            trays_state[i] = update_as;
            break;
        }
    }

}
void print_tray_state(){
    int i;
    for (i = 0; i < CONVEYOR_SIZE; i++) {
        printf("%s ", get_tray_state(trays_state[i]));
    }
    printf(" \n");
}
int get_time(){
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    return timeinfo->tm_sec;

    /*printf("%d", timeinfo->tm_sec);
    printf ( "Current local time and date: %s", asctime (timeinfo) );*/
}
void insert_time_node(char buffer[]) {

    if (head == NULL){
        head = malloc(sizeof(times_node));
        strcpy(head->buffer, buffer);
        head->next = NULL;
    }
    else {
        times_node * current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = malloc(sizeof(times_node));
        strcpy(current->next->buffer, buffer);
        current->next->next = NULL;
    }

}
void print_time_nodes() {
    times_node * cur = head;
    while (cur != NULL){
        printf("%s \n", cur->buffer);
        cur = cur->next;
    }
}
