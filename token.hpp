#ifndef __TOKEN__
#define __TOKEN__
#include "required/intrinsic.hpp"
#include "required/nix.hpp"
#include "required/memory.hpp"
#include "required/platform.hpp"
#include "strings.hpp"

struct token
{
	size_t length;
	i32 type;
	s32 *data;
	char tok;
};

struct tokenizer
{
	int count;
	s32 *at;
};

struct token MoveToken(struct tokenizer* tokenizer);

#endif

