#include "token.hpp"

struct token MoveToken(struct tokenizer* tokenizer)
{
	struct token result = {};

	result.length = 1;
	result.data = tokenizer->at;
	result.tok = tokenizer->at[0];


	if (result.tok != '\0')
	{
		++tokenizer->count;
		++tokenizer->at;
	}

	return result;
}
