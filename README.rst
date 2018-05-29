===========================================
Fixed size memory allocator using free list
===========================================

:Date: 2018-05-29

Based on Arpan Sen and Rahul Kumar Kardam tutorial
"Building your own memory manager for C/C++ projects"
<https://www.ibm.com/developerworks/aix/tutorials/au-memorymanager>.


How it works?
=============

TL;DR: We keep a single linked list of free chunks.

A chunk is an allocation done with malloc(). The size
of this allocation is always the same for a given
memory structure. Each chunk while free is used as a
pointer to the next free chunk (see ``struct free_chunk``
in allocator.h). Also, notice that a free chunk is free
to our allocator, but it's still an unfreed memory to
the glibc malloc() implementation (and perhaps to the
kernel).

A memory structure contains information about multiple
allocations that belong to one list of free chunks.
See ``struct memory`` in allocator.h

Demonstration of the allocator behavior:

- Initial state, all chunks are free::

	A -> B -> [NULL]

- When a chunk is allocated, it's always from head (the
  1st) of the list::

	B -> [NULL]

- If list becomes empty and another chunk is requested,
  our behavior will depend of ``auto_expand_amount`` in
  ``struct memory``. If ``auto_expand_amount`` is set to
  zero we return NULL, otherwise we increase list
  capacity by ``auto_expand_amount`` and return a valid
  chunk.

- When a chunk is freed it becomes the head of the list.
