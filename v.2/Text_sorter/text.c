#include "text.h"

#define CODE_POSITIONS 8 * byte_amt - byte_amt - 1

//*************************************************************************************************

int opener (File *file, int argc, char **argv)
{
	if (argc >= 2)
		file->text = fopen (argv [1], "r");
	else
		file->text = fopen ("text", "r");

	if (!(file->text))
	{
		perror ("problema with reading file: \n");
		exit (1);
	}

	fseek (file->text, 0, SEEK_END);
	file->size = ftell (file->text);
	fseek (file->text, 0, SEEK_SET);
	file->buf_char = (char *) calloc (file->size, sizeof (char));

	if (!(file->buf_char))
	{
		perror ("can't read file into buffer: \n");
		exit (2);
	}

	return 0;
}

//*************************************************************************************************

void str_data (File *file)
{
	file->strs = (Str *) calloc (file->nstr, sizeof (Str));
	if (!(file->strs))
	{
		perror ("can't allocate memory for str ptr array: \n");
		exit (3);
	}

	for (size_t let = 0, num = 0; let < file->size;)
	{
		while (let < file->size && (file->buf [let] == '\n' || file->buf [let] == '\r'))
		{
			file->buf [let] = '\0';
			let++;
			break;
		}

		if (let < file->size)
			file->strs[num].str  = file->buf + let;

		size_t len = 0;
		while (let < file->size && file->buf [let] != '\n' && file->buf [let] != '\r')
		{
			let++;
			len++;
		}

		if (let < file->size)
			file->strs[num].slen = len;

		num++;
	}
}

//*************************************************************************************************

void get_file (File *file)
{
	size_t nsymb = fread (file->buf_char, sizeof (char), file->size, file->text);
	if (!(nsymb == file->size))
	{
		printf ("fread returned invalid value: %lu;\nfile size = %lu.\n", nsymb, file->size);
		exit (4);
	}

	for (size_t let = 0; let < file->size;)
	{
		while (let < file->size && (file->buf_char [let] == '\n' || file->buf_char [let] == '\r'))
		{
			let++;
			break;
		}

		if (let < file->size)
			file->nstr++;

		while (let < file->size && file->buf_char [let] != '\n' && file->buf_char [let] != '\r')
			let++;
	}

	if (decode (file)) 
		printf ("Error: decode() returned 1:\n"
				"	unknown file coding system:\n"
				"	byte amount >= 4 or = 1.\n");
}

//*************************************************************************************************

void print_file (File *file, char *name)
{
	FILE *output = fopen (name, "w");

	for (size_t num = 0; num < file->nstr; num++)
		if (line_chk (file->strs[num].str))
			fprintf (output, "%ls\n", formater(file->strs[num].str));
		else
			num++;

	fclose (output);
}

//*************************************************************************************************

wchar_t *formater (wchar_t *str_start)
{
	wchar_t *new = str_start;
	while (*new == '\t'|| *new == ' ')
		new++;

	return new;
}

//*************************************************************************************************

int decode (File *file)
{
	wchar_t *buf_dec = calloc (file->size, sizeof (wchar_t));
	if (!buf_dec)
	{
		printf ("Error: decode () line %d:\n", __LINE__ - 3);
		printf ("	can't allocate memory for\n"
				"	buffer with decoded symbols.\n");
		exit (EXIT_FAILURE);
	}
	size_t symb = 0;

	for (size_t let = 0; let < file->size; let++)
	{
		size_t shift    = 7;
		size_t byte_amt = 0;
	
		while (file->buf_char [let] & (1 << shift) && shift)
		{
			byte_amt++;
			shift--;
		}

		if (byte_amt == 1 || byte_amt > 4)
		{
			return 1;
		}

		if (!byte_amt)
		{
			buf_dec [symb] = (wchar_t) file->buf_char [let];
			symb++;
			continue;
		}

		wchar_t code = 0;
		wchar_t fund = 1;

		let += byte_amt - 1;

		for (size_t bit = 0; bit < CODE_POSITIONS; bit++)
		{
			if (bit % 8 == 6)
			{
				bit += 2;
				let--;
			}
			code += fund * ((file->buf_char [let] >> (bit % 8)) & 1);
			fund *= 2;
		}

		let += byte_amt - 1;
		buf_dec [symb] = code;
		symb++;
	}

	free (file->buf_char);

	buf_dec = (wchar_t *) realloc (buf_dec, symb * sizeof (wchar_t));
	if (!buf_dec)
	{
		printf ("Error: decode () line %d:\n", __LINE__);
		printf ("	can't reallocate memory for\n"
				"	buffer with decoded symbols.\n");
		exit (EXIT_FAILURE);
	}

	file->buf = buf_dec;
	file->size = symb;

	return 0;
}

//*************************************************************************************************

#ifdef EN

