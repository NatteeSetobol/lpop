#include "time_block.hpp"


#ifdef COMPILER_MSVC




void ShowRecordTable()
{
	for (int recordTableIndex = 0; recordTableIndex < MAX_RECORD_TABLE;recordTableIndex++)
	{
		record_info currentTable = recordTable.records[recordTableIndex];
		if (currentTable.name)
		{
			printf("%s:%f (%u ticks)\n", recordTable.records[recordTableIndex].name,
							   recordTable.records[recordTableIndex].elaspTick,
							   recordTable.records[recordTableIndex].elaspTime
				);
		}
	}


}
#endif
