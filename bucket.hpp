#ifndef __BUCKET_H__
#define __BUCKET_H__

#include "required/intrinsic.hpp"
#include "required/nix.hpp"
#include "required/memory.hpp"
#include "required/platform.hpp"
#include "./strings.hpp"

#define BUCKET_LIMIT 256


//#include <typeinfo>

/*NOTE(): Bucket Example:

	bucket_list buckets = {};
	InitBucket(&buckets);

	s32 *one = S32("one\n");
	s32 *two = S32("two\n");
	s32 *three = S32("three\n");
	s32 *four = S32("three3\n");
	s32 *five= S32("three4\n");
	s32 *six= S32("three5\n");

	AddToBucket(&buckets,"one", (i8*)  one);
	AddToBucket(&buckets,"two", (i8*) two); 
	AddToBucket(&buckets,"three", (i8*) three); 
	AddToBucket(&buckets,"four", (i8*) four); 
	AddToBucket(&buckets,"five", (i8*) five); 
	AddToBucket(&buckets,"six", (i8*) six); 
	FreeAllBuckets(&buckets);
*/


typedef i32 (*HASHFUNCTION)(char* key, i8* ptrToValue);
#define AddS32ToBucket(chunk,key,value) do {  char *tempKey = S32(key);char *tempChar = S32((char*)value); AddToBucket(chunk,tempKey, (i8*) tempChar);   }while(0)

struct hash
{
	bool isTaken;
	s32* key;
	s32* value;
	hash *next;
};


struct bucket
{
	i32 id;
	bool32 deleted;
	s32 *key;
	bool filled;
	i8 *value;
	struct bucket *next;
};


struct bucket_list
{
	/*NOTE: leave this for now until the array is fixed*/
	//int keys[256];
	char* keys[256];
	i32 total;
	i32 count;
	struct bucket *buckets;
	HASHFUNCTION hashFunction;
	bool32 deleted;
};

struct Map_Node
{
	i32 id;
	bool deleted;
	s32*key;
	void *value;
	bool filled;
	_Array *CollusionArray;
	//Map_Node *next;
};

class Map
{
	public:
		_Array *keys;
		Map();
		~Map();
		void Add(s32 *key, void* value);
		void *Get(s32 *key);
		i32 HashFunction(s32 *key);
		Map_Node *GetNode(s32 *key);
		void FillNode(struct Map_Node *mapNode, s32 *key, void *value);
		void AddCollusion(struct Map_Node *mapNode, s32 *key, void *value);
		void Remove(s32 *key);
		void CleanUp();
	private:
		int total;
		Map_Node* maps;
};

i32 DefaultHashFunction(s32* key, i8* value);
void InitBucket(struct bucket_list* buckets, int numberOfBuckets, HASHFUNCTION hashFunction);
void AddToBucket(struct bucket_list* buckets, s32* key, i8* value);
struct bucket* GetBucketFromKey(struct bucket_list* buckets, char* key);
i8* GetFromBucket(struct bucket_list* buckets, char* key);
void BucketPrintAllKeys(i8* chunk);
void FreeAllBuckets(struct bucket_list* buckets);
i32 CreateHashKey(s32 *key, s32* value, i32 arrayLength);
s32* GetValueFromKey(struct hash hashArray[],s32 *key,i32 arrayLength);
i32 AddToHash(struct hash hashArray[], s32* key, s32* value, i32 arrayLength, memory_arena *arena);

class HashBucket
{
	i32 total;
	struct bucket *buckets=NULL;
	HASHFUNCTION hashFunction=NULL;
	public:
		i32 count;
		Array keys;
		HashBucket();
		~HashBucket();
		i8* Get(char* key);
		void Add(i8* key, i8* value);
		struct bucket* GetBucketFromKey(char* key);
		void DeleteAll();
};


#endif
