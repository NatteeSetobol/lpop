#ifndef __HTMLPARSER_H__
#define __HTMLPARSER_H__

#include "required/intrinsic.hpp"
//#include "httpprotocol.h"
#include "required/nix.hpp"
#include "required/memory.hpp"
#include "required/platform.hpp"
#include "required/assert.hpp"
#include "maths.hpp"
#include "array.hpp"
#include "strings.hpp"
#include "bucket.hpp"
#include "token.hpp"
#include "queue.hpp"

/*TODO(): Need to deal with html comment janky-ness </--, Right now it is added as a singleton
 which doesn't fully work.
 */

typedef enum html_token_type
{
	NONE,
	HTML_SPACE,
	HTML_TAB,
	OPEN_BRACKET,
	CLOSE_BRACKET,
	ESCAPE_CHAR,
	HTML_DATA,
	EQUAL,
	QUOTATION,
	S_QUOTATION,
	DASH,
	HTML_START,
	HTML_TOKEN_COUNT,
} html_token_type;

typedef enum html_element_type
{
	CLASS,
	ID,
	TAG
} html_element_type;

typedef struct html_token
{
	size_t length;
	enum html_token_type type;
	char* data;
	char c;
} html_token;

typedef struct html_tokenizer
{
	int count;
	char* at;
} html_tokenizer;

typedef struct html_tag
{
	char *name;
	int start;
	int end;
	bool found;
	Map *parameters;
}html_tag;

typedef struct http_packet
{
	struct marray headers;
	char *data;
	int dataLen;
} http_packet;

typedef struct html_element_result
{
	i32 start;
	i32 end;
	bool isFound;
	bool isOpened;
	Queue htmlCommandList;
	Map *parameters;
	//struct bucket_list *parameters;
}html_element_result;

typedef struct html
{
	struct html_element_result element;
	char *data;
	int isSet;
} html;

class Html_Parser
{
	private:
		s32 *htmlContent;
	public:
		html htmlData={};
		Queue singletonTags={};
		bool isParsingScript=false;
		
		s32 *GetHtmlContent();
		Html_Parser();
		~Html_Parser();
		bool isElementFound();
		bool IsLetter(char c);
		void FreeHtml();
		void FreeCommandList(Queue *htmlCommandList);
		struct html_token GetHTMLToken(struct html_tokenizer* tokenizer);
		s32* StriptHtml(s32* data);

		struct html_element_result FindElementBy(enum html_element_type eType,s32 *key, s32 *html);
		void htmlFree(struct html_element_result *htmlResult);
		char* ToLower_(char* lower, char* filename, int line);
		s32 *GetCommandFromHTMLData(struct html_tokenizer *tokenizer, struct html_token *token,enum html_element_type eType, s32 *data);
		s32 *GetFromHTMLData(struct html_tokenizer *tokenizer, struct html_token *token, enum html_element_type eType,s32 *data);
		s32 *GetParameter(struct html_tokenizer *tokenizer, struct html_token *token,enum html_element_type eType,s32 *tag);
		bool FindValueInParameter(struct html_tokenizer *tokenizer, struct html_token *token,enum html_element_type eType,s32 *tag);
		char* GetNextElement(s32 *key, s32 *html,enum html_element_type eType );
		struct marray *htmlGetList(char *htmlData);
		s32 *RemoveHTMLTags(s32 *html);
		struct html_element_result ParseHTMLData(struct html_token *token,struct html_tokenizer *tokenizer,enum html_element_type eType,s32 *tag, Queue *htmlCommandList);
		struct html_element_result GetElementInfoBy(enum html_element_type eType,s32 *key, s32 *html);
		void FillSingletonTags();
		Map* GetAllParametersFromTag(struct html_tokenizer *tokenizer, struct html_token *token);
		bool IsClosingTag(char *htmlCommand);
		void PopCommand(queue *q, queue_info* popped2,struct html_tag *foundTag);

		void FreeAllParameter(Map *parameters);
};


s32 *ReplaceHtmlEscChar(s32 *html);

#endif
