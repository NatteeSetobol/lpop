#include "array.hpp"

_Array::_Array()
{
	head = NULL;
	tail = NULL;
	count = 0;
}

_Array::~_Array()
{
	struct link *list = NULL;
	struct link *dlist = NULL;

	list = head;
	dlist = tail;

	while(head)
	{
		struct link *templink=NULL;
		
		templink = head;

		head = head->next;

		//Free(templink->ptr);
		Free(templink);
	}
}

void _Array::ArrayTest()
{
	struct link *list = NULL;
	i64 listCount = 0;

	list = head;

	while(list)
	{
		char* text = NULL;

		text = (char*) Get(listCount);

		printf("%s\n", text);
		listCount++;
		list = list->next;
	}
}

void _Array::Remove(i64 id)
{
	struct link *list = NULL;
	i64 currentCount = 0;
	list = head;

	while(list)
	{
		if (currentCount == id)
		{
			//Free(list->ptr);

			if (list == head)
			{
				struct link *tempLink = NULL;

				tempLink = head;
				head = head->next;

				Free(tempLink);
				count--;
				break;
			} else 
			if (list == tail)
			{
				struct link *tempTail = NULL;

				tempTail = tail;

				tail = tail->pre;
				tail->next = NULL;

				Free(tempTail);

				count--;
				break;
			} else {
				struct link *tempLink = NULL;

				tempLink = list;
				
				if (list->pre)
				{
					if (list->next)
					{
						list->next->pre =list->pre;
					}
					list->pre->next = list->next;
				}

				Free(tempLink);

				count--;
				break;

			}
		}
		currentCount++;
		list = list->next;
	}
}

void _Array::Add(void *address)
{
	if (head == NULL)
	{
		head = (struct link*) MemoryRaw(sizeof(struct link));
	//	head->ptr = MemoryRaw(size);
	//	memcpy(head->ptr, address,size);
		//head->size = size;
		head->ptr = address;
		head->next = NULL;
		head->id = count;
		head->pre = NULL;
	} else {
		if (tail)
		{
			tail->next = (struct link*) MemoryRaw(sizeof(struct link));

		//	tail->next->ptr = MemoryRaw(size);
			//memcpy(tail->next->ptr, address,size);
	//		tail->next->size = size;
			tail->next->ptr = address;
			tail->next->id = count;
			tail->next->pre = tail;
			tail->next->next = NULL;
			tail = tail->next;
		} else {
			tail= (struct link*) MemoryRaw(sizeof(struct link));

			//tail->ptr = MemoryRaw(size);
		//	memcpy(tail->ptr, address,size);
			//tail->size = size;
			tail->ptr = address;
			tail->id = count;
			tail->pre = head;
			tail->next = NULL;
			head->next = tail;
		}
	}
	count++;
}

void * _Array::Get(i64 id)
{
	struct link *list = NULL;
	int loopCount = 0;

	list = head;

	while(list)
	{
		if (loopCount == id)
		{
			return list->ptr;
		}
		loopCount++;
		list = list->next;
	}

	return NULL;
}


struct marray_list marrayFreeList = {};


void AddToMFreeList(int index)
{
	if (marrayFreeList.head == NULL)
	{
		marrayFreeList.head = Memory(struct marray_link);
		marrayFreeList.head->id = index;
		marrayFreeList.head->next = NULL;
		marrayFreeList.tail = marrayFreeList.head;
	}
	else {
		marrayFreeList.tail->next = Memory(struct marray_link);
		marrayFreeList.tail->next->id = index;
		marrayFreeList.tail->next->next = NULL;
		marrayFreeList.tail = marrayFreeList.tail->next;
	}
}

void CreateMArray_(struct marray* mArray, char* filename, int line)
{
	mArray->realCount = 0;
	mArray->count = 0;
	mArray->size = 2;
	mArray->ptr = MemoryAL(i32*, mArray->size, line, filename);
}

void AddToMArray_(struct marray* mArray, i32* addr, char* filename, int line)
{
	if (mArray->ptr == NULL)
	{
		CreateMArray(mArray);
	}
	else {
		if (marrayFreeList.head)
		{

			int newIndex = marrayFreeList.head->id;
			struct marray_link* temp = marrayFreeList.head;
			marrayFreeList.head = marrayFreeList.head->next;

			if (temp)
			{
				FreeMemory((i8*)temp);
				temp = NULL;
			}

			mArray->ptr[newIndex] = (i32*)addr;
			return;
		}
	}

	if (mArray->realCount < mArray->size)
	{
		mArray->ptr[mArray->realCount] = (i32*)addr;
		//newArray= addr;
		mArray->count++;
		mArray->realCount++;
	}
	else {
		i32 oldSize = 0;
		i32** tempChunk = mArray->ptr;
		oldSize = mArray->size;
		mArray->size *= 2;
		mArray->ptr = MemoryA(i32*, mArray->size);

		for (int i = 0; i < oldSize; i++)
		{
			mArray->ptr[i] = tempChunk[i];
		}

		mArray->ptr[mArray->realCount] = addr;

		mArray->count++;
		mArray->realCount++;
		if (tempChunk)
		{
			FreeMemory((i8*)tempChunk);
			tempChunk = NULL;
		}
	}
}

