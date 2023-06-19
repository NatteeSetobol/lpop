#include "JsonParser.hpp"

struct Json_Branch* AddSubBranch(struct Json_Branch* realCurrent, enum Json_Type type)
{
	struct Json_Branch* subBranch = (struct Json_Branch*) Memory(struct Json_Branch);

	subBranch->type = type;
	subBranch->key = NULL;
	subBranch->value = NULL;
	subBranch->head = NULL;
	subBranch->current = NULL;
	subBranch->prevBranch = realCurrent;
	subBranch->subBranch = NULL;
	subBranch->head = NULL;
	subBranch->current = NULL;

	return subBranch;
}

void JSON_Parse(char* json, struct Json_Branch* jBranch)
{
	struct JSON_Tokenizer jTokenizer = { };
	struct JSON_Token jToken = { };
	int dataLen = NULL;
	struct Json_Branch* current = NULL;
	bool isKey = false;
	int cnt = 0;

	dataLen = Strlen(json);

	current = jBranch;

	jTokenizer.at = json;

	for (int i = 0; i < dataLen; i++)
	{
		jToken = GetJSONToken(&jTokenizer);

		switch (jToken.type)
		{
			case SPLITTER:
			{
				if (isKey == true)
				{
					char* val = NULL;
					char* tempVal = NULL;

					do
					{
						jToken = GetJSONToken(&jTokenizer);

						switch (jToken.type)
						{
							case ENTER:
							case SPACE:
							{
								break;
							}
							case LIST_START:
							{
								struct Json_Branch* realCurrent = NULL;
								
								realCurrent = current;
								if (isKey == true)
								{
									current = current->current;
									isKey = false;
								}

								if (current->type == JARRAY)
								{
									Json_AddBranch(current, JLIST, NULL, NULL);
									current->current->prevBranch = current;
									current = current->current;
								}

								current->subBranch = AddSubBranch(realCurrent, JLIST);
								current = current->subBranch;
								break;
							}
							case ARRAY_START:
							{
								struct Json_Branch* realCurrent = NULL;
								realCurrent = current;

								if (isKey == true)
								{
									current = current->current;
									isKey = false;
								}

								current->subBranch = AddSubBranch(realCurrent, JARRAY);
								current = current->subBranch;
								break;
							}
							default:
							{
								if (jToken.type == IDENTIFIER)
								{
									s32 *oldValue = NULL;
									size_t valLength = 0;


									oldValue = jTokenizer.at;
									jToken = GetJSONToken(&jTokenizer);

									while (jToken.type != IDENTIFIER && jToken.type != STRING_TERM)
									{
										/*

										   char v[3] = { 0 };
										   v[0] = jToken.token;


										   if (val == NULL)
										   {
										   val = S32(v);

										   }
										   else {
										   tempVal = S32Cat(val, 1, v);

										   Free(val);
										   val = NULL;

										   val = S32(tempVal);

										   Free(tempVal);
										   tempVal = NULL;
										   }

										 */
										jToken = GetJSONToken(&jTokenizer);
									}

									valLength =  jTokenizer.at - oldValue;
									val = MemoryA(char,valLength);
									StrCopy(val,oldValue,valLength-1); 
								}
								else 
								{
									s32 *oldValue = NULL;
									size_t valLength = 0;

									oldValue = jTokenizer.at-1;

									while (jToken.type != LIST_END && jToken.type != NEXT && jToken.type != STRING_TERM)
									{
										/*
										   char v[3] = { 0 };
										   v[0] = jToken.token;

										   if (val == NULL)
										   {
										   val = S32(v);

										   }
										   else 
										   {
										   tempVal = S32Cat(val, 1, v);

										   Free(val);
										   val = NULL;

										   val = S32(tempVal)

										   if (StrCmp(val, (char*) "null"))
										   {
										   int breakhere = 0;
										   }

										   Free(tempVal);
										   tempVal = NULL;
										   }
										 */
										jToken = GetJSONToken(&jTokenizer);
									}


									valLength =  jTokenizer.at - oldValue;
									val = MemoryA(char,valLength);
									StrCopy(val,oldValue,valLength-1); 

								}


								if (val)
								{
									current->current->value = S32(val);
									Free(val);

									if (jToken.type == LIST_END)
									{
										jTokenizer.at--;
									}
								}
								break;
							}
						}
					} while (jToken.type == SPACE);

					isKey = false;
				}
				break;
			}
			case NEXT:
			{
				if (isKey == true)
				{
					isKey = false;
				}
				break;
			}
			case SPACE:
			{
				break;
			}
			case LIST_START:
			{
				struct Json_Branch* realCurrent = NULL;
				realCurrent = current;

				if (isKey == true)
				{
					current = current->current;
					isKey = false;
				}

				if (current->type == JARRAY)
				{
					Json_AddBranch(current, JLIST, NULL, NULL);
					current->current->prevBranch = current;
					current = current->current;

				}

				current->subBranch = AddSubBranch(realCurrent, JLIST);
				current = current->subBranch;
				break;
			}
			case ARRAY_START:
			{
				struct Json_Branch* realCurrent = NULL;

				realCurrent = current;

				if (isKey == true)
				{
					current = current->current;
					isKey = false;
				}

				current->subBranch = AddSubBranch(realCurrent, JARRAY);
				current = current->subBranch;

				break;
			}
			case LIST_END:
			{
				if (isKey == true) isKey = false;
				if (current->prevBranch)
				{
					current = current->prevBranch;
				}
				break;
			}
			case ARRAY_END:
			{
				if (isKey == true) isKey = false;

				if (current->prevBranch)
				{
					current = current->prevBranch;
				}
				break;
			}

			case IDENTIFIER:
			{
				char* value = NULL;
				bool isStringLoopDone = false;
				char *currentPlacement = NULL;

				currentPlacement = jTokenizer.at;

				for (int j = 0; j < dataLen && isStringLoopDone == false; j++)
				{
					jToken = GetJSONToken(&jTokenizer);

					switch (jToken.type)
					{
						case IDENTIFIER:
						{
							value = MidString(currentPlacement,0,j);
							isStringLoopDone = true;
							break;
						}
						default:
						{
								/*
								   char temp[2] = { 0 };
								   temp[0] = jToken.token;
								   temp[1] = '\0';

								   if (value == NULL)
								   {
								   value = S32(temp);
								   }
								   else {
								   char* tempValue = NULL;

								   tempValue = S32Cat(value, 1, temp);

								   Free(value);
								   value = NULL;

								   value = S32(tempValue);

								   Free(tempValue);
								   tempValue = NULL;
								   }
								 */
							break;
						}
					}
				}

				if (value)
				{
					if (isKey == false)
					{
						if (value)
						{
							Json_AddBranch(current, JDICTIONARY, value, NULL);
							isKey = true;
						}
					}
					else {
						current->current->value = S32(value);
						isKey = false;
					}

					Free(value);
					value = NULL;
				}

				break;
			}
		}
	}
}


