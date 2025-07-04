#define _GNU_SOURCE
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>

/* this code is shit and rushed */

#define IOCTL_CHECK_MIS _IO(69, 0)
#define IOCTL_CORE _IOR(69, 1, unsigned int)
#define IOCTL_PMC _IOR(69, 2, unsigned int)

unsigned char benchmark_routine1[] = 
{ 
    0x0F, 0x1F, 0x04, 0x00, 0x90, 0x90, 0x89, 0xF9, 
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
    0x90, 0x0F, 0x33, 0x89, 0xC6, 0x89, 0xD7, 0x89, 
    0xC8, 0x66, 0x83, 0xC0, 0x04, 0x66, 0x83, 0xE8, 
    0x02, 0x66, 0x83, 0xE8, 0x02, 0x0F, 0xB7, 0xC8, 
    0x0F, 0x33, 0x90, 0x90, 0x90, 0x90, 0x48, 0xC1,
    0xE7, 0x20, 0x48, 0xC1, 0xE2, 0x20, 0x48, 0x09, 
    0xFE, 0x48, 0x09, 0xD0, 0x48, 0x29, 0xF0, 0xC3
};

int main(void)
{
    int fd = open("/dev/predecode_re", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    unsigned int core = 0;
    if (ioctl(fd, IOCTL_CORE, &core) < 0) {
        close(fd);
        perror("core");
        return 1;
    }

    unsigned int pmc_no = 0;
    if (ioctl(fd, IOCTL_PMC, &pmc_no) < 0) {
        close(fd);
        perror("pmc");
        return 1;
    }

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(core, &mask);
    if (sched_setaffinity(getpid(), sizeof(mask), &mask) < 0) {
        close(fd);
        perror("affinity");
        return 1;
    }

    char *mem = mmap(NULL, 65536, PROT_READ | PROT_WRITE | PROT_EXEC, 
                     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (mem == MAP_FAILED) {
        close(fd);
        perror("mmap");
        return 1;
    }

    int no_blocks = 65536/64;
    for (int i = 0; i < no_blocks; i++)
        memcpy(mem + (i * 64),  benchmark_routine1, sizeof(benchmark_routine1));

    unsigned long count = 0;
    for (int i = 0; i < no_blocks; i++) {
        char *cacheline = mem + (i * 64);
        __asm__ __volatile__ (
            "movl %[pmc_no], %%edi;"
            "call *%[func];"
            :"=a"(count)
            :[func]"r"(cacheline), 
             [pmc_no]"r"(pmc_no)
            :"%rcx", "%rdx", "%rsi", "%rdi", "%r8");
    }

    ioctl(fd, IOCTL_CHECK_MIS);

    unsigned long re_count = 0;
    for (int i = 0; i < no_blocks; i++) {
        char *cacheline = mem + (i * 64);
        __asm__ __volatile__ (
            "movl %[pmc_no], %%edi;"
            "call *%[func];"
            :"=a"(re_count)
            :[func]"r"(cacheline), 
             [pmc_no]"r"(pmc_no)
            :"%rcx", "%rdx", "%rsi", "%rdi", "%r8");
    }

    ioctl(fd, IOCTL_CHECK_MIS);

    printf("----- usr -----\n");
    printf("count %llu\n", count);
    printf("recount: %llu\n", re_count);
    printf("----- end -----\n");

    munmap(mem, 65536);
    close(fd);

    return 0;
}