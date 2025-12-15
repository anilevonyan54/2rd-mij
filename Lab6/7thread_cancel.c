#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

static void *threadFunc(void *arg)
{
    /* ✅ Disable thread cancellation */
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

    printf("New thread started (cancellation disabled)\n");

    for (int i = 0; ; i++)
    {
        printf("%d\n", i);
        sleep(1); /* cancellation request will be ignored */
    }

    return NULL;
}

int main(void)
{
    printf("Main thread started\n");

    pthread_t t1;
    void *res;
    int s;

    s = pthread_create(&t1, NULL, threadFunc, NULL);
    if (s != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    sleep(3);

    printf("Main thread sending cancellation request\n");
    s = pthread_cancel(t1);
    if (s != 0) {
        perror("pthread_cancel");
        exit(EXIT_FAILURE);
    }

    /* Wait a bit to observe behavior */
    sleep(5);

    printf("Main thread exiting (worker keeps running)\n");
    exit(EXIT_SUCCESS);
}
