
#ifdef USE_SQL
#include "sqlite3_helper.hpp"

bool32 SL3_OpenDatabase(s32 *filename, sqlite3 *database)
{
	bool32 result = false;

	if (sqlite3_open(filename, &database) == SQLITE_OK)
	{
		result = true;
	}

	return result;
}

bool32 SL3_SaveDatabase(sqlite3_stmt *selectStmt)
{
	bool32 result = false;

	if (sqlite3_finalize(selectStmt))
	{
		result = true;
	}

	return result;
}

bool32 SL3_CloseDatabase(sqlite3* database)
{
	sqlite3_close(database);
}


void BeginSQLQueue(struct sql_queue *sQueue,char *databaseFile,s32* tableName,s32 *items, s32* query)
{
	s32 *sqlStatement = NULL;
	s32 *tempSqlStatement=NULL;

	sqlStatement = S32("select ");

	tempSqlStatement = sqlStatement;

	sqlStatement = S32Cat(tempSqlStatement,1,items);

	if (tempSqlStatement)
	{
		Free(tempSqlStatement);
		tempSqlStatement = NULL;
	}

	tempSqlStatement = sqlStatement;

	sqlStatement = S32Cat(tempSqlStatement,1," from ");

	if (tempSqlStatement)
	{
		Free(tempSqlStatement);
		tempSqlStatement = NULL;
	}

	tempSqlStatement = sqlStatement;

	sqlStatement = S32Cat(tempSqlStatement,1,tableName);

	if (tempSqlStatement)
	{
		Free(tempSqlStatement);
		tempSqlStatement = NULL;
	}

	tempSqlStatement = sqlStatement;

	sqlStatement = S32Cat(tempSqlStatement,3," ",query,";");

	if (tempSqlStatement)
	{
		Free(tempSqlStatement);
		tempSqlStatement = NULL;
	}

	if (sqlite3_open(databaseFile, &sQueue->testDB) == SQLITE_OK)
	{
		if (sqlite3_prepare_v2(sQueue->testDB,sqlStatement , -1, &sQueue->selectStmt, NULL) == SQLITE_OK)
		{
		}
	}
	
	if (sqlStatement)
	{
		Free(sqlStatement);
		sqlStatement=NULL;
	}
}


bool GetSQLNextRow(struct sql_queue *sQueue,struct marray *results)
{
	bool result = false;
	if (sqlite3_step(sQueue->selectStmt) == SQLITE_ROW)
	{
		result = true;

		for (int i=0;i<sqlite3_column_count(sQueue->selectStmt);i++)
		{
			int colType = -1;
			colType = sqlite3_column_type(sQueue->selectStmt, i);

			sql_item_info *itemInfo = Memory(sql_item_info);

			switch(colType)
			{
				case SQLITE_INTEGER:
				{
					int intData = sqlite3_column_int(sQueue->selectStmt,i);
					int *intDataPoint = Memory(int);
					*intDataPoint = intData;

					itemInfo->pointerTo = intDataPoint;
					itemInfo->type = SQL3_INTEGER;

					AddToMArray(results, itemInfo);

					break;
				}

				case SQLITE_FLOAT:
				{
					//TODO(): do double here
					double realData = sqlite3_column_double(sQueue->selectStmt,i);

					double *newRealData = (double*) Memory(double);
					*newRealData = realData;
					itemInfo->pointerTo = (void*) newRealData;;

					itemInfo->type = SQL3_REAL;
					AddToMArray(results, itemInfo);
					break;
				}
				case SQLITE_TEXT:
				{
					char *charData = (char*) sqlite3_column_text(sQueue->selectStmt,i);
					s32 *charDataPointer = S32(charData);

					if (charDataPointer)
					{
						itemInfo->pointerTo = (void*) charDataPointer;
						itemInfo->type = SQL3_TEXT;

						AddToMArray(results, itemInfo);
					}
					break;
				}
				default:
				{
					Free(itemInfo);
					itemInfo=NULL;
					break;
				}
			}
		}
	}

	return result;
}

void EndSQLQueue(struct sql_queue *sQueue)
{
	SL3_CloseDatabase(sQueue->testDB);
}

