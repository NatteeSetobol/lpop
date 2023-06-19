#ifndef __STRINGZ_H__
#define __STRINGZ_H__
//NOTES(): EWWW
#include <stdarg.h>
#include <string.h>
#include "./required/intrinsic.hpp"
#include "./required/memory.hpp"
#include "array.hpp"
#include "memory_arena.hpp"

typedef unsigned char  us32;
typedef char  s32;

enum s32_token_type
{
	S32_NULL,
	S32_STRING
};

struct string_token
{
	size_t length;
	enum s32_token_type type;
	char* data;
	char c;
};

struct string_tokenizer
{
	int count;
	char* at;
};


Array *S32Find(s32* stringToFind, s32* source);
Array* S32Split(char* source, char* determinator);
s32* PushMidString(memory_arena *arena, s32 *source, i32 from, i32 to);
s32 *PushCatString(memory_arena *arena, i32 size, ...);
s32 *PushString(memory_arena *arena, s32* string, int32 size);
s32* S32TrimRetCarriage(s32* string);
s32* S32TrimSpaces(s32* string);
s32* S32Replace(char* source, char* replaceThis, char* replaceWith);
us32* BaseToUString(i32 number, i32 base);
s32* S32Trim(s32* string);
us32* S32Cat(us32* source, i32 size, ...);
char* ToLower_(char* lower, char* filename, int line);
struct string_token GetStringToken(struct string_tokenizer* tokenizer);
struct marray* FindString_(char* stringToFind, char* source, char* filename, int line);
int Strlen(char* string);
void Strcpy(char* dest, char* source);
char* vCreateString(char* newString, char* filename, int line);
char* vMidString(char* source, int from, int to, char* filename, int fileLine);
char* IToS(int integer);
int SToI(char* str);
b32 StrCmp(char* source, char* compareString);
struct marray* StrSplit_(char* source, char* determinator, char* filename, int line);
void Strcpy(unsigned char* dest, unsigned char* source);
Array* FindStringA(char* stringToFind, char* source);
unsigned char* vCreateString(unsigned char* newString, char* filename, int line);
int Strlen(unsigned char* string);
s32 *Char2BinaryAscii(us32 c );
s32* CS32Cat(i32 size, ...);
#define S32(newstring)  vCreateString(newstring,(char*) __FILE__,__LINE__);
#define S32M(newstring,file,line)  vCreateString(newstring,file,line);

#define US32(newstring)  vCreateString(newstring,(char*) __FILE__,__LINE__);
#define US32M(newstring,file,line)  vCreateString(newstring,file,line);

s32* S32Cat(s32* source, i32 size, ...);

//#define S32Cat(source,...) _S32Cat((char*) __FILE__,__LINE__,source,...);
#define MidString(source,from,to) vMidString(source,from,to,(char*) __FILE__,__LINE__) 
#define FindString(stringToFind, sourceString)  FindString_(stringToFind, sourceString,(char*) __FILE__,__LINE__)
//#define FindStringA(stringToFind, sourceString)  FindString__(stringToFind, sourceString, (char*) __FILE__,__LINE__)
#define ToLower(string) ToLower_(string, (char*) __FILE__,__LINE__)
#define  StrSplit(source,determinator) StrSplit_(source, determinator,(char*) __FILE__, __LINE__)


s32* URLEncode(s32 *text);
void StrCopy(char* dest, char* source, size_t length);
bool StrCmpCS(char* source, char* compareString);
#endif
