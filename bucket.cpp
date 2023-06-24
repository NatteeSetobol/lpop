#include "bucket.hpp"

Map::Map()
{

	total = BUCKET_LIMIT;
	maps = MemoryA(struct Map_Node, total);
	keys = new _Array();

	for (int i=0; i <total; i++)
	{
		struct Map_Node *mapNode = maps + i;
		mapNode->filled = false;
		mapNode->CollusionArray = NULL;
	}
	
}

void *Map::Get(s32 *key)
{
	bool found = false;
	struct Map_Node *mapNode = NULL;

	mapNode = GetNode(key);

	if (mapNode == NULL) return NULL;
	if (mapNode->key == NULL) return NULL;
	if (StrCmp(mapNode->key, key) == false)
	{
		if (mapNode->CollusionArray)
		{
			// Note(): if the keys don't match then look in the collusion Array
			for (int i=0;i < mapNode->CollusionArray->count; i++)
			{
				struct Map_Node *mnode = NULL;
				mnode = (Map_Node*) mapNode->CollusionArray->Get(i);
				if (mnode)
				{
					if (StrCmp(mnode->key, key))
					{
						found =true;
						mapNode = mnode;
						break;
					}
				}
			}
		}
	} else {
		found = true;
	}

	if (found == false) return NULL;
	return mapNode->value;
}

void Map::Add(s32 *key, void *value)
{
	struct Map_Node *mapNode = NULL;
	s32 *currentKey = NULL;

	currentKey = S32(key);

	mapNode = GetNode(key);

	keys->Add( (void*) currentKey);

	if (mapNode->filled)
	{
		AddCollusion(mapNode, key,value);
	} else {
		FillNode(mapNode, key,value);
	}

}

void Map::Remove(s32 *key)
{
	struct Map_Node *mapNode = NULL;

	mapNode = GetNode(key);

}

void Map::FillNode(struct Map_Node *mapNode, s32 *key, void *value)
{
	mapNode->filled = true;
	mapNode->key = S32(key);
	mapNode->value = value;
}


void Map::AddCollusion(struct Map_Node *mapNode, s32 *key, void *value)
{
	struct Map_Node *newMapNode = NULL;

	if (mapNode->CollusionArray == NULL)
	{
		mapNode->CollusionArray = new _Array();
	}

	newMapNode = (struct Map_Node*) MemoryRaw(sizeof(struct Map_Node));

	FillNode(newMapNode,key,value);
	mapNode->CollusionArray->Add((void*) newMapNode);
}

struct Map_Node* Map::GetNode(s32 *key)
{
	struct Map_Node *mapNode = NULL;

	i32 keyValue = 0;

	keyValue = HashFunction(key);

	if (keyValue == 0) return NULL;

	keyValue = keyValue / total;

	mapNode = maps + keyValue;

	return mapNode;
}

void Map::CleanUp()
{
	if (keys == NULL) return ;
	for (int i=0;i< keys->count;i++)
	{
		s32 *key = NULL;
		struct Map_Node *mapNode = NULL;

		key = (s32*) keys->Get(i);

		mapNode = (struct Map_Node*) GetNode(key);
		
		if (mapNode)
		{
			mapNode->filled = false;
			if (mapNode->key)
			{
				Free(mapNode->key);
				mapNode->key = NULL;
			}

			if (mapNode->CollusionArray)
			{

				for (int j=0;j < mapNode->CollusionArray->count;j++)
				{
					struct Map_Node *mNode = NULL;
					mNode = (struct Map_Node*) mapNode->CollusionArray->Get(j);

					if (mNode)
					{
						if (mNode->key)
						{
							Free(mNode->key);
							mNode->key = NULL;
						}

						Free(mNode);
						mNode=NULL;
					}
				}
				delete(mapNode->CollusionArray);
				mapNode->CollusionArray = NULL;
			}
			
		}

		if (key)
		{
			Free(key);
			key=NULL;
		}
		
	}

	Free(maps);
	maps=NULL;
	delete keys; 
}

Map::~Map()
{
	CleanUp();
}

i32 Map::HashFunction(s32 *key)
{
	i32 newKey = 0;

	for (int i=0; i < Strlen(key); i++)
	{
		newKey += key[i] + i;
	}

	return newKey;
}

