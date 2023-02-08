//
// Created by Jacob van Steyn on 2/7/23.
//
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_LARGE_PLANES 15
#define NUM_SMALL_PLANES 30
#define NUM_REGIONS 6

#define IDLE 0
#define AWAIT_TAKEOFF 1
#define TAKEOFF 2
#define FLYING 3
#define AWAIT_LANDING 4
#define LANDING 5

struct Plane {
    pthread_t thread;
    int id;
    int large;
    int state;
    int runway;
    int queued;
};

int largeRuns[2][3] = {
        {1,4,6},
        {2,3,5}
};
int smallRuns[6][2] = {
        {1,2},
        {3,4},
        {1,4},
        {2,3},
        {3,5},
        {4,6}
};

sem_t semPlanes[NUM_LARGE_PLANES + NUM_SMALL_PLANES];
sem_t semRegions[NUM_REGIONS];

struct Plane largePlanes[NUM_LARGE_PLANES];
struct Plane smallPlanes[NUM_SMALL_PLANES];

void *Plane(void *arg) {
    struct Plane *plane = (struct Plane *)arg;
    int sleepTime, runway;

    while (1) {
        switch (plane->state) {
            case IDLE:
                // Sleep for random amount of time
                sleepTime = rand() % 1000000;
                printf("Plane Thread %d (%s) is IDLE and will sleep for %d usecs\n",
                       plane->id, plane->large ? "Large" : "Small", sleepTime);
                usleep(sleepTime);
                plane->state = AWAIT_TAKEOFF;
                break;
            case AWAIT_TAKEOFF:
                // Randomly choose a runway
                runway = rand() % (plane->large ? 2 : 6);
                // Choose the ordering of the regions
                int order = rand() % (plane->large ? 3 : 2);
                int regions[3];
                for (int i = 0; i < (plane->large ? 3 : 2); i++) {
                    regions[i] = (plane->large ? largeRuns[runway][i] : smallRuns[runway][i]);
                }
                printf("Plane Thread %d (%s) is AWAITING TAKEOFF and will use runway %d with order %d\n",
                       plane->id, plane->large ? "Large" : "Small", runway, order);
                // Check if plane can proceed
                int canProceed = 1;
                for (int i = 0; i < (plane->large ? 3 : 2); i++) {
                    int region = regions[(order + i) % (plane->large ? 3 : 2)];
                    int value;
                    sem_getvalue(&semRegions[region], &value);
                    if (value == 0) {
                        canProceed = 0;
                        break;
                    }
                }
                if (canProceed) {
                    // Take control of the regions
                    for (int i = 0; i < (plane->large ? 3 : 2); i++) {
                        int region = regions[(order + i) % (plane->large ? 3 : 2)];
                        sem_wait(&semRegions[region]);
                        printf("Plane Thread %d (%s) takes control of Region %d\n",
                               plane->id, plane->large ? "Large" : "Small", region);
                    }
                    // Set state to TAKEOFF
                    plane->state = TAKEOFF;
                    plane->runway = runway;
                } else {
                    // Wait for semaphore for this plane
                    printf("Plane Thread %d (%s) is blocked and waiting for semaphore\n",
                           plane->id, plane->large ? "Large" : "Small");
                    sem_wait(&semPlanes[plane->id]);
                    plane->queued = 0;
                }
                break;
            case TAKEOFF:
                printf("Plane Thread %d (%s) is TAKING OFF from Runway %d\n",
                       plane->id, plane->large ? "Large" : "Small", plane->runway);
                // Sleep for random amount of time
                sleepTime = rand() % 1000000;
                usleep(sleepTime);
                // Set state to FLYING
                plane->state = FLYING;
                break;
            case FLYING:
                printf("Plane Thread %d (%s) is FLYING\n",
                       plane->id, plane->large ? "Large" : "Small");
                // Sleep for random amount of time
                sleepTime = rand() % 1000000;
                usleep(sleepTime);
                // Set state to AWAIT_LANDING
                plane->state = AWAIT_LANDING;
                break;
            case AWAIT_LANDING:
                // Choose the ordering of the regions
                order = rand() % (plane->large ? 3 : 2);
                //int regions[3];
                for (int i = 0; i < (plane->large ? 3 : 2); i++) {
                    regions[i] = (plane->large ? largeRuns[plane->runway][i] : smallRuns[plane->runway][i]);
                }
                printf("Plane Thread %d (%s) is AWAITING LANDING and will use runway %d with order %d\n",
                       plane->id, plane->large ? "Large" : "Small", plane->runway, order);
                // Check if plane can proceed
                canProceed = 1;
                for (int i = 0; i < (plane->large ? 3 : 2); i++) {
                    int region = regions[(order + i) % (plane->large ? 3 : 2)];
                    int value;
                    sem_getvalue(&semRegions[region], &value);
                    if (value == 0) {
                        canProceed = 0;
                        break;
                    }
                }
                if (canProceed) {
                    // Take control of the regions
                    for (int i = 0; i < (plane->large ? 3 : 2); i++) {
                        int region = regions[(order + i) % (plane->large ? 3 : 2)];
                        sem_wait(&semRegions[region]);
                        printf("Plane Thread %d (%s) takes control of Region %d\n",
                               plane->id, plane->large ? "Large" : "Small", region);
                    }
                    // Set state to LANDING
                    plane->state = LANDING;
                } else {
                    // Wait for semaphore for this plane
                    printf("Plane Thread %d (%s) is blocked and waiting for semaphore\n",
                           plane->id, plane->large ? "Large" : "Small");
                    sem_wait(&semPlanes[plane->id]);
                    plane->queued = 0;
                }
                break;
            case LANDING:
                printf("Plane Thread %d (%s) is LANDING on Runway %d\n",
                       plane->id, plane->large ? "Large" : "Small", plane->runway);
                // Sleep for random amount of time
                sleepTime = rand() % 1000000;
                usleep(sleepTime);
                // Release the regions
                for (int i = 0; i < (plane->large ? 3 : 2); i++) {
                    int region = regions[i];
                    sem_post(&semRegions[region]);
                    printf("Plane Thread %d (%s) releases control of Region %d\n",
                           plane->id, plane->large ? "Large" : "Small", region);
                }
                // Set state to IDLE
                plane->state = IDLE;
//                // Wake up any queued planes
//                for (int i = 0; i < NUM_LARGE_PLANES; i++) {
//                    if (largePlanes[i].queued) {
//                        largePlanes[i].queued = 0;
//                    }
//                }
//                for (int i = 0; i < NUM_SMALL_PLANES; i++) {
//                    if (smallPlanes[i].queued) {
//
//                    }
//                }
        }
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    for (int i = 0; i < NUM_REGIONS; i++) {
        sem_init(&semRegions[i], 0, 1);
    }

    for (int i = 0; i < NUM_LARGE_PLANES + NUM_SMALL_PLANES; i++) {
        struct Plane *plane = (i < NUM_LARGE_PLANES ? &largePlanes[i] : &smallPlanes[i - NUM_LARGE_PLANES]);
        pthread_create(&plane->thread, NULL, Plane, (void *)plane);
    }

    for (int i = 0; i < NUM_LARGE_PLANES + NUM_SMALL_PLANES; i++) {
        struct Plane *plane = (i < NUM_LARGE_PLANES ? &largePlanes[i] : &smallPlanes[i - NUM_LARGE_PLANES]);
        pthread_join(plane->thread, NULL);
    }

    return 0;
}



                        //            case AWAIT_LANDING:
//                // Randomly choose a runway
//                runway = rand() % (plane->large ? 2 : 6);
//                // Choose the ordering of the regions
//                order = rand() % (plane->large ? 3 : 2);
//                for (int i = 0; i < (plane->large ? 3 : 2); i++) {
//                    regions[i] = (plane->large ? largeRuns[runway][i] : smallRuns[runway][i]);
//                }
//                printf("Plane Thread %d (%s) is AWAITING LANDING and will use runway %d with order %d\n",
//                       plane->id, plane->large ? "Large" : "Small", runway, order);
//                // Check if plane can proceed
//                canProceed = 1;
//                for (int i = 0; i < (plane->large ? 3 : 2); i++) {
//                    int region = regions[(order + i) % (plane->large ? 3 : 2)];
//                    int value;
//                    sem_getvalue(&semRegions[region], &value);
//                    if (value == 0) {
//                        canProceed = 0;
//                        break;
//                    }
//                }
//                if (canProceed) {
//                    // Take control of the regions
//                    for (int i = 0; i < (plane->large ? 3 : 2); i++) {
//                        int region = regions[(order + i) % (plane->large ? 3 : 2)];
//                        sem_wait