//
// Created by Jacob van Steyn on 1/31/23.
//

//#include <pthread.h>
//#include <semaphore.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//
//#define NUM_LARGE_PLANES 15
//#define NUM_SMALL_PLANES 30
//#define NUM_REGIONS 6
//#define NUM_RUNWAYS 4
//#define LARGE_RUNWAY_LENGTH 3
//#define SMALL_RUNWAY_LENGTH 2
//
//enum Runway { RUNWAY1 = 0, RUNWAY2, RUNWAY3, RUNWAY4 };
//enum PlaneType { LARGE = 0, SMALL };
//enum State { IDLE_AT_TERMINAL, AWAITING_TAKEOFF, TAKEOFF, FLYING, AWAITING_LANDING, LANDING };
//
//int runway_usage[NUM_RUNWAYS];
//int regions_usage[NUM_REGIONS];
//
//sem_t runway_locks[NUM_RUNWAYS];
//sem_t region_locks[NUM_REGIONS];
//
//struct Plane {
//    int id;
//    PlaneType type;
//    State state;
//    int runway;
//    int* route;
//};
//
//void* simulate_plane(void* arg) {
//    Plane* plane = (Plane*)arg;
//
//    while (1) {
//        switch (plane->state) {
//            case IDLE_AT_TERMINAL: {
//                int sleep_duration = rand() % 1000 + 1000;
//                printf("Plane Thread %d (%s) IDLE_AT_TERMINAL for %d us\n", plane->id, (plane->type == LARGE) ? "Large" : "Small", sleep_duration);
//                usleep(sleep_duration);
//                plane->state = AWAITING_TAKEOFF;
//                break;
//            }
//            case AWAITING_TAKEOFF: {
//                int runway = rand() % NUM_RUNWAYS;
//                plane->runway = runway;
//                printf("Plane Thread %d (%s) AWAITING_TAKEOFF on runway %d\n", plane->id, (plane->type == LARGE) ? "Large" : "Small", runway + 1);
//
//                sem_wait(&runway_locks[runway]);
//                if (runway_usage[runway] == 0) {
//                    runway_usage[runway] = 1;
//                    sem_post(&runway_locks[runway]);
//                    int route_length = (plane->type == LARGE) ? LARGE_RUNWAY_LENGTH : SMALL_RUNWAY_LENGTH;
//                    plane->route = (int*)malloc(route_length * sizeof(int));
//                    for (int i = 0; i < route_length; i++) {
//                        int region = rand() % NUM_REGIONS;
//                        sem_wait(&region_locks[region]);
//                        if (regions_usage[region] == 0) {
//                            regions_usage[region] = 1;
//                            sem_post(&region_locks[region]);
//                            plane->route[i] = region;
//                        } else {
//                            sem_post(&region_locks[region]);
//                            i--;
//                        }
//                    }
//                    printf("Plane Thread %d (%s

//#include <pthread.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//
//#define NUM_LARGE_PLANES 15
//#define NUM_SMALL_PLANES 30
//
//#define NUM_REGIONS 6
//#define NUM_RUNWAYS 4
//
//#define MAX_SLEEP_TIME 3
//#define MAX_FLIGHT_TIME 10
//
//typedef struct {
//    int id;
//    int size;
//    int *runway;
//} Plane;
//
//int regions[NUM_REGIONS];
//pthread_mutex_t region_locks[NUM_REGIONS];
//
//int choose_runway(int size) {
//    return rand() % NUM_RUNWAYS + 1;
//}
//
//void shuffle_regions(int *runway, int size) {
//    for (int i = size - 1; i > 0; i--) {
//        int j = rand() % (i + 1);
//        int temp = runway[i];
//        runway[i] = runway[j];
//        runway[j] = temp;
//    }
//}
//
//void *fly_plane(void *arg) {
//    Plane *plane = (Plane *)arg;
//
//    while (1) {
//        int sleep_time = rand() % MAX_SLEEP_TIME + 1;
//        printf("Plane Thread %d (%s) is idle at terminal for %d seconds\n", plane->id, plane->size == 3 ? "Large" : "Small", sleep_time);
//        sleep(sleep_time);
//
//        int runway = choose_runway(plane->size);
//        switch (runway) {
//            case 1:
//                plane->runway = malloc(2 * sizeof(int));
//                plane->runway[0] = 1;
//                plane->runway[1] = 2;
//                break;
//            case 2:
//                plane->runway = malloc(3 * sizeof(int));
//                plane->runway[0] = 1;
//                plane->runway[1] = 4;
//                plane->runway[2] = 6;
//                break;
//            case 3:
//                plane->runway = malloc(3 * sizeof(int));
//                plane->runway[0] = 2;
//                plane->runway[1] = 3;
//                plane->runway[2] = 5;
//                break;
//            case 4:
//                plane->runway = malloc(2 * sizeof(int));
//                plane->runway[0] = 3;
//                plane->runway[1] = 4;
//                break;
//        }
//        shuffle_regions(plane->runway, plane->size);
//
//        printf("Plane Thread %d (%s) is awaiting takeoff for runway %d in order:", plane->id, plane->size == 3 ? "Large" : "Small", runway);
//        for (int i = 0; i < plane->size; i++) {
//            printf(" %d", plane->runway[i]);
//        }
//        printf("\n");
//
//        int proceed = 1;
//        for (int i = 0; i < plane->size; i++) {
//            int

