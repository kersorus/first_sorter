#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>


typedef struct String
{
	char *str;
	size_t slen;
}Str;

typedef struct File
{
	char *buf;
	FILE *text;
	size_t nstr;
	size_t size;
	Str *strs;	
}File;


void text_form   (File *file);
void str_data    (File *file);
void get_file    (File *file);
void print_file  (File *file, char *name);
int opener       (File *file, int argc, char **argv);
int scmp         (const void *s1, const void *s2);
int scmp_rev     (const void *s1, const void *s2);
int line_chk     (char *line);
void swapv       (void *e1, void *e2, size_t esize);
void qsort_my    (void *array, size_t asize, size_t esize, int (*cmp) (const void *, const void *));


int main (int argc, char *argv[])
{
	remove ("log");

	File file = {0};
	if (opener (&file, argc, argv)) return 1;

	get_file   (&file);
	str_data   (&file);
	print_file (&file, "text1");

	qsort_my (file.strs, file.nstr, sizeof (Str), scmp);
	print_file (&file, "text2");

	qsort_my (file.strs, file.nstr, sizeof (Str), scmp_rev);
	print_file (&file, "text3");

	free (file.buf);
	free (file.strs);
	fclose (file.text);

	printf ("#  results here:\n"
			"#  1) 'text1' <- buffer text;\n"
			"#  2) 'text2' <- sorted text;\n"
			"#  3) 'text3' <- sorted text (in reverse).\n");

	return 0;
}


int opener (File *file, int argc, char **argv)
{
	assert (file);
	assert (argv);

	if (argc >= 2)
		file->text = fopen (argv [1], "r");
	else
		file->text = fopen ("text", "r");

	if (!(file->text))
	{
		perror ("problema with reading file: \n");
		return 1;
	}

	fseek (file->text, 0, SEEK_END);
	file->size = ftell (file->text);
	fseek (file->text, 0, SEEK_SET);
	file->buf = (char *) calloc (file->size, sizeof (char));

	assert (file->buf);

	return 0;
}


void str_data (File *file)
{
	assert (file);

	file->strs = (Str *) calloc (file->nstr, sizeof (Str));
	assert (file->strs);
                                                                   	//Sublime Text, VS Code, Atom, CLion
	for (size_t i = 0, nomer = 0; i < file->size;)
	{
		while (i < file->size && file->buf [i] == '\n')
		{
			file->buf [i] = '\0';
			i++;
		}

		if (i < file->size)
			file->strs[nomer].str  = file->buf + i;

		size_t j = 0;
		while (i < file->size && file->buf [i] != '\n')
		{
			i++;
			j++;
		}

		if (i < file->size)
			file->strs[nomer].slen = j;

		nomer++;
	}
}


void get_file (File *file)
{
	assert (file);

	FILE *log = fopen ("log", "a");
	assert (log);

	size_t nsymb = fread (file->buf, sizeof (char), file->size, file->text);
	assert (nsymb == file->size);
	fprintf (log, "#  Number of symbols in start file: %lu.\n", nsymb);

	//TODO strtoul()
	for (size_t i = 0; i < file->size;)
	{
		while (i < file->size && file->buf [i] == '\n')
			i++;

		if (i < file->size)
			file->nstr++;

		while (i < file->size && file->buf [i] != '\n')
			i++;
	}

	fprintf (log, "#  Amount of strings with letters in start file: %lu.\n", file->nstr);
}

/*
void text_form (File *file)
{
	assert (file);
}
*/

void qsort_my (void *array, size_t asize, size_t esize, int (*scmp) (const void *, const void *))
{
	void *stdval = array + esize * (asize - 1);
	void *sep = array;

	if (asize > 1)
	{
		for (size_t index = 0; index < asize; index++)
		{	
			if (scmp (stdval, array + esize * index) > 0)
			{
				swapv (sep, array + esize * index, esize);
				sep += esize;
			}
		}

		swapv (sep, stdval, esize);

		qsort_my (array, (sep - array) / esize, esize, scmp);
		qsort_my (sep + esize, (stdval - sep) / esize, esize, scmp);
	}	
}

void swapv (void *e1, void *e2, size_t esize)
{
	for (size_t byte = 0; byte < esize; byte++)
	{
		char temp = *((char *) e1 + byte);
		*((char *) e1 + byte) = *((char *) e2 + byte);
		*((char *) e2 + byte) = temp;
	}
}


int scmp (const void *s1, const void *s2)
{
	assert (s1);
	assert (s2);

	int i = 0, j = 0;

	char *l1 = ((const struct String *) s1)->str;
	char *l2 = ((const struct String *) s2)->str;
	
	while (*(l1 + i) && *(l2 + j))
	{
		while (*(l1 + i) && !isalpha (*(l1 + i)))
				i++;

		while (*(l2 + j) && !isalpha (*(l2 + j)))
				j++;

		char c1 = tolower (*(l1 + i));
		char c2 = tolower (*(l2 + j));

		if (c1 != c2)
			return c1 - c2;

		if (c1 && c2)
		{
			i++;
			j++;
		}
	}

	return 0;
}


int scmp_rev (const void *s1, const void *s2)
{
	assert (s1);
	assert (s2);

	char *l1 = ((const struct String *) s1)->str;
	char *l2 = ((const struct String *) s2)->str;

	int i = ((const struct String *) s1)->slen;
	int j = ((const struct String *) s2)->slen;

	while (i >= 0 && j >= 0)
	{
		while (i >= 0 && !isalpha (*(l1 + i)))
				i--;

		while (j >= 0 && !isalpha (*(l2 + j)))
				j--;

		char c1 = tolower (*(l1 + i));
		char c2 = tolower (*(l2 + j));

		if (c1 != c2)
			return c1 - c2;

		i--;
		j--;
	}

	return 0;
}


void print_file (File *file, char *name)
{
	assert (file);

	FILE *output = fopen (name, "w");

	for (size_t i = 0; i < file->nstr; i++)
		if (line_chk (file->strs[i].str))
			fprintf (output, "%s\n", file->strs[i].str);
		else
			i++;

	fclose (output);
}


int line_chk (char *line)
{
	int letter = 0;
	for (int i = 0; *(line + i); i++)
		if (isalpha(*(line + i)))
		{
			letter++;
			break;
		}

	return letter;
}