void JsonPrintAll(struct Json_Branch* branch)
{
	struct Json_Branch* result = branch;
	struct Json_Branch* current = branch;
	bool found = false;
	int count = 0;

	switch (current->type)
	{
		case JARRAY:
		{
			current = current->head;
			while (current)
			{
				if (current != NULL)
				{
					if (current->subBranch)
					{
						JsonPrintAll(current->subBranch);
					}

					current = current->next;
				}
			}
			break;
		}
		case JLIST:
		{
			if (current->head)
			{

				current = current->head;

				if (current->value)
				{
					printf("key: %s, value: %s\n", current->key, current->value);
				}
				else {
					printf("key: %s\n", current->key);
					if (current->subBranch)
					{
						JsonPrintAll(current->subBranch);
					}

				}

				JsonPrintAll(current);
			}
			break;
		}
		case JDICTIONARY:
		{
			if (current->next)
			{
				current = current->next;
				if (current->value)
				{
					printf("key: %s, value: %s\n", current->key, current->value);
				}
				else {
					printf("key: %s\n", current->key);
					if (current->subBranch)
					{
						JsonPrintAll(current->subBranch);
					}
				}

				JsonPrintAll(current);
			}
			break;
		}
	}
	return;
}




struct Json_Results* FindByKey(s32* key, struct Json_Branch* branch, struct Json_Results* savedResult)
{
	struct Json_Results* result = NULL;
	struct Json_Branch* current = branch;
	bool found = false;
	int count = 0;

