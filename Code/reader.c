#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

double readTime(char* filename, size_t block_size, size_t block_count, double* throughput) {
    int fd = open(filename, O_RDONLY);

     if (fd == -1) {
        printf("Error opening file for reading\n");
        exit(0);
    }

    char* buffer = (char*)malloc(block_size);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    size_t i;
    ssize_t total_bytes = 0;
    for (i = 0; i < block_count; ++i) {
        ssize_t bytes = read(fd, buffer, block_size);
        if (bytes == -1) {
            printf("Error reading from file\n");
            free(buffer);
            close(fd);
            exit(0);
        }
        total_bytes += bytes;
    }

    gettimeofday(&end, NULL);

    free(buffer);
    close(fd);

    double time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1.0e6;
    *throughput = (total_bytes / (1024.0 * 1024.0)) / time;
    return time;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Arguments Error: Usage-> ./run <filename> <block_size> <block_count>\n");
        return 0;
    }

    char* filename = argv[1];
    size_t block_size = atoi(argv[2]);
    size_t block_count = atoi(argv[3]);

    if(block_size == 0 || block_count == 0){
        printf("Zero not allowed\n");
        return 0;
    }

    double throughput;
    double read_time = readTime(filename, block_size, block_count, &throughput);

    printf("Read time: %.6f seconds\n", read_time);
    printf("Performance: %.6f MiB/s\n", throughput);
    printf("Performance in B/s: %.6f B/s\n", throughput/(1024*1024));

    return 1;
}
