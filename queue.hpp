#ifndef __QUEUE_H__
#define __QUEUE_H__

#define ForEachQuery(que,item) for (struct queue_info* item=que->head;item;item=item->next)
#define QueuePushS32(queue, stringItem) { do { char *newStringItem = S32(stringItem);QueueAddToBottom(queue,(i32*) newStringItem);} while(0); }

#include "required/intrinsic.hpp"
#include "required/nix.hpp"
#include "required/memory.hpp"
#include "required/platform.hpp"
#include "required/assert.hpp"
//#include "win_threaded_queue.h"
#include "bucket.hpp"
#include "array.hpp"
#include "strings.hpp"


struct queue_info
{
	i32 id;
	i32 *address;
	struct queue_info *next;
	struct queue_info *prev;
};

typedef struct queue
{
	int count;
	struct queue_info *head;
	struct queue_info *current;
} Queue;

void QueuePush(Queue *queue, i32 *item);
struct queue_info *QueuePop(Queue *queue);
void QueueAddToBottom(Queue *queue, i32 *item);
void QueueRemoveById(Queue *queue, i32 index);
void QueueRemoveAll(Queue *queue);

class Stack
{
	public:
		int count;
		struct queue_info *head;
		struct queue_info *current;

		void Push(i32 *item);
		struct queue_info *Pop();
		void Add( i32 *item);
		void Clear();
		void RemoveById(i32 index);
};
#endif
