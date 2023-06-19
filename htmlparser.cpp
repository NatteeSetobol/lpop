#include "htmlparser.hpp"

//extern Queue htmlCommandList = {};

// NOTES(): Rename this to a better name that fits what this does.

Html_Parser::Html_Parser()
{
	htmlContent=NULL;
}

Html_Parser::~Html_Parser()
{
	FreeHtml();
	if (htmlData.data)
	{
		Free(htmlData.data);
		htmlData.data = NULL;
	}

	if (htmlContent)
	{
		Free(htmlContent);
		htmlContent = NULL;
	}

	QueueRemoveAll(&singletonTags);
}


s32 * Html_Parser::GetHtmlContent()
{
	return htmlContent;
}

bool Html_Parser::IsLetter(char c)
{
	bool result = false;
	if (c == ':')
	{
		result = true;
	}
	if (c == '-')
	{
		result = true;
	}
	if (c == '/')
	{
		result = true;
	} else
	if (c == '!')
	{
		result = true;
	} else
	if ((c >= 'a') && (c <= 'z') || (c >= 'A') && (c <= 'Z'))
	{
		result = true;
	}
	return result;
}

bool Html_Parser::isElementFound()
{
	return htmlData.element.isFound;
}

char * Html_Parser::GetNextElement(s32 *key, s32 *html,enum html_element_type eType )
{
	struct html_element_result htmlTable = {};
	char *tempHTML = NULL;

	if (html != NULL)
	{
		if (htmlData.data)
		{
			Free(htmlData.data);
			htmlData.data = NULL;
		}
		FreeHtml();
		htmlData.data = S32(html);
	} else
	if (htmlData.data == 0)
	{
		htmlData.data = S32(html);
	} else {
		FreeHtml();
	}

	if (htmlContent)
	{
		Free(htmlContent);
		htmlContent = NULL;
	}

	tempHTML = S32(htmlData.data);

	do
	{
		char *temp = NULL;
		htmlTable = FindElementBy(eType,key,tempHTML);

		temp = tempHTML;

		htmlData.element.isFound = htmlTable.isFound;
		if (htmlTable.isFound)
		{
			char *HTMLTable = NULL;

			tempHTML= MidString(temp,htmlTable.end,Strlen(tempHTML));

			if (htmlData.data)
			{
				Free(htmlData.data);
				htmlData.data = NULL;
			}
			htmlData.element = htmlTable;

			if (htmlData.data)
			{
				Free(htmlData.data);
			}
			htmlData.data = S32(tempHTML);

			htmlContent = MidString(temp,htmlTable.start,htmlTable.end);

			if (temp)
			{
				Free(temp);
				temp=NULL;
			}

			if (HTMLTable)
			{
				Free(HTMLTable);
				HTMLTable=NULL;
			}
			break;
		}


	} while (htmlTable.isFound);


	if (tempHTML)
	{
		Free(tempHTML);
		tempHTML=NULL;
	}

	QueueRemoveAll(&singletonTags);
	return htmlContent;

}


void Html_Parser::FreeCommandList(Queue *htmlCommandList)
{
    struct queue_info *htmlCommandListHead=NULL;
    if (htmlCommandList->count != 0)
    {
		htmlCommandListHead = htmlCommandList->head;

		while(htmlCommandListHead)
		{
			struct html_tag *oldTag = (struct html_tag*)htmlCommandListHead->address;
			
			if (oldTag)
			{
				if (oldTag->parameters)
				{
					FreeAllParameter(oldTag->parameters);	
					delete oldTag->parameters;
					oldTag->parameters = NULL;

					//FreeAllBuckets(oldTag->parameters);
					//Free(oldTag->parameters);
					//oldTag->parameters=NULL;

				}
				
				if (oldTag->name)
				{
					Free(oldTag->name);
					oldTag->name=NULL;
				}

			}

			htmlCommandListHead = htmlCommandListHead->next;
		}

		QueueRemoveAll(htmlCommandList);
		htmlCommandList->count = 0;
    }
}


struct html_token Html_Parser::GetHTMLToken(struct html_tokenizer* tokenizer)
{
	struct html_token tok = {};
	char c = 0;
	i8 *tokAddr = NULL;

	tok.length = 1;

	if (tokenizer->at)
	{
		tok.data = tokenizer->at;
		c = tokenizer->at[0];
		tok.c = c;

		if (c != '\0')
		{
			++tokenizer->count;
			++tokenizer->at;
		}

		switch(c)
		{
			case '\0': {tok.type=NONE;break;}
			case ' ': {tok.type=HTML_SPACE;break;}
			case '\t': {tok.type=HTML_TAB;break;}
			case '\\': {tok.type=ESCAPE_CHAR;break;}
			case '<': {tok.type=OPEN_BRACKET;break;}
			case '>': {tok.type=CLOSE_BRACKET;break;}
			case '=': {tok.type=EQUAL;break;}
			case '"': {tok.type=QUOTATION;break;}
			case '-': {tok.type=DASH;break;}
			case '\'': {tok.type=S_QUOTATION;break;}
			default: { tok.type=HTML_DATA;break;}
		}
	}
	return tok;
}