HashBucket::HashBucket()
{

	total = BUCKET_LIMIT;
	buckets = MemoryA(struct bucket, total);
	for (int i=0; i < total; i++)
	{
		struct bucket* currentBucket = buckets + i;
		currentBucket->filled = false;
	}
	count = 0;
}

void HashBucket::DeleteAll()
{
	int i = 0;
	for (i = 0; i < count; i++)
	{
		char* index = (char*) keys.Get(i);
		struct bucket* buck = GetBucketFromKey(index);

		if (buck)
		{
			if (buck->deleted == false) { 
				//if (buck->key) {
				if (buck->key)
				{
					Free(buck->key);
					buck->key = NULL;
				}

				if (buck->value)
				{
					Free(buck->value);
					buck->value = NULL;
				}

				if (buck->next)
				{
					struct bucket* nextBucketBlock = buck->next;

					while (nextBucketBlock)
					{
						if (nextBucketBlock)
						{
							struct bucket* tempNextBucketBlock = nextBucketBlock;
							nextBucketBlock = nextBucketBlock->next;

							if (tempNextBucketBlock->key)
							{
								FreeMemory((i8*)tempNextBucketBlock->key);
								tempNextBucketBlock->key = NULL;
							}

							if (tempNextBucketBlock->value)
							{
								FreeMemory((i8*)tempNextBucketBlock->value);
								tempNextBucketBlock->value = NULL;
							}

							if (tempNextBucketBlock)
							{
								FreeMemory((i8*)tempNextBucketBlock);
								tempNextBucketBlock = NULL;
							}
						}
					}
				} else {

				}
				buck->deleted = true;
			} else {
				
				if (buck->key)
				{
					Free(buck->key);
					buck->key = NULL;
				}
				
			}
		}
		/*
		if (index)
		{
			Free(index);
			index = NULL;
		}
		*/
	}

	count = 0;
	if (buckets)
	{
		Free(buckets);
		buckets=NULL;
	}

}


HashBucket::~HashBucket()
{
	int i = 0;
	for (i = 0; i < count; i++)
	{
		char* index = (char*) keys.Get(i);
		struct bucket* buck = GetBucketFromKey(index);

		if (buck)
		{
			if (buck->deleted == false) { 
				//if (buck->key) {
				if (buck->key)
				{
					Free(buck->key);
					buck->key = NULL;
				}
				/*
				if (buck->value)
				{
					Free(buck->value);
					buck->value = NULL;
				}
				*/

				if (buck->next)
				{
					struct bucket* nextBucketBlock = buck->next;

					while (nextBucketBlock)
					{
						if (nextBucketBlock)
						{
							struct bucket* tempNextBucketBlock = nextBucketBlock;
							nextBucketBlock = nextBucketBlock->next;

							if (tempNextBucketBlock->key)
							{
								FreeMemory((i8*)tempNextBucketBlock->key);
								tempNextBucketBlock->key = NULL;
							}

							if (tempNextBucketBlock->value)
							{
								FreeMemory((i8*)tempNextBucketBlock->value);
								tempNextBucketBlock->value = NULL;
							}

							if (tempNextBucketBlock)
							{
								FreeMemory((i8*)tempNextBucketBlock);
								tempNextBucketBlock = NULL;
							}
						}
					}
				} else {

				}
				buck->deleted = true;
			} else {
				/*
				if (buck->key)
				{
					Free(buck->key);
					buck->key = NULL;
				}
				*/
			}
		}
		/*
		if (index)
		{
			Free(index);
			index = NULL;
		}
		*/
	}

	count = 0;
	if (buckets)
	{
		Free(buckets);
		buckets=NULL;
	}
}


i8* HashBucket::Get(char* key)
{
	i8* result = NULL;
	i32 index = 0;

	if (key == NULL)
	{
		return result;
	}

	hashFunction = DefaultHashFunction;
	index = (hashFunction)(key, NULL);

	index = index / total;
	while (index > total)
	{
		index = index / total;
	}

	//Assert(index > -1);
	struct bucket* tempBucket = (struct bucket*) (buckets + index);

	if (tempBucket)
	{
		if (count > 0)
		{
			if (tempBucket->deleted == false)
			{
				if (tempBucket->key && key != NULL)
				{
					if (StrCmp(key, tempBucket->key))
					{
						result = tempBucket->value;
					}
					else {
						struct bucket* currentBucket = tempBucket->next;

						while (currentBucket != NULL)
						{
							if (StrCmp(key, currentBucket->key))
							{
								result = currentBucket->value;
								break;
							}

							currentBucket = currentBucket->next;
						}
					}
				}
			}
		}
	}


	return result;
}

