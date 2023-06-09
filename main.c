#include <stddef.h>
#include <pthread.h>
#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include "my_lib.h"

#define THREADS_PER_FUNC 6 //  THREADS / 3
#define OUTPUT_FILE "out.txt"
const size_t BLOCK_SIZES[] = {16, 1024, 1024*1024};

#define BLOCKS_PER_THREAD sizeof(BLOCK_SIZES) / sizeof(size_t)

FILE *output_file;
unsigned char *blocks[THREADS_PER_FUNC][BLOCKS_PER_THREAD];




void *WriteAndFreeThread(void* arg)  //запись в файл и очищение потока
{
    int threadGroup = *(int *)arg;
    printf("thread group %d: waiting before start writing to file\n", threadGroup);
    flockfile(output_file);
    printf("thread group %d: writing to file\n", threadGroup);

    for (int blockNum = 0; blockNum < BLOCKS_PER_THREAD; blockNum++)
    {   
        fprintf(output_file, "thread group %d, address %p:", threadGroup, blocks[threadGroup][blockNum]);

        for (int i = 0; i < BLOCK_SIZES[blockNum]; i++)
        {
            fprintf(output_file, " %d", (int)blocks[threadGroup][blockNum][i]);
        }

        my_free(blocks[threadGroup][blockNum]);
        fprintf(output_file, "\n");
    }

    funlockfile(output_file);
    return NULL;
}

void *AllocThread(void* arg) //выделение памяти для потока
{
    int threadGroup = *(int *)arg;

    for (int blockNum = 0; blockNum < BLOCKS_PER_THREAD; blockNum++)
    {
        printf("thread group %d: is allocaing block of %lu bytes\n", threadGroup, (unsigned long)BLOCK_SIZES[blockNum]);
        blocks[threadGroup][blockNum] = my_malloc(sizeof(unsigned char) * BLOCK_SIZES[blockNum]);
        printf("thread group %d: allocated block of %lu bytes at the addr %p\n", threadGroup, (unsigned long)BLOCK_SIZES[blockNum], blocks[threadGroup][blockNum]);
    }

    return NULL;
}

void *FillThread(void* arg) //заполнение потока
{
    int threadGroup = *(int *)arg;

    printf("thread group %d: filling\n", threadGroup);
    for (int blockNum = 0; blockNum < BLOCKS_PER_THREAD; blockNum++)
    {
        for (int i = 0; i < BLOCK_SIZES[blockNum]; i++)
        {
            blocks[threadGroup][blockNum][i] = threadGroup;
        }
    }

    return NULL;
}

void *CreateThreadsThread(void* arg) //создание
{
    pthread_t newThread;

    pthread_create(&newThread, NULL, AllocThread, arg);
    pthread_join(newThread, NULL);

    pthread_create(&newThread, NULL, FillThread, arg);
    pthread_join(newThread, NULL);
    pthread_create(&newThread, NULL, WriteAndFreeThread, arg);
    pthread_join(newThread, NULL);

    printf("thread group %d: done!\n", *(int *)arg);
    my_free(arg);
    return NULL;
}

int main()
{
    _Static_assert(THREADS_PER_FUNC <= UCHAR_MAX + 1, "THREADS_PER_FUNC should be less or equal than UCHAR_MAX + 1");
    output_file = fopen(OUTPUT_FILE, "w");


    pthread_t threads[THREADS_PER_FUNC];
    for (int threadGroup = 0; threadGroup < THREADS_PER_FUNC; threadGroup++)
    {
        int *threadGroupArg = my_malloc(sizeof(int));
        *threadGroupArg = threadGroup;
        pthread_create(&threads[threadGroup], NULL, CreateThreadsThread, (void *)threadGroupArg);
    }

    for (int threadGroup = 0; threadGroup < THREADS_PER_FUNC; threadGroup++)
    {
        pthread_join(threads[threadGroup], NULL);
    }

    fclose(output_file);


    return 0;
}