s32* Html_Parser::StriptHtml(s32* data)
{
	s32 *result = NULL;
	struct html_tokenizer htmlTokenizer= {0};
	struct html_token htmlToken = {};

	htmlTokenizer.at = data;

	htmlToken = GetHTMLToken(&htmlTokenizer);

	while (htmlToken.type != NONE)
	{
		switch(htmlToken.type)
		{
			case CLOSE_BRACKET:
			{
				htmlToken = GetHTMLToken(&htmlTokenizer);
				break;
			}
			case OPEN_BRACKET:
			{
				while(htmlToken.type != CLOSE_BRACKET && htmlToken.type != NONE )
				{
					htmlToken = GetHTMLToken(&htmlTokenizer);
				} 
				
				break;
			}
			default:
			{
				int startOf = htmlTokenizer.count;
				char *tokenAt = htmlTokenizer.at-1;
				int count = 0;
				do
				{
					htmlToken = GetHTMLToken(&htmlTokenizer);
					count++;
				} while(htmlToken.type != OPEN_BRACKET && htmlToken.type != NONE );


				if (result == NULL)
				{
					result = MidString(tokenAt, 0, count);
				} else {
					char *temp = NULL;
					char *temp2 = NULL;

					temp = result;

					temp2 = MidString(tokenAt,0,count);

					result = CS32Cat(2, temp, temp2);

					if (temp)
					{
						Free(temp);
						temp = NULL;
					}
					
					if (temp2)
					{
						Free(temp2);
						temp2 = NULL;
					}
				}

				break;
			}
		}
	}

	return result;

}

struct Map* Html_Parser::GetAllParametersFromTag(struct html_tokenizer *tokenizer, struct html_token *token)
{
	enum html_token_type quoteType = NONE; 
	char *inBracketData= NULL;
	struct html_token htmlToken = {};
	bool foundQuotes=false;
	bool foundEnd = false;
	bool foundValue=false;
	struct html_tokenizer bracketTokenizer = {0};
	struct html_token bracketToken = {0};
	Map *newBucket = NULL;
	char *startAt = NULL;
	char *endAt = NULL;
	i32 innerTaglength = 0;

	newBucket = new Map();

	//InitBucket(newBucket,256,DefaultHashFunction);

	startAt = tokenizer->at;

	htmlToken.type = HTML_START;

	while (htmlToken.type != NONE && foundEnd == false)
	{
		switch(htmlToken.type)
		{
			case CLOSE_BRACKET:
			{
				foundEnd = true;
				break;
			} 
			default:
			{
				break;
			}
		}

		htmlToken = GetHTMLToken(tokenizer);
	}

	endAt = tokenizer->at;
	
	innerTaglength = endAt - startAt;
	inBracketData = (s32*) MemoryRaw(innerTaglength+1);

	memcpy(inBracketData, startAt, innerTaglength);
	inBracketData[innerTaglength] = '\0';

	struct html_tokenizer linkTokenizer = {0};
	struct html_token linkToken = {0};
	char*  linkData = NULL;
	int quotationCount= 2;
	char *option = NULL;
	bool isTheEnd = false;

	s32* linkStartAt = NULL;
	s32* linkEndAt = NULL;
	i32 linkLen = 0;

	s32 *key = NULL;
	s32 *value = NULL;
	s32 *keyValueStart  = NULL; 
	i32 keyValueLen = 0;

	linkTokenizer.at = inBracketData;
	
	linkStartAt = linkTokenizer.at;
	keyValueStart  = linkTokenizer.at; 
	linkToken.type = HTML_START;

	while (isTheEnd == false && linkToken.type != NONE && linkToken.type != CLOSE_BRACKET)
	{
		switch(linkToken.type)
		{
			case EQUAL:
			{
				keyValueLen =linkTokenizer.at -  keyValueStart;
				key = (s32*) MemoryRaw(keyValueLen+1);
				memcpy(key, keyValueStart, keyValueLen-1);
				key[keyValueLen] = '\0';
				keyValueStart  =  linkTokenizer.at;

				bool valueEnd = false;

				do
				{
					switch(linkToken.type)
					{
						case S_QUOTATION:
						case QUOTATION:
						{
							do 
							{
								switch (linkToken.type)
								{
									case ESCAPE_CHAR:
									{

										linkToken = GetHTMLToken(&linkTokenizer);
										break;
									}
								}

								linkToken = GetHTMLToken(&linkTokenizer);

							} while (linkToken.type != S_QUOTATION && linkToken.type != QUOTATION && linkToken.type != NONE && linkToken.type != CLOSE_BRACKET);

							keyValueLen = linkTokenizer.at - keyValueStart;
							value = (s32*) MemoryRaw(keyValueLen+1);

							memcpy(value, keyValueStart+1, keyValueLen-2);
							valueEnd = true;

							while ( (linkTokenizer.at[0] == '\n' || linkTokenizer.at[0] == ' ')  && linkTokenizer.at[0] != '>'  && linkTokenizer.at[0]  != '\0')
							{
								linkTokenizer.at++;
							}
							keyValueStart  =  linkTokenizer.at;
							break;
						}
						case ESCAPE_CHAR:
						{
							linkToken = GetHTMLToken(&linkTokenizer);
							break;
						}
					}

					linkToken = GetHTMLToken(&linkTokenizer);

				} while (valueEnd == false && linkToken.type != NONE && linkToken.type != CLOSE_BRACKET);

				newBucket->Add(key,value);
				//AddS32ToBucket(newBucket,key,value);
				
				if (key)
				{
					Free(key);
					key=NULL;
				}
				/*
				if (value)
				{
					Free(value);
					value=NULL;
				}
				*/

				break;
			}
			case NONE:
			case CLOSE_BRACKET:
			default:
			{
				break;
			}
		}

		linkToken = GetHTMLToken(&linkTokenizer);
	}