void HashBucket::Add(i8* key, i8* value)
{
	i32 index = 0;
	hashFunction = DefaultHashFunction;

	index = (hashFunction)( (s32*) key, NULL);

	index = index / total;
	while (index > total)
	{
		index = index / total;
	}

	s32* newKey  = S32( (s32*) key);
	keys.Add((int32*) newKey);

	struct bucket* newBucket = buckets + index;

	if (newBucket->filled == false)
	{
		newBucket->id = count;
		newBucket->deleted = false;
		newBucket->filled = true;
		newBucket->key = S32((s32*) key);
		newBucket->value = (i8*)value;
		newBucket->next = NULL;
	}
	else {

		struct bucket* nextNewBucket = Memory(struct bucket);
		nextNewBucket->id = count;
		nextNewBucket->filled = true;
		nextNewBucket->key = (s32*) S32((s32*) key);
		nextNewBucket->value = (i8*)value;

		if (newBucket->next)
		{
			nextNewBucket->next = newBucket->next;
			newBucket->next = nextNewBucket;
		}
		else {
			newBucket->next = nextNewBucket;
		}
	}
	count++;
}


struct bucket* HashBucket::GetBucketFromKey(char* key)
{
	struct bucket* result = NULL;
	i32 index = 0;

	if (key == NULL)
	{
		return result;
	}

	hashFunction = DefaultHashFunction;

	index = (hashFunction)(key, NULL);

	index = index / total;
	while (index > total)
	{
		index = index / total;
	}


	struct bucket* tempBucket = (struct bucket*) (buckets + index);

	if (tempBucket)
	{
		if (tempBucket->deleted == false)
		{
			if (tempBucket->key)
			{
				if (StrCmp(key, tempBucket->key))
				{
					result = tempBucket;
				}
			}
			else {
				struct bucket* currentBucket = tempBucket->next;

				while (currentBucket != NULL)
				{
					if (currentBucket->key)
					{
						if (StrCmp(key, currentBucket->key))
						{
							result = currentBucket;
							break;
						}
					}

					currentBucket = currentBucket->next;
				}
			}
		}
	}


	return result;
}




i32 CreateHashKey(s32 *key, s32* value, i32 arrayLength)
{
	i32 valueLength = Strlen(value);
	i32 keyLength = Strlen(key);
	i32 result = 0;
	bool isDone = false;

	// NOTE(): Not a very good hash function!!!!

	i32 valueCount = 0;

	//if (valueLength > keyLength)
	{
		//while (valueCount != valueLength - 1 && isDone == false)
		{
			for (i32 keyIndex = 0; keyIndex < keyLength; keyIndex++)
			{
				//if (valueCount <= valueLength-1)
			//	{
					result +=  key[keyIndex];
			//	} else {
			//		isDone = true;
			//		break;
			//	}
			//	++valueCount;
			}
		}
	}
	
	result = result / arrayLength;

	return result;
}


s32* GetValueFromKey(struct hash hashArray[],s32 *key,i32 arrayLength)
{
	s32 *result = NULL;
	i32 keyIndex = 0;
	i32 keyLength=0;
	struct hash arrayKey = {};

	keyLength = Strlen(key);

	for (i32 keyArrayIndex = 0; keyArrayIndex < keyLength; keyArrayIndex++)
	{
		keyIndex +=  key[keyArrayIndex];
	}

	keyIndex = keyIndex / arrayLength;

	arrayKey = hashArray[keyIndex];

	if (arrayKey.key)
	{
		if (StrCmp(arrayKey.key, key))
		{
			result = arrayKey.value;
		} else {
			struct hash *arrayNode = hashArray[keyIndex].next;
			while (arrayNode)
			{
				if (StrCmp(arrayNode->key, key))
				{
					result = arrayNode->value;
					break;
				}

				arrayNode = arrayNode->next;
			}
		}
	}
	
	return result;
}