i32* GetFromMArray(marray* Array, i32 index)
{
	return (i32*)Array->ptr[index];
}

void RemoveFromArray(struct marray* Array, i32 index)
{
	void* ptr = Array->ptr[index];
	if (ptr)
	{
		Free((i8*)ptr);
		ptr = NULL;

	}
	//	AddToMFreeList(index);
	Array->count--;
}

void FreeMArray(struct marray* Array)
{
	struct marray_link* arrayListHead = NULL;

	//if (Array->realCount == 0) return;

	for (int i = 0; i < Array->realCount; i++)
	{
		RemoveFromArray(Array, i);
	}

	if (Array->ptr)
	{
		FreeMemory((i8*)Array->ptr);
		Array->ptr = NULL;
	}


	arrayListHead = marrayFreeList.head;
	while (arrayListHead)
	{
		struct marray_link* temp = arrayListHead;
		arrayListHead = arrayListHead->next;

		if (temp)
		{
			FreeMemory((i8*)temp);
			temp = NULL;
		}
	}
}


Array::Array()
{
	length=0;
	arrayData.realCount = 0;
	arrayData.count = 0;
	arrayData.size = 2;
	arrayData.ptr = MemoryAL(i32*, arrayData.size, __LINE__, __FILE__);
}

Array::~Array()
{

	struct marray_link* arrayListHead = NULL;
	marray *Array = &arrayData;

	//if (Array->realCount == 0) return;

	for (int i = 0; i < Array->realCount; i++)
	{
		Free(arrayData.ptr[i]);
		arrayData.ptr[i] = NULL;
	}



	arrayListHead = freeList.head;
	while (arrayListHead)
	{
		struct marray_link* temp = arrayListHead;
		arrayListHead = arrayListHead->next;

		if (temp)
		{
			FreeMemory((i8*)temp);
			temp = NULL;
		}
	}
	Free(arrayData.ptr);
	arrayData.ptr = NULL;
}

void Array::Add( i32* addr)
{
	marray *mArray = &arrayData;
	//NOTE(): A shitty way to detect use-after-free?
	for (int i=0;i<mArray->count;i++)
	{
		if (mArray->ptr[i] == addr)
		{
			return ;
		}
	}

	if (mArray->realCount < mArray->size)
	{
		mArray->ptr[mArray->realCount] = (i32*)addr;
		//newArray= addr;
		mArray->count++;
		mArray->realCount++;
		length++;
	}
	else {
		i32 oldSize = 0;
		i32** tempChunk = mArray->ptr;
		oldSize = mArray->size;
		mArray->size *= 2;
		mArray->ptr = MemoryA(i32*, mArray->size);

		for (int i = 0; i < oldSize; i++)
		{
			mArray->ptr[i] = tempChunk[i];
		}

		mArray->ptr[mArray->realCount] = addr;

		mArray->count++;
		mArray->realCount++;
		length++;
		if (tempChunk)
		{
			FreeMemory((i8*)tempChunk);
			tempChunk = NULL;
		}
	}
}

i32* Array::Get( i32 index)
{
	marray *Array = &arrayData;
	return (i32*)Array->ptr[index];
}

void Array::FreeArray()
{
}

void Array::Remove( i32 index)
{
	marray *Array = &arrayData;
	if (length > 0)
	{
		i32 **oldPtr = arrayData.ptr;
		arrayData.ptr  = MemoryA(i32*, arrayData.size);

		for (int i=0,j=0;i < length;i++)
		{
			if (i != index)
			{
				arrayData.ptr[j] = oldPtr[i];
				j++;
			} else {
				Free(oldPtr[i]);
				oldPtr[i]=NULL;
			}
		}

		Free(oldPtr);
		oldPtr = NULL;
		length--;
		arrayData.count = length;
		arrayData.realCount = length;
	} 
	/*
	if (ptr)
	{
		Free(Array->ptr[index]);
		Array->ptr[index] = NULL;
	}
	Array->count--;
	*/
}
