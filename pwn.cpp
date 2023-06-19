#include "pwn.hpp"

char *Dup(char ch, int times)
{
	char *result = (char*) MemoryRaw(sizeof(char) * times + 1);

	for (int i=0;i<times;i++)
	{
		result[i] = ch;
	}

	return result;
}

i64 *p64(i64 addr)
{
	i64 *result = NULL;
	result = (i64*) MemoryRaw(sizeof(i64));
	*result =  addr;

	return result;
}

void HexPrint(char* data, i32 dataSize)
{
	for (int i=0; i < dataSize; i++)
	{
		if (i % 16 == 0 && i!=0)
		{
			DebugLog("\n");
		}

		DebugLog("0x%02x ", data[i] & 0xFF);

	}
	DebugLog("\n");
}
