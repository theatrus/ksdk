
#ifndef MFS_BENCHMARK_H_
#define MFS_BENCHMARK_H_

#define NAME_SIZE     12

int32_t Sh_Benchmark(int32_t argc, char *argv[]);

int32_t mfs_speed_write(int file_handle,        /* File handle */
                        uint32_t block_size,    /* Size of one data block */
                        uint32_t block_count);  /* Number of blocks to write */

int32_t mfs_speed_read(int file_handle,         /* File handle */
                       uint32_t block_size,     /* Size of one data block */
                       uint32_t block_count);   /* Number of blocks to read */

#endif /* MFS_BENCHMARK_H_ */