	tokenizer->at = endAt;//linkTokenizer.at;
	
	if (inBracketData)
	{
		Free(inBracketData);
		inBracketData=NULL;
	}
	return newBucket;
}

bool Html_Parser::IsClosingTag(char *htmlCommand)
{
	bool result = false;

	result =  (htmlCommand[0] == '/');
	return result;
}

void Html_Parser::PopCommand(queue *q, queue_info* popped2,struct html_tag *foundTag)
{
	struct queue_info *popped = QueuePop(q);

	if (popped)
	{
		struct html_tag *oldTag = (struct html_tag *) popped->address;

		if (oldTag )
		{
			if (oldTag->name)
			{
				Free(oldTag->name);
				oldTag->name=NULL;
			}
			
			if (foundTag->found == false)
			{
				if (oldTag->parameters)
				{
					FreeAllParameter(oldTag->parameters);	
					delete oldTag->parameters;
					oldTag->parameters = NULL;
				}
			} 

			Free(oldTag);
			oldTag = NULL;
		}

		Free(popped);
		popped=NULL;
	}

}

struct html_element_result Html_Parser::ParseHTMLData(struct html_token *token,struct html_tokenizer *tokenizer,enum html_element_type eType,s32 *tag, Queue *htmlCommandList)
{
	struct html_element_result htmlElementResult= {};
	bool isDoneParsing = false;

