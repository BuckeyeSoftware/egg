#include <bes/foundation/memory.h>
#include <bes/foundation/stream.h>
#include <bes/foundation/buffer.h>
#include <bes/foundation/string.h>

#include <bes/egg/lex.h>

#include <stdlib.h> // malloc, realloc, free
#include <stdio.h> // fread, fwrite, fseek, ftell

// allocator
static void*
allocate(bes_allocator *allocator, bes_size size)
{
	(void)allocator;
	return malloc(size);
}

static void*
reallocate(bes_allocator *allocator, void *data, bes_size size)
{
	(void)allocator;
	return realloc(data, size);
}

static void
deallocate(bes_allocator *allocator, void *data)
{
	(void)allocator;
	free(data);
}

static bes_allocator allocator =
{
	allocate,
	reallocate,
	deallocate,
	0
};

// stream (wrap stdio)
static bes_bool
stream_read(bes_stream *stream, void *data_, bes_size size, bes_size *read_)
{
	FILE *fp = stream->aux;
	*read_ = fread(data_, 1, size, fp);
	return *read_ == size ? BES_TRUE : BES_FALSE;
}

static bes_bool
stream_write(bes_stream *stream, const void *data, bes_size size, bes_size *write_)
{
	FILE *fp = stream->aux;
	*write_ = fwrite(data, 1, size, fp);
	return *write_ == size ? BES_TRUE : BES_FALSE;
}

static bes_bool
stream_seek(bes_stream *stream, bes_u64 offset)
{
	FILE *fp = stream->aux;
	return fseek(fp, offset, SEEK_SET) == 0;
}

static bes_bool
stream_tell(bes_stream *stream, bes_u64 *offset_)
{
	FILE *fp = stream->aux;

	const long tell = ftell(fp);

	if (tell == -1L)
	{
		return BES_FALSE;
	}

	*offset_ = tell;

	return BES_TRUE;
}

int main()
{
	bes_allocator_set(&allocator);

	bes_stream stream;
	stream.read = stream_read;
	stream.write = stream_write;
	stream.seek = stream_seek;
	stream.tell = stream_tell;
	stream.aux = stdin;

	BES_BUFFER(char) line = BES_BUFFER_INITIALIZER;

	printf("egg> ");
	for (char data; bes_stream_read(&stream, &data, 1, &(bes_size){0}); )
	{
		if (data == '\n')
		{
			// null-terminator
			if (!bes_buffer_push(line, '\0'))
			{
				bes_buffer_free(line);
				return 1;
			}

			if (!bes_strcmp(line, ".exit") ||
			    !bes_strcmp(line, ".quit") ||
			    !bes_strcmp(line, ".q") ||
			    !bes_strcmp(line, ".e"))
			{
				break;
			}

			BES_BUFFER(bes_egg_token) tokens = BES_BUFFER_INITIALIZER;
			if (bes_egg_tokenize(line, bes_buffer_size(line) - 1, &tokens))
			{
				const bes_size n_tokens = bes_buffer_size(tokens);
				for (bes_size i = 0; i < n_tokens; i++)
				{
					bes_egg_token *const token = tokens + i;
					switch (token->type)
					{
					case BES_TOKEN_ATOM:
						printf("(atom %s)", token->atom);
						bes_buffer_free(token->atom);
						break;
					case BES_TOKEN_ERROR:
						printf("(error %c)", token->error);
						break;
					case BES_TOKEN_PAREN:
						printf("(paren %c)", token->paren);
						break;
					case BES_TOKEN_STRING:
						printf("(string \"%s\")", token->string);
						bes_buffer_free(token->string);
						break;
					}
				}
			}
			bes_buffer_free(tokens);
			bes_buffer_clear(line);

			printf("\negg> ");
		}
		else if (!bes_buffer_push(line, data))
		{
			bes_buffer_free(line);
			return 1;
		}
	}

	bes_buffer_free(line);
	return 0;
}
