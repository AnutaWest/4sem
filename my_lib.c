#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>	
#include <sys/mman.h>
#include <errno.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stddef.h>
#include <stdalign.h>
#include <stdint.h>
#include <inttypes.h>
#include <fcntl.h>
#include "my_lib.h"

void* my_malloc(size_t memory) 
{
    int fd = open("/dev/zero", O_RDWR);
    char* data = mmap(0, memory + sizeof(int), PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
    close(fd);
    *((int*)(data)) = memory;
    return (int*)(data) + 1;
}

void* my_calloc(size_t num, size_t size) 
{
	void* pointer = my_malloc(num*size);
	if (pointer) {memset(pointer,0,num*size);}
	return pointer;
}

void my_free(void* pointer) 
{
    int* data = (int*)pointer - 1;
    munmap(data,*data + sizeof(int));
}

void* my_realloc( void* pointer, size_t memory ) 
{
    void* new_pointer = my_malloc(memory);
    if (new_pointer) {
        int* data = (int*)pointer - 1;
        memmove(new_pointer, pointer, *data);
        my_free(pointer);
    }
    return new_pointer;
}