int scmp (const void *s1, const void *s2)
{
	if (!s1 || !s2)
	{
		perror ("scmp () received invalid strs: \n");
		exit (5);
	}

	size_t pos1 = 0, pos2 = 0;

	wchar_t *str1 = ((const Str *) s1)->str;
	wchar_t *str2 = ((const Str *) s2)->str;
	
	while (str1 [pos1] && str2 [pos2])
	{
		while (str1 [pos1] && !iswalpha (str1 [pos1]))
				pos1++;

		while (str1 [pos1] && !iswalpha (str2 [pos2]))
				pos2++;

		wchar_t let1 = towlower (str1 [pos1]);
		wchar_t let2 = towlower (str2 [pos2]);

		if (let1 != let2)
			return let1 - let2;

		if (let1 && let2)
		{
			pos1++;
			pos2++;
		}
	}

	return 0;
}

//*************************************************************************************************

int scmp_rev (const void *s1, const void *s2)
{
	if (!s1 || !s2)
	{
		perror ("scmp_rev () received invalid strs: \n");
		exit (5);
	}

	wchar_t *str1 = ((const Str *) s1)->str;
	wchar_t *str2 = ((const Str *) s2)->str;

	int pos1 = ((const Str *) s1)->slen;
	int pos2 = ((const Str *) s2)->slen;

	while (pos1 >= 0 && pos2 >= 0)
	{
		while (pos1 >= 0 && !iswalpha (str1 [pos1]))
				pos1--;

		while (pos2 >= 0 && !iswalpha (str2 [pos2]))
				pos2--;

		wchar_t let1 = towlower (str1 [pos1]);
		wchar_t let2 = towlower (str2 [pos2]);

		if (let1 != let2)
			return let1 - let2;

		pos1--;
		pos2--;
	}

	return 0;
}

//*************************************************************************************************

int line_chk (wchar_t *line)
{
	int letter = 0;
	for (size_t pos = 0; line [pos]; pos++)
		if (iswalpha (line [pos]))
		{
			letter++;
			break;
		}

	return letter;
}

#endif

//*************************************************************************************************

#ifdef RU

#define ISLET(letter) letter >= L'А' && letter <= L'я'
#define ISSPL(letter) letter == L'Ё' || letter == L'ё'

//*************************************************************************************************

int isalpha_ru (wchar_t letter)
{
	if (ISLET(letter))
		return 1;
	if (ISSPL(letter))
		return 1;

	return 0;
}

//*************************************************************************************************

wchar_t tolower_ru (wchar_t letter)
{
	if (letter >= L'А' && letter <= L'Я')
		return letter + 32;
	if (letter >= L'а' && letter <= L'я')
		return letter;
	if (letter == L'Ё')
		return L'ё';

	return letter;
}

//*************************************************************************************************

int scmp (const void *s1, const void *s2)
{
	if (!s1 || !s2)
	{
		perror ("scmp () received invalid strs: \n");
		exit (5);
	}

	size_t pos1 = 0, pos2 = 0;

	wchar_t *str1 = ((const Str *) s1)->str;
	wchar_t *str2 = ((const Str *) s2)->str;
	
	while (str1 [pos1] && str2 [pos2])
	{
		while (str1 [pos1] && !isalpha_ru (str1 [pos1]))
				pos1++;

		while (str1 [pos1] && !isalpha_ru (str2 [pos2]))
				pos2++;

		wchar_t let1 = tolower_ru (str1 [pos1]);
		wchar_t let2 = tolower_ru (str2 [pos2]);

		if (let1 != let2)
		{
			if (ISLET(let1) && ISLET(let2))
				return let1 - let2;
			if (ISSPL(let1))
			{
				if (let2 < L'ж') return  1;
				if (let2 > L'е') return -1;
			}
			if (ISSPL(let2))
			{
				if (let1 < L'ж') return -1;
				if (let1 > L'е') return  1;
			}	
		}

		if (let1 && let2)
		{
			pos1++;
			pos2++;
		}
	}

	return 0;
}

//*************************************************************************************************

int scmp_rev (const void *s1, const void *s2)
{
	if (!s1 || !s2)
	{
		perror ("scmp () received invalid strs: \n");
		exit (5);
	}

	wchar_t *str1 = ((const Str *) s1)->str;
	wchar_t *str2 = ((const Str *) s2)->str;

	int pos1 = ((const Str *) s1)->slen;
	int pos2 = ((const Str *) s2)->slen;

	while (pos1 >= 0 && pos2 >= 0)
	{
		while (pos1 >= 0 && !isalpha_ru (str1 [pos1]))
				pos1--;

		while (pos2 >= 0 && !isalpha_ru (str2 [pos2]))
				pos2--;

		wchar_t let1 = tolower_ru (str1 [pos1]);
		wchar_t let2 = tolower_ru (str2 [pos2]);

		if (let1 != let2)
		{
			if (ISLET(let1) && ISLET(let2))
				return let1 - let2;
			if (ISSPL(let1))
			{
				if (let2 < L'ж') return  1;
				if (let2 > L'е') return -1;
			}
			if (ISSPL(let2))
			{
				if (let1 < L'ж') return -1;
				if (let1 > L'е') return  1;
			}	
		}

		pos1--;
		pos2--;
	}

	return 0;
}

//*************************************************************************************************

int line_chk (wchar_t *line)
{
	int letter = 0;
	for (size_t pos = 0; line [pos]; pos++)
		if (isalpha_ru (line [pos]))
		{
			letter++;
			break;
		}

	return letter;
}

#endif
