//
// Created by Jacob van Steyn on 1/27/23.
//

/* NOTES:
 
planes can navigate around the airport to either end of runways without collisions
airfield can be defined as 6 separate regions
 
planes are either large or small
no 2 planes can be in the same region
planes travel in straight lines once on runway in immediate succession without stopping
 
Runways go through these regions:
 - runway 1: 1,2
 - runway 2: 1,4,6
 - runway 3: 2,3,5
 - runway 3: 3,4
 
large planes:
 - must use 3 segment continuous straight run
     - i.e. regions 1,4,6 | 2,3,5 | 6,4,1
small planes:
 - 2 segment continuous
     - 1,2 | 3,4 | 1,4 | 2,3 | 3,5 | 4,6
 
 Code should generate 15 threads for large planes and 30 threads for small planes
 threads run continuously, cycling through states until simulation ends
 each thread can be in these states which proceed in order:
     idle at terminal
         - sleep for random amount of time using usleep() function
         - thread needs to announce sleep duration
 
     awaiting takeoff
         - randomly chooses a runway (e.g. set of segments - 3 for large, 2 for small)
         - then chooses the ordering of the regions
         - this selection must be done without awareness of airport state
         - must print it's intended region order
         - if plane can proceed, then advance to next state
             - otherwise print that it must wait, then sleep on a semaphore
 
     takeoff
         - move to first region in list then announce identity and position
         - sleep for random amount of time in that region
         - after sleep, proceeds to next region without delay
 
     flying
         - simulated with random sleep & announce duration before sleeping
         - after sleep proceed to awaiting landing
 
     awaiting landing (similar to awaiting takeoff)
         - randomly selects compatible runway regions & order then prints them
         - if able to land, it prints and proceeds else it sleeps on a semaphore
 
     landing (similar to taking off)
         - regions selected traversed with random sleep on each region before going to next
         - plane must announce its identity and runway position with each region traversed
         - after leaving last region, plane transitions to idle at terminal state
 
 When a plane enters each state, it must print to the console:
     - message must include its identity (e.g., “Plane Thread 4”), its type (“(Large)” or “(Small)”), its state, and any relevant details for that state as described earlier.
 
 the threads can share a global array or diagram representing the runways and their use
 
 The simulator should advance planes as efficiently as possible by maximizing parallelism while avoiding deadlock or starvation
 
 Planes cannot pass each other by swapping positions (e.g., a plane going from region 2 to region 3 would collide with a plane going from region 3 to region 2)
 */
 

//const char* states[] = {"Idle at Terminal", "Awaiting Takeoff", "Takeoff", "Flying", "Awaiting Landing", "Landing"};
//
//
//int main(int argc, char* argv[]){
//
//
//
//}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_LARGE_PLANES 15
#define NUM_SMALL_PLANES 30
#define NUM_REGIONS 6
#define NUM_RUNWAYS 4

int runway_regions[NUM_RUNWAYS][3] = {
        {1, 2},
        {1, 4, 6},
        {2, 3, 5},
        {3, 4},
};

//int runway_regions[NUM_RUNWAYS][7] = {
//        {1, 2},
//        {2, 1},
//        {1, 4, 6},
//        {6, 4, 1},
//        {2, 3, 5},
//        {5, 3, 2},
//        {3, 4},
//        {4, 3}
//};

int plane_regions[NUM_LARGE_PLANES + NUM_SMALL_PLANES][2];
int plane_type[NUM_LARGE_PLANES + NUM_SMALL_PLANES];

sem_t runway[NUM_REGIONS];