i32 AddToHash(struct hash hashArray[], s32* key, s32* value, i32 arrayLength, memory_arena *arena)
{
	i32 keyIndex = 0;
	keyIndex = CreateHashKey(key, value, arrayLength);

	if (hashArray[keyIndex].isTaken)
	{
		// NOTES(): Manage Collusions.

		if (hashArray[keyIndex].next == NULL)
		{
			hashArray[keyIndex].next = PushBlock(arena, sizeof(struct hash), struct hash);
			hashArray[keyIndex].next->isTaken = true;
			hashArray[keyIndex].next->key = (s32*) PushString(arena,key, Strlen(key));
			hashArray[keyIndex].next->value = (s32*) PushString(arena,value, Strlen(value)); 
	
		} else {
			struct hash *oldHash = NULL;

			oldHash = hashArray[keyIndex].next;
			
			hashArray[keyIndex].next = PushBlock(arena, sizeof(struct hash), struct hash);
			hashArray[keyIndex].next->isTaken = true;
			hashArray[keyIndex].next->key = (s32*) PushString(arena,key, Strlen(key));
			hashArray[keyIndex].next->value = (s32*) PushString(arena,value, Strlen(value)); 
			hashArray[keyIndex].next->next = oldHash; 

		}

	} else {
		hashArray[keyIndex].key = (s32*) PushString(arena,key, Strlen(key)); 
		hashArray[keyIndex].value = (s32*) PushString(arena,value, Strlen(value)); 
		hashArray[keyIndex].isTaken = true;
	}

	return keyIndex;
}

i32 DefaultHashFunction(s32* key, i8* value)
{
	i32 keyLen = Strlen(key);
	i32 index = -1;

	for (int i = 0; i < keyLen; i++)
	{
		index += key[i];
	}

	return index;
}

void InitBucket(struct bucket_list* buckets, int numberOfBuckets, HASHFUNCTION hashFunction)
{
	//Assert(numberOfBuckets > 5);
	buckets->total = numberOfBuckets;
	buckets->count = 1;
	buckets->buckets = NULL;
	buckets->hashFunction = hashFunction;
	//buckets->keys = CreateArray(int);
}



void AddToBucket(struct bucket_list* buckets, s32* key, i8* value)
{
	i32 index = 0;

	//Assert(buckets->total > 5);

	if (buckets->hashFunction == NULL)
	{
		buckets->hashFunction = DefaultHashFunction;
	}

	index = (buckets->hashFunction)(key, NULL);

	index = index / buckets->total;

	while (index >= buckets->total)
	{
		index = index / buckets->total;
	}

	//buckets->keys[buckets->count] = index;
	buckets->keys[buckets->count] = S32(key);
	//AddToArray(  buckets->keys,  index);

	if (buckets->buckets == NULL)
	{
		buckets->buckets = MemoryA(struct bucket, buckets->total);

		for (int i=0; i < buckets->total; i++)
		{
			
			struct bucket* currentBucket = buckets->buckets + i;
			currentBucket->filled = false;
		}
	}

	struct bucket* newBucket = buckets->buckets + index;

	if (newBucket->filled == false)
	{
		newBucket->id = buckets->count;
		newBucket->deleted = false;
		newBucket->filled = true;
		newBucket->key = key;
		newBucket->value = (i8*)value;
		newBucket->next = NULL;
	}
	else {

		struct bucket* nextNewBucket = Memory(struct bucket);
		nextNewBucket->id = buckets->count;
		nextNewBucket->filled = true;
		nextNewBucket->key = key;
		nextNewBucket->value = (i8*)value;
		//nextNewBucket->next = NULL;

		if (newBucket->next)
		{
			nextNewBucket->next = newBucket->next;
			newBucket->next = nextNewBucket;
			/*
			newBucket->next->next =  Memory(bucket);
			newBucket->next->next->id = buckets->count;
			newBucket->next->next->filled = true;
			newBucket->next->next->key = S32(key);
			newBucket->next->next->value= (i8*)value;
			newBucket->next->next->deleted =  false;
			newBucket->next->next->next = NULL;

			newBucket->next= newBucket->next->next;
			*/
		}
		else {
			newBucket->next = nextNewBucket;
			/*
			newBucket->next=  Memory(bucket);
			newBucket->next->id = buckets->count;
			newBucket->next->filled = true;
			newBucket->next->key = S32(key);
			newBucket->next->deleted = false;
			newBucket->next->value= (i8*)value;
			newBucket->next->next = NULL;
		//	newBucket = nextNewBucket;
		*/
		}
	}
	buckets->count++;
}


struct bucket* GetBucketFromKey(struct bucket_list* buckets, char* key)
{
	struct bucket* result = NULL;
	i32 index = 0;

