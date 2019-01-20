#ifndef BES_EGG_LEX_H
#define BES_EGG_LEX_H

#include <bes/foundation/buffer.h>

typedef struct bes_egg_token bes_egg_token;

enum
{
	BES_TOKEN_ERROR,
	BES_TOKEN_PAREN,
	BES_TOKEN_ATOM,
	BES_TOKEN_STRING,
	BES_TOKEN_NUMBER
};

struct bes_egg_token
{
	int type;

	union
	{
		int paren;
		int error;
		BES_BUFFER(char) atom;
		BES_BUFFER(char) string;
	};
};

bes_bool
bes_egg_tokenize(const char *contents,
                 bes_size length,
                 BES_BUFFER(bes_egg_token) *tokens_);

#endif