	if (savedResult)
	{
		result = savedResult;
	}

	switch (current->type)
	{
	case JARRAY:
	{
		current = current->head;
		while (current)
		{
			if (current != NULL)
			{
				if (current->subBranch)
				{
					result = FindByKey(key, current->subBranch, result);
				}
				current = current->next;
			}
		}
		break;
	}
	case JLIST:
	{
		if (current->head)
		{
			current = current->head;

			if (current->value)
			{
				if (StrCmp(current->key, key)) // && result == NULL)
				{
					if (result == NULL)
					{
						result = (struct Json_Results*) MemoryRaw(sizeof(struct Json_Results));
						result->success = true;
					}

					if (current->value)
					{
						s32* newValue = S32(current->value);
						AddToMArray(&result->items, newValue);
					}
				}
			}
			else {
				if (StrCmp(current->key, key))
				{
					if (result == NULL)
					{
						result = (struct Json_Results*) MemoryRaw(sizeof(struct Json_Results));
						result->success = true;
					}
					if (current->value)
					{
						s32* newValue = S32(current->value);
						AddToMArray(&result->items, newValue);
					}

				}

				if (current->subBranch)
				{
					result = FindByKey(key, current->subBranch, result);
				}

			}
			result = FindByKey(key, current, result);
		}
		break;
	}
	case JDICTIONARY:
	{
		if (current->next)
		{
			current = current->next;
			if (current->value)
			{
				if (StrCmp(current->key, key))
				{
					if (result == NULL)
					{
						result = (struct Json_Results*) MemoryRaw(sizeof(struct Json_Results));
						result->success = true;
					}

					if (current->value)
					{
						s32* newValue = S32(current->value);
						AddToMArray(&result->items, newValue);
					}

				}

			}
			else {
				if (StrCmp(current->key, key))
				{
					if (result == NULL)
					{
						result = (struct Json_Results*) MemoryRaw(sizeof(struct Json_Results));
						result->success = true;
					}

					if (current->value)
					{
						s32* newValue = S32(current->value);
						AddToMArray(&result->items, newValue);
					}

				}

				if (current->subBranch)
				{
					result = FindByKey(key, current->subBranch, result);
				}
			}

			result = FindByKey(key, current, result);
		}
		break;
	}
	}

	return result;
}

struct JSON_Token GetJSONToken(struct JSON_Tokenizer* jsonTokenizer)
{
	struct JSON_Token jsonTokenResults = { 0 };

	char c = 0;
	i8* tokAddr = NULL;

	jsonTokenResults.length = 1;
	jsonTokenResults.JsonData = jsonTokenizer->at;

	c = jsonTokenizer->at[0];
	jsonTokenResults.token = c;
	if (c != '\0')
	{
		++jsonTokenizer->at;
	}

	switch (c)
	{
		case '\0':
		{
			jsonTokenResults.type = STRING_TERM;
			break;
		}
		case 0x20:
		{
			jsonTokenResults.type = SPACE;
			break;
		}
		case '{':
		{
			jsonTokenResults.type = LIST_START;
			break;
		}
		case '}':
		{
			jsonTokenResults.type = LIST_END;
			break;
		}
		case '[':
		{
			jsonTokenResults.type = ARRAY_START;
			break;
		}
		case ']':
		{
			jsonTokenResults.type = ARRAY_END;
			break;
		}
		case '"':
		{
			jsonTokenResults.type = IDENTIFIER;
			break;
		}
		case ':':
		{
			jsonTokenResults.type = SPLITTER;
			break;
		}
		case '\n':
		{
			jsonTokenResults.type = ENTER;
			break;
		}
		case ',':
		{
			jsonTokenResults.type = NEXT;
			break;
		}
	
	};

	return jsonTokenResults;
}

