#ifndef __CL_OPTIONS__
#define  __CL_OPTIONS__

#include "required/intrinsic.hpp"
#include "required/nix.hpp"
#include "required/memory.hpp"
#include "required/platform.hpp"
#include "required/assert.hpp"
#include "bucket.hpp"
#include "array.hpp"
#include "strings.hpp"
#include "memory_arena.hpp"


class Options;

#define OPTION_CALLBACK(name) void name(Options *option)
typedef OPTION_CALLBACK(option_callback);

enum option_types{OPTION_NONE, OPTION_PARAM };

struct options_params
{
	s32 *trigger;
	s32 *name;
	s32 *description;
	option_types type;
	s32 *value;
	bool hasArg;
	bool isRequired;
	bool isTriggered;
	bool hasMultiParams;
	Map *params;
	option_callback *callback; 
};

class Options
{
	public:
	s32 **arguments;
	i32 argumentsCount;
	s32 *exeName;
	s32 *description;
	void Add(options_params *params);
	options_params *Get(s32* trigger);
	void Execute();
	bool CheckRequirements();
	Options(int argc, char *args[]);
	~Options();

	HashBucket *options;
	i32 *pointer;
	option_callback *executeOption;
};


#endif