	switch(token->type)
	{
		case HTML_SPACE:
		{
			break;
		}
		case ESCAPE_CHAR:
		{
			break;
		}
		case OPEN_BRACKET:
		{
			struct html_tokenizer htmlCommandTokenizer = {0};
			struct html_token htmlCommandToken = {0};
			s32 *htmlStuff = NULL;
			s32 *dataInsideOfBrackets=NULL;
			s32 *htmlCommand = NULL;

			htmlStuff = GetFromHTMLData(tokenizer, token,eType,tag);

			htmlCommandTokenizer.at = htmlStuff;

			htmlCommand =  GetCommandFromHTMLData(&htmlCommandTokenizer,&htmlCommandToken,eType,tag);

			if (htmlCommand)
			{
				if (StrCmp(htmlCommand, "!--"))
				{
					int parseComment = true;
					struct html_token commentToken = {};

					while (parseComment)
					{
						switch(commentToken.type)
						{
							case NONE:
							{
								parseComment = false;
								break;
							}
							case DASH:
							{
								commentToken = GetHTMLToken(tokenizer);

								if (commentToken.type == DASH)
								{
									commentToken = GetHTMLToken(tokenizer);

									if (commentToken.type == CLOSE_BRACKET)
									{
										parseComment = false;
									}
								}
								break;
							}
						}
						commentToken = GetHTMLToken(tokenizer);
					}
					htmlElementResult.isFound = false;
				} else 

				if (IsClosingTag(htmlCommand))
				{
					struct queue_info* queueItem= NULL;
					char *tempDeletedQuery = NULL;
					struct queue *htmlCommandListPtr = htmlCommandList;

					tempDeletedQuery = MidString((s32*) htmlCommand,  1,Strlen((char*) htmlCommand));
					for (queueItem=htmlCommandListPtr->head;queueItem;queueItem=queueItem->next)
					{
						struct html_tag *foundTag = (struct html_tag*) queueItem->address;

						if (StrCmpCS(tempDeletedQuery,foundTag->name))
						{
							if (StrCmpCS((char*) tempDeletedQuery,"script"))
							{
								isParsingScript = false;
							}

							foundTag->end =tokenizer->count;

							if (foundTag->found == true)
							{
								htmlElementResult.parameters = foundTag->parameters;
								htmlElementResult.start = foundTag->start;
								htmlElementResult.end = foundTag->end - (Strlen(foundTag->name)+3);
								htmlElementResult.isOpened = false;
								htmlElementResult.isFound = true;
								isDoneParsing=true;
							} else {
								htmlElementResult.isFound = false;
								
								if (foundTag->parameters)
								{

									FreeAllParameter(foundTag->parameters);	
									delete foundTag->parameters;
									foundTag->parameters = NULL;
									
								//	FreeAllBuckets(foundTag->parameters);
								//	Free(foundTag->parameters);
									//foundTag->parameters=NULL;
								}
							}

							PopCommand(htmlCommandList,queueItem, foundTag);
							break;
						}
						
					}

					if (tempDeletedQuery)
					{
						Free(tempDeletedQuery);
						tempDeletedQuery=NULL;
					}

				} else {
					if (isParsingScript == false)
					{
						Map *param = NULL;
						bool closeTagFound = false;

						/*NOTESO(): Check for self Closing Tag*/
						if (htmlStuff[Strlen(htmlStuff) - 1] == '/')
						{
							closeTagFound = true;
						}

						struct queue *singletonTagsPtr = &singletonTags;

						for (struct queue_info* singletonTag=singletonTagsPtr->head;singletonTag;singletonTag=singletonTag->next)
						{
							if (singletonTag->address)
							{
								if (StrCmpCS((char*) singletonTag->address , htmlCommand))
								{
									closeTagFound =true;
									break;
								}
							}
						}
						if (closeTagFound == false)
						{
							bool foundInParameter = false;

							if (isParsingScript == false)
							{
								if (htmlStuff)
								{
									switch(eType)
									{
										case TAG:
										{
											if (StrCmpCS((char*) htmlCommand, (char*) tag))
											{
												char *paramString = NULL;

												paramString = MidString(htmlStuff,Strlen(htmlCommand)+1,Strlen(htmlStuff));
												struct html_tokenizer pTokenizer = {0};
												struct html_token pToken = {0};

												pTokenizer.at = paramString;
												param = GetAllParametersFromTag(&pTokenizer,&pToken);
												foundInParameter=true;



												if (paramString)
												{
													Free(paramString);
													paramString=NULL;
												}
											}
											break;
										}
										default:
										{
											char *parameterString = MidString(htmlStuff,Strlen(htmlCommand)+1,Strlen(htmlStuff));
											struct html_tokenizer paramTokenizer = {0};
											struct html_token paramToken = {0};

											paramTokenizer.at = parameterString;

											foundInParameter= FindValueInParameter(&paramTokenizer,&paramToken,eType,tag);
											if (foundInParameter)
											{
												char *paramString = NULL;
												if (Strlen(htmlCommand) < Strlen(htmlStuff))
												{
													paramString = MidString(htmlStuff,Strlen(htmlCommand)+1,Strlen(htmlStuff));

													struct html_tokenizer pTokenizer = {0};
													struct html_token pToken = {0};
													pTokenizer.at = paramString;
													param = GetAllParametersFromTag(&pTokenizer,&pToken);

													if (paramString)
													{
														Free(paramString);
														paramString=NULL;
													}

												}

											}

											if (parameterString)
											{
												Free(parameterString);
												parameterString= NULL;
											}
											break;
										}
									}
								}

							} else {

							}

							struct html_tokenizer paramTokenizer = {0};
							struct html_token paramToken = {0};
							paramTokenizer.at = htmlStuff;

							if (StrCmpCS((char*)htmlCommand  , "script"))
							{
								isParsingScript = true;
							}

							struct html_tag *newTag = Memory(struct html_tag);

							newTag->name = S32(htmlCommand);
							newTag->start = tokenizer->count;
                            newTag->found = false;
                    
							if (foundInParameter )
							{
								if (param)
								{
									newTag->parameters = param;
								}

								newTag->found = true;
							} else {
								FreeAllParameter(param);	
								delete param;
								param = NULL;
							} 

						
							
							if (htmlCommand)
							{
								Free(htmlCommand);
								htmlCommand=NULL;
							}
							

							htmlElementResult.isOpened= true;
							QueuePush(htmlCommandList, (i32*)newTag);
						} else {
							if (tag)
							{

								if (StrCmpCS((char*) htmlCommand, (char*) tag ) && eType == TAG )
								{
									struct html_tokenizer paramTokenizer = {1};
									struct html_tokenizer pTokenizer = {0};
									char* paramString=NULL;
									struct html_token pToken = {0};

									paramString = MidString(htmlStuff,Strlen(htmlCommand)+1,Strlen(htmlStuff));

									pTokenizer.at = paramString;

									htmlElementResult.start = tokenizer->count;
									htmlElementResult.end = tokenizer->count + 2; 
									htmlElementResult.isFound = true;
									htmlElementResult.isOpened= true;
									htmlElementResult.parameters = GetAllParametersFromTag(&pTokenizer,&pToken);  
									isDoneParsing=true;

									if (paramString)
									{
										Free(paramString);
										paramString = NULL;
									}

											if (htmlStuff)
											{
												Free(htmlStuff);
												htmlStuff=NULL;
											}
											if (htmlCommand)
											{
												Free(htmlCommand);
												htmlCommand=NULL;
											}

									break;
								} else {

									char* paramString=NULL;
									bool foundInParameter= false; 
									struct html_tokenizer paramTokenizer = {0};
									struct html_token paramToken = {0};

									paramString = MidString(htmlStuff,Strlen(htmlCommand)+1,Strlen(htmlStuff));

									paramTokenizer.at = paramString;

									foundInParameter= FindValueInParameter(&paramTokenizer,&paramToken,eType,tag);

									if (foundInParameter)
									{
										struct html_tokenizer paramTokenizer = {0};
										struct html_tokenizer pTokenizer = {0};
										struct html_token pToken = {0};


										pTokenizer.at = paramString;

										htmlElementResult.start = tokenizer->count;;
										htmlElementResult.end = Strlen(htmlStuff);
										htmlElementResult.isFound = true;
										htmlElementResult.isOpened = true;

										htmlElementResult.parameters = GetAllParametersFromTag(&pTokenizer,&pToken);  
										isDoneParsing=true;

										if (htmlStuff)
										{
											Free(htmlStuff);
											htmlStuff=NULL;
										}
										if (htmlCommand)
										{
											Free(htmlCommand);
											htmlCommand=NULL;
										}
										if (paramString)
										{
											Free(paramString);
											paramString=NULL;
										}

										break;
									}

									if (paramString)
									{
										Free(paramString);
										paramString=NULL;
									}
								}
							}
						}
					}
				}

				Free(htmlCommand);
				htmlCommand=NULL;
			}

			if (htmlStuff)
			{
				Free(htmlStuff);
				htmlStuff=NULL;
			}
			break;
		}
		case CLOSE_BRACKET:
		{
			break;
		}
		default:
		{
			break;
		}
	}
	
