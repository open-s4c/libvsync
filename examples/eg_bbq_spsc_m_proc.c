#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <vsync/vtypes.h>
#include <vsync/queue/bbq_spsc.h>

#define QUEUE_SIZE      4095U
#define EPOCH_SIZE      100U
#define NUM_ENTRIES     1U
#define SLEEP_MICRO_SEC 10
#define NON_BLOCKING    false
#define BLOCKING        true

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

bbq_spsc_t *g_bbq;

void *
create_shared_memory(vsize_t size)
{
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    return mmap(NULL, size, protection, visibility, -1, 0);
}

void
check_space(void)
{
    vuint64_t data  = 0;
    vuint64_t count = 0;

    while (bbq_spsc_dequeue(g_bbq, &data, NUM_ENTRIES, NON_BLOCKING)) {}

    while (bbq_spsc_enqueue(g_bbq, &data, NUM_ENTRIES, NON_BLOCKING)) {
        count++;
    }
    while (bbq_spsc_dequeue(g_bbq, &data, NUM_ENTRIES, NON_BLOCKING)) {}

    printf(
        "Current size of the queue is %ld after %d crashes, shouldn't be "
        "zero.\n",
        count, EPOCH_SIZE);
    assert(count != 0);
}

void *
writer(void *arg)
{
    V_UNUSED(arg);
    vuint64_t data = 0;
    while (true) {
        data = (data + 1) % (QUEUE_SIZE - 1);
        while (!bbq_spsc_enqueue(g_bbq, &data, NUM_ENTRIES, NON_BLOCKING)) {}
        usleep(SLEEP_MICRO_SEC);
    }
}

void *
reader(void *arg)
{
    V_UNUSED(arg);
    vuint64_t data;
    while (true) {
        while (!bbq_spsc_dequeue(g_bbq, &data, 1, NON_BLOCKING)) {}
        usleep(SLEEP_MICRO_SEC);
        assert(data < QUEUE_SIZE - 1);
    }
}

int
main(void)
{
    srand(time(NULL));
    vuint64_t sz = bbq_spsc_memsize(QUEUE_SIZE);
    g_bbq        = create_shared_memory(sz);
    assert(g_bbq);
    vbool_t success = bbq_spsc_init(g_bbq, sz);
    ASSERT(success && "BBQ init failed");
    for (vuint32_t i = 0; i < EPOCH_SIZE; i++) {
        pid_t pid = fork();
        assert(pid >= 0);
        if (pid != 0) {
            usleep(rand() % 10000);
            while (kill(pid, SIGTERM) < 0) {}
        } else {
            pthread_t t_writer, t_reader;
            pthread_create(&t_writer, NULL, writer, NULL);
            pthread_create(&t_reader, NULL, reader, NULL);
            pthread_join(t_writer, NULL);
            pthread_join(t_reader, NULL);
            return 0;
        }
    }
    check_space();
    return 0;
}