	if (key == NULL)
	{
		return result;
	}

	if (buckets->hashFunction == NULL)
	{
		buckets->hashFunction = DefaultHashFunction;
	}

	index = (buckets->hashFunction)(key, NULL);

	index = index / buckets->total;
	while (index > buckets->total)
	{
		index = index / buckets->total;
	}

	//Assert(index > -1);


	struct bucket* tempBucket = (struct bucket*) (buckets->buckets + index);

	if (tempBucket)
	{
		if (tempBucket->deleted == false)
		{
			if (tempBucket->key)
			{
				if (StrCmp(key, tempBucket->key))
				{
					result = tempBucket;
				}
			}
			else {
				struct bucket* currentBucket = tempBucket->next;

				while (currentBucket != NULL)
				{
					if (currentBucket->key)
					{
						if (StrCmp(key, currentBucket->key))
						{
							result = currentBucket;
							break;
						}
					}

					currentBucket = currentBucket->next;
				}
			}
		}
	}


	return result;
}




i8* GetFromBucket(struct bucket_list* buckets, char* key)
{
	i8* result = NULL;
	i32 index = 0;

	if (key == NULL)
	{
		return result;
	}

	if (buckets->hashFunction == NULL)
	{
		buckets->hashFunction = DefaultHashFunction;
	}
	index = (buckets->hashFunction)(key, NULL);

	index = index / buckets->total;
	while (index > buckets->total)
	{
		index = index / buckets->total;
	}

	//Assert(index > -1);
	struct bucket* tempBucket = (struct bucket*) (buckets->buckets + index);

	if (tempBucket)
	{
		if (buckets->count > 0)
		{
			if (buckets->deleted == false)
			{
				if (tempBucket->key && key != NULL)
				{
					if (StrCmp(key, tempBucket->key))
					{
						result = tempBucket->value;
					}
					else {
						struct bucket* currentBucket = tempBucket->next;

						while (currentBucket != NULL)
						{
							if (StrCmp(key, currentBucket->key))
							{
								result = currentBucket->value;
								break;
							}

							currentBucket = currentBucket->next;
						}
					}
				}
			}
		}
	}


	return result;
}

void BucketPrintAllKeys(i8* chunk)
{
#if 0
	struct array_info* chunkInfo = (struct array_info*) chunk;
	int* allKeys = (int*)(chunk + sizeof(struct array_info));

	for (int i = 0; i < chunkInfo->count; i++)
	{
		printf("%i\n", allKeys[i]);
	}
#endif
}




void FreeAllBuckets(struct bucket_list* buckets)
{
	int i = 0;
	for (i = 0; i < buckets->count; i++)
	{
		char* index = buckets->keys[i];
		struct bucket* buck = GetBucketFromKey(buckets, index);

		if (buck)
		{
			if (buck->deleted == false)
			{
				if (buck->key)
				{
					Free(buck->key);
					buck->key = NULL;
				}

				if (buck->value)
				{
					Free(buck->value);
					buck->value = NULL;
				}

				if (buck->next)
				{
					struct bucket* nextBucketBlock = buck->next;

					while (nextBucketBlock)
					{
						if (nextBucketBlock)
						{
							struct bucket* tempNextBucketBlock = nextBucketBlock;
							nextBucketBlock = nextBucketBlock->next;

							if (tempNextBucketBlock->key)
							{
								FreeMemory((i8*)tempNextBucketBlock->key);
								tempNextBucketBlock->key = NULL;
							}

							if (tempNextBucketBlock->value)
							{
								FreeMemory((i8*)tempNextBucketBlock->value);
								tempNextBucketBlock->value = NULL;
							}

							if (tempNextBucketBlock)
							{
								FreeMemory((i8*)tempNextBucketBlock);
								tempNextBucketBlock = NULL;
							}
						}
					}
				} else {

				}
				buck->deleted = true;
			} else {
				
				if (buck->key)
				{
					Free(buck->key);
					buck->key = NULL;
				}
				
			}
		}

		if (index)
		{
			Free(index);
			index = NULL;
		}
	}

	if (buckets->buckets)
	{
		FreeMemory((i8*)buckets->buckets);
		buckets->buckets = NULL;
	}
	buckets->count = 0;
	
	if (buckets->keys)
	{
		Free(buckets->keys);
	}
	
}