struct Json_Branch* Json_DelTree(struct Json_Branch* branch)
{
	struct Json_Branch* retTree = NULL;
	if (branch->subBranch)
	{
		retTree = Json_DelTree(branch->subBranch);

	}
	if (branch->head)
	{
		struct Json_Branch* retBranch = Json_DelBranch(branch->head);

		if (retBranch != NULL)
		{
			if (retBranch->key)
			{
				Free(retBranch->key);
				retBranch->key = NULL;
			}
			if (retBranch->value)
			{
				Free(retBranch->value);
				retBranch->value = NULL;
			}
			Free(retBranch);
			retBranch = NULL;
		}
	}

	if (retTree != NULL)
	{
		if (retTree->key)
		{
			Free(retTree->key);
			retTree->key = NULL;
		}
		if (retTree->value)
		{
			Free(retTree->value);
			retTree->value = NULL;
		}

		Free(retTree);
		retTree = NULL;
	}

	return branch;
}

struct Json_Branch* Json_DelBranch(struct Json_Branch* branch)
{
	struct Json_Branch* retTree = NULL;

	if (branch->subBranch)
	{
		retTree = Json_DelTree(branch->subBranch);
	}
	if (branch->next)
	{
		struct Json_Branch* retBranch = Json_DelBranch(branch->next);

		if (retBranch != NULL)
		{
			if (retBranch->key)
			{
				Free(retBranch->key);
				retBranch->key = NULL;
			}
			if (retBranch->value)
			{
				Free(retBranch->value);
				retBranch->value = NULL;
			}
			Free(retBranch);
			retBranch = NULL;
		}
	}

	if (retTree != NULL)
	{
		if (retTree->key)
		{
			Free(retTree->key);
			retTree->key = NULL;
		}
		if (retTree->value)
		{
			Free(retTree->value);
			retTree->value = NULL;
		}
		Free(retTree);
		retTree = NULL;
	}
	return branch;
}

void GetBranchType(struct Json_Branch* branch)
{
	switch (branch->type)
	{
		case JNONE:
		{
			printf("NO TYPE\n");
			break;
		}
		case JLIST:
		{
			printf("Json type\n");
			break;
		}
		case JDICTIONARY:
		{
			printf("Dictionary type\n");
			break;
		}
		case JARRAY:
		{
			printf("JArray type\n");
			break;
		}
	}
	return;
}

void Json_Free(struct Json_Branch* branch)
{
	Json_DelTree(branch);
}

void Json_AddBranch(struct Json_Branch* branch, enum Json_Type type, char* key, char* value)
{
	if (branch->head == NULL)
	{
		branch->head = (struct Json_Branch*) Memory(struct Json_Branch);
		branch->head->type = type;
		if (key != NULL)
		{
			branch->head->key = S32(key);
		}
		else {
			branch->head->key = NULL;
		}

		if (value != NULL)
		{
			branch->head->value = S32(value);
		}
		else {
			branch->head->value = NULL;
		}
		branch->head->next = NULL;
		branch->head->prev = NULL;
		branch->head->head = NULL;
		branch->head->current = NULL;
		branch->current = branch->head;
	}
	else {
		branch->current->next = (struct Json_Branch*) Memory(struct Json_Branch);
		branch->current->next->type = type;
		if (key)
		{
			branch->current->next->key = S32(key);
		}
		else {
			branch->current->next->key = NULL;
		}
		if (value)
		{
			branch->current->next->value = S32(value);;
		}
		else {
			branch->current->next->value = NULL;;
		}
		branch->current->next->prev = branch->current;
		branch->current->next->head = NULL;
		branch->current->next->current = NULL;
		branch->current = branch->current->next;
	}
}

Json_Branch *AddArray(Json_Branch *branchToAddTo,s32* key = NULL, Json_Type type = JARRAY)
{
	branchToAddTo->subBranch = (Json_Branch*) MemoryRaw(sizeof(Json_Branch));
	branchToAddTo->type = type;
	if (key)
	{
		branchToAddTo->key = S32(key);
	}
	return branchToAddTo; 
}