bool32 SL3_GetFromDatabase(char *databaseFile,s32* tableName,s32 *items, s32* query, struct marray *result)
{
	s32 *sqlStatement = NULL;
	s32 *tempSqlStatement=NULL;
    sqlite3_stmt *selectStmt=NULL;

	sqlStatement = S32("select ");

	tempSqlStatement = sqlStatement;

	sqlStatement = S32Cat(tempSqlStatement,1,items);

	if (tempSqlStatement)
	{
		Free(tempSqlStatement);
		tempSqlStatement = NULL;
	}

	tempSqlStatement = sqlStatement;

	sqlStatement = S32Cat(tempSqlStatement,1," from ");

	if (tempSqlStatement)
	{
		Free(tempSqlStatement);
		tempSqlStatement = NULL;
	}

	tempSqlStatement = sqlStatement;

	sqlStatement = S32Cat(tempSqlStatement,1,tableName);

	if (tempSqlStatement)
	{
		Free(tempSqlStatement);
		tempSqlStatement = NULL;
	}

	tempSqlStatement = sqlStatement;

	sqlStatement = S32Cat(tempSqlStatement,3," ",query,";");

	if (tempSqlStatement)
	{
		Free(tempSqlStatement);
		tempSqlStatement = NULL;
	}


	sqlite3 *testDB;

	if (sqlite3_open(databaseFile, &testDB) == SQLITE_OK)
	{
		if (sqlite3_prepare_v2(testDB,sqlStatement , -1, &selectStmt, NULL) == SQLITE_OK)
		{
			int colType = -1;


			while (sqlite3_step(selectStmt) == SQLITE_ROW)
			{
				struct marray  *itemRow = Memory(marray);

				for (int i=0;i<sqlite3_column_count(selectStmt);i++)
				{
					colType = sqlite3_column_type(selectStmt, i);

					sql_item_info *itemInfo = Memory(sql_item_info);


					switch(colType)
					{
						case SQLITE_INTEGER:
						{
							int intData = sqlite3_column_int(selectStmt,i);
							int *intDataPoint = Memory(int);
							*intDataPoint = intData;

							itemInfo->pointerTo = intDataPoint;
							itemInfo->type = SQL3_INTEGER;

							AddToMArray(itemRow, itemInfo);

							break;
						}
					
						case SQLITE_FLOAT:
						{
							//TODO(): do double here
							double realData = sqlite3_column_double(selectStmt,i);

							double *newRealData = (double*) Memory(double);
							*newRealData = realData;
							itemInfo->pointerTo = (void*) newRealData;;

							itemInfo->type = SQL3_REAL;
							AddToMArray(itemRow, itemInfo);
							break;
						}
						case SQLITE_TEXT:
						{
							char *charData = (char*) sqlite3_column_text(selectStmt,i);
							s32 *charDataPointer = S32(charData);

							if (charDataPointer)
							{
								itemInfo->pointerTo = (void*) charDataPointer;
								itemInfo->type = SQL3_TEXT;

								AddToMArray(itemRow, itemInfo);
							}
							break;
						}
						default:
						{
							Free(itemInfo);
							itemInfo=NULL;
							break;
						}
					}

				}

				AddToMArray(result,itemRow);
			}

		}
	}


	if (sqlStatement)
	{
		Free(sqlStatement);
		sqlStatement=NULL;
	}


	SL3_CloseDatabase(testDB);
}

