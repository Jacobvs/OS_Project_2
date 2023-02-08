#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>

#define NUM_REGIONS 6
sem_t runway[NUM_REGIONS];
pthread_mutex_t mutex;

void *printSemaphoreValue(void *arg) {
    int region = *((int *)arg);
    pthread_mutex_lock(&mutex);
    int value;
    sem_getvalue(&runway[region], &value);
    printf("Semaphore value for region %d: %d \n", region, value);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < NUM_REGIONS; i++) {
        sem_init(&runway[i], 0, 1);
    }
    // ...
    pthread_t threads[NUM_REGIONS];
    int region_id[NUM_REGIONS];
    for (int i = 0; i < NUM_REGIONS; i++) {
        region_id[i] = i;
        pthread_create(&threads[i], NULL, printSemaphoreValue, &region_id[i]);
    }
    for (int i = 0; i < NUM_REGIONS; i++) {
        pthread_join(threads[i], NULL);
    }
    // ...
    pthread_mutex_destroy(&mutex);
    for (int i = 0; i < NUM_REGIONS; i++) {
        sem_destroy(&runway[i]);
    }
    return 0;
}
