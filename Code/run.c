#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

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

void readFile(char* filename, size_t block_size, size_t block_count) {
    
    int fd = open(filename, O_RDONLY);

    if (fd == -1) {
        printf("Error opening file for reading\n");
        exit(0);
    }

    struct stat file_stat;
    if (stat(filename, &file_stat) == -1) {
        printf("Error getting file information\n");
        exit(0);
    }

    if(file_stat.st_size < block_count*block_size){
        printf("Error: Your file is too small to read for the given block size and block count\n");
        exit(0);
    }

    char* buffer = (char*)malloc(block_size);
    size_t i;
    for (i = 0; i < block_count; ++i) {
        ssize_t bytes = read(fd, buffer, block_size);
        if (bytes == -1) {
            printf("Error reading from file\n");
            free(buffer);
            close(fd);
            exit(0);
        }
        write(STDOUT_FILENO, buffer, sizeof(buffer));
    }

    free(buffer);
    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        printf("Arguments Error: Usage-> ./run <filename> [-r|-w] <block_size> <block_count>\n");
        return 0;
    }

    char* filename = argv[1];
    char* mode = argv[2];
    size_t block_size = atoi(argv[3]);
    size_t block_count = atoi(argv[4]);


    if(block_size == 0 || block_count == 0){
        printf("Zero not allowed\n");
        return 0;
    }

    if (strcmp(mode, "-w") == 0) {
        writeFile(filename, block_size, block_count);
    } else if (strcmp(mode, "-r") == 0) {
        readFile(filename, block_size, block_count);
    } else {
        printf("Arguments Error: Usage-> ./run <filename> [-r|-w] <block_size> <block_count>\n");
        return 0;
    }

    return 1;
}