//#include <stdio.h>
//#include <stdlib.h>
//#include <pthread.h>
//#include <semaphore.h>
//#include <unistd.h>
//
//#define NUM_LARGE_PLANES 15
//#define NUM_SMALL_PLANES 30
//#define NUM_REGIONS 6
//#define NUM_RUNWAYS 4
//
//int runway_regions[NUM_RUNWAYS][3] = {
//        {1, 2, 0},
//        {1, 4, 6},
//        {2, 3, 5},
//        {3, 4, 0}
//};
//
//int plane_regions[NUM_LARGE_PLANES + NUM_SMALL_PLANES][2];
//int plane_type[NUM_LARGE_PLANES + NUM_SMALL_PLANES];
//
//sem_t runway[NUM_REGIONS];
//
//void *plane_thread(void *arg) {
//    int id = (int) arg;
//    int type = plane_type[id];
//    int regions[3];
//    int i, sleep_time, region;
//
//    while (1) {
//        // IDLE AT TERMINAL
//        sleep_time = rand() % 1000000;
//        printf("Plane Thread %d (%s) is idle at terminal for %d us\n",
//               id, (type == 0) ? "Large" : "Small", sleep_time);
//        usleep(sleep_time);
//
//        // AWAITING TAKEOFF
//        if (type == 0) {
//            regions[0] = rand() % NUM_RUNWAYS;
//            for (i = 0; i < 3; i++) {
//                regions[i] = runway_regions[regions[0]][i];
//            }
//        } else {
//            regions[0] = rand() % NUM_REGIONS;
//            do {
//                regions[1] = rand() % NUM_REGIONS;
//            } while (regions[0] == regions[1]);
//        }
//        printf("Plane Thread %d (%s) is awaiting takeoff, regions: [%d, %d, %d]\n",
//               id, (type == 0) ? "Large" : "Small", regions[0], regions[1], regions[2]);
//
//        // Check if regions are available
//        for (i = 0; i < (type == 0 ? 3 : 2); i++) {
//            region = regions[i];
//            if (sem_trywait(&runway[region]) == -1) {
//                printf("Plane Thread %d (%s) must wait\n", id, (type == 0) ? "Large" : "Small");
//                sem_wait(&runway[region]);
//            }
//        }
//
//        // TAKEOFF
//        for (i = 0; i < (type == 0 ? 3 : 2); i++) {
////            region = regions[i];
////            sleep_time = rand() % 1000000;
////            printf("Plane Thread %d (%s) is taking off, region: %d for %d us\n",
////                   id, (type == 0) ? "Large" : "Small", region, sleep_time);
////            usleep(sleep_time);
//            int region = runways[runway][i];
//            sem_wait(&regions[region]);
//            printf("Plane Thread %d (%s) Taking off: Region %d\n", plane_id, (type == 0 ? "Large" : "Small"),
//                   region + 1);
//            sleep_rand();
//            printf("Plane Thread %d (%s) Flying: Region %d\n", plane_id, (type == 0 ? "Large" : "Small"), region + 1);
//        }
//
//        // AWAITING LANDING
//        runway_selected = rand() % (type == 0 ? 3 : 6);
//        for (i = 0; i < (type == 0 ? 3 : 2); i++) {
//            int region = runway[runway_selected][i];
//            sem_wait(&regions[region]);
//            printf("Plane Thread %d (%s) Approaching landing: Region %d\n", plane_id, (type == 0 ? "Large" : "Small"),
//                   region + 1);
//            sleep_rand();
//            printf("Plane Thread %d (%s) Landed: Region %d\n", plane_id, (type == 0 ? "Large" : "Small"), region + 1);
//            sem_post(&regions[region]);
//        }
//        printf("Plane Thread %d (%s) IDLE at terminal\n", plane_id, (type == 0 ? "Large" : "Small"));
//    }
//}
//
//int main(int argc, char *argv[]) {
//    int i;
//    srand(time(NULL));
//    sem_init(&regions[0], 0, 1);
//    sem_init(&regions[1], 0, 1);
//    sem_init(&regions[2], 0, 1);
//    sem_init(&regions[3], 0, 1);
//    sem_init(&regions[4], 0, 1);
//    sem_init(&regions[5], 0, 1);
//    pthread_t large_planes[15];
//    pthread_t small_planes[30];
//    for (i = 0; i < 15; i++)
//        pthread_create(&large_planes[i], NULL, plane, (void *)0);
//    for (i = 0; i < 30; i++)
//        pthread_create(&small_planes[i], NULL, plane, (void *)1);
//    for (i = 0; i < 15; i++)
//        pthread_join(large_planes[i], NULL);
//    for (i = 0; i < 30; i++)
//        pthread_join(small_planes[i], NULL);
//    return 0;
//    }