Json_Branch *AddDictionary(Json_Branch *subBranch, s32* key, s32* value)
{
	if (subBranch->head == NULL)
	{
			subBranch->head = (Json_Branch*) MemoryRaw(sizeof(Json_Branch));
			subBranch->head->type = JDICTIONARY;
			subBranch->head->key = S32(key);
			subBranch->head->value = S32(value);
			subBranch->current = subBranch->head;
	} else {
			subBranch->current->next = (Json_Branch*) MemoryRaw(sizeof(Json_Branch));
			subBranch->current->next->type = JDICTIONARY;
			subBranch->current->next->key = S32(key);
			subBranch->current->next->value = S32(value);
			subBranch->current = subBranch->current->next;
	}

	return subBranch->current;
}

s32 *JsonToString(Json_Branch *mainBranch)
{
	Stack branches={};
	Json_Branch *currentBranch = NULL;
	s32 *jsonString = NULL;
	Json_Type branchType = JNONE;

	currentBranch = mainBranch;
	
	while(currentBranch)
	{
		if (currentBranch->type == JLIST || currentBranch->type == JARRAY)
		{
			if (jsonString == NULL)
			{
				switch(currentBranch->type)
				{
					case JLIST:
					{
						jsonString = S32("{");
						break;
					}
					case JARRAY:
					{
						if (currentBranch->key)
						{
							jsonString = CS32Cat(4,"\"", currentBranch->key, "\"", ": [");
						} else {
							jsonString = S32("[");
						}
						break;
					}
				}
			} else {
				s32 *temp = NULL;
				temp = jsonString;

				switch(currentBranch->type)
				{
					case JLIST:
					{
						jsonString = CS32Cat(2,temp, "{");
						break;
					}
					case JARRAY:
					{	
						if (currentBranch->key)
						{
							jsonString = CS32Cat(5,temp,"\"", currentBranch->key, "\"", ": [");
						} else {

							jsonString = CS32Cat(2,temp, "[");
						}
						break;
					}
				}

				Free(temp);
			}

			branchType = currentBranch->type;

			branches.Push( (i32*) currentBranch);
			currentBranch = currentBranch->subBranch->head;

			switch(branchType)
			{	
				case JARRAY:
				{
					s32 *temp = NULL;
					temp = jsonString;
					if (currentBranch->type == JDICTIONARY || currentBranch->type == JLIST)
					{
						jsonString = CS32Cat(2,temp, "{");
					}

					Free(temp);
					break;
				}

			}

		} else {
			/* do calulation */
			
			if (jsonString == NULL)
			{
				Assert("this should never happen");
			} else {
				s32* temp = NULL;

				temp = jsonString;

				jsonString = CS32Cat(8,temp, "\"", currentBranch->key, "\"", ":", "\"", currentBranch->value, "\""  );

				Free(temp);
			}
			
			if (currentBranch->next == NULL)
			{
				if (branches.count > 0)
				{
					Json_Branch *returnBranch = NULL;
					queue_info *info = NULL;
					
					s32 *temp = NULL;

					if (currentBranch->type == JDICTIONARY)
					{
						s32 *tempVar = NULL;
						tempVar = jsonString;

						jsonString = CS32Cat(2,tempVar, "}");

						Free(tempVar);
					}

					temp = jsonString;

					info = branches.Pop();

					returnBranch = (Json_Branch*) info->address;

					currentBranch = returnBranch->next; 
					//Free(info->address);
					Free(info);

					if (currentBranch == NULL) 
					{
						switch(returnBranch->type)
						{
							case JDICTIONARY:
							case JLIST:
							{
								jsonString = CS32Cat(2,temp, "}");
								break;
							}
							case JARRAY:
							{
								jsonString = CS32Cat(2,temp, "]}");
								break;
							}
						}

						Free(temp);
						return jsonString;
					} else {
						switch(returnBranch->type)
						{
							case JDICTIONARY:
							case JLIST:
							{
								jsonString = CS32Cat(2,temp, "},");
								break;
							}
							case JARRAY:
							{
								jsonString = CS32Cat(2,temp, "],");
								break;
							}
						}
					}

					Free(temp);

					s32 *tempChar = NULL;

					jsonString = CS32Cat(2,tempChar, "}");

					Free(tempChar);

				} else {
					break;
				}
			} else {
				
				s32 *temp = NULL;

				temp = jsonString;

				jsonString = CS32Cat(2, temp, ",");

				Free(temp);
				
				currentBranch = currentBranch->next;
			}
		}

	}
	return jsonString;
}