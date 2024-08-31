## CS-GY 6233 FINAL PROJECT README:

The project is centered around performance.
We will try to get disk I/O as fast as possible and evaluate the effects of caches and the cost of system calls. In the end you should have a good understanding of what sits in the way between your process requesting data from disk and receiving it.

### Part 1:

A program that can read and write a file from disk using the standard C/C++ library's `open`, `read`, `write`, and `close` functions.

Code file: run.c

compile the C program-> gcc run.c -o run
Run the C program->
	a) ./run <filename> -r <block_size> <block_count>
	b) ./run <filename> -w <block_size> <block_count>

Note:
a) If file doesn't exist and you write, it creates a new file and writes to it.
b) If file exists and you write, it rewrites the file.
c) if file doesn't exist and you read, the program exits with error.
d) If file size is less than block_size*block_count, the program exits with error. So make sure you have a big enough file for a particular block size and block count.



### Part 2:

When measuring things, it helps if they run for "reasonable" time. It is hard to measure things that run too fast as you need high-precision clocks and a lot of other things can affect the measurement. It is also annoying to wait for a long time for an experiment, especially if you have to do many experiments. For this reason you should make sure that your experiments take "reasonable" time.

The following is a program to find a file size which can be read in "reasonable" time.


Code file: run2.c

compile the C program-> gcc run2.c -o run2
Run the C program->
	a) ./run2 <filename> <block_size>

Note:
This program takes a file name and creates it if it doesn't exist or rewrites it if it exists with a small file size. It then checks the time taken to read it and keeps doubling until the time taken to read it lies between 5 and 15 seconds. At the end you will have a file big enough to be read within the time limit for a given block size. The program will also output the corresponding block count.


### Part 3:

Because we would be looking at different file sizes when using different block sizes, it makes sense to use units for performance that are independent of the size. We use MiB/s (this is megabytes per second).

The following is a program that outputs the performance it achieved in MiB/s

Code file: reader.c

compile the C program-> gcc reader.c -o reader
Run the C program->
	a) ./reader <filename> <block_size> <block_count>

Note:
This program reads an existing file, takes block size and block count input as well. It outputs time taken and performance to read entire file for given block size and block count. TO ensure that your file is read completely, give the correct block size and block count whose product is atleast equal to file size.


### Part 4:

Same as Part 3. Just clear the cache and execute the code to see the difference in performance for different block sizes.


### Part 5:

To measure performance in MiB/s for 1 block size, use same reader.c file with block size argument as 1 and block count argument as file_size(2 GB, 4 GB) etc.
Note: It will take lot of time to run since we are doing 1 byte at a time.
Performance in B/s is also printed.

To test with other system calls. Use the file syscalltest.c

This will run the performance for read, write and lseek with various block sizes defined in the main() function which can be changed as needed.

Code file: syscalltest.c

compile the C program-> gcc syscalltest.c -o syscalltest
Run the C program->
	a) ./syscalltest <filename>


### Part 6:

Code file: fast.c

compile the C program-> gcc fast.c -o fast
Run the C program->
	a) ./fast <filename>

Note:
This program, returns the XOR value of the file(where 4 bytes(characters from file) are taken together as an integer). It also prints the time taken to accomplish the read operation and also the file size that was read.
