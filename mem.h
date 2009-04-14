/*
 * Copyright 2009  Raphael Kubo da Costa <kubito@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __MEM_H
#define __MEM_H

#include <stdlib.h>

/**
 * Wrapper over MEM_ALLOC_N. Equivalent to allocating
 * memory for 1 entry of type /type/.
 */
#define MEM_ALLOC(type) (MEM_ALLOC_N(type, 1))

/**
 * Allocates memory for numelem elements of the given type.
 */
#define MEM_ALLOC_N(type, numelem) ((type*)__memAllocate(numelem, sizeof(type)))

/**
 * This function should not be called directly. It's a wrapper
 * over calloc.
 * If memory can't be properly allocated, it displays an error
 * message and exits the program.
 *
 * @param numelem The number of the elements in the array.
 * @param elemsize The size of each element.
 */
void *__memAllocate(size_t numelem, size_t elemsize);

#endif /* __MEM_H */
