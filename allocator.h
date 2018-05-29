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

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

/*
 * when a chunk is free it becomes a `struct free_chunk`,
 * so we can maintain a single linked list of free chunks
 * (a free_list). Therefore, if the chunk size you request
 * is smaller than size of `struct free_chunk` we set the
 * chunk size as the size of `struct free_chunk`.
 */
struct free_chunk {
	struct free_chunk *next;
};

struct memory {
	/* head of the free_list */
	struct free_chunk *free_list;

	/* size of the chunks in this memory instance */
	unsigned long chunk_size;

	/*
	 * if set to a value different of 0 we
	 * automatically increase capacity by this value
	 * when free_list empties
	 */
	unsigned long auto_expand_amount;

	/* current charge level of allocator */
	unsigned long charge;
	/* current capacity of allocator */
	unsigned long capacity;
};

int
allocator_expand_capacity(struct memory *mem, unsigned long size);

void
allocator_free(struct memory *mem, void *ptr);

void*
allocator_alloc(struct memory *mem, unsigned long size);

void
allocator_clean_up(struct memory *mem);

int
allocator_init(struct memory *mem, unsigned long object_size,
	       unsigned long initial_size, unsigned long auto_expand_amount);

#endif /* ALLOCATOR_H */
