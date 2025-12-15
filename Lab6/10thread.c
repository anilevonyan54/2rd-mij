#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define SIZE 10

/* Shared data array */
int data[SIZE] = {1,2,3,4,5,6,7,8,9,10};

/* Structure to pass range info */
typedef struct {
    int start;
    int end;
} Range;

/* Thread function: compute sum of a range */
void *sum_range(void *arg)
{
    Range *r = (Range *)arg;
    int *sum = malloc(sizeof(int));
    *sum = 0;

    for (int i = r->start; i <= r->end; ++i) {
        *sum += data[i];
        usleep(100000);   // simulate competitive execution
    }

    printf("Thread computed sum [%d..%d] = %d\n",
           r->start, r->end, *sum);

    return sum;
}

int main(void)
{
    pthread_t t1, t2;
    int *sum1, *sum2;

    Range r1 = {0, 4};   // first 5 elements
    Range r2 = {5, 9};   // last 5 elements

    /* Create threads */
    pthread_create(&t1, NULL, sum_range, &r1);
    pthread_create(&t2, NULL, sum_range, &r2);

    /* Wait for results */
    pthread_join(t1, (void **)&sum1);
    pthread_join(t2, (void **)&sum2);

    /* Output results */
    printf("\nArray element count: %d\n", SIZE);
    printf("Sum of first 5 elements: %d\n", *sum1);
    printf("Sum of last 5 elements: %d\n", *sum2);
    printf("Total sum: %d\n", *sum1 + *sum2);

    /* Cleanup */
    free(sum1);
    free(sum2);

    return 0;
}
