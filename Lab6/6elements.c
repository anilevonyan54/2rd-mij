#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define ARR_SIZE 5
#define MAX_LENGTH 64

/* Constant elements (read-only, safe to share) */
static const char *array[ARR_SIZE] = {
    "Element 1",
    "Element 2",
    "Element 3",
    "Element 4",
    "Element 5",
};

/* ✅ THREAD-LOCAL BUFFER
   Each thread gets its OWN copy */
static __thread char buf[MAX_LENGTH];

char *getElement(int index)
{
    if (index >= ARR_SIZE) {
        snprintf(buf, MAX_LENGTH, "Unknown element");
    } else {
        strncpy(buf, array[index], MAX_LENGTH - 1);
        buf[MAX_LENGTH - 1] = '\0';
    }

    return buf;
}

static void *threadFunc(void *arg)
{
    char *el;

    printf("Other thread about to call getElement()\n");
    el = getElement(1);
    printf("Other thread: element = %s (addr=%p)\n", el, (void *)el);

    return NULL;
}

int main(void)
{
    pthread_t t;
    char *el;

    el = getElement(0);
    printf("Main thread has called getElement()\n");

    if (pthread_create(&t, NULL, threadFunc, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(t, NULL) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    printf("Main thread: element = %s (addr=%p)\n", el, (void *)el);
    return 0;
}
