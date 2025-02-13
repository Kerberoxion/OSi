#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#define PAGE_SIZE 4096

int main(int argc, char* argv[]) {
    pid_t pid;
    char filename[256] = "/proc/self/pagemap";
    int fd;
    uint64_t pagemap_value;
    unsigned long v_address;
    off_t offset;
    ssize_t bytes_read = sizeof(uint64_t);

    pid = getpid();
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open pagemap file");
        return 1;
    }

    if(argc>1 && strcmp(argv[1],"stack") == 0) {
        v_address = (unsigned long)(&v_address);
    }

    else if(argc>1 && strcmp(argv[1],"heap") == 0) {
        int* heap = malloc(sizeof(int));
        v_address = (unsigned long)(&heap[0]);
        free(heap);
    }
    else if(argc>1 && strcmp(argv[1],"static") == 0){
        static int num = 5;
        v_address = (unsigned long)(&num);
    }

    else{
        v_address = 0;
    }

    offset = (v_address / PAGE_SIZE) * sizeof(uint64_t);

    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("Failed to seek in pagemap file");
        close(fd);
        return 1;
    }
    
    while (bytes_read == sizeof(uint64_t)) {
        bytes_read = read(fd, &pagemap_value, sizeof(uint64_t));
        printf("%.12lx: 0x%" PRIx64 " pid: %d \n", v_address, pagemap_value, pid);
        v_address += 1;
        sleep(1);
    }
    close(fd);

    return 0;
}

