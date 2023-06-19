#ifndef __MEMORY_ARENA_H__
#define __MEMORY_ARENA_H__

#include "required/intrinsic.hpp"
#include "required/nix.hpp"
#include "required/memory.hpp"
#include "required/platform.hpp"
#include "required/assert.hpp"

#define PushBlock(arene, size, type) (type*) PushBlock_(arena, size)
#define PushStruct(arena, type) (type *) PushBlock_(arena, sizeof(type))
#define PushArray(arena, count, type) (type*)PushBlock_(arena, (count)*sizeof(type))

struct memory_storage
{
	uint64 size;
	void *storage;	
};

struct memory_arena
{
	size_t size;
	uint8* base;
	size_t used;
};

inline void PlatformAllocateMemory(memory_storage *appMemory)
{
	appMemory->storage = MemoryRaw(appMemory->size);
}

inline void CreateArena(memory_arena *arena, size_t size, void *base)
{
	arena->size = size;
	arena->base = (uint8 *) base;
	arena->used = 0;
}

inline void *Copy(size_t size, void *sourceInit, void* destInit)
{
	ui8 *source = (ui8*) sourceInit;
	ui8 *dest = (ui8*)destInit;

	while(size--) {*dest++ = *source++;}

	return destInit;
}

inline void *PushBlock_(memory_arena *arena, size_t size)
{
	void* result = NULL;

	Assert((arena->used + size) <= arena->size);

	result = arena->base + arena->used;
	arena->used += size;

	return result;
}

inline void CreateSubArena(memory_arena *result, memory_arena *arena, size_t size)
{
	result->size = size;
	result->base = (ui8*) PushBlock_(arena,size);
	result->used = 0;
}
#endif
