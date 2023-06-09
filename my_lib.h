#pragma once

#ifndef __my_lib__
#define __my_lib__

void *my_malloc(size_t size);
void my_free(void *ptr);
void *my_calloc(size_t num, size_t size);
void* my_realloc( void* pointer, size_t memory );

#endif