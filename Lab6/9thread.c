#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* Global shared array */
int data[10] = {0};

/* Mutex to protect shared data */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* Thread function: fills EVEN indices */
void *fill_even(void *arg)
{
    for (int i = 0; i < 10; i += 2) {
        pthread_mutex_lock(&mutex);
        data[i] = i + 1;  // values in [1..10]
        printf("Thread: filled data[%d] = %d\n", i, data[i]);
        pthread_mutex_unlock(&mutex);
        usleep(100000); // simulate competition
    }
    return NULL;
}

int main(void)
{
    pthread_t t;

    /* Create worker thread */
    if (pthread_create(&t, NULL, fill_even, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    /* Main thread fills ODD indices */
    for (int i = 1; i < 10; i += 2) {
        pthread_mutex_lock(&mutex);
        data[i] = i + 1;  // values in [1..10]
        printf("Main: filled data[%d] = %d\n", i, data[i]);
        pthread_mutex_unlock(&mutex);
        usleep(150000); // simulate competition
    }

    /* Wait for worker thread */
    pthread_join(t, NULL);

    /* Verification stage */
    printf("\nVerification of filled array:\n");
    int valid = 1;

    for (int i = 0; i < 10; i++) {
        printf("data[%d] = %d\n", i, data[i]);
        if (data[i] < 1 || data[i] > 10) {
            valid = 0;
        }
    }

    if (valid)
        printf("\n✅ Array verification PASSED\n");
    else
        printf("\n❌ Array verification FAILED\n");

    pthread_mutex_destroy(&mutex);
    return 0;
}