	htmlElementResult.htmlCommandList.head =  htmlCommandList->head;
	htmlElementResult.htmlCommandList.current =  htmlCommandList->current;
	htmlElementResult.htmlCommandList.count = htmlCommandList->count;

	return htmlElementResult;
}


s32 * Html_Parser::GetFromHTMLData(struct html_tokenizer *tokenizer, struct html_token *token,enum html_element_type eType, s32 *data)
{
	char *inBracketData= NULL;
	struct html_token htmlToken = {}; //GetHTMLToken(tokenizer);
	struct html_element_result tagFound = {};
	
	if (isParsingScript == false)
	{

		s32 *startAt = tokenizer->at;
		s32 *endAt = NULL;
		i32 textLen = 0;
		htmlToken.type = HTML_START;

		while (htmlToken.type != CLOSE_BRACKET &&  htmlToken.type != NONE )
		{

			htmlToken = GetHTMLToken(tokenizer);
		}

		endAt = tokenizer->at;

		textLen = endAt - startAt;

		if (textLen > 0)
		{
			inBracketData = (s32*) MemoryRaw(textLen+1);

			memcpy(inBracketData, startAt, textLen-1);
			inBracketData[textLen] = '\0';
		}
	} else {
		char scriptTag[] = "script";
		int scriptTagIndex = 0;

		htmlToken = GetHTMLToken(tokenizer);

		if (htmlToken.c == '/')
		{
			while(1)
			{
				htmlToken = GetHTMLToken(tokenizer);

				if (htmlToken.c == scriptTag[scriptTagIndex])
				{
					scriptTagIndex++;

					if (scriptTagIndex >= Strlen(scriptTag)-1)
					{
						inBracketData = S32("/script");
						return inBracketData;
					}

					if (scriptTagIndex > Strlen(scriptTag)) break;
				} else {
					break;
				}
			}
		}

		return inBracketData;
	}

	return inBracketData;
}

s32 * Html_Parser::GetCommandFromHTMLData(struct html_tokenizer *tokenizer, struct html_token *token,enum html_element_type eType,s32* data)
{
	struct html_element_result  isTagFound = {};
	char *inBracketData= NULL;
	struct html_token htmlToken = {}; 
	char tempChar[2] = {0};
	char scriptTag[] = "script";
	int scriptTagIndex = 0;

	s32 *startAt = tokenizer->at;
	s32 *endAt = NULL;
	i32 textLen = 0;

	if (isParsingScript == false)
	{
		while (IsLetter(tokenizer->at[0]))
		{
			htmlToken = GetHTMLToken(tokenizer);
		}

		endAt = tokenizer->at;

		textLen = endAt - startAt;

		if (textLen > 0)
		{
			inBracketData = (s32*) MemoryRaw(textLen+1);

			memcpy(inBracketData, startAt, textLen);
			inBracketData[textLen] = '\0';
		}

	} else {
		scriptTagIndex = 0;

		htmlToken = GetHTMLToken(tokenizer);
		if (htmlToken.c == '/')
		{
			while(1)
			{
				htmlToken = GetHTMLToken(tokenizer);

				if (htmlToken.c == scriptTag[scriptTagIndex])
				{
					scriptTagIndex++;

					if (scriptTagIndex >= Strlen(scriptTag)-1)
					{
						inBracketData = S32("/script");
						return inBracketData;
					}

					if (scriptTagIndex > Strlen(scriptTag)-1) break;
				} else {
					break;
				}
			}

		}

		return inBracketData;
	}

	return inBracketData;
}

