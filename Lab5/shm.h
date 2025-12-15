#ifndef SHM_H
#define SHM_H

#define BUF_SIZE 1024

struct shmseg {
    int cnt;
    char buf[BUF_SIZE];
};

#define SEM_KEY 0x1234
#define SHM_FILE "/tmp/mmap_shmseg"

#define READ_SEM 0
#define WRITE_SEM 1

#endif
