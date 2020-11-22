#include "text.h"
#include <locale.h>

//*************************************************************************************************

int scmp      (const void *s1, const void *s2);
int scmp_rev  (const void *s1, const void *s2);
void swapv    (void *e1, void *e2, size_t esize);
void qsort_my (void *array, size_t asize, size_t esize, int (*cmp) (const void *, const void *));

//*************************************************************************************************

int main (int argc, char *argv[])
{
	setlocale (LC_CTYPE, "C.UTF-8");

	remove ("log");

	File file = {0};

	opener (&file, argc, argv);
	get_file   (&file);
	str_data   (&file);

	qsort_my (file.strs, file.nstr, sizeof (Str), scmp);
	print_file (&file, "text1");

	qsort_my (file.strs, file.nstr, sizeof (Str), scmp_rev);
	print_file (&file, "text2");

	free (file.buf);
	free (file.strs);
	fclose (file.text);

	printf ("#  results here:\n"
			"#  1) 'text1' <- sorted text;\n"
			"#  2) 'text2' <- sorted text (in reverse).\n");

	return 0;
}

//*************************************************************************************************

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

//*************************************************************************************************

void swapv (void *e1, void *e2, size_t esize)
{
	for (size_t byte = 0; byte < esize; byte++)
	{
		char temp = *((char *) e1 + byte);
		*((char *) e1 + byte) = *((char *) e2 + byte);
		*((char *) e2 + byte) = temp;
	}
}
