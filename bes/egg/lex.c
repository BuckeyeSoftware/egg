#include <bes/egg/lex.h>

#define TRY_PUSH(BUFFER, VALUE) \
	do \
	{ \
		if (!bes_buffer_push((BUFFER), (VALUE))) \
		{ \
			bes_buffer_free((BUFFER)); \
			goto oom; \
		} \
	} while (0)

static bes_bool
is_whitespace(int ch)
{
	return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

static bes_bool
is_paren(int ch)
{
	return ch == '(' || ch == ')';
}

static bes_bool
is_quote(int ch)
{
	return ch == '\'' || ch == '"';
}

static bes_bool
is_atom(int ch)
{
	return !is_whitespace(ch) && !is_paren(ch) && !is_quote(ch);
}

bes_bool
bes_egg_tokenize(const char *contents,
                 bes_size length,
                 BES_BUFFER(bes_egg_token) *tokens_)
{
	BES_BUFFER(bes_egg_token) tokens = BES_BUFFER_INITIALIZER;

	for (bes_size i = 0; i < length; )
	{
		int ch1 = contents[i];
		if (is_whitespace(ch1))
		{
			// skip whitespace
			i++;
			continue;
		}
		else if (is_paren(ch1))
		{
			bes_egg_token token;
			token.type = BES_TOKEN_PAREN;
			token.paren = ch1;

			TRY_PUSH(tokens, token);

			// skip ( or )
			i++;
		}
		else if (is_quote(ch1))
		{
			bes_egg_token token;
			token.type = BES_TOKEN_STRING;
			token.string = BES_BUFFER_INITIALIZER;

			// skip starting ' or "
			bes_size j = i + 1;
			for (; j < length; j++)
			{
				int ch2 = contents[j];
				if (ch2 == ch1)
				{
					// ensure string is terminated
					TRY_PUSH(token.string, '\0');
					// skip terminating ' or "
					i = j + 1;
					break;
				}

				TRY_PUSH(token.string, (char)ch2);
			}

			// string not terminated is an error
			if (j + 1 != i)
			{
				bes_buffer_free(token.string);
				token.type = BES_TOKEN_ERROR;
				token.error = '?';
			}

			TRY_PUSH(tokens, token);
		}
		else if (is_atom(ch1))
		{
			bes_egg_token token;
			token.type = BES_TOKEN_ATOM;
			token.atom = BES_BUFFER_INITIALIZER;

			bes_size j;
			for (j = i; j < length; j++)
			{
				int ch2 = contents[j];
				if (!is_atom(ch2))
				{
					break;
				}

				TRY_PUSH(token.atom, (char)ch2);
			}

			i = j;

			// null terminate
			TRY_PUSH(token.atom, '\0');
			TRY_PUSH(tokens, token);
		}
		else
		{
			bes_egg_token token;
			token.type = BES_TOKEN_ERROR;
			token.error = ch1;

			TRY_PUSH(tokens, token);

			// skip erroring token
			i++;
		}
	}

	*tokens_ = tokens;
	return BES_TRUE;

oom:
	bes_buffer_free(tokens);
	return BES_FALSE;
}
