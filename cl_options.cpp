#include "cl_options.hpp"

bool Options::CheckRequirements()
{
	bool result = true;

	for (int i = 0; i < options->keys.length; i++)
	{
		s32 *key = (s32*) options->keys.Get(i);
		options_params *optionsParams = (options_params*)options->Get(key);

		if (optionsParams->isRequired)
		{
			if (optionsParams->isTriggered == false)
			{
				result = false;
				break;
			}
		}
	}

	return result;
}

void Options::Add(options_params *params)
{
	if (params)
	{
		options->Add((i8*)params->trigger, (i8*)params);
	}
}

options_params *Options::Get(s32* trigger)
{
	options_params *oParams = (options_params*) options->Get(trigger);

	return oParams;
}

void Options::Execute()
{

	for (int i=0; i < argumentsCount; i++)
	{
		s32 *option = (char *) *(arguments + i);
		
		options_params *oParams = (options_params*) options->Get(option);
		if (oParams)
		{
			if (oParams->hasArg)
			{
				if (i+1 < argumentsCount)
				{
					oParams->value = S32( (char*) *(arguments+ i + 1));
				}
			}

			if (oParams->hasMultiParams)
			{
				if (i+1 < argumentsCount)
				{
					Array *equalSplit = NULL;
					if (oParams->params == NULL)
					{
						oParams->params = new Map();
					}
					equalSplit = S32Split(oParams->value,"=");

					if (equalSplit)
					{
						if (equalSplit->length == 2)
						{
							s32* key = NULL;
							s32* value = NULL;
							s32* v2 = NULL;
							s32* k2 = NULL;

							key = (s32*) equalSplit->Get(0);
							value = (s32*) equalSplit->Get(1);
							
							v2 = S32(value);

							oParams->params->Add(key,v2);
						}
						delete equalSplit;
					}

					Free(oParams->value);
					oParams->value=NULL;
				}
			}
			
			oParams->isTriggered = true;

			if (oParams->callback)
			{
				if (oParams->hasArg)
				{
					if (executeOption == NULL)
					{
						executeOption = oParams->callback;
					}
				} else {
					oParams->callback(this);
					break;
				}
			}
		}
	}

	if (CheckRequirements())
	{
		if (executeOption)
		{
			executeOption(this);
		}
	}
}

OPTION_CALLBACK(ShowHelp)
{
	printf("Usage: %s <options>\n", option->exeName);
	printf("%s", option->description);
	printf("\n\n");

	for (int i = 0; i < option->options->keys.length; i++)
	{
		s32 *key = (s32*) option->options->keys.Get(i);
		options_params *optionsParams = (options_params*)option->options->Get(key);
		printf("%s\t%s\n", optionsParams->trigger, optionsParams->description);
	}
}

Options::Options(int argc, char *args[])
{
	arguments = args;
	argumentsCount = argc;
	options = new HashBucket();
	exeName = S32(args[0]);
	executeOption=NULL;

	struct options_params *help=  Memory(struct options_params);
	help->trigger = S32("--help");
	help->name = S32("help");
	help->description = S32("Shows this help menu");
	help->type = OPTION_PARAM;
	help->hasArg = false;
	help->isRequired = false;
	help->callback = ShowHelp;
	Add(help);
}

Options::~Options()
{
	struct options_params *help = NULL;

    help = Get("--help");

	Free(help->trigger);
	Free(help->name);
	Free(help->description);

	Free(help);
	
	if (exeName)
	{
		Free(exeName);
		exeName = NULL;
	}

	delete options;
	options=NULL;
	#if 0
	for (int i=0; i < options->count; i++)
	{
		s32 *key = NULL;
		struct options_params *optionParams = NULL;
	
		key = (s32*) options->keys.Get(i);
		
		optionParams = (options_params*) options->Get(key);
		/*
		if (optionParams->trigger)
		{
			Free(optionParams->trigger);
			optionParams->trigger=NULL;
		}
		*/
		if (optionParams->name)
		{
			Free(optionParams->name);
			optionParams->name = NULL;
		}
		
		if (optionParams->description)
		{
			Free(optionParams->description);
			optionParams->description=NULL;
		}
		
		if (optionParams->value)
		{
			Free(optionParams->value);
			optionParams->value = NULL;
		}
		
		
	//	Free(optionParams);
		//optionParams=NULL;

	}
	
	if (description)
	{
		Free(description);
		description = NULL;
	}

	Free(exeName);
	exeName=NULL;

	delete options;
	options=NULL;
	#endif
}

