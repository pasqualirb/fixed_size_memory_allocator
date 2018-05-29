
/*
 * 11/04/2017
 */

#include <stdio.h>  /* printf() */
#include <stdlib.h> /* malloc() */

#include "allocator.h"

#define TEST_ALLOC  1000
#define TEST_ROTATIONS  5000

static void
print_test_info(struct memory *mem, unsigned long expected_capacity,
                unsigned long expected_charge)
{
	printf("[%s] expected capacity: %ld (capacity: %ld)\n",
	       expected_capacity == mem->capacity ? "OK" : "ERROR",
	       expected_capacity, mem->capacity);
	printf("[%s] expected charge: %ld (charge: %ld)\n",
	       expected_charge == mem->charge ? "OK" : "ERROR",
	       expected_charge, mem->charge);
	printf("----------\n");
}

int
main(int argc, char **argv)
{
	unsigned long object_size;
	unsigned long initial_capacity;

	struct memory mem;
	void **ptr_lst;
	int i, j;


	if (argc < 3) {
		printf("usage: cmd <object_size> <initial_capacity>\n");
		return 1;
	}

	object_size = atoi(argv[1]);
	initial_capacity = atoi(argv[2]);


	/* this allocation will be used in tests (0, 1, 2) */
	ptr_lst = malloc(sizeof(void*) * (initial_capacity + 1));

	/*
	 * test 0: auto_expand_factor enabled
	 */
	printf("==========test 0==========\n");
	if (allocator_init(&mem, object_size, initial_capacity, 5) == -1)
		return 1;
	print_test_info(&mem, initial_capacity, 0);

	for (i = 0; i < (initial_capacity + 1); i++)
		ptr_lst[i] = allocator_alloc(&mem, object_size);
	print_test_info(&mem, initial_capacity + 5, (initial_capacity + 1));

	for (i = 0; i < (initial_capacity + 1); i++)
		allocator_free(&mem, ptr_lst[i]);
	print_test_info(&mem, initial_capacity + 5, 0);

	allocator_clean_up(&mem);
	print_test_info(&mem, 0, 0);

	/*
	 * test 1: auto_expand_factor disabled
	 */
	printf("==========test 1==========\n");
	if (allocator_init(&mem, object_size, initial_capacity, 0) == -1)
		return 1;
	print_test_info(&mem, initial_capacity, 0);

	for (i = 0; i < (initial_capacity + 1); i++)
		ptr_lst[i] = allocator_alloc(&mem, object_size);
	print_test_info(&mem, initial_capacity, initial_capacity);

	for (i = 0; i < initial_capacity; i++)
		allocator_free(&mem, ptr_lst[i]);
	print_test_info(&mem, initial_capacity, 0);

	allocator_clean_up(&mem);
	print_test_info(&mem, 0, 0);

	/*
	 * test 2: alloc/free rotations
	 */
	printf("==========test 2==========\n");

	ptr_lst = realloc(ptr_lst, sizeof(void*) * TEST_ALLOC);

	if (allocator_init(&mem, object_size, initial_capacity, 10) == -1)
		return 1;
	print_test_info(&mem, initial_capacity, 0);

	printf("beginning the rotations ...\n");
	for (i = 0; i < TEST_ROTATIONS; i++) {
		for (j = 0; j < TEST_ALLOC; j++) {
			ptr_lst[j] = allocator_alloc(&mem, object_size);
		}

		for (j = 0; j < TEST_ALLOC; j++) {
			allocator_free(&mem, ptr_lst[j]);
		}
	}

	allocator_clean_up(&mem);
	print_test_info(&mem, 0, 0);

	/* free ptr_lst */
	free(ptr_lst);

	return 0;
}
