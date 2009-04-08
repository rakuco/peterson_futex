#include <assert.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

void *__memAllocate(size_t numelem, size_t elemsize)
{
  void *chunk = calloc(numelem, elemsize);

  assert(chunk != NULL);

  memset(chunk, 0, elemsize);

  return chunk;
}