bool Html_Parser::FindValueInParameter(struct html_tokenizer *tokenizer, struct html_token *token,enum html_element_type eType,s32 *tag)
{
	bool result = false;
	enum html_token_type quoteType = NONE; 
	char *inBracketData= NULL;
	struct html_token htmlToken = {};
	bool foundQuotes=false;
	bool foundEnd = false;
	bool foundValue=false;
	struct html_tokenizer bracketTokenizer = {0};
	struct html_token bracketToken = {0};
	char *startAt = NULL;
	char *endAt = NULL;
	i32 innerTaglength = 0;

	startAt = tokenizer->at;

	htmlToken.type = HTML_START;

	while (htmlToken.type != NONE && foundEnd == false)
	{
		switch(htmlToken.type)
		{
			case CLOSE_BRACKET:
			{
				foundEnd = true;
				break;
			} 
			default:
			{
				break;
			}
		}

		htmlToken = GetHTMLToken(tokenizer);
	}

	endAt = tokenizer->at;
	
	innerTaglength = endAt - startAt;
	inBracketData = (s32*) MemoryRaw(innerTaglength+1);

	memcpy(inBracketData, startAt, innerTaglength);
	inBracketData[innerTaglength] = '\0';

	struct html_tokenizer linkTokenizer = {0};
	struct html_token linkToken = {0};
	char*  linkData = NULL;
	int quotationCount= 2;
	char *option = NULL;
	bool isTheEnd = false;

	s32* linkStartAt = NULL;
	s32* linkEndAt = NULL;
	i32 linkLen = 0;

	s32 *key = NULL;
	s32 *value = NULL;
	s32 *keyValueStart  = NULL; 
	i32 keyValueLen = 0;

	linkTokenizer.at = inBracketData;
	
	linkStartAt = linkTokenizer.at;
	keyValueStart  = linkTokenizer.at; 
	linkToken.type = HTML_START;

	while (isTheEnd == false && linkToken.type != NONE && linkToken.type != CLOSE_BRACKET)
	{
		switch(linkToken.type)
		{
			case EQUAL:
			{
				keyValueLen =linkTokenizer.at -  keyValueStart;
				keyValueStart  =  linkTokenizer.at;

				bool valueEnd = false;

				do
				{
					switch(linkToken.type)
					{
						case S_QUOTATION:
						case QUOTATION:
						{
							do 
							{
								switch (linkToken.type)
								{
									case ESCAPE_CHAR:
									{

										linkToken = GetHTMLToken(&linkTokenizer);
										break;
									}
								}

								linkToken = GetHTMLToken(&linkTokenizer);

							} while (linkToken.type != S_QUOTATION && linkToken.type != QUOTATION && linkToken.type != NONE && linkToken.type != CLOSE_BRACKET);

							keyValueLen = linkTokenizer.at - keyValueStart;
							value = (s32*) MemoryRaw(keyValueLen+1);

							memcpy(value, keyValueStart+1, keyValueLen-2);
							valueEnd = true;

							while ( (linkTokenizer.at[0] == '\n' || linkTokenizer.at[0] == ' ')  && linkTokenizer.at[0] != '>'  && linkTokenizer.at[0]  != '\0')
							{
								linkTokenizer.at++;
							}

							if (StrCmp(value,tag))
							{
								result = true;
								isTheEnd = true;
								valueEnd = true;
								if (value)
								{
									Free(value);
									value=NULL;
								}


								break;
                            }

                            if (value)
                            {
								Free(value);
								value=NULL;
                            }

							keyValueStart  =  linkTokenizer.at;

							break;
						}
						case ESCAPE_CHAR:
						{
							linkToken = GetHTMLToken(&linkTokenizer);
							break;
						}
					}

					linkToken = GetHTMLToken(&linkTokenizer);

				} while (valueEnd == false && linkToken.type != NONE && linkToken.type != CLOSE_BRACKET);
				
				break;
			}
			case NONE:
			case CLOSE_BRACKET:
			default:
			{
				break;
			}
		}

		linkToken = GetHTMLToken(&linkTokenizer);
	}

	tokenizer->at = linkTokenizer.at;
	
	if (inBracketData)
	{
		Free(inBracketData);
		inBracketData = NULL;
	}
	return result;
}

void Html_Parser::FillSingletonTags()
{
	QueuePushS32(&singletonTags,"area");
	QueuePushS32(&singletonTags,"bass");
	QueuePushS32(&singletonTags,"br");
	QueuePushS32(&singletonTags,"col");
	QueuePushS32(&singletonTags,"command");
	QueuePushS32(&singletonTags,"embed");
	QueuePushS32(&singletonTags,"hr");
	QueuePushS32(&singletonTags,"img");
	QueuePushS32(&singletonTags,"input");
	QueuePushS32(&singletonTags,"keygen");
	QueuePushS32(&singletonTags,"param");
	QueuePushS32(&singletonTags,"source");
	QueuePushS32(&singletonTags,"track");
	QueuePushS32(&singletonTags,"wbr");
    QueuePushS32(&singletonTags,"meta");
    QueuePushS32(&singletonTags,"link");
	
    QueuePushS32(&singletonTags,"!--");
    QueuePushS32(&singletonTags,"!--<div");
    QueuePushS32(&singletonTags,"--");
    QueuePushS32(&singletonTags,"!DOCTYPE");
	

}


struct html_element_result Html_Parser::GetElementInfoBy(enum html_element_type eType,s32 *key, s32 *html)
{
	Queue htmlCommandList = {};
	struct html_tokenizer htmlTokenizer = {0};
	struct html_token htmlToken = {0};
	struct html_element_result htmlElementResult= {};

