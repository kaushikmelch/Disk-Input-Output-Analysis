#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/stat.h>

#define MAX_THREADS_ALLOWED 512

typedef struct {
    char* filename;
    size_t block_size;
    size_t block_count;
    size_t thread_id;
    size_t num_threads;
    size_t thread_blocks;
    uint32_t result;
} thread_struct;

void* threadReader(void* arg) {
    thread_struct* data = (thread_struct*)arg;
    int fd = open(data->filename, O_RDONLY);

    if (fd == -1) {
        printf("Error opening file for reading\n");
        exit(0);
    }

    char* buffer = (char*)malloc(data->block_size);

    uint32_t partial_result = 0;

    size_t start_block = data->thread_id * data->thread_blocks;
    size_t end_block;

    if(data->thread_id == data->num_threads - 1) end_block = data->block_count;
    else end_block = (data->thread_id + 1) * data->thread_blocks;
    
    off_t offset = start_block*(data->block_size);
    lseek(fd, offset, SEEK_SET);

    for (size_t i = start_block; i < end_block; ++i) {
        ssize_t bytes = read(fd, buffer, data->block_size);
        if (bytes == -1) {
            printf("Error reading from file\n");
            free(buffer);
            close(fd);
            exit(0);
        }

        for (size_t j = 0; j < bytes; j += 4) {
            uint32_t value;
            memcpy(&value, buffer + j, sizeof(uint32_t));
            partial_result ^= value;
        }
    }

    free(buffer);
    close(fd);

    data->result = partial_result;
    return NULL;
}

double readTime(char* filename, size_t block_size, size_t block_count, size_t num_threads, uint32_t* result) {
    pthread_t threads[MAX_THREADS_ALLOWED];
    thread_struct thread_data[MAX_THREADS_ALLOWED];

    struct timeval start, end;
    size_t threadBlocks = block_count/num_threads;
    size_t threads_count = num_threads;

    if(block_count%num_threads != 0){
        threads_count++;
    }

    gettimeofday(&start, NULL);

    for (size_t i = 0; i < threads_count; ++i) {
        thread_data[i].filename = filename;
        thread_data[i].block_size = block_size;
        thread_data[i].block_count = block_count;
        thread_data[i].thread_id = i;
        thread_data[i].num_threads = threads_count;
        thread_data[i].thread_blocks = threadBlocks;
        pthread_create(&threads[i], NULL, threadReader, (void*)&thread_data[i]);
    }

    for (size_t i = 0; i < threads_count; ++i) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&end, NULL);

    double time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1.0e6;

    *result = thread_data[0].result;
    for (size_t i = 1; i < threads_count; ++i) {
        *result ^= thread_data[i].result;
    }

    return time;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Arguments Error: Usage: ./fast <filename>\n");
        return 0;
    }

    char* filename = argv[1];
    struct stat file_stat;

    if (stat(filename, &file_stat) == -1) {
        printf("Error getting file information\n");
        exit(0);
    }

    size_t block_size = 16384;
    size_t block_count = file_stat.st_size / block_size;
    
    if(file_stat.st_size % block_size != 0){
        block_count += 1;
    }

    size_t num_threads = 256;

    uint32_t result;
    double read_time = readTime(filename, block_size, block_count, num_threads, &result);

    printf("File Size: %zu bytes\n", file_stat.st_size);
    printf("XOR value: %08x\n", result);
    printf("Read time: %.6f seconds\n", read_time);

    double throughput = (file_stat.st_size/(1024*1024*1024))/read_time;
    printf("Performance: %.6f GiB/s\n", throughput);

    return 1;
}