#ifndef BASE_MEMORY_H
#define BASE_MEMORY_H

// NOTE(simon): @os_implementation
internal Void *memory_reserve(U64 size);
internal Void  memory_commit(Void *pointer, U64 size);
internal Void  memory_decommit(Void *pointer, U64 size);
internal Void  memory_release(Void *pointer, U64 size);

#endif // BASE_MEMORY_H