	if (singletonTags.count == 0)
	{
		FillSingletonTags();
	}
	if (html)
	{
		int htmlLen = 0;
		int sourceIndex=0;

		htmlTokenizer.at = html;
		htmlLen = Strlen(html);

		for (sourceIndex=0; sourceIndex < htmlLen; sourceIndex++)
		{
			htmlToken = GetHTMLToken(&htmlTokenizer);
			htmlElementResult = ParseHTMLData(&htmlToken,&htmlTokenizer, eType,key,&htmlCommandList);

			if (htmlElementResult.isFound== true) 
			{
				break;
			}
		}

		struct queue_info *htmlCommandListHead=NULL;

		htmlCommandListHead = htmlCommandList.head;

		while(htmlCommandListHead)
		{
			struct html_tag *oldTag = (struct html_tag*)htmlCommandListHead->address;
			if (oldTag->parameters)
			{

				FreeAllParameter(oldTag->parameters);	
				delete oldTag->parameters;
				oldTag->parameters = NULL;
				//FreeAllBuckets(oldTag->parameters);
				//Free(oldTag->parameters);
				//oldTag->parameters= NULL;
			}
			if (oldTag->name)
			{
				Free(oldTag->name);
				oldTag->name=NULL;
			}

			htmlCommandListHead = htmlCommandListHead->next;
		}

		QueueRemoveAll(&htmlCommandList);
		singletonTags.count = 0;
	}

	return htmlElementResult;
}

struct html_element_result Html_Parser::FindElementBy(enum html_element_type eType,s32 *key, s32 *html)
{
	struct html_tokenizer htmlTokenizer = {0};
	struct html_token htmlToken = {0};
	struct html_element_result htmlElementResult= {};
	
    if (singletonTags.count == 0)
    {
        FillSingletonTags();
    }
	if (html)
	{
		int htmlLen = 0;
		int sourceIndex=0;

		htmlTokenizer.at = html;
		htmlLen = Strlen(html);

		for (sourceIndex=0; sourceIndex < htmlLen; sourceIndex++)
		{
			htmlToken = GetHTMLToken(&htmlTokenizer);
			htmlElementResult = ParseHTMLData(&htmlToken,&htmlTokenizer, eType,key, &htmlElementResult.htmlCommandList);

			if (htmlElementResult.isFound== true) 
			{

				//FreeCommandList(&htmlData.element.htmlCommandList);
				//htmlFree(&htmlElementResult);
				break;
			} else {
				//htmlFree(&htmlelementresult);
			}
		}
		int breakhere =0;
	}
 
	return htmlElementResult;
}

// NOTES(): Use this free
void Html_Parser::FreeHtml()
{

	FreeCommandList(&htmlData.element.htmlCommandList);
	htmlFree(&htmlData.element);


}

void Html_Parser::htmlFree(struct html_element_result *htmlResult)
{
	if (htmlResult->parameters)
	{
		FreeAllParameter(htmlResult->parameters);	
		delete htmlResult->parameters;
		htmlResult->parameters = NULL;

		//if (htmlResult->parameters->total != 0)
	//	{
			//FreeAllBuckets(htmlResult->parameters);
	//	}

	//	Free(htmlResult->parameters);
		//htmlResult->parameters = NULL;
	}

	FreeCommandList(&htmlResult->htmlCommandList);
}

s32 * Html_Parser::RemoveHTMLTags(s32 *html)
{
    s32 *result = NULL;
    
    struct html_tokenizer tokenizer = {0};
    struct html_token token = {0};
    
    tokenizer.at = html;
    
    token = GetHTMLToken(&tokenizer);

    while (token.type != NONE)
    {
        switch(token.type)
        {
            case ESCAPE_CHAR:
            {
                token = GetHTMLToken(&tokenizer);
                break;
            }
            case OPEN_BRACKET:
            {
                while (token.type != NONE && token.type != CLOSE_BRACKET)
                {
                    token = GetHTMLToken(&tokenizer);
                }
                break;
            }
            default:
            {
                char toString[2] = {};
                toString[0] = token.c;

                if (result == NULL)
                {
                    result = S32(toString);
                } else {
                    char *temp = S32Cat(result,1,toString);
                    
                    if (result)
                    {
                        Free(result);
                        result = NULL;
                    }
                    
                    result = S32(temp);
                    
                    if (temp)
                    {
                        Free(temp);
                        temp=NULL;
                    }
                    
                }
                
                break;
            }
        }
        
        token = GetHTMLToken(&tokenizer);
    }
    
    return result;
}

struct marray *Html_Parser::htmlGetList(char *htmlData)
{
	struct html_element_result liTag = {};
	struct marray *results = NULL;
	char *elementList = NULL;

	results =  Memory(struct marray);
	elementList = S32(htmlData);

	liTag = FindElementBy(TAG,"li",elementList);

