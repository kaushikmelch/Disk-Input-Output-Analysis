#include <benchmark/benchmark.h>
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

static void BM_ReadTime(benchmark::State& state) {
    const char* filename = "gg.txt";
    const size_t block_size = static_cast<size_t>(state.range(0));
    const size_t block_count = static_cast<size_t>(state.range(1));

    double throughput;
    double read_time;

    for (auto _ : state) {
        read_time = readTime(const_cast<char*>(filename), block_size, block_count, &throughput);
    }

    state.SetBytesProcessed(static_cast<int64_t>(block_size) * static_cast<int64_t>(block_count) * state.iterations());
    state.SetIterationTime(read_time);

    state.counters["Throughput (MiB/s)"] = throughput;
}

BENCHMARK(BM_ReadTime)
    ->Args({128, 33554432}) 
    ->Args({256, 16777216})
    ->Args({512, 8388608})
    ->Args({1024, 4194304})
    ->Args({2048, 2097152});

BENCHMARK_MAIN();
