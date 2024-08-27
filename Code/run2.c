#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

void writeFile(char* filename, size_t block_size, size_t block_count) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (fd == -1) {
        printf("Error opening file for writing\n");
        exit(0);
    }

    char* buffer = (char*)malloc(block_size);
    memset(buffer, 'C', block_size);
    size_t i;
    for (i = 0; i < block_count; ++i) {
        ssize_t bytes = write(fd, buffer, block_size);
        if (bytes == -1) {
            printf("Error writing to file\n");
            free(buffer);
            close(fd);
            exit(0);
        }
    }

    free(buffer);
    close(fd);
}

double readTime(char* filename, size_t block_size, size_t block_count) {
    int fd = open(filename, O_RDONLY);

    if (fd == -1) {
        printf("Error opening file for reading\n");
        exit(0);
    }

    char* buffer = (char*)malloc(block_size);

    struct timeval start, end;
    gettimeofday(&start, NULL);
    size_t i;
    for (i = 0; i < block_count; ++i) {
        ssize_t bytes = read(fd, buffer, block_size);
        if (bytes == -1) {
            printf("Error reading from file\n");
            free(buffer);
            close(fd);
            exit(0);
        }
    }

    gettimeofday(&end, NULL);

    free(buffer);
    close(fd);

    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1.0e6;
}

size_t findBlockCount(char* filename, size_t block_size) {
    size_t block_count = 1;
    double read_time = 0.0;

    while (read_time < 5.0 || read_time > 15.0){
        block_count *= 2;
        writeFile(filename, block_size, block_count);
        read_time = readTime(filename, block_size, block_count);
        printf("Block count: %zu, Read time: %.6f seconds\n", block_count, read_time);
    }

    return block_count;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Arguments Error: Usage-> ./run2 <filename> <block_size>\n");
        return 0;
    }

    char* filename = argv[1];
    size_t block_size = atoi(argv[2]);
    
    if(block_size == 0){
        printf("Zero not allowed\n");
        return 0;
    }

    size_t file_size = findBlockCount(filename, block_size);
    printf("Block Count: %zu\n", file_size);
    printf("Block Size: %zu\n", block_size);
    printf("Optimal file size for reasonable read time: %zu bytes\n", file_size * block_size);

    return 1;
}