	while(liTag.isFound)
	{
		if (liTag.isFound)
		{
			char *liItem = MidString(elementList,liTag.start,liTag.end);

			if (liItem)
			{
				char *newItem = S32(liItem);
				AddToMArray(results,newItem);
			}

			if (liItem)
			{
				Free(liItem);
				liItem=NULL;
			}

		}
		char *tempElement = elementList;
		elementList = MidString(tempElement,liTag.end,Strlen(tempElement));

		if (tempElement)
		{
			Free(tempElement);
			tempElement=NULL;
		}
		
		if (liTag.parameters)
		{
			FreeAllParameter(liTag.parameters);	
			delete liTag.parameters;
			liTag.parameters = NULL;
			//FreeAllBuckets(liTag.parameters);
			//Free(liTag.parameters);
			//liTag.parameters=NULL;
		}
		
		htmlFree(&liTag);
		liTag = FindElementBy(TAG,"li",elementList);
	}

	if (elementList)
	{
		Free(elementList);
		elementList=NULL;
	}

	htmlFree(&liTag);
	return results;
}


enum esc_token_type
{
	ESC_NULL,
	ESC_AMP,
	ESC_SEMICOLON,
	ESC_DEFAULT
};

struct esc_token
{
	size_t length;
	enum esc_token_type type;
	char* data;
	char c;
};

struct esc_token GetEscToken(struct string_tokenizer* tokenizer)
{
	struct esc_token tok = {};
	char c = 0;
	i8 tokAddr = NULL;

	tok.length = 1;

	if (tokenizer->at)
	{
		tok.data = tokenizer->at;
		c = tokenizer->at[0];
		tok.c = c;

		if (c != '\0')
		{
			++tokenizer->count;
			++tokenizer->at;
		}
		switch (c)
		{
			case '\0': {tok.type = ESC_NULL; break; }
			case '&': {tok.type = ESC_AMP; break; }
			case ';': {tok.type = ESC_SEMICOLON; break; }
			default: 
			{ 
				/*
				while (
					(tokenizer->at[0] >= 'A' && tokenizer->at[0] <= 'Z') ||
					(tokenizer->at[0] >= 'a' && tokenizer->at[0] <= 'z'))
				{

					printf("%c", tokenizer->at[0]);
					tok.length++;
					++tokenizer->count;
					++tokenizer->at;
				}
				printf("\n");
				printf("len: %i\n", tok.length);
				*/
				tok.type = ESC_DEFAULT;
				break; 
			}
		}

	}

	return tok;
}


s32 *ReplaceHtmlEscChar(s32 *html)
{
	struct esc_token stringToken  = {};
	struct string_tokenizer stringTokenizer = {};

	stringTokenizer.at = html;

	stringToken.type = ESC_DEFAULT;

	while(stringToken.type != ESC_NULL);
	{
		switch(stringToken.type)
		{
			case ESC_NULL:
			{
				break;
			}
			case ESC_AMP:
			{
				/*
				while(stringToken.type !=  ESC_SEMICOLON && stringToken.type !=  ESC_NULL)
				{
					stringToken.length++;
					stringToken = GetEscToken(&stringTokenizer);
				}
				*/
				
				//printf("length is %i\n", stringToken.length);
				break;
			}
			default:
			{
				printf("%c",stringTokenizer.at[0]);
				break;
			}

		} 

		stringToken = GetEscToken(&stringTokenizer);
	}
	printf("\n");
	return NULL;
}

void Html_Parser::FreeAllParameter(Map *parameters)
{
	if (parameters == NULL) return ;
	for (int i=0;i<parameters->keys->count;i++)
	{
		s32* keyValue = NULL;
		s32* parameterValue = NULL;
		struct Map_Node *mapNode = NULL;

		keyValue = (s32*) parameters->keys->Get(i);

		//parameterValue = (s32*) parameters->Get(keyValue);

		mapNode = (struct Map_Node*) parameters->GetNode(keyValue);

		if (mapNode->value)
		{
			s32 *mapNodeValue = NULL;
			mapNodeValue = (s32*) mapNode->value;

			if (mapNodeValue)
			{
				Free(mapNodeValue);
				mapNodeValue = NULL;

				mapNode->value = NULL;
			}
		}

		if (mapNode->CollusionArray)
		{
			for (int j=0;j < mapNode->CollusionArray->count;j++)
			{
				struct Map_Node *mNode = NULL;
				mNode = (struct Map_Node*) mapNode->CollusionArray->Get(j);

				if (mNode)
				{
					s32 *mNodeValue = NULL;
					s32 *mNodeKey=NULL;

					mNodeKey = (s32*) mNode->key;
					mNodeValue = (s32*) mNode->value;
					if (mNodeKey)
					{
						Free(mNodeKey);
						mNodeKey=NULL;
					}
					if (mNodeValue)
					{
						Free(mNodeValue);
						mNodeValue = NULL;
					}

					Free(mNode);
					mNode=NULL;
				}
			}

			delete mapNode->CollusionArray;
			mapNode->CollusionArray = NULL;
		}


		/*
		if (parameterValue)
		{
			Free(parameterValue);
			parameterValue = NULL;
		}*/

		if (keyValue)
		{
			Free(keyValue);
			keyValue = NULL;
		}
	}

	delete parameters->keys;
	parameters->keys = NULL;

	delete parameters;
	parameters=NULL;
}