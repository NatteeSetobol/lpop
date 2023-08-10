#ifndef JSON_PARSER
#define JSON_PARSER
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include "required/intrinsic.hpp"
#include "required/nix.hpp"
#include "required/memory.hpp"
#include "required/platform.hpp"
#include "required/assert.hpp"
#include "array.hpp"
#include "strings.hpp"
#include "../lpop/queue.hpp"

enum Json_Type{ JNONE,JLIST, JDICTIONARY, JARRAY };

struct Json_Branch
{
	enum Json_Type type;
	char *key;
	char *value;
	struct Json_Branch *prev;
	struct Json_Branch *next;

	struct Json_Branch *head;
	struct Json_Branch *current;

	struct Json_Branch *prevBranch;
	struct Json_Branch *subBranch;
};

enum JSON_Token_Type
{
	DATA,
	LIST_START,
	LIST_END,
	ARRAY_START,
	ARRAY_END,
	IDENTIFIER,
	SPLITTER,
	NEXT,
	SPACE,
	STRING_TERM,
	ENTER,
	ESCAPE
};

struct JSON_Token
{
	size_t length;
	enum JSON_Token_Type type;
	char *JsonData;
	char token;
};

struct JSON_Tokenizer
{
	char *at;
};

struct Json_Results
{
	bool32 success;
	struct marray items;
};

void JSON_Parse(char* json, struct Json_Branch* jBranch);
struct Json_Branch* AddSubBranch(struct Json_Branch* realCurrent, enum Json_Type type);
void JSON_Parse(char* json, struct Json_Branch* jBranch, struct Error_Handler* error);
void JsonPrintAll(struct Json_Branch* branch);
struct Json_Results* FindByKey(s32* key, struct Json_Branch* branch, struct Json_Results* savedResult);
struct JSON_Token GetJSONToken(struct JSON_Tokenizer* jsonTokenizer);
struct Json_Branch* Json_DelTree(struct Json_Branch* branch);
struct Json_Branch* Json_DelBranch(struct Json_Branch* branch);
void GetBranchType(struct Json_Branch* branch);
void Json_Free(struct Json_Branch* branch);
void Json_AddBranch(struct Json_Branch* branch, enum Json_Type type, char* key, char* value);

Json_Branch *AddArray(Json_Branch *branchToAddTo,s32* key , Json_Type type);
Json_Branch *AddDictionary(Json_Branch *subBranch, s32* key, s32* value);
s32 *JsonToString(Json_Branch *mainBranch);
#endif
