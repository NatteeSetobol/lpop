#ifndef __time_block_h__
#define __time_block_h__
#define MAX_RECORD_TABLE 256

#include <string.h>
#ifndef COMPILER_MSVC
#include <sys/time.h>
#endif
#include <stdio.h>

#ifndef COMPILER_MSVC
#include <x86intrin.h>
#endif
#include "./required/intrinsic.hpp"

// NOTES(): This will not work on ARM due to the __rdtsc(); function

#ifdef COMPILER_MSVC
struct timed_block
{
	int Counter;
	timed_block(int counterInit, char* filename, int lineNumber, char* blockName);
	~timed_block();
};

struct record_info
{
	ui32 id;
	ui64 beginTick;
	ui64 endTick;
	ui64 elaspTick;
	ui32 index;
	char *name;
	struct timeval beginClock;
	struct timeval endClock;
	double elaspTime;
};

struct global_record_info_table
{
	ui32 totalRecords;
	record_info records[MAX_RECORD_TABLE];
};


void ShowRecordTable();
#define TimedBlock() \
	timed_block timeBlock(0,(char*) __FILE__,__LINE__, (char*) __FUNCTION__);


#ifdef COMPILER_MSVC
#define GetTime(beginClock)
#else
#define GetTime(beginClock) gettimeofday(beginClock,0)
#endif

#define RecordInfo(eventName) \
		ui64 recordIndex =   ++recordTable.totalRecords;\
		GetTime(&recordTable.records[recordIndex].beginClock);\
		recordTable.records[recordIndex].beginTick= __rdtsc();\
		recordTable.records[recordIndex].index = recordIndex;\
		recordTable.records[recordIndex].name = (char*) malloc(strlen(eventName)+1);\
		strcpy(recordTable.records[recordIndex].name, eventName);\


extern struct global_record_info_table recordTable = {};

timed_block::timed_block(int counterInit, char* filename, int lineNumber, char* blockName)
	{
	//	if (recordTable)
	//	{
	//		recordTable =(struct global_record_info_table*) malloc(sizeof(struct global_record_info_table));
	//	}
		RecordInfo(blockName);
		Counter = recordTable.totalRecords;

	}

timed_block::~timed_block()
	{
		long seconds = 0;
		long mseconds = 0;

		recordTable.records[Counter].endTick = __rdtsc();
		GetTime(&recordTable.records[Counter].endClock);

		seconds =  recordTable.records[Counter].endClock.tv_sec - recordTable.records[Counter].beginClock.tv_sec;
		mseconds =  recordTable.records[Counter].endClock.tv_usec - recordTable.records[Counter].beginClock.tv_usec;

		recordTable.records[Counter].elaspTime = seconds + mseconds*1e-6;

		recordTable.records[Counter].elaspTick = recordTable.records[Counter].endTick - recordTable.records[Counter].beginTick;
		//RecordInfo((char*)"testing end", __COUNTER__);
	}

#endif


#endif

