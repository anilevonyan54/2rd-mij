#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* Thread 1: doubles the number */
void *threadFunc1(void *arg)
{
    int value = *(int *)arg;

    int *result = malloc(sizeof(int));
    *result = value * 2;  // double it

    printf("Thread 1: received %d, returning %d\n", value, *result);
    return result;
}

/* Thread 2: triples the number */
void *threadFunc2(void *arg)
{
    int value = *(int *)arg;

    int *result = malloc(sizeof(int));
    *result = value * 3;  // triple it

    printf("Thread 2: received %d, returning %d\n", value, *result);
    return result;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int input = atoi(argv[1]);
    pthread_t t1, t2;
    int *res1, *res2;

    /* Create Thread 1 */
    if (pthread_create(&t1, NULL, threadFunc1, &input) != 0) {
        perror("pthread_create t1");
        exit(EXIT_FAILURE);
    }

    /* Wait for Thread 1 result */
    if (pthread_join(t1, (void **)&res1) != 0) {
        perror("pthread_join t1");
        exit(EXIT_FAILURE);
    }

    printf("Main: received %d from Thread 1\n", *res1);

    /* Create Thread 2 with the result from Thread 1 */
    if (pthread_create(&t2, NULL, threadFunc2, res1) != 0) {
        perror("pthread_create t2");
        free(res1);
        exit(EXIT_FAILURE);
    }

    /* Wait for Thread 2 result */
    if (pthread_join(t2, (void **)&res2) != 0) {
        perror("pthread_join t2");
        free(res1);
        exit(EXIT_FAILURE);
    }

    printf("Main: final result = %d\n", *res2);

    /* free dynamically allocated results */
    free(res1);
    free(res2);

    return 0;
}
