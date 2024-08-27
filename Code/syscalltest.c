#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

double writeTime(char* filename, size_t block_size, size_t block_count, double* throughput) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

     if (fd == -1) {
        printf("Error opening file for reading\n");
        exit(0);
    }

    char* buffer = (char*)malloc(block_size);
    memset(buffer, 'C', block_size);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    size_t i;
    ssize_t total_syscalls = 0;
    for (i = 0; i < block_count; ++i) {
        ssize_t bytes = write(fd, buffer, block_size);
        if (bytes == -1) {
            printf("Error writing to file\n");
            free(buffer);
            close(fd);
            exit(0);
        }
        total_syscalls++;
    }

    gettimeofday(&end, NULL);

    free(buffer);
    close(fd);

    double time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1.0e6;
    *throughput = total_syscalls / time;
    return time;
}

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
    ssize_t total_syscalls = 0;
    for (i = 0; i < block_count; ++i) {
        ssize_t bytes = read(fd, buffer, block_size);
        if (bytes == -1) {
            printf("Error reading from file\n");
            free(buffer);
            close(fd);
            exit(0);
        }
        total_syscalls++;
    }

    gettimeofday(&end, NULL);

    free(buffer);
    close(fd);

    double time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1.0e6;
    *throughput = total_syscalls / time;
    return time;
}

double measureLseekTime(char* filename, size_t block_size, size_t block_count, double* throughput) {
    int fd = open(filename, O_RDONLY);

    if (fd == -1) {
        printf("Error opening file for reading\n");
        exit(0);
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    size_t i;
    ssize_t total_syscalls = 0;
    for (i = 0; i < block_count; ++i) {
        off_t offset = lseek(fd, block_size, SEEK_CUR);
        if (offset == (off_t)-1) {
            printf("Error seeking in file\n");
            close(fd);
            exit(0);
        }
        total_syscalls++;
    }

    gettimeofday(&end, NULL);

    close(fd);

    double time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1.0e6;
    *throughput = total_syscalls / time;

    return time;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Arguments Error: Usage-> ./syscalltest <filename>\n");
        return 0;
    }

    char* filename = argv[1];
    size_t blockSizeArray[] = {64, 128, 256, 512};
    size_t blockCountArray[] = {16777216, 8388608, 4194304, 2097152};
    char writeFile[] = "random.txt";

    double readThroughput, lseekThroughput, writeThroughput;

    for(int i=0;i<4; i++){
        double read_time = readTime(filename, blockSizeArray[i], blockCountArray[i], &readThroughput);
        printf("Read Perf: Block size-> %zu, Syscalls/s-> %.6f\n", blockSizeArray[i], readThroughput);
        double write_time = writeTime(writeFile, blockSizeArray[i], blockCountArray[i], &writeThroughput);
        printf("Write Perf: Block size-> %zu, Syscalls/s-> %.6f\n", blockSizeArray[i], writeThroughput);
        double lseek_time = measureLseekTime(filename, blockSizeArray[i], blockCountArray[i], &lseekThroughput);
        printf("Lseek Perf: Block size-> %zu, Syscalls/s-> %.6f\n", blockSizeArray[i], lseekThroughput); 
    }

    return 1;
}
