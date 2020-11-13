#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>


typedef struct File
{
	char *buf;
	FILE *text;
	char **strs;
	size_t nstr;
	size_t size;
}File;


void str_data  (File *file);
void get_file    (File *file);
void sort_file   (File *file);
void print_file  (File *file, char *name);
int opener       (File *file, int argc, char **argv);
int scompare (const void *s1, const void *s2);


int main (int argc, char *argv[])
{
	remove ("log");

	File file = {0};
	if (opener (&file, argc, argv)) return 1;

	get_file   (&file);
    str_data   (&file); 
	print_file (&file, "text1");

	sort_file  (&file);
	print_file (&file, "text2");

	free (file.buf);
	free (file.strs);
	fclose (file.text);

	printf ("#  results here:\n"
			"#  1) 'text1' <- formated text;\n"
			"#  2) 'text2' <- sorted text.\n");

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

	if (!file)
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

	file->strs = (char **) calloc (file->nstr, sizeof (char *));
    assert (file->strs);

	for (size_t i = 0 /* hot fix by Alesh #2 */, nomer = 0; i < file->size;)
	{
		while (i < file->size && file->buf [i] == '\n')
		{
			file->buf [i] = '\0';
			i++;
		}

		file->strs [nomer++] = file->buf + i;

		while (i < file->size && file->buf [i] != '\n')
			i++;
	}
}


void get_file (File *file)
{
	assert (file);

	FILE *log = fopen ("log", "a");
	assert (log);

	size_t nsymb = fread (file->buf, sizeof (char), file->size, file->text);
	assert (nsymb == file->size);
	fprintf (log, "Number of symbols in start file: %lu.\n", nsymb);

	file->nstr = 0; //hot fix by Alesha
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


int scompare (const void *s1, const void *s2)
{
	assert (s1);
	assert (s2);

	int i = 0, j = 0;

	char *l1 = *(char **)s1;
	char *l2 = *(char **)s2;

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
}


void sort_file (File *file)
{
	assert (file);

	qsort (file->strs, file->nstr, sizeof (char *), scompare);
}


void print_file (File *file, char *name)
{
	assert (file);

	FILE *output = fopen (name, "w");

	for (size_t i = 0; i < file->nstr; i++)
		fprintf (output, "%s\n", file->strs [i]);

	fclose (output);
}