int SL3_InsertIntoDatabase(char* databaseFile,s32* tableName ,i32 itemCount, ...)
{
	i32 i = 0;
	va_list vl;
	va_start(vl,itemCount);
	int result = -1;
	s32 *sqlQuery = NULL;
	s32 *sqlQueryTemp=NULL;
    sqlite3_stmt *selectStmt=NULL;

	sqlQuery = S32("INSERT INTO ");
	sqlQueryTemp = sqlQuery;

	sqlQuery = S32Cat(sqlQueryTemp,1,tableName);

	if (sqlQueryTemp)
	{
		Free(sqlQueryTemp);
		sqlQueryTemp=NULL;
	}

	sqlQueryTemp = sqlQuery;
	sqlQuery = S32Cat(sqlQueryTemp,1, " VALUES(");

	if (sqlQueryTemp)
	{
		Free(sqlQueryTemp);
		sqlQueryTemp=NULL;
	}


	for (i=0;i<itemCount;i+=2)
	{
		i32 varType = va_arg(vl,i32);
		
		switch(varType)
		{
			case SQL3_NULL:
			{
				sqlQueryTemp = sqlQuery;

				if (sqlQuery[Strlen(sqlQuery) - 1] == '(')
				{
					sqlQuery = S32Cat(sqlQueryTemp,1, "NULL");
				} else {
					sqlQuery = S32Cat(sqlQueryTemp,2, ",","NULL");
				}

				if (sqlQueryTemp)
				{
					Free(sqlQueryTemp);
					sqlQueryTemp=NULL;
				}

				break;
			}
			case SQL3_INTEGER:
			{

				i32 intType = va_arg(vl,i32);
				s32 *intToStr = IToS(intType);
				sqlQueryTemp = sqlQuery;

				if (sqlQuery[Strlen(sqlQuery) - 1] == '(')
				{
					sqlQuery = S32Cat(sqlQueryTemp,1, intToStr);
				} else {
					sqlQuery = S32Cat(sqlQueryTemp,2, ",",intToStr);
				}

				if (sqlQueryTemp)
				{
					Free(sqlQueryTemp);
					sqlQueryTemp=NULL;
				}

				if (intToStr)
				{
					Free(intToStr);
					intToStr=NULL;
				}
				//printf("int type %i\n",intType);
				break;
			}
			case SQL3_REAL:
			{
				double real32Type = va_arg(vl,double);
				sqlQueryTemp = sqlQuery;
				char veryLongString[1028] = {};

				//NOTES(): I don't know how else I would do this. For now use sprintf
				//and hopefuly it would overflow

				sprintf(veryLongString,"%f",real32Type);

				if (sqlQuery[Strlen(sqlQuery) - 1] == '(')
				{
					sqlQuery = S32Cat(sqlQueryTemp,1, veryLongString);
				} else {
					sqlQuery = S32Cat(sqlQueryTemp,2, ",",veryLongString);
				}

				if (sqlQueryTemp)
				{
					Free(sqlQueryTemp);
					sqlQueryTemp=NULL;
				}
				//printf("real32 type %f\n",real32Type);
				break;
			}
			case SQL3_TEXT:
			{
				s32 *stringType = va_arg(vl,s32*);

				sqlQueryTemp = sqlQuery;

				if (sqlQuery[Strlen(sqlQuery) - 1] == '(')
				{
					sqlQuery = S32Cat(sqlQueryTemp,3, "\"",stringType, "\"");
				} else {
					sqlQuery = S32Cat(sqlQueryTemp,3, ",\"",stringType, "\"");
				}

				if (sqlQueryTemp)
				{
					Free(sqlQueryTemp);
					sqlQueryTemp=NULL;
				}

				break;
			}
			case SQL3_FUNCTION:
			{
				s32 *stringType = va_arg(vl,s32*);

				sqlQueryTemp = sqlQuery;

				if (sqlQuery[Strlen(sqlQuery) - 1] == '(')
				{
					sqlQuery = S32Cat(sqlQueryTemp,1,stringType);
				} else {
					sqlQuery = S32Cat(sqlQueryTemp,2, ",",stringType); 
				}

				if (sqlQueryTemp)
				{
					Free(sqlQueryTemp);
					sqlQueryTemp=NULL;
				}

				break;
			}

			case SQL3_BLOB:
			{
				break;
			}
		}
	}
	sqlQueryTemp = sqlQuery;
	sqlQuery = S32Cat(sqlQueryTemp,1, " );");

	if (sqlQueryTemp)
	{
		Free(sqlQueryTemp);
		sqlQueryTemp=NULL;
	}

	sqlite3 *testDB=NULL;

	if (sqlite3_open_v2(databaseFile, &testDB, SQLITE_OPEN_READWRITE, NULL) == SQLITE_OK)
	{
		result = true;
	} else {
		printf("%s\n", sqlite3_errmsg(testDB));
	}

	printf("%s\n", sqlQuery);
	if (testDB)
	{
		if (sqlite3_prepare_v2(testDB,sqlQuery , -1, &selectStmt, NULL) == SQLITE_OK)
		{
			if(sqlite3_step(selectStmt)==SQLITE_DONE)
			{
				result= true;
			}
			else if(sqlite3_step(selectStmt) != SQLITE_BUSY)
			{
				result= false;
			}
		} else {
			result = false;
		}

		sqlite3_finalize(selectStmt);
        result = sqlite3_last_insert_rowid(testDB);
	} else {
		printf("database not open\n");
	}
	if (sqlQuery)
	{
		Free(sqlQuery);
		sqlQuery=NULL;
	}
	SL3_CloseDatabase(testDB);
	return result;
}

static int CommandCallback(void *NotUsed, int argc, char **argv, char **azColName) 
{ }

bool32 SL3_RunCommand(char* sqlFile, char* command)
{
	bool32 result = false;
	sqlite3 *testDB=NULL;
    sqlite3_stmt *selectStmt;

	if (sqlite3_open(sqlFile, &testDB) == SQLITE_OK)
	{
		result = true;
	}
	
	if (testDB)
	{
		if (sqlite3_exec(testDB,command,0,0,0) == SQLITE_OK)
		{
			result = true;
		}

	} else {
		printf("database not open\n");
	}
	

	SL3_CloseDatabase(testDB);

	return result;
}


bool32 SL3_FreeItems(marray *items)
{
	for (int itemsIndex = 0; itemsIndex < items->count;itemsIndex++)
	{
		marray *itemRow = (marray*) GetFromMArray(items,itemsIndex);

		for (int colIndex=0;colIndex < itemRow->count;colIndex++)
		{
			sql_item_info *itemInfo = (sql_item_info*) GetFromMArray(itemRow,colIndex);

			if (itemInfo->pointerTo)
			{
				Free(itemInfo->pointerTo);
				itemInfo->pointerTo= NULL;
			}
			
		}

		FreeMArray(itemRow);
	}

	FreeMArray(items);
}
#endif