void *plane_thread(void *arg) {
    int id = (int) arg;
    int type = plane_type[id];
    int regions[3];
    int i, sleep_time, region;

    while (1) {
        // IDLE AT TERMINAL
        sleep_time = rand() % 1000000;
        printf("Plane Thread %d (%s) is idle at terminal for %d us\n",
               id, (type == 0) ? "Large" : "Small", sleep_time);
        usleep(sleep_time);

        // AWAITING TAKEOFF
        if (type == 0) {
            regions[0] = rand() % NUM_RUNWAYS;
            for (i = 0; i < 3; i++) {
                regions[i] = runway_regions[regions[0]][i];
            }
        } else {
            regions[0] = rand() % NUM_REGIONS;
            do {
                regions[1] = rand() % NUM_REGIONS;
            } while (regions[0] == regions[1]);
        }
        printf("Plane Thread %d (%s) is awaiting takeoff, regions: [%d, %d, %d]\n",
               id, (type == 0) ? "Large" : "Small", regions[0], regions[1], regions[2]);

        // Check if regions are available
        for (i = 0; i < (type == 0 ? 3 : 2); i++) {
            region = regions[i];
            if (sem_trywait(&runway[region]) == -1) {
                printf("Plane Thread %d (%s) must wait to takeoff \n", id, (type == 0) ? "Large" : "Small");
                sem_wait(&runway[region]);
            }
        }

        // TAKEOFF
        for (i = 0; i < (type == 0 ? 3 : 2); i++) {
            region = regions[i];
            sleep_time = rand() % 1000000;
            printf("Plane Thread %d (%s) is taking off, region: %d for %d us\n",
                   id, (type == 0) ? "Large" : "Small", region, sleep_time);
            usleep(sleep_time);
        }

        // FLYING

        sleep_time = rand() % 1000000;
        printf("Plane Thread %d (%s) is flying for %d us\n",
               id, (type == 0) ? "Large" : "Small", sleep_time);
        usleep(sleep_time);

        // AWAITING LANDING
        printf("Plane Thread %d (%s) is awaiting landing, regions: [%d, %d, %d]\n",
               id, (type == 0) ? "Large" : "Small", regions[0], regions[1], regions[2]);
        // Check if regions are available
        for (i = 0; i < (type == 0 ? 3 : 2); i++) {
            region = regions[i];
            if (sem_trywait(&runway[region]) == -1) {
                printf("Plane Thread %d (%s) must wait to land \n", id, (type == 0) ? "Large" : "Small");
                sem_wait(&runway[region]);
            }
        }

        // LANDING
        for (i = (type == 0 ? 2 : 1); i >= 0; i--) {
            region = regions[i];
            sleep_time = rand() % 1000000;
            printf("Plane Thread %d (%s) is landing, region: %d for %d us\n",
                   id, (type == 0) ? "Large" : "Small", region, sleep_time);
            usleep(sleep_time);
            sem_post(&runway[region]);
        }



        printf("Plane Thread %d (%s) has landed\n", id, (type == 0) ? "Large" : "Small");
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int i;
    pthread_t planes[NUM_LARGE_PLANES + NUM_SMALL_PLANES];

    srand(time(NULL));

    for (i = 0; i < NUM_REGIONS; i++) {
        sem_init(&runway[i], 0, 1);
    }

    for (i = 0; i < NUM_LARGE_PLANES; i++) {
        plane_regions[i][0] = i % NUM_REGIONS;
        plane_regions[i][1] = (i + 1) % NUM_REGIONS;
        plane_type[i] = 0;
        pthread_create(&planes[i], NULL, plane_thread, (void *) i);
    }

    for (i = NUM_LARGE_PLANES; i < NUM_LARGE_PLANES + NUM_SMALL_PLANES; i++) {
        plane_regions[i][0] = i % NUM_REGIONS;
        plane_regions[i][1] = (i + 1) % NUM_REGIONS;
        plane_type[i] = 1;
        pthread_create(&planes[i], NULL, plane_thread, (void *) i);
    }

    for (i = 0; i < NUM_LARGE_PLANES + NUM_SMALL_PLANES; i++) {
        pthread_join(planes[i], NULL);
    }

    for (i = 0; i < NUM_REGIONS; i++) {
        sem_destroy(&runway[i]);
    }

    return 0;
}



