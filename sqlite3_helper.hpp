
#ifdef USE_SQL
#include <sqlite3.h>
#include "required/intrinsic.hpp"
#include "required/nix.hpp"
#include "required/memory.hpp"
#include "required/platform.hpp"
#include "required/assert.hpp"
#include "bucket.hpp"
#include "array.hpp"
#include "strings.hpp"
#include "maths.hpp"
#include "JsonParser.hpp"
#include "threads.hpp"

enum sqlite3_types
{
	SQL3_NULL,
	SQL3_INTEGER,
	SQL3_REAL,
	SQL3_TEXT,
	SQL3_BLOB,
	SQL3_FUNCTION
};

struct sql_item_info
{
	void *pointerTo;
	sqlite3_types type;
};

struct sql_queue 
{
	sqlite3 *testDB;
	s32 *sqlStatement;
    sqlite3_stmt *selectStmt;
};

bool32 SL3_OpenDatabase(s32 *filename, sqlite3 *database);
bool32 SL3_SaveDatabase(sqlite3_stmt *selectStmt);
bool32 SL3_CloseDatabase(sqlite3* database);
bool32 SL3_GetFromDatabase(char *databaseFile,s32* tableName,s32 *items, s32* query, marray *result);
int SL3_InsertIntoDatabase(char* databaseFile,s32* tableName ,i32 itemCount, ...);
bool32 SL3_FreeItems(marray *items);
bool32 SL3_RunCommand(char* sqlFile, char* command);
void BeginSQLQueue(struct sql_queue *sQueue,char *databaseFile,s32* tableName,s32 *items, s32* query);
void EndSQLQueue(struct sql_queue *sQueue);
bool GetSQLNextRow(struct sql_queue *sQueue,struct marray *results);
#endif
