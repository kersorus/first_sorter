#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>


char **str_data (char *data, int size, int num);

void get_file (char *buf, int file_size, FILE *file, int *num_of_str);

int scompare (const void *s1, const void *s2);

void sort_file (char **p_str, int num_of_str);

void print_file (char **p_str, int num_of_str);


int main ()
{	
	char *filename = "text.txt";

	FILE *file = fopen (filename, "r");

	if (!file) 
	{
		fprintf (stderr, "problema with reading file\n");

		return 1;
	}


	struct stat about_file;

	stat (filename, &about_file);

	int file_size = about_file.st_size;

	char *buf = (char *) calloc (file_size, sizeof (char));

	
	int num_of_str = 0;

	get_file (buf, file_size, file, &num_of_str);

	char **p_str = str_data (buf, file_size, num_of_str); 


	sort_file (p_str, num_of_str);


	print_file (p_str, num_of_str);


	free (p_str);

	return 0;
}


char **str_data (char *data, int size, int num)
{
	char **str_data = (char **) calloc (num, sizeof (char *));

	int nomer = 0;

	for (int i = 0; i < size;)
	{
		while (data [i] == '\n')
		{
			data [i] = '\0';

			if (i < size)

				i++;

			else

				break;
		}

		str_data [nomer++] = &(data [i]);

		while (data [i] != '\n')

			if (i < size)

				i++;

			else

				break;
	}


	return str_data;
}


void get_file (char *buf, int file_size, FILE *file, int *num_of_str)
{
	fread (buf, sizeof (char), file_size, file);

	int num = 0;

	for (int i = 0; i < file_size;)
	{
		while (buf [i] == '\n')
		{
			if (i < file_size)

				i++;

			else

				break;
		}

		if (i < file_size)

			num++;

		while (buf [i] != '\n')

			if (i < file_size)

				i++;

			else

				break;
	}
	

	*num_of_str = num;

	printf ("#	Amount of strings with letters in start file: %d.\n", num);


	return;
}


int scompare (const void *s1, const void *s2)
{
	assert (s1);
	assert (s2);

	int len1 = strlen (s1);
	int len2 = strlen (s2);

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


void sort_file (char **p_str, int num_of_str)
{
	assert (p_str);

	qsort (p_str, num_of_str, sizeof (char *), scompare);
}


void print_file (char **p_str, int num_of_str)
{
	assert (p_str);


	for (int i = 0; i < num_of_str; i++)
		
		printf ("%s\n", p_str [i]);
	

	return;
}
