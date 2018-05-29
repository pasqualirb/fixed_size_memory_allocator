/*
 * fixed size memory allocator using free list
 * Copyright (C) 2017  Ricardo Biehl Pasquali
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * 11/04/2017
 */

#ifdef DEBUG
#include <stdio.h>
#define TRACE(...)  printf(__VA_ARGS__)
#else
#define TRACE(...)  do { } while (0)
#endif

#include <stdlib.h> /* malloc() */

#include "allocator.h"

/*
 * NOTE: size argument can't be zero!
 *
 * before expand:
 *   A -> B -> [NULL]
 * after expanded:
 *   C -> D -> A -> B -> [NULL]
 * 1st element is the list head
 */
static void
expand_capacity(struct memory *mem, unsigned long size)
{
	struct free_chunk *new_head;
	struct free_chunk *current_chunk;
	unsigned long i;

	new_head = malloc(mem->chunk_size);
	current_chunk = new_head;

	/* run only if size is greater than 1 */
	for (i = 1; i < size; i++) {
		current_chunk->next = malloc(mem->chunk_size);
		current_chunk = current_chunk->next;
	}

	current_chunk->next = mem->free_list;
	mem->free_list = new_head;
	mem->capacity += size;
}

/*
 * in application code you can check when free_list is
 * getting full with:
 *   if (mem.capacity - mem.charge < X)
 *       allocator_expand_capacity(Y);
 */
int
allocator_expand_capacity(struct memory *mem, unsigned long size)
{
	TRACE("allocator: expanding capacity");

	if (!size)
		return -1;

	expand_capacity(mem, size);

	return 0;
}

void
allocator_free(struct memory *mem, void *ptr)
{
	struct free_chunk *free_chunk;

	free_chunk = ptr;

	/*
	 * insert the free chunk in the free_list -- we
	 * set the next of the new free chunk to the head
	 * of free_list, and set the head of free_list to
	 * the new free chunk
	 */
	free_chunk->next = mem->free_list;
	mem->free_list = free_chunk;

	mem->charge--;
}

void*
allocator_alloc(struct memory *mem, unsigned long size)
{
	struct free_chunk *free_chunk;

	if (size > mem->chunk_size)
		return NULL;

	/*
	 * expand capacity if there isn't any free chunk
	 * and auto_expand is enabled
	 *
	 * a.k.a.  if (mem->charge == mem->capacity)
	 */
	if (mem->free_list == NULL) {
		if (mem->auto_expand_amount)
			expand_capacity(mem, mem->auto_expand_amount);
		else
			return NULL;
	}

	/*
	 * remove the free chunk from the free_list -- we
	 * simply get the fist free chunk and set the head
	 * of free_list to the next free chunk
	 */
	free_chunk = mem->free_list;
	mem->free_list = free_chunk->next;

	mem->charge++;

	return free_chunk;
}

void
allocator_clean_up(struct memory *mem)
{
	TRACE("allocator: cleaning up");

	struct free_chunk *current;

	while (mem->free_list) {
		current = mem->free_list;
		mem->free_list = current->next;
		free(current);
		mem->capacity--;
	}

	/*
	 * notice that here capacity is not necessarily
	 * zero, that's because there may be chunks
	 * currently in use
	 */
}

/*
 * check parameters, initialize memory struct,
 * and allocate memory with expand_capacity()
 */
int
allocator_init(struct memory *mem, unsigned long object_size,
	       unsigned long initial_size, unsigned long auto_expand_amount)
{
	TRACE("allocator: initializing");

	if (!initial_size)
		return -1;

	/* chunk size must be at least the size of `struct free_chunk` */
	if (object_size < sizeof(struct free_chunk))
		mem->chunk_size = sizeof(struct free_chunk);
	else
		mem->chunk_size = object_size;

	mem->free_list = NULL;
	mem->auto_expand_amount = auto_expand_amount;
	mem->charge = 0;
	mem->capacity = 0;

	expand_capacity(mem, initial_size);

	return 0;
}
