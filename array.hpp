#ifndef __MARRAY_H__
#define __MARRAY_H__


#include <stdio.h>
#include "../lpop/required/nix.hpp"
#include "./required/memory.hpp"
#include "./required/intrinsic.hpp"
#include "required/platform.hpp"
typedef struct marray
{
	i32 realCount;
	i32 count;
	i32 size;
	i32** ptr;
}marray;

typedef struct marray_link
{
	i32 id;
	struct marray_link *next;
} marray_link;

typedef struct marray_list
{
	struct marray_link *head;
	struct marray_link *tail;
}marray_list;



void AddToMArray_(struct marray* mArray, i32* addr, char* filename, int line);
i32* GetFromMArray(struct marray* Array, i32 index);
void RemoveFromArray(struct marray* Array, i32 index);
void FreeMArray(struct marray* Array);
void CreateMArray_(struct marray* mArray, char* filename, int line);
void AddToMFreeList(int index);

#define AddToMArray(ArrayStruct, Data) AddToMArray_(ArrayStruct, (i32*) Data,(char*) __FILE__, __LINE__)

#define for_marray(MArrayStruct,var) struct marray *MArrayStruct##ary =  MArrayStruct; for(int var=0;var <MArrayStruct##ary->count;var++)
#define CreateMArray(array) CreateMArray_(array, (char*) __FILE__,__LINE__)

struct link {
	i64 id;
	void *ptr;
	i64 size;
	struct link *next;
	struct link *pre;
};

class _Array
{
	public:
		_Array();
		~_Array();
		void Add(void *address);
		void *Get(i64 id);
		void Remove(i64 id);
		void ArrayTest();
		struct link* GetList(i64 id);
		int count;
	private:
		struct link *head;
		struct link *tail;
};

class Array
{
	public:
		void Add(i32* addr);
		i32* Get(i32 index);
		void Remove(i32 index);

		void FreeArray();
		Array();
		~Array();
		i32 length;
	private:
		marray arrayData = {};
		marray_list freeList = {};
};